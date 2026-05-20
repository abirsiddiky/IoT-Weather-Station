/*
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║          IoT WEATHER STATION  •  ESP32 / ESP8266 + BME280               ║
 * ║          Version 2.0.0  |  Production Grade  |  MIT License             ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 *
 *  FEATURES
 *  ─────────────────────────────────────────────────────────────────────────
 *  ✓ Glassmorphism dark/light web dashboard (served from ESP flash)
 *  ✓ WebSocket real-time updates (no page refresh ever)
 *  ✓ NTP time sync — 12-hour AM/PM format
 *  ✓ Temperature sparkline chart (last 60 readings)
 *  ✓ Dark / Light mode toggle
 *  ✓ REST API  GET /api/data  →  JSON payload
 *  ✓ OTA (over-the-air) firmware updates
 *  ✓ mDNS  →  http://weatherstation.local
 *  ✓ WiFi signal strength + auto-reconnect
 *  ✓ Device uptime tracker
 *  ✓ PWA manifest
 *  ✓ Pressure-to-weather condition mapping
 *  ✓ Heat index "feels like" calculation
 *
 * ═══════════════════════════════════════════════════════════════════════════
 *  WIRING DIAGRAM
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *  ESP32  ──────────────────────────  BME280
 *  ──────                             ──────
 *  3.3V   ──────────────────────────  VCC
 *  GND    ──────────────────────────  GND
 *  GPIO21 (SDA) ────────────────────  SDA
 *  GPIO22 (SCL) ────────────────────  SCL
 *
 *  ESP8266 / NodeMCU ───────────────  BME280
 *  ─────────────────                  ──────
 *  3.3V   ──────────────────────────  VCC
 *  GND    ──────────────────────────  GND
 *  D2 / GPIO4 (SDA) ───────────────  SDA
 *  D1 / GPIO5 (SCL) ───────────────  SCL
 *
 *  ⚠  Use 3.3V ONLY — BME280 is NOT 5V tolerant!
 *  ⚠  Default I2C address is 0x76 (SDO pin → GND).
 *     If sensor not found, try BME280_I2C_ADDR 0x77 (SDO → 3.3V)
 *
 * ═══════════════════════════════════════════════════════════════════════════
 *  REQUIRED LIBRARIES  (Install via Arduino Library Manager)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *  1. ESPAsyncWebServer   →  Search "ESPAsyncWebServer" by lacamera
 *                            or:  github.com/lacamera/ESPAsyncWebServer
 *  2. AsyncTCP (ESP32)    →  Search "AsyncTCP"
 *     ESPAsyncTCP (ESP8266) →  Search "ESPAsyncTCP"
 *  3. Adafruit BME280     →  Search "Adafruit BME280 Library"
 *  4. Adafruit Unified Sensor → Search "Adafruit Unified Sensor"
 *  5. NTPClient           →  Search "NTPClient" by Fabrice Weinberg
 *
 * ═══════════════════════════════════════════════════════════════════════════
 *  BOARD SETTINGS (Arduino IDE 2.x)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *  ESP32:
 *    Board        → "ESP32 Dev Module" (or your specific ESP32 board)
 *    CPU Speed    → 240 MHz
 *    Flash Size   → 4MB (Scheme: Default 4MB with spiffs)
 *    Upload Speed → 921600
 *    Partition    → "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)"
 *
 *  ESP8266 (NodeMCU/Wemos D1 Mini):
 *    Board        → "NodeMCU 1.0 (ESP-12E Module)"
 *    CPU Speed    → 160 MHz  (higher = more stable async server)
 *    Flash Size   → 4MB (FS:2MB, OTA:~1019KB)
 *    Upload Speed → 921600
 *
 * ═══════════════════════════════════════════════════════════════════════════
 *  UPLOAD STEPS
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *  1. Install all 5 libraries above
 *  2. Edit WIFI_SSID, WIFI_PASSWORD below
 *  3. Set UTC_OFFSET_SEC for your timezone
 *     Bangladesh (UTC+6) = 6 * 3600 = 21600
 *     India      (UTC+5:30) = 19800
 *     UK         (UTC+0)   = 0
 *     US/New York (UTC-5)  = -18000
 *  4. Select board + COM port in Arduino IDE
 *  5. Click Upload
 *  6. Open Serial Monitor at 115200 baud
 *  7. Copy the IP shown → open in browser
 *     OR navigate to:  http://weatherstation.local
 *
 * ═══════════════════════════════════════════════════════════════════════════
 *  TROUBLESHOOTING
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *  ✗ "BME280 not found"
 *    → Check wiring; change BME280_I2C_ADDR from 0x76 to 0x77
 *    → Make sure VCC goes to 3.3V (NOT 5V)
 *
 *  ✗ Won't connect to WiFi
 *    → Verify SSID/password; router must be 2.4 GHz (not 5 GHz)
 *    → Check for special characters in password
 *
 *  ✗ "weatherstation.local" doesn't work
 *    → Windows: install Bonjour (Apple), or use the IP address directly
 *    → Android: mDNS may not work; use the IP address
 *
 *  ✗ Dashboard doesn't update
 *    → Open browser console (F12); check for WebSocket errors
 *    → Try a private/incognito window or disable ad blockers
 *
 *  ✗ OTA update fails
 *    → Verify partition scheme has OTA partition allocated
 *    → Check Serial output during OTA for error messages
 *
 *  ✗ Compile errors on ESP8266
 *    → Ensure you have "ESPAsyncTCP" (not "AsyncTCP") installed
 *    → Core version should be ≥ 3.1.x
 *
 *  ✗ Date shows wrong year (e.g. 5427055) on ESP8266
 *    → gmtime((time_t*)&epoch) is buggy on ESP8266 — corrupts tm_year
 *    → Fixed in this sketch: getDate() uses manual epoch calculation
 *       instead of gmtime(), works correctly on both ESP32 and ESP8266
 *
 *  ✗ operator+ compile error on ESP8266 (const char[] + const char*)
 *    → Fixed in this sketch: buildJSON() wraps bool results in String()
 */

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  PLATFORM DETECTION & INCLUDES
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <ESPmDNS.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <ESP8266mDNS.h>
#else
  #error "Unsupported board! Select ESP32 or ESP8266 in Arduino IDE."
#endif

#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  ★  USER CONFIGURATION  ← Edit only this section  ★
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

