#pragma once

/*
 * dashboard.h
 * -----------
 * Stores the web dashboard HTML as a PROGMEM string literal.
 * The ESP32 serves this from flash memory, not RAM.
 *
 * The dashboard auto-polls /data every 2 seconds and updates
 * all readings without a page reload.
 */

const char DASHBOARD_HTML[] PROGMEM = R"HTMLEOF(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Power Monitor</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link href="https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Barlow+Condensed:wght@300;600&display=swap" rel="stylesheet">
  <style>
    :root {
      --bg:        #080c14;
      --surface:   #0d1626;
      --border:    #1a2f4a;
      --accent:    #00c8ff;
      --accent2:   #ff5f1f;
      --safe:      #00e676;
      --danger:    #ff1744;
      --text:      #cfe8ff;
      --muted:     #4a7fa0;
    }

    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

    body {
      background: var(--bg);
      color: var(--text);
      font-family: 'Barlow Condensed', sans-serif;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 32px 16px 48px;
    }

    /* Subtle scanline texture */
    body::before {
      content: '';
      position: fixed;
      inset: 0;
      background: repeating-linear-gradient(
        0deg,
        transparent,
        transparent 2px,
        rgba(0,200,255,0.015) 2px,
        rgba(0,200,255,0.015) 4px
      );
      pointer-events: none;
      z-index: 0;
    }

    header {
      width: 100%;
      max-width: 600px;
      display: flex;
      align-items: baseline;
      justify-content: space-between;
      margin-bottom: 36px;
      border-bottom: 1px solid var(--border);
      padding-bottom: 14px;
    }

    header h1 {
      font-family: 'Share Tech Mono', monospace;
      font-size: 1.1rem;
      letter-spacing: 0.22em;
      color: var(--accent);
      text-shadow: 0 0 18px rgba(0,200,255,0.45);
      text-transform: uppercase;
    }

    #live-indicator {
      display: flex;
      align-items: center;
      gap: 7px;
      font-size: 0.7rem;
      letter-spacing: 0.12em;
      color: var(--muted);
      text-transform: uppercase;
    }

    #live-dot {
      width: 7px;
      height: 7px;
      border-radius: 50%;
      background: var(--safe);
      box-shadow: 0 0 6px var(--safe);
      animation: pulse 1.4s ease-in-out infinite;
    }

    @keyframes pulse {
      0%, 100% { opacity: 1; transform: scale(1); }
      50%       { opacity: 0.4; transform: scale(0.8); }
    }

    .grid {
      display: grid;
      grid-template-columns: 1fr 1fr 1fr;
      gap: 14px;
      width: 100%;
      max-width: 600px;
    }

    .card {
      background: var(--surface);
      border: 1px solid var(--border);
      border-top: 2px solid var(--accent);
      border-radius: 4px;
      padding: 22px 16px 18px;
      text-align: center;
      position: relative;
      transition: border-color 0.3s, box-shadow 0.3s;
    }

    .card:hover {
      box-shadow: 0 0 20px rgba(0,200,255,0.08);
    }

    .card .label {
      font-size: 0.65rem;
      letter-spacing: 0.2em;
      color: var(--muted);
      text-transform: uppercase;
      margin-bottom: 12px;
    }

    .card .value {
      font-family: 'Share Tech Mono', monospace;
      font-size: 2.2rem;
      color: var(--text);
      line-height: 1;
      transition: color 0.3s;
    }

    .card .unit {
      font-size: 0.75rem;
      letter-spacing: 0.1em;
      color: var(--accent);
      margin-top: 8px;
      text-transform: uppercase;
    }

    /* Status bar */
    #status-bar {
      margin-top: 20px;
      width: 100%;
      max-width: 600px;
      padding: 16px 24px;
      border-radius: 4px;
      display: flex;
      align-items: center;
      gap: 12px;
      font-family: 'Share Tech Mono', monospace;
      font-size: 0.85rem;
      letter-spacing: 0.1em;
      text-transform: uppercase;
      transition: background 0.4s, border-color 0.4s;
    }

    #status-bar.normal {
      background: rgba(0,230,118,0.06);
      border: 1px solid rgba(0,230,118,0.25);
      color: var(--safe);
    }

    #status-bar.alert {
      background: rgba(255,23,68,0.10);
      border: 1px solid rgba(255,23,68,0.5);
      color: var(--danger);
      animation: blink-bar 0.75s step-start infinite;
    }

    @keyframes blink-bar {
      0%, 100% { opacity: 1; }
      50%       { opacity: 0.55; }
    }

    #status-icon {
      font-size: 1rem;
    }

    /* History table */
    .history-section {
      margin-top: 30px;
      width: 100%;
      max-width: 600px;
    }

    .section-label {
      font-size: 0.65rem;
      letter-spacing: 0.2em;
      color: var(--muted);
      text-transform: uppercase;
      margin-bottom: 10px;
      padding-bottom: 6px;
      border-bottom: 1px solid var(--border);
    }

    table {
      width: 100%;
      border-collapse: collapse;
      font-family: 'Share Tech Mono', monospace;
      font-size: 0.78rem;
    }

    th {
      color: var(--muted);
      text-align: left;
      padding: 6px 8px;
      letter-spacing: 0.1em;
      font-weight: 400;
      border-bottom: 1px solid var(--border);
    }

    td {
      padding: 6px 8px;
      color: var(--text);
      border-bottom: 1px solid rgba(26,47,74,0.5);
    }

    tr.sc-row td { color: var(--danger); }

    /* Footer */
    footer {
      margin-top: 40px;
      font-size: 0.62rem;
      letter-spacing: 0.14em;
      color: var(--border);
      text-align: center;
      text-transform: uppercase;
    }
  </style>