const char* WIFI_SSID       = "YOUR_WIFI_SSID";     // ← Your WiFi network name
const char* WIFI_PASSWORD   = "YOUR_WIFI_PASSWORD"; // ← Your WiFi password
const long  UTC_OFFSET_SEC  = 6 * 3600;             // ← UTC+6 (Bangladesh). Change as needed.
const char* DEVICE_HOSTNAME = "weatherstation";      // ← mDNS: weatherstation.local
const char* OTA_PASSWORD    = "ota1234";             // ← OTA update password (change this!)

// BME280 I2C address: 0x76 (SDO→GND) or 0x77 (SDO→3.3V)
#define BME280_I2C_ADDR   0x76
#define SEA_LEVEL_HPA     1013.25f   // Standard atmosphere (hPa) for altitude calc

// Update intervals (milliseconds)
#define SENSOR_INTERVAL   2000       // Read sensor every 2 seconds
#define BROADCAST_INTERVAL 1000      // Push data via WebSocket every 1 second
#define NTP_INTERVAL      60000      // Re-sync NTP every 60 seconds
#define WIFI_CHECK_INTERVAL 10000    // Check WiFi status every 10 seconds
#define HISTORY_RECORD_INTERVAL 30000 // Log temperature every 30 seconds

// Temperature history buffer (30 points × 30s = 15 min of history)
#define HISTORY_SIZE 30

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  HTML / CSS / JS  —  The complete web dashboard (stored in flash)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en" data-theme="dark">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="theme-color" content="#0d1117">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="description" content="IoT Weather Station — Real-time environmental monitoring">
<link rel="manifest" href="/manifest.json">
<title>⛅ Weather Station</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
:root{
  --bg:#0d1117;--bg2:#161b22;--bg3:#21262d;
  --glass:rgba(255,255,255,0.04);--glass2:rgba(255,255,255,0.07);
  --border:rgba(255,255,255,0.08);--border2:rgba(255,255,255,0.12);
  --shadow:rgba(0,0,0,0.4);
  --accent:#00d4ff;--accent2:#7c3aed;--accent3:#f59e0b;
  --text:#e6edf3;--text2:#8b949e;--text3:#484f58;
  --ok:#3fb950;--warn:#d29922;--err:#f85149;
  --temp:#ff6b6b;--humid:#4ecdc4;--pres:#a78bfa;--alt:#fbbf24;
  --radius:20px;--radius-sm:12px;
}
[data-theme=light]{
  --bg:#f0f4f8;--bg2:#ffffff;--bg3:#e2e8f0;
  --glass:rgba(255,255,255,0.7);--glass2:rgba(255,255,255,0.85);
  --border:rgba(0,0,0,0.07);--border2:rgba(0,0,0,0.12);
  --shadow:rgba(0,0,0,0.08);
  --text:#1a202c;--text2:#4a5568;--text3:#a0aec0;
}
html{scroll-behavior:smooth}
body{
  font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;
  background:var(--bg);color:var(--text);min-height:100vh;
  background-image:
    radial-gradient(ellipse 60% 80% at 10% 50%,rgba(0,212,255,.07) 0%,transparent 60%),
    radial-gradient(ellipse 60% 60% at 90% 10%,rgba(124,58,237,.07) 0%,transparent 60%);
  transition:background .3s,color .3s;
}

/* ── Loader ─────────────────────────────────────── */
#loader{
  position:fixed;inset:0;background:var(--bg);z-index:9999;
  display:flex;flex-direction:column;align-items:center;justify-content:center;
  transition:opacity .5s ease,visibility .5s ease;
}
#loader.done{opacity:0;visibility:hidden}
.ld-icon{font-size:4rem;animation:float 2.4s ease-in-out infinite}
.ld-txt{color:var(--text2);margin-top:1rem;font-size:.9rem;letter-spacing:.05em}
.ld-bar{width:200px;height:3px;background:var(--border);border-radius:2px;margin-top:1.5rem;overflow:hidden}
.ld-fill{height:100%;background:linear-gradient(90deg,var(--accent),var(--accent2));border-radius:2px;animation:ldbar 1.8s ease infinite}

/* ── Lost-connection overlay ─────────────────── */
#lost{
  display:none;position:fixed;inset:0;
  background:rgba(0,0,0,.65);backdrop-filter:blur(6px);-webkit-backdrop-filter:blur(6px);
  z-index:8888;align-items:center;justify-content:center;
}
#lost.show{display:flex}
.lost-box{
  background:var(--bg2);border:1px solid var(--border);
  padding:2rem 2.5rem;border-radius:var(--radius);text-align:center;
  max-width:320px;
}
.lost-box h3{color:var(--err);font-size:1.1rem;margin-bottom:.5rem}
.lost-box p{color:var(--text2);font-size:.85rem}
.spinner{
  width:28px;height:28px;border:3px solid var(--border2);border-top-color:var(--accent);
  border-radius:50%;animation:spin .8s linear infinite;margin:1rem auto 0;
}

/* ── App shell ───────────────────────────────── */
.app{max-width:1100px;margin:0 auto;padding:1.5rem;display:flex;flex-direction:column;gap:1.25rem}

/* ── Header ──────────────────────────────────── */
.hdr{display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:.75rem}
.logo{display:flex;align-items:center;gap:.75rem}
.logo-ico{font-size:2rem;line-height:1}
.logo h1{font-size:1.3rem;font-weight:700;background:linear-gradient(135deg,var(--accent),var(--accent2));-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text}
.logo p{font-size:.7rem;color:var(--text3);letter-spacing:.06em;text-transform:uppercase}
.hdr-r{display:flex;align-items:center;gap:.6rem;flex-wrap:wrap}

/* ── Badges ───────────────────────────────────── */
.badge{
  display:inline-flex;align-items:center;gap:.4rem;
  padding:.28rem .7rem;border-radius:50px;font-size:.72rem;font-weight:600;
  border:1px solid transparent;white-space:nowrap;transition:all .3s;
}
.badge .dot{width:6px;height:6px;border-radius:50%;flex-shrink:0}
.ok{background:rgba(63,185,80,.12);color:var(--ok);border-color:rgba(63,185,80,.2)}
.ok .dot{background:var(--ok);box-shadow:0 0 6px var(--ok);animation:pdot 2s ease infinite}
.err{background:rgba(248,81,73,.12);color:var(--err);border-color:rgba(248,81,73,.2)}
.err .dot{background:var(--err)}

/* ── Theme & API buttons ─────────────────────── */
.icon-btn{
  background:var(--glass);border:1px solid var(--border);color:var(--text);
  width:36px;height:36px;border-radius:50%;cursor:pointer;
  font-size:1rem;display:flex;align-items:center;justify-content:center;
  transition:all .25s;text-decoration:none;
}
.icon-btn:hover{background:var(--glass2);border-color:var(--border2);transform:scale(1.08)}

/* ── Hero clock ──────────────────────────────── */
.hero{
  background:var(--glass);border:1px solid var(--border);border-radius:var(--radius);
  backdrop-filter:blur(24px);-webkit-backdrop-filter:blur(24px);
  padding:2.5rem 2rem;text-align:center;position:relative;overflow:hidden;
}
.hero::before{
  content:'';position:absolute;top:-60%;left:-60%;width:220%;height:220%;
  background:radial-gradient(ellipse at center,rgba(0,212,255,.05) 0%,transparent 65%);
  animation:rot 25s linear infinite;pointer-events:none;
}
.clock{
  font-size:clamp(2.8rem,9vw,5.5rem);font-weight:200;
  letter-spacing:-.03em;font-variant-numeric:tabular-nums;line-height:1;
}
.ampm{font-size:clamp(1rem,3vw,2rem);color:var(--accent);font-weight:500;margin-left:.25em}
.date-str{color:var(--text2);font-size:clamp(.85rem,2.5vw,1.1rem);margin-top:.6rem;font-weight:300}

/* ── Grid layouts ────────────────────────────── */
.row2{display:grid;grid-template-columns:1fr 1fr;gap:1.25rem}
.row4{display:grid;grid-template-columns:repeat(4,1fr);gap:1.25rem}

/* ── Cards ───────────────────────────────────── */
.card{
  background:var(--glass);border:1px solid var(--border);
  border-radius:var(--radius);padding:1.5rem;
  backdrop-filter:blur(24px);-webkit-backdrop-filter:blur(24px);
  transition:transform .3s,box-shadow .3s,background .3s;
  position:relative;overflow:hidden;
}
.card:hover{transform:translateY(-4px);box-shadow:0 24px 48px var(--shadow);background:var(--glass2)}
.card::after{content:'';position:absolute;top:-40%;right:-20%;width:160px;height:160px;border-radius:50%;opacity:.08;pointer-events:none}
.t-card::after{background:radial-gradient(circle,var(--temp),transparent)}
.h-card::after{background:radial-gradient(circle,var(--humid),transparent)}
.p-card::after{background:radial-gradient(circle,var(--pres),transparent)}
.a-card::after{background:radial-gradient(circle,var(--alt),transparent)}

.c-lbl{font-size:.68rem;text-transform:uppercase;letter-spacing:.1em;color:var(--text3);font-weight:600;margin-bottom:.5rem}
.c-val{font-size:2.8rem;font-weight:700;line-height:1;font-variant-numeric:tabular-nums}
.c-unit{font-size:1rem;font-weight:400;color:var(--text2);margin-left:.1em}
.c-sub{font-size:.78rem;color:var(--text3);margin-top:.4rem}

/* Card accent colors */
.t-val{color:var(--temp)}
.h-val{color:var(--humid)}
.p-val{color:var(--pres)}
.a-val{color:var(--alt)}