</head>
<body>

  <header>
    <h1>Power Monitor</h1>
    <div id="live-indicator">
      <span id="live-dot"></span>
      <span id="last-update">Connecting...</span>
    </div>
  </header>

  <div class="grid">
    <div class="card" id="card-v">
      <div class="label">Voltage</div>
      <div class="value" id="val-v">--</div>
      <div class="unit">Volts DC</div>
    </div>
    <div class="card" id="card-i">
      <div class="label">Current</div>
      <div class="value" id="val-i">--</div>
      <div class="unit">Amperes</div>
    </div>
    <div class="card" id="card-p">
      <div class="label">Power</div>
      <div class="value" id="val-p">--</div>
      <div class="unit">Watts</div>
    </div>
  </div>

  <div id="status-bar" class="normal">
    <span id="status-icon">[OK]</span>
    <span id="status-text">System operating normally</span>
  </div>

  <div class="history-section">
    <div class="section-label">Recent readings (last 10)</div>
    <table>
      <thead>
        <tr>
          <th>Time</th>
          <th>Voltage (V)</th>
          <th>Current (A)</th>
          <th>Power (W)</th>
          <th>Status</th>
        </tr>
      </thead>
      <tbody id="history-body">
      </tbody>
    </table>
  </div>

  <footer>
    ESP32 IoT Power Monitor &mdash; Auto-refresh 2s &mdash; /data JSON endpoint available
  </footer>

  <script>
    const history = [];
    const MAX_HISTORY = 10;

    function now() {
      const d = new Date();
      const pad = n => String(n).padStart(2, '0');
      return `${pad(d.getHours())}:${pad(d.getMinutes())}:${pad(d.getSeconds())}`;
    }

    function updateHistory(d) {
      history.unshift({ time: now(), ...d });
      if (history.length > MAX_HISTORY) history.pop();

      const tbody = document.getElementById('history-body');
      tbody.innerHTML = '';
      history.forEach(row => {
        const tr = document.createElement('tr');
        if (row.short_circuit) tr.classList.add('sc-row');
        tr.innerHTML = `
          <td>${row.time}</td>
          <td>${row.voltage.toFixed(2)}</td>
          <td>${row.current.toFixed(3)}</td>
          <td>${row.power.toFixed(2)}</td>
          <td>${row.short_circuit ? 'SHORT CIRCUIT' : 'Normal'}</td>
        `;
        tbody.appendChild(tr);
      });
    }

    async function fetchData() {
      try {
        const res = await fetch('/data');
        if (!res.ok) throw new Error('HTTP ' + res.status);
        const d = await res.json();

        document.getElementById('val-v').textContent = d.voltage.toFixed(2);
        document.getElementById('val-i').textContent = d.current.toFixed(3);
        document.getElementById('val-p').textContent = d.power.toFixed(2);
        document.getElementById('last-update').textContent = 'Updated ' + now();

        const bar      = document.getElementById('status-bar');
        const icon     = document.getElementById('status-icon');
        const text     = document.getElementById('status-text');
        const liveDot  = document.getElementById('live-dot');

        if (d.short_circuit) {
          bar.className   = 'alert';
          icon.textContent = '[!!]';
          text.textContent = 'SHORT CIRCUIT DETECTED -- CHECK CONNECTIONS';
          liveDot.style.background   = 'var(--danger)';
          liveDot.style.boxShadow    = '0 0 6px var(--danger)';
        } else {
          bar.className   = 'normal';
          icon.textContent = '[OK]';
          text.textContent = 'System operating normally';
          liveDot.style.background   = 'var(--safe)';
          liveDot.style.boxShadow    = '0 0 6px var(--safe)';
        }

        updateHistory(d);
      } catch (err) {
        document.getElementById('last-update').textContent = 'Connection error';
        console.error('Fetch failed:', err);
      }
    }

    fetchData();
    setInterval(fetchData, 2000);
  </script>

</body>
</html>
)HTMLEOF";

inline const char* getDashboardHTML() {
    return DASHBOARD_HTML;
}