/* ── Progress bars ───────────────────────────── */
.prog{height:4px;background:var(--border);border-radius:2px;margin-top:1rem;overflow:hidden}
.prog-fill{height:100%;border-radius:2px;transition:width .9s cubic-bezier(.4,0,.2,1)}
.t-prog{background:linear-gradient(90deg,#4ecdc4,var(--temp))}
.h-prog{background:linear-gradient(90deg,#667eea,var(--humid))}

/* ── Sparkline chart ─────────────────────────── */
.chart-wrap{margin-top:1rem;height:64px;position:relative}
#tempChart{display:block;width:100%!important;height:64px!important}

/* ── Humidity visual ─────────────────────────── */
.humid-drops{display:flex;align-items:flex-end;gap:3px;margin-top:1rem;height:48px}
.drop{flex:1;border-radius:2px 2px 0 0;background:var(--border);transition:height .9s ease,background .5s}

/* ── WiFi bars ───────────────────────────────── */
.wifi-bars{display:flex;align-items:flex-end;gap:3px;height:28px;margin-bottom:.5rem}
.wb{border-radius:2px;background:var(--border);transition:background .4s}
.wb:nth-child(1){width:7px;height:8px}
.wb:nth-child(2){width:7px;height:13px}
.wb:nth-child(3){width:7px;height:19px}
.wb:nth-child(4){width:7px;height:28px}
.wb.active{background:var(--accent)}

/* ── Uptime / small card value ───────────────── */
.c-val-sm{font-size:1.8rem;font-weight:700;line-height:1;font-variant-numeric:tabular-nums}

/* ── Footer ──────────────────────────────────── */
.footer{text-align:center;color:var(--text3);font-size:.72rem;padding:.5rem 0 1.5rem}
.update-row{display:inline-flex;align-items:center;gap:.4rem}
.live-dot{width:7px;height:7px;background:var(--ok);border-radius:50%;animation:pdot 2s infinite}

/* ── Animations ──────────────────────────────── */
@keyframes float{0%,100%{transform:translateY(0)}50%{transform:translateY(-10px)}}
@keyframes ldbar{0%{transform:translateX(-100%)}100%{transform:translateX(250%)}}
@keyframes pdot{0%,100%{opacity:1;transform:scale(1)}50%{opacity:.4;transform:scale(.7)}}
@keyframes rot{from{transform:rotate(0)}to{transform:rotate(360deg)}}
@keyframes spin{to{transform:rotate(360deg)}}
@keyframes fadeUp{from{opacity:0;transform:translateY(16px)}to{opacity:1;transform:translateY(0)}}
@keyframes flash{0%,100%{opacity:1}40%{opacity:.35}}

.ani{animation:fadeUp .5s ease both}
.flash{animation:flash .35s ease}

/* ── Responsive ──────────────────────────────── */
@media(max-width:700px){
  .row4{grid-template-columns:1fr 1fr}
  .app{padding:1rem}
}
@media(max-width:500px){
  .row2{grid-template-columns:1fr}
  .c-val{font-size:2.2rem}
  .hero{padding:1.75rem 1rem}
}
@media(max-width:380px){
  .row4{grid-template-columns:1fr 1fr}
  .hdr-r .badge:not(#sbadge){display:none}
}
</style>
</head>
<body>

<!-- ── Loading screen ───────────────────────────────────────── -->
<div id="loader">
  <div class="ld-icon">⛅</div>
  <div class="ld-txt">Connecting to Weather Station…</div>
  <div class="ld-bar"><div class="ld-fill"></div></div>
</div>

<!-- ── Connection lost overlay ──────────────────────────────── -->
<div id="lost">
  <div class="lost-box">
    <h3>⚠ Connection Lost</h3>
    <p>Reconnecting to weather station…</p>
    <div class="spinner"></div>
  </div>
</div>

<!-- ── Main app ─────────────────────────────────────────────── -->
<main class="app">

  <!-- Header -->
  <header class="hdr ani">
    <div class="logo">
      <div class="logo-ico">🌤</div>
      <div>
        <h1>Weather Station</h1>
        <p>IoT Environmental Monitor</p>
      </div>
    </div>
    <div class="hdr-r">
      <span class="badge err" id="sbadge"><span class="dot"></span><span id="slbl">Sensor</span></span>
      <span class="badge err" id="wbadge"><span class="dot"></span>WiFi</span>
      <a class="icon-btn" href="/api/data" target="_blank" title="JSON API">⚙</a>
      <button class="icon-btn" id="themeBtn" title="Toggle theme">🌙</button>
    </div>
  </header>

  <!-- Hero clock -->
  <section class="hero ani" style="animation-delay:.08s">
    <div class="clock">
      <span id="th">--</span>:<span id="tm">--</span>:<span id="ts">--</span><span class="ampm" id="ta">--</span>
    </div>
    <div class="date-str" id="dt">Syncing time…</div>
  </section>

  <!-- Primary row: Temperature + Humidity -->
  <div class="row2">
    <!-- Temperature card -->
    <div class="card t-card ani" style="animation-delay:.14s">
      <div class="c-lbl">🌡 Temperature</div>
      <div class="c-val t-val" id="vt">--<span class="c-unit">°C</span></div>
      <div class="c-sub" id="feel">Feels like: --</div>
      <div class="prog"><div class="prog-fill t-prog" id="tbar" style="width:0"></div></div>
      <div class="chart-wrap"><canvas id="tempChart"></canvas></div>
    </div>

    <!-- Humidity card -->
    <div class="card h-card ani" style="animation-delay:.18s">
      <div class="c-lbl">💧 Humidity</div>
      <div class="c-val h-val" id="vh">--<span class="c-unit">%</span></div>
      <div class="c-sub" id="hlvl">Loading…</div>
      <div class="prog"><div class="prog-fill h-prog" id="hbar" style="width:0"></div></div>
      <div class="humid-drops" id="hdrops">
        <!-- generated by JS -->
      </div>
    </div>
  </div>

  <!-- Secondary row: Pressure / Altitude / WiFi / Uptime -->
  <div class="row4">

    <!-- Pressure -->
    <div class="card p-card ani" style="animation-delay:.22s">
      <div class="c-lbl">🔵 Pressure</div>
      <div class="c-val p-val" id="vp">--<span class="c-unit" style="font-size:.75rem">mmHg</span></div>
      <div class="c-sub" id="plvl">Loading…</div>
    </div>

    <!-- Altitude -->
    <div class="card a-card ani" style="animation-delay:.26s">
      <div class="c-lbl">⛰ Altitude</div>
      <div class="c-val a-val" id="va">--<span class="c-unit">m</span></div>
      <div class="c-sub">Est. above sea level</div>
    </div>

    <!-- WiFi -->
    <div class="card ani" style="animation-delay:.3s">
      <div class="c-lbl">📶 WiFi Signal</div>
      <div class="wifi-bars">
        <div class="wb" id="wb1"></div>
        <div class="wb" id="wb2"></div>
        <div class="wb" id="wb3"></div>
        <div class="wb" id="wb4"></div>
      </div>
      <div class="c-sub" id="wrssi">-- dBm</div>
      <div class="c-sub" id="wqual" style="color:var(--accent);font-weight:600">--</div>
    </div>

    <!-- Uptime -->
    <div class="card ani" style="animation-delay:.34s">
      <div class="c-lbl">⏱ Device Uptime</div>
      <div class="c-val-sm" id="vup">--</div>
      <div class="c-sub" style="margin-top:.4rem">Since last boot</div>
    </div>

  </div>

  <!-- Footer -->
  <footer class="footer ani" style="animation-delay:.38s">
    <div class="update-row">
      <span class="live-dot"></span>
      Last updated: <strong id="upd">Never</strong>
    </div>
    &nbsp;·&nbsp; ESP Weather Station v2.0
    &nbsp;·&nbsp; <a href="/api/data" style="color:var(--accent);text-decoration:none">REST API</a>
  </footer>

</main>

<script>
// ── Config ────────────────────────────────────────────────────
const WS_URL = 'ws://' + location.hostname + '/ws';
let ws, reconnTimer, dark = true;
let histData = [], chartCanvas, ctx;

// ── Shortcuts ─────────────────────────────────────────────────
const $ = id => document.getElementById(id);
const loader = $('loader');
const lostDiv = $('lost');

// ── WebSocket lifecycle ───────────────────────────────────────
function connect() {
  ws = new WebSocket(WS_URL);

  ws.onopen = () => {
    clearTimeout(reconnTimer);
    lostDiv.classList.remove('show');
    setTimeout(() => loader.classList.add('done'), 600);
  };

  ws.onmessage = e => {
    try { render(JSON.parse(e.data)); } catch(_) {}
  };

  ws.onclose = ws.onerror = () => {
    lostDiv.classList.add('show');
    reconnTimer = setTimeout(connect, 3000);
  };
}

// ── Render incoming data ──────────────────────────────────────
function render(d) {
  // Clock
  if (d.time) {
    const [hms, ap] = d.time.split(' ');
    const [h, m, s] = hms.split(':');
    $('th').textContent = h;
    $('tm').textContent = m;
    $('ts').textContent = s || '00';
    $('ta').textContent = ap || '';
  }
  if (d.date) $('dt').textContent = d.date;

  // Status badges
  setBadge('sbadge', $('slbl'), d.sensorOk, d.sensorOk ? 'Online' : 'Offline');
  setBadge('wbadge', null, d.wifiOk, null);

  if (d.sensorOk) {
    const t = +d.temperature, h = +d.humidity, p = +d.pressure, a = +d.altitude;

    // Temperature
    setVal('vt', t.toFixed(1), '°C', 'c-unit');
    $('tbar').style.width = clamp((t / 50) * 100, 0, 100) + '%';
    $('feel').textContent = 'Feels like: ' + heatIndex(t, h).toFixed(1) + '°C';

    // Humidity
    setVal('vh', h.toFixed(1), '%', 'c-unit');
    $('hbar').style.width = clamp(h, 0, 100) + '%';
    $('hlvl').textContent = humidLabel(h);
    drawDrops(h);

    // Pressure (mmHg)
    setVal('vp', p.toFixed(1), ' mmHg', 'c-unit', 'font-size:.75rem');
    $('plvl').textContent = pressLabel(p);

    // Altitude
    setVal('va', a.toFixed(1), ' m', 'c-unit');

    // History chart
    if (d.history && d.history.length > 1) {
      histData = d.history;
      drawChart();
    }
  }

  // WiFi
  if (d.rssi !== undefined) {
    const rssi = +d.rssi;
    const bars = rssi > -50 ? 4 : rssi > -65 ? 3 : rssi > -75 ? 2 : 1;
    [1,2,3,4].forEach(i => $('wb'+i).classList.toggle('active', i <= bars));
    $('wrssi').textContent = rssi + ' dBm';
    $('wqual').textContent = ['','Weak','Fair','Good','Excellent'][bars];
  }

  // Uptime
  if (d.uptime !== undefined) $('vup').textContent = fmtUp(+d.uptime);

  // Timestamp
  $('upd').textContent = new Date().toLocaleTimeString();
}

// ── Helpers ───────────────────────────────────────────────────
function clamp(v, mn, mx) { return Math.min(Math.max(v, mn), mx); }

function setVal(id, val, unit, cls, style) {
  const el = $(id);
  el.innerHTML = val + `<span class="${cls}"${style?' style="'+style+'"':''}>${unit}</span>`;
  el.classList.remove('flash'); void el.offsetWidth; el.classList.add('flash');
}

function setBadge(id, lbl, ok, text) {
  const el = $(id);
  el.className = 'badge ' + (ok ? 'ok' : 'err');
  if (lbl && text) lbl.textContent = text;
}

function heatIndex(t, h) {
  if (t < 27 || h < 40) return t;
  return -8.78469 + 1.61139*t + 2.33855*h - 0.14612*t*h
         - 0.012308*t*t - 0.016425*h*h + 0.002212*t*t*h
         + 0.000725*t*h*h - 0.00000358*t*t*h*h;
}

function humidLabel(h) {
  if (h < 25) return '🏜 Very Dry';
  if (h < 40) return '😐 Dry — Comfortable';
  if (h < 55) return '✅ Ideal Humidity';
  if (h < 70) return '💧 Moderately Humid';
  if (h < 85) return '💦 Humid';
  return '🌊 Very Humid';
}

function pressLabel(mmHg) {
  const hpa = mmHg * 1.33322;
  if (hpa < 970) return '⛈ Very Low — Storm expected';
  if (hpa < 990) return '🌧 Low — Unsettled weather';
  if (hpa < 1010) return '🌥 Normal — Variable';
  if (hpa < 1030) return '🌤 High — Fair & pleasant';
  return '☀ Very High — Dry & sunny';
}

function fmtUp(sec) {
  const d = Math.floor(sec/86400), h = Math.floor(sec%86400/3600);
  const m = Math.floor(sec%3600/60), s = sec%60;
  if (d) return d+'d '+h+'h '+m+'m';
  if (h) return h+'h '+m+'m '+s+'s';
  return m+'m '+s+'s';
}

// ── Humidity drops visual ─────────────────────────────────────
function drawDrops(h) {
  const c = $('hdrops');
  const n = 20;
  if (!c.children.length) {
    c.innerHTML = Array.from({length:n},(_,i)=>
      `<div class="drop" id="dr${i}"></div>`).join('');
  }
  Array.from({length:n},(_,i) => {
    const el = $('dr'+i);
    const filled = i < Math.round(h/100*n);
    el.style.height = (20 + i*1.4) + 'px';
    el.style.background = filled ? `hsl(${175-h*.5},70%,${55-i}%)` : 'var(--border)';
  });
}

// ── Temperature sparkline ─────────────────────────────────────
function initChart() {
  chartCanvas = $('tempChart');
  if (!chartCanvas) return;
  const w = chartCanvas.parentElement.offsetWidth;
  const dpr = window.devicePixelRatio || 1;
  chartCanvas.width  = w * dpr;
  chartCanvas.height = 64 * dpr;
  chartCanvas.style.width  = w + 'px';
  chartCanvas.style.height = '64px';
  ctx = chartCanvas.getContext('2d');
  ctx.scale(dpr, dpr);
  drawChart();
}

function drawChart() {
  if (!ctx || histData.length < 2) return;
  const W = chartCanvas.offsetWidth, H = 64;
  ctx.clearRect(0, 0, W, H);

  const mn = Math.min(...histData) - 0.5;
  const mx = Math.max(...histData) + 0.5;
  const rng = mx - mn || 1;
  const x = i => (i / (histData.length-1)) * W;
  const y = v => H - ((v-mn)/rng)*(H-10) - 5;

  // Area fill
  const grad = ctx.createLinearGradient(0,0,0,H);
  grad.addColorStop(0, 'rgba(255,107,107,.35)');
  grad.addColorStop(1, 'rgba(255,107,107,0)');

  ctx.beginPath();
  ctx.moveTo(x(0), y(histData[0]));
  for (let i=1; i<histData.length; i++) {
    const cx = (x(i-1)+x(i))/2;
    ctx.bezierCurveTo(cx,y(histData[i-1]),cx,y(histData[i]),x(i),y(histData[i]));
  }
  ctx.lineTo(x(histData.length-1), H);
  ctx.lineTo(x(0), H);
  ctx.closePath();
  ctx.fillStyle = grad;
  ctx.fill();

  // Stroke
  ctx.beginPath();
  ctx.moveTo(x(0), y(histData[0]));
  for (let i=1; i<histData.length; i++) {
    const cx = (x(i-1)+x(i))/2;
    ctx.bezierCurveTo(cx,y(histData[i-1]),cx,y(histData[i]),x(i),y(histData[i]));
  }
  ctx.strokeStyle = '#ff6b6b';
  ctx.lineWidth = 2;
  ctx.stroke();

  // Latest dot
  const lx = x(histData.length-1), ly = y(histData[histData.length-1]);
  ctx.beginPath(); ctx.arc(lx,ly,4,0,Math.PI*2);
  ctx.fillStyle='#ff6b6b'; ctx.fill();
  ctx.strokeStyle='rgba(255,255,255,.8)'; ctx.lineWidth=1.5; ctx.stroke();
}

// ── Theme toggle ──────────────────────────────────────────────
$('themeBtn').addEventListener('click', () => {
  dark = !dark;
  document.documentElement.setAttribute('data-theme', dark?'dark':'light');
  $('themeBtn').textContent = dark ? '🌙' : '☀️';
  setTimeout(drawChart, 80);
});

// ── Boot ──────────────────────────────────────────────────────
window.addEventListener('load', () => {
  setTimeout(() => { initChart(); connect(); }, 150);
});
window.addEventListener('resize', () => { initChart(); });
</script>
</body>
</html>
)rawliteral";

// ─── PWA Manifest ──────────────────────────────────────────────────────────
const char MANIFEST_JSON[] PROGMEM = R"rawliteral(
{
  "name": "Weather Station",
  "short_name": "WeatherStn",
  "description": "IoT Environmental Monitor",
  "theme_color": "#0d1117",
  "background_color": "#0d1117",
  "display": "standalone",
  "start_url": "/",
  "icons": [{"src":"/favicon.ico","sizes":"64x64","type":"image/x-icon"}]
}
)rawliteral";

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  GLOBAL OBJECTS & STATE
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
AsyncWebServer  server(80);
AsyncWebSocket  ws("/ws");
WiFiUDP         ntpUDP;
NTPClient       timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_SEC, NTP_INTERVAL);
Adafruit_BME280 bme;

// Live sensor readings
struct SensorData {
  float temp     = 0.0f;  // °C
  float humidity = 0.0f;  // %
  float pressure = 0.0f;  // mmHg
  float altitude = 0.0f;  // metres
};
SensorData live;

// Flags & timers
bool sensorOK      = false;
bool ntpSynced     = false;
unsigned long tSensor   = 0;
unsigned long tBcast    = 0;
unsigned long tWifi     = 0;
unsigned long tHistory  = 0;

// Temperature ring-buffer history
float  tempHist[HISTORY_SIZE];
uint8_t histHead  = 0;
uint8_t histCount = 0;

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  HELPER — FORMATTED TIME (12-hour AM/PM)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
String getTime12h() {
  if (!ntpSynced) return "--:--:-- --";
  int raw = timeClient.getHours();
  int min = timeClient.getMinutes();
  int sec = timeClient.getSeconds();
  const char* ap = (raw >= 12) ? "PM" : "AM";
  int h12 = raw % 12;
  if (h12 == 0) h12 = 12;
  char buf[14];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d %s", h12, min, sec, ap);
  return String(buf);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  HELPER — FORMATTED DATE
//  NOTE: gmtime((time_t*)&epoch) is unreliable on ESP8266 — it corrupts
//        tm_year and returns random huge numbers like 5427055.
//        We use a manual epoch → date calculation instead which works
//        correctly on both ESP32 and ESP8266.
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
String getDate() {
  if (!ntpSynced) return "Syncing…";

  unsigned long epoch = timeClient.getEpochTime();  // UTC+offset already applied
  unsigned long days  = epoch / 86400UL;            // Total days since Jan 1 1970

  // ── Day of week (Jan 1 1970 was a Thursday = index 4) ────────────────
  int dow = (int)((days + 4UL) % 7UL);

  // ── Calculate year from days ──────────────────────────────────────────
  int year = 1970;
  while (true) {
    bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    unsigned int daysInYear = leap ? 366 : 365;
    if (days < daysInYear) break;
    days -= daysInYear;
    year++;
  }

  // ── Calculate month from remaining days ───────────────────────────────
  bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
  int daysInMonth[] = { 31, leap ? 29 : 28, 31, 30, 31, 30,
                        31, 31, 30, 31, 30, 31 };
  int month = 0;
  while (month < 12 && days >= (unsigned long)daysInMonth[month]) {
    days -= daysInMonth[month];
    month++;
  }
  int day = (int)days + 1;   // 1-based day of month

  const char* wd[] = { "Sunday","Monday","Tuesday","Wednesday",
                       "Thursday","Friday","Saturday" };
  const char* mn[] = { "January","February","March","April",
                       "May","June","July","August",
                       "September","October","November","December" };

  char buf[64];
  snprintf(buf, sizeof(buf), "%s, %s %d, %d", wd[dow], mn[month], day, year);
  return String(buf);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  HELPER — BUILD JSON PAYLOAD
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
String buildJSON() {
  String j = "{";
  j += "\"temperature\":"  + String(live.temp,     1) + ",";
  j += "\"humidity\":"     + String(live.humidity,  1) + ",";
  j += "\"pressure\":"     + String(live.pressure,  1) + ",";
  j += "\"altitude\":"     + String(live.altitude,  1) + ",";
  j += "\"time\":\""       + getTime12h()               + "\",";
  j += "\"date\":\""       + getDate()                  + "\",";
  j += "\"uptime\":"       + String(millis() / 1000UL)  + ",";
  j += "\"rssi\":"         + String(WiFi.RSSI())         + ",";
  j += "\"sensorOk\":"     + String(sensorOK ? "true" : "false") + ",";  // String() wrap required for ESP8266
  j += "\"wifiOk\":"       + String(WiFi.status() == WL_CONNECTED ? "true" : "false") + ",";

  // History array (ordered oldest → newest)
  j += "\"history\":[";
  for (uint8_t i = 0; i < histCount; i++) {
    uint8_t idx = (histHead - histCount + i + HISTORY_SIZE) % HISTORY_SIZE;
    if (i) j += ",";
    j += String(tempHist[idx], 1);
  }
  j += "]}";
  return j;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SENSOR — READ BME280
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void readSensor() {
  float t = bme.readTemperature();
  float h = bme.readHumidity();
  float p = bme.readPressure() / 100.0f; // hPa

  // NaN guard — sensor read failed
  if (isnan(t) || isnan(h) || isnan(p) || t == 0.0f) {
    sensorOK = false;
    Serial.println("[WARN] BME280 read failed — check wiring / I2C address");
    return;
  }

  sensorOK         = true;
  live.temp        = t;
  live.humidity    = h;
  live.pressure    = p * 0.750062f;             // hPa → mmHg
  live.altitude    = bme.readAltitude(SEA_LEVEL_HPA);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  HISTORY — ADD TEMPERATURE SAMPLE
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void pushHistory() {
  if (!sensorOK) return;
  tempHist[histHead] = live.temp;
  histHead = (histHead + 1) % HISTORY_SIZE;
  if (histCount < HISTORY_SIZE) histCount++;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  WEBSOCKET EVENT HANDLER
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void onWSEvent(AsyncWebSocket*       server,
               AsyncWebSocketClient* client,
               AwsEventType          type,
               void*                 arg,
               uint8_t*              data,
               size_t                len)
{
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("[WS] Client #%u connected from %s\n",
                    client->id(), client->remoteIP().toString().c_str());
      // Send current snapshot immediately so the browser renders at once
      client->text(buildJSON());
      break;

    case WS_EVT_DISCONNECT:
      Serial.printf("[WS] Client #%u disconnected\n", client->id());
      break;

    case WS_EVT_ERROR:
      Serial.printf("[WS] Client #%u error(%u): %s\n",
                    client->id(), *((uint16_t*)arg), (char*)data);
      break;

    default:
      break;
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  WIFI — CONNECT (blocking at boot, then non-blocking check in loop)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void connectWiFi() {
  Serial.printf("[WiFi] Connecting to '%s' ", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint8_t tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 40) {
    delay(500);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("[WiFi] Connected!");
    Serial.print  ("[WiFi] IP:      "); Serial.println(WiFi.localIP());
    Serial.print  ("[WiFi] RSSI:    "); Serial.print(WiFi.RSSI()); Serial.println(" dBm");
    Serial.print  ("[WiFi] URL:     http://"); Serial.print(DEVICE_HOSTNAME); Serial.println(".local");
  } else {
    Serial.println("\n[WiFi] FAILED — check SSID/password. Device will retry in background.");
  }
}

void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Reconnecting…");
    WiFi.reconnect();
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  OTA — OVER-THE-AIR UPDATE SETUP
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void setupOTA() {
  ArduinoOTA.setHostname(DEVICE_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("[OTA] Start updating " + type);
  });
  ArduinoOTA.onEnd([]()   { Serial.println("\n[OTA] Done!"); });
  ArduinoOTA.onProgress([](unsigned int done, unsigned int total) {
    Serial.printf("[OTA] %u%%\r", done * 100 / total);
  });
  ArduinoOTA.onError([](ota_error_t err) {
    Serial.printf("[OTA] Error[%u]: ", err);
    const char* msgs[] = {"Auth Failed","Begin Failed","Connect Failed","Receive Failed","End Failed"};
    if (err <= 4) Serial.println(msgs[err]);
  });

  ArduinoOTA.begin();
  Serial.println("[OTA] Ready — password: " + String(OTA_PASSWORD));
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SERVER — ROUTE DEFINITIONS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void setupServer() {
  // Attach WebSocket handler
  ws.onEvent(onWSEvent);
  server.addHandler(&ws);

  // ── GET /  →  Dashboard HTML ─────────────────────────────────
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send_P(200, "text/html", INDEX_HTML);
  });

  // ── GET /manifest.json  →  PWA manifest ──────────────────────
  server.on("/manifest.json", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send_P(200, "application/json", MANIFEST_JSON);
  });

  // ── GET /api/data  →  Full JSON snapshot (REST API) ──────────
  server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest* req) {
    AsyncWebServerResponse* res =
      req->beginResponse(200, "application/json", buildJSON());
    res->addHeader("Access-Control-Allow-Origin", "*");
    res->addHeader("Cache-Control", "no-cache");
    req->send(res);
  });

  // ── GET /api/history  →  Temperature history only ────────────
  server.on("/api/history", HTTP_GET, [](AsyncWebServerRequest* req) {
    String j = "[";
    for (uint8_t i = 0; i < histCount; i++) {
      uint8_t idx = (histHead - histCount + i + HISTORY_SIZE) % HISTORY_SIZE;
      if (i) j += ",";
      j += String(tempHist[idx], 1);
    }
    j += "]";
    AsyncWebServerResponse* res =
      req->beginResponse(200, "application/json", j);
    res->addHeader("Access-Control-Allow-Origin", "*");
    req->send(res);
  });

  // ── 404 handler ───────────────────────────────────────────────
  server.onNotFound([](AsyncWebServerRequest* req) {
    req->send(404, "text/plain", "404 Not Found");
  });

  server.begin();
  Serial.println("[HTTP] Server started on port 80");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SETUP
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void setup() {
  Serial.begin(115200);
  delay(400);
  Serial.println();
  Serial.println("╔══════════════════════════════════════╗");
  Serial.println("║   IoT Weather Station v2.0.0          ║");
  Serial.println("╚══════════════════════════════════════╝");

  // ── I2C + BME280 ────────────────────────────────────────────────
#ifdef ESP32
  Wire.begin();           // SDA=21, SCL=22
#else
  Wire.begin(4, 5);       // D2=SDA, D1=SCL on NodeMCU/D1 Mini
#endif

  if (bme.begin(BME280_I2C_ADDR)) {
    sensorOK = true;
    // Weather-station sampling mode: longer intervals, less power
    bme.setSampling(
      Adafruit_BME280::MODE_NORMAL,
      Adafruit_BME280::SAMPLING_X2,   // Temperature
      Adafruit_BME280::SAMPLING_X16,  // Pressure
      Adafruit_BME280::SAMPLING_X1,   // Humidity
      Adafruit_BME280::FILTER_X16,
      Adafruit_BME280::STANDBY_MS_500
    );
    Serial.println("[BME280] Sensor found at 0x" + String(BME280_I2C_ADDR, HEX));
  } else {
    sensorOK = false;
    Serial.println("[BME280] *** NOT FOUND *** — check wiring & I2C address");
    Serial.println("[BME280]   Try: #define BME280_I2C_ADDR 0x77");
  }

  // ── WiFi ─────────────────────────────────────────────────────────
  connectWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    // ── mDNS ───────────────────────────────────────────────────────
    if (MDNS.begin(DEVICE_HOSTNAME)) {
      MDNS.addService("http", "tcp", 80);
      Serial.println("[mDNS]  http://" + String(DEVICE_HOSTNAME) + ".local");
    }

    // ── NTP ────────────────────────────────────────────────────────
    timeClient.begin();
    timeClient.update();
    ntpSynced = (timeClient.getEpochTime() > 1000000UL);
    Serial.println("[NTP]   " + (ntpSynced ? getTime12h() + "  " + getDate() : "Not synced yet"));

    // ── OTA ────────────────────────────────────────────────────────
    setupOTA();
  }

  // ── Web server ────────────────────────────────────────────────────
  setupServer();

  // ── Initial sensor read & history seed ───────────────────────────
  readSensor();
  if (sensorOK) pushHistory();

  Serial.println();
  Serial.println("[READY] Dashboard: http://" + WiFi.localIP().toString());
  Serial.println("[READY] REST API : http://" + WiFi.localIP().toString() + "/api/data");
  Serial.println("[READY] OTA      : upload to " + String(DEVICE_HOSTNAME) + ".local");
  Serial.println("[READY] ────────────────────────────────────");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  LOOP  — non-blocking, timer-driven
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void loop() {
  unsigned long now = millis();

  // ── OTA handle (must be called every loop) ────────────────────────
  ArduinoOTA.handle();

  // ── mDNS update (ESP8266 only) ────────────────────────────────────
#ifdef ESP8266
  MDNS.update();
#endif

  // ── WebSocket cleanup (frees disconnected client memory) ──────────
  ws.cleanupClients();

  // ── NTP update ────────────────────────────────────────────────────
  if (WiFi.status() == WL_CONNECTED) {
    if (timeClient.update()) {
      if (!ntpSynced) {
        ntpSynced = true;
        Serial.println("[NTP] Synced: " + getTime12h());
      }
    }
  }

  // ── WiFi health check ─────────────────────────────────────────────
  if (now - tWifi >= WIFI_CHECK_INTERVAL) {
    tWifi = now;
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[WiFi] Lost connection — reconnecting…");
      checkWiFi();
    }
  }

  // ── Read sensor ───────────────────────────────────────────────────
  if (now - tSensor >= SENSOR_INTERVAL) {
    tSensor = now;
    readSensor();

    if (sensorOK) {
      Serial.printf("[DATA] T:%.1f°C  H:%.1f%%  P:%.1fmmHg  Alt:%.1fm  RSSI:%ddBm\n",
                    live.temp, live.humidity, live.pressure, live.altitude,
                    WiFi.RSSI());
    }
  }

  // ── Log history sample ────────────────────────────────────────────
  if (now - tHistory >= HISTORY_RECORD_INTERVAL) {
    tHistory = now;
    pushHistory();
  }

  // ── Broadcast data to all WebSocket clients ───────────────────────
  if (now - tBcast >= BROADCAST_INTERVAL) {
    tBcast = now;
    if (ws.count() > 0) {          // Only build+send if someone is watching
      ws.textAll(buildJSON());
    }
  }
}

/*
 * ─────────────────────────────────────────────────────────────────────────
 *  REST API REFERENCE
 * ─────────────────────────────────────────────────────────────────────────
 *
 *  GET http://<IP>/api/data
 *  ┌─────────────────────────────────────────────────────────────────────┐
 *  │ {                                                                   │
 *  │   "temperature": 26.5,    // °C                                     │
 *  │   "humidity": 72.3,       // %                                      │
 *  │   "pressure": 756.2,      // mmHg                                   │
 *  │   "altitude": 8.5,        // metres                                 │
 *  │   "time": "10:30:45 AM",  // 12-hour format                         │
 *  │   "date": "Sunday, May 17, 2026",                                   │
 *  │   "uptime": 3723,         // seconds since boot                     │
 *  │   "rssi": -65,            // dBm                                    │
 *  │   "sensorOk": true,                                                 │
 *  │   "wifiOk": true,                                                   │
 *  │   "history": [25.1, 25.3, 25.5, 26.0, ...]  // temp ring-buffer    │
 *  │ }                                                                   │
 *  └─────────────────────────────────────────────────────────────────────┘
 *
 *  GET http://<IP>/api/history   →  temperature array only (JSON)
 *  GET http://<IP>/manifest.json →  PWA manifest
 *
 *  WebSocket: ws://<IP>/ws
 *    Pushes the same JSON payload every 1 second to all connected clients.
 *    On first connect the current snapshot is sent immediately.
 *
 * ─────────────────────────────────────────────────────────────────────────
 *  DASHBOARD PREVIEW  (what you'll see in the browser)
 * ─────────────────────────────────────────────────────────────────────────
 *
 *  ╔══════════════════════════════════════════════════════════════════════╗
 *  ║  🌤 Weather Station          ● Online  ● WiFi  ⚙ 🌙              ║
 *  ╠══════════════════════════════════════════════════════════════════════╣
 *  ║                                                                     ║
 *  ║               10:30:45 PM                                           ║
 *  ║          Sunday, May 17, 2026                                       ║
 *  ║                                                                     ║
 *  ╠══════════════════╦══════════════════════════════════════════════════╣
 *  ║ 🌡 TEMPERATURE  ║ 💧 HUMIDITY                                      ║
 *  ║  26.5 °C         ║  72.3 %                                          ║
 *  ║  Feels like 28.1 ║  ✅ Ideal Humidity                               ║
 *  ║  ████░░░░░░░░    ║  ████████████░░                                  ║
 *  ║  [sparkline ~~]  ║  [drop visual ▃▄▅▆▇]                            ║
 *  ╠══════════╦═══════╩════════════╦═══════════════════════════════════  ║
 *  ║🔵 PRES. ║ ⛰ ALTITUDE        ║ 📶 WIFI         ║ ⏱ UPTIME        ║
 *  ║ 756.2   ║  8.5 m             ║  ▂▄▆█ -65dBm    ║  2h 15m 30s     ║
 *  ║ mmHg    ║  Est. sea level    ║  Excellent       ║  Since boot      ║
 *  ╚══════════╩════════════════════╩═════════════════╩══════════════════╝
 *  ● Live · Last updated: 10:30:45 PM · ESP Weather Station v2.0 · API
 * ─────────────────────────────────────────────────────────────────────────
 */
