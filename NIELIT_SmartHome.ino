/*********************************************************************
  NIELIT ROPAR SMART LAB – ESP8266 SMART HOME
  Firmware v3.0.O | Production Ready | © NIELIT ROPAR 2025
  
  CHANGELOG v3.0.O:
  - Fixed OTA Update page loading issue (PROGMEM string handling)
  - Fixed Factory Reset authentication and execution
  - Improved code documentation and readability
  - Enhanced error handling for web endpoints
  
  DEFAULT CREDENTIALS:
  - Username: admin
  - Password: rccrcc
  - AP Password: nielit2025
  
  FEATURES:
  ✓ 2 Relay Control (Web + Physical Buttons)
  ✓ DHT11 Temperature & Humidity Sensor
  ✓ WiFi Manager with Auto-Connect
  ✓ OTA Firmware Updates
  ✓ Factory Reset (Clear WiFi + Settings)
  ✓ System Logs & Monitoring
  ✓ Rate Limiting (20 toggles/min per relay)
  ✓ EEPROM State Persistence
  ✓ Watchdog Timer Protection
  ✓ Memory Management & Auto-Recovery
*********************************************************************/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <DHT.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPUpdateServer.h>

// ==================== FIRMWARE CONFIGURATION ====================
#define FIRMWARE_VERSION "3.0.0"
#define COPYRIGHT_NOTICE "© NIELIT ROPAR 2025"
#define DEVICE_NAME      "NIELIT-SmartLab"
#define AP_PASSWORD      "nielit2025"         // Access Point password when in setup mode
#define ADMIN_USER       "admin"              // Web interface username
#define ADMIN_PASS       "rccrcc"             // Web interface password

// ==================== HARDWARE PIN MAPPING ====================
// Relay outputs (ACTIVE-HIGH: HIGH = ON, LOW = OFF)
const int RELAY1  = 5;   // D1 – Connected to Relay Module Channel 1
const int RELAY2  = 4;   // D2 – Connected to Relay Module Channel 2

// LED indicators (mirror relay states)
const int LED1    = 13;  // D7 – Status LED for Relay 1
const int LED2    = 15;  // D8 – Status LED for Relay 2

// Physical control buttons (ACTIVE-LOW with internal pullup)
const int BUTTON1 = 3;   // D9 (RX) – Toggle Relay 1
const int BUTTON2 = 1;   // D10 (TX) – Toggle Relay 2

// Additional peripherals
const int BUZZER  = 0;   // D3 – Audio feedback buzzer
const int DHT_PIN = 14;  // D5 – DHT11 sensor data pin

// DHT sensor configuration
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// ==================== SYSTEM CONFIGURATION ====================
#define EEPROM_SIZE              512    // Total EEPROM storage size
#define EEPROM_MAGIC             0xAA   // Magic byte to verify valid EEPROM data
#define LOG_BUFFER_SIZE          20     // Number of log entries to keep in memory
#define DEBOUNCE_DELAY           50     // Button debounce time in milliseconds
#define DHT_UPDATE_INTERVAL      5000   // DHT sensor read interval (5 seconds)
#define HEAP_CHECK_INTERVAL      30000  // Memory check interval (30 seconds)
#define MIN_FREE_HEAP            8192   // Minimum free heap before auto-restart (8KB)
#define MAX_TOGGLES_PER_MIN      20     // Rate limit: max relay toggles per minute
#define TOGGLE_RESET_INTERVAL    60000  // Reset toggle counter every 60 seconds

// ==================== GLOBAL OBJECTS ====================
ESP8266WebServer server(80);                    // Web server on port 80
ESP8266HTTPUpdateServer httpUpdater;            // OTA update handler
WiFiManager wifiManager;                        // WiFi connection manager

// ==================== DEVICE STATE STRUCTURE ====================
// This structure is saved to EEPROM for persistence across reboots
struct DeviceState {
  bool relay1;                    // Relay 1 state (true = ON, false = OFF)
  bool relay2;                    // Relay 2 state (true = ON, false = OFF)
  float temperature;              // Last DHT temperature reading (°C)
  float humidity;                 // Last DHT humidity reading (%)
  int relay1Toggles;              // Toggle counter for rate limiting
  int relay2Toggles;              // Toggle counter for rate limiting
  unsigned long lastToggleReset;  // Timestamp of last counter reset
  char deviceName[32];            // Custom device name
  uint8_t magic;                  // Validation byte (must be EEPROM_MAGIC)
} state = {false, false, 0.0, 0.0, 0, 0, 0, DEVICE_NAME, EEPROM_MAGIC};

// ==================== LOGGING SYSTEM ====================
// Circular buffer for storing recent system events
String logBuffer[LOG_BUFFER_SIZE];
int logIndex = 0;

/**
 * Log a system event with timestamp
 * Events are stored in a circular buffer and also printed to serial
 * @param event: Description of the event to log
 */
void logEvent(const String& event) {
  String entry = String(millis() / 1000) + "s: " + event;
  logBuffer[logIndex] = entry;
  logIndex = (logIndex + 1) % LOG_BUFFER_SIZE;  // Circular buffer wrap-around
  Serial.println(entry);
}

/*********************************************************************
  EEPROM MANAGEMENT FUNCTIONS
  - Handles persistent storage of device state across power cycles
  - Uses magic byte validation to detect uninitialized EEPROM
*********************************************************************/

/**
 * Save current device state to EEPROM
 * Called after any state change that should persist across reboots
 */
void saveState() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(0, state);           // Write entire state struct to address 0
  EEPROM.commit();                // Commit changes to flash memory
  EEPROM.end();
  logEvent("State saved to EEPROM");
}

/**
 * Load device state from EEPROM
 * @return: true if valid state was loaded, false if EEPROM is empty/invalid
 */
bool loadState() {
  EEPROM.begin(EEPROM_SIZE);
  DeviceState loaded;
  EEPROM.get(0, loaded);          // Read state struct from address 0
  EEPROM.end();
  
  // Validate magic byte to ensure EEPROM contains valid data
  if (loaded.magic == EEPROM_MAGIC) {
    state = loaded;
    logEvent("State loaded from EEPROM");
    return true;
  }
  
  logEvent("EEPROM invalid or empty - using defaults");
  return false;
}

/**
 * Clear all EEPROM data (used for factory reset)
 * Writes zeros to entire EEPROM space
 */
void resetEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
  logEvent("EEPROM cleared");
}

/*********************************************************************
  BUZZER CONTROL FUNCTIONS
  - Provides audio feedback for user interactions and system events
*********************************************************************/

/**
 * Single beep (short duration)
 * @param ms: Beep duration in milliseconds (default 50ms)
 */
void beepStart(int ms = 50) {
  digitalWrite(BUZZER, HIGH);
  delay(ms);
  digitalWrite(BUZZER, LOW);
}

/**
 * Multiple beeps in sequence (pattern indication)
 * @param count: Number of beeps to play
 */
void beepPattern(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(30);
    digitalWrite(BUZZER, LOW);
    if (i < count - 1) delay(100);  // Pause between beeps (except after last)
  }
}

/*********************************************************************
  RELAY CONTROL FUNCTIONS
  - Manages relay states with rate limiting and persistence
  - ACTIVE-HIGH logic: HIGH = relay ON, LOW = relay OFF
*********************************************************************/

/**
 * Set relay state with rate limiting and optional persistence
 * @param relay: Relay number (1 or 2)
 * @param on: Desired state (true = ON, false = OFF)
 * @param save: Whether to save state to EEPROM (default true)
 * @return: true if successful, false if rate limited
 */
bool setRelay(int relay, bool on, bool save = true) {
  // Reset toggle counter every minute to prevent permanent lockout
  if (millis() - state.lastToggleReset > TOGGLE_RESET_INTERVAL) {
    state.relay1Toggles = 0;
    state.relay2Toggles = 0;
    state.lastToggleReset = millis();
  }

  // Handle Relay 1
  if (relay == 1) {
    // Rate limiting check
    if (state.relay1Toggles >= MAX_TOGGLES_PER_MIN) {
      logEvent("Relay1 rate limit hit (max 20/min)");
      return false;
    }
    
    // Update hardware outputs
    digitalWrite(RELAY1, on ? HIGH : LOW);  // Set relay state
    digitalWrite(LED1, on ? HIGH : LOW);    // Mirror state on LED
    
    // Update software state
    state.relay1 = on;
    state.relay1Toggles++;
    logEvent("Relay1 → " + String(on ? "ON" : "OFF"));
  } 
  // Handle Relay 2
  else if (relay == 2) {
    // Rate limiting check
    if (state.relay2Toggles >= MAX_TOGGLES_PER_MIN) {
      logEvent("Relay2 rate limit hit (max 20/min)");
      return false;
    }
    
    // Update hardware outputs
    digitalWrite(RELAY2, on ? HIGH : LOW);
    digitalWrite(LED2, on ? HIGH : LOW);
    
    // Update software state
    state.relay2 = on;
    state.relay2Toggles++;
    logEvent("Relay2 → " + String(on ? "ON" : "OFF"));
  } 
  else {
    return false;  // Invalid relay number
  }

  // Persist state to EEPROM if requested
  if (save) saveState();
  
  // Audio feedback
  beepStart();
  
  return true;
}

/*********************************************************************
  DHT SENSOR FUNCTIONS
  - Reads temperature and humidity from DHT11 sensor
  - Validates readings before updating state
*********************************************************************/

/**
 * Read DHT sensor and update state variables
 * Includes validation to reject invalid/out-of-range readings
 */
void updateDHT() {
  float t = dht.readTemperature();  // Read temperature in Celsius
  float h = dht.readHumidity();     // Read relative humidity
  
  // Validate readings (DHT11 range: -40°C to 80°C, 0% to 100% RH)
  if (!isnan(t) && !isnan(h) && t > -40 && t < 80 && h >= 0 && h <= 100) {
    state.temperature = t;
    state.humidity = h;
  } else {
    logEvent("DHT read error - invalid data");
  }
}

/*********************************************************************
  WEB PAGES (STORED IN FLASH MEMORY)
  - HTML/CSS/JavaScript stored in PROGMEM to save RAM
  - Modern responsive design with real-time updates
*********************************************************************/

/**
 * Main dashboard page
 * Features: Relay control, sensor display, system info, factory reset
 */
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang="en"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>NIELIT Ropar Smart Lab</title>
<style>
:root{
  --bg1:#0f172a;--bg2:#1e3a8a;--accent:#3b82f6;--on:#10b981;--off:#ef4444;--danger:#dc2626;
  --glass:rgba(255,255,255,0.12);--blur:10px;
}
*{margin:0;padding:0;box-sizing:border-box}
body{
  font-family:-apple-system,system-ui,Segoe UI,Roboto,sans-serif;
  background:linear-gradient(135deg,var(--bg1),var(--bg2));
  color:#f9fafb;min-height:100vh;padding:20px;
  display:flex;justify-content:center;align-items:flex-start;
}
.container{
  width:100%;max-width:850px;backdrop-filter:blur(var(--blur));
}
.card{
  background:var(--glass);border:1px solid rgba(255,255,255,0.1);
  border-radius:20px;padding:25px;margin-bottom:25px;
  box-shadow:0 8px 32px rgba(0,0,0,0.3);
  transition:transform .3s ease, box-shadow .3s ease;
}
.card:hover{transform:translateY(-3px);box-shadow:0 10px 40px rgba(0,0,0,0.4)}
h1{
  font-size:34px;text-align:center;font-weight:700;
  margin-bottom:5px;text-shadow:2px 2px 6px rgba(0,0,0,0.4);
}
.version{
  font-size:13px;text-align:center;opacity:.9;margin-bottom:25px;color:#cbd5e1;
}
h2{font-size:22px;margin-bottom:15px;text-transform:uppercase;letter-spacing:1px;color:#93c5fd}
.relay-control{display:flex;flex-direction:column;gap:15px}
.relay-box{
  display:flex;justify-content:space-between;align-items:center;
  padding:15px 20px;background:rgba(255,255,255,0.1);border-radius:14px;
  transition:background .2s;
}
.relay-box:hover{background:rgba(255,255,255,0.18)}
.relay-name{font-weight:600;font-size:18px}
.status-badge{
  padding:4px 12px;border-radius:20px;font-size:12px;font-weight:600;text-transform:uppercase;
}
.status-on{background:var(--on);color:#fff}
.status-off{background:var(--off);color:#fff}
.btn{
  padding:9px 18px;border:none;border-radius:10px;font-weight:600;
  cursor:pointer;color:#fff;transition:.25s;
  text-decoration:none;display:inline-block;
}
.btn-on{background:var(--on)}.btn-on:hover{background:#059669;transform:translateY(-2px)}
.btn-off{background:var(--off)}.btn-off:hover{background:#dc2626;transform:translateY(-2px)}
.btn-danger{background:var(--danger)}.btn-danger:hover{background:#b91c1c;transform:translateY(-2px)}
.relay-buttons{display:flex;gap:10px}
.sensor-grid{
  display:grid;grid-template-columns:repeat(auto-fit,minmax(160px,1fr));gap:20px;
}
.sensor-item{
  background:rgba(255,255,255,0.1);border-radius:15px;padding:20px;
  text-align:center;color:#fff;transition:transform .3s ease;
}
.sensor-item:hover{transform:scale(1.05)}
.sensor-label{font-size:13px;opacity:.85;text-transform:uppercase;letter-spacing:1px}
.sensor-value{font-size:30px;font-weight:700;margin-top:5px;color:#93c5fd}
.info-item{
  padding:12px 15px;background:rgba(255,255,255,0.1);
  border-radius:12px;margin:8px 0;display:flex;justify-content:space-between;align-items:center;
}
.info-label{font-weight:600;color:#cbd5e1;font-size:12px;text-transform:uppercase}
footer{
  text-align:center;margin-top:25px;font-size:13px;opacity:.8;color:#e2e8f0;
}
footer a{color:#93c5fd;text-decoration:none;font-weight:600}
footer a:hover{text-decoration:underline}
@keyframes blink{50%{opacity:0.4}}
#s{animation:blink 1.5s infinite;color:var(--on)}
</style></head>
<body><div class="container">
<h1>NIELIT Ropar Smart Lab</h1>
<div class="version">Firmware v<span id="v"></span> • <span id="s">●</span></div>

<div class="card">
  <h2>Relay Control</h2>
  <div class="relay-control">
    <div id="r1" class="relay-box"></div>
    <div id="r2" class="relay-box"></div>
  </div>
</div>

<div class="card">
  <h2>Environment Sensors</h2>
  <div class="sensor-grid">
    <div class="sensor-item">
      <div class="sensor-label">Temperature</div>
      <div class="sensor-value"><span id="t">--</span>°C</div>
    </div>
    <div class="sensor-item">
      <div class="sensor-label">Humidity</div>
      <div class="sensor-value"><span id="h">--</span>%</div>
    </div>
  </div>
</div>

<div class="card">
  <h2>System Info</h2>
  <div class="info-item"><div class="info-label">Uptime</div><div id="u">--</div></div>
  <div class="info-item"><div class="info-label">IP Address</div><div id="ip">--</div></div>
  <div class="info-item"><div class="info-label">Free Heap</div><div id="heap">--</div></div>
   <div class="info-item">
    <div class="info-label">Local URL</div>
    <div><a href="http://nielit-ropar.local/" target="_blank" style="color:#3b82f6;">http://nielit-ropar.local/</a></div>
  </div>
</div>

<div class="card">
  <h2>Factory Reset</h2>
  <button onclick="factoryReset()" class="btn btn-danger" style="width:100%;padding:14px;font-size:16px;">
    Factory Reset (Erase Wi-Fi & Settings)
  </button>
  <p style="font-size:12px;margin-top:10px;opacity:0.8;">
    ⚠️ Requires password: <strong>admin / ******</strong>
  </p>
</div>

<footer>© NIELIT Ropar 2025 • <a href="/update">OTA Update</a> • <a href="/logs">System Logs</a></footer>
</div>

<script>
// Format milliseconds to human-readable uptime
function fmt(ms){
  let s=ms/1000,d=Math.floor(s/86400),h=Math.floor((s%86400)/3600),m=Math.floor((s%3600)/60);
  return d>0?`${d}d ${h}h ${m}m`:`${h}h ${m}m`;
}

// Update dashboard with latest system status
function update(){
  fetch('/api/status').then(r=>r.json()).then(d=>{
    document.getElementById('v').textContent=d.version;
    document.getElementById('s').textContent='Online';
    document.getElementById('s').style.color='#10b981';
    
    // Update relay controls
    ['1','2'].forEach(n=>{
      let on=d[`r${n}`];
      let statusClass=on?'on':'off';
      let statusText=on?'ON':'OFF';
      let btnText=on?'Turn OFF':'Turn ON';
      let btnClass=on?'btn-off':'btn-on';
      document.getElementById(`r${n}`).innerHTML=`
        <div class="relay-name">Relay ${n}</div>
        <div class="relay-buttons">
          <span class="status-badge status-${statusClass}">${statusText}</span>
          <a href="/api/relay${n}/${on?'off':'on'}" class="btn ${btnClass}">${btnText}</a>
        </div>`;
    });
    
    // Update sensor readings
    document.getElementById('t').textContent=d.temp.toFixed(1);
    document.getElementById('h').textContent=d.hum.toFixed(1);
    document.getElementById('u').textContent=fmt(d.uptime);
    document.getElementById('ip').textContent=d.ip;
    document.getElementById('heap').textContent=(d.heap/1024).toFixed(1)+' KB';
  }).catch(e=>console.error('Status fetch error:',e));
}

// Factory reset with password authentication
function factoryReset() {
  if (!confirm("⚠️ FACTORY RESET WARNING\n\nThis will:\n• Erase all Wi-Fi settings\n• Clear all saved states\n• Reboot the device\n\nContinue?")) return;
  
  const user = prompt("Username:", "admin");
  if (!user) return;
  const pass = prompt("Password:", "");
  if (!pass) return;

  fetch('/api/factory-reset', {
    method: 'GET',
    headers: {
      'Authorization': 'Basic ' + btoa(user + ':' + pass)
    }
  }).then(r => {
    if (r.ok) {
      alert("✅ Factory reset initiated. Device rebooting...");
      setTimeout(() => location.reload(), 5000);
    } else {
      alert("❌ Failed: Wrong password or server error.");
    }
  }).catch(() => alert("❌ Network error."));
}

// Auto-update every 2 seconds
update(); 
setInterval(update,2000);
</script>
</body></html>
)rawliteral";

/**
 * OTA Firmware Update Page
 * Allows uploading new firmware .bin files with password protection
 * NOTE: Uses %FIRMWARE_VERSION% placeholder replaced at runtime
 */
const char UPDATE_PAGE[] PROGMEM = R"=====(<!DOCTYPE html><html><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>OTA Update – NIELIT ROPAR</title><style>body{font-family:sans-serif;background:#1e40af;color:#fff;display:flex;align-items:center;justify-content:center;min-height:100vh;padding:20px}
.card{background:#fff;color:#1f2937;border-radius:16px;padding:30px;max-width:420px;width:100%;box-shadow:0 12px 40px rgba(0,0,0,.15);text-align:center}
h1{color:#1e40af}input,button{width:100%;padding:12px;margin:10px 0;border-radius:8px;border:2px solid #e5e7eb}
input:focus{border-color:#1e40af;outline:none}button{background:#10b981;color:#fff;border:none;font-weight:600;cursor:pointer}
button:disabled{background:#6b7280}progress{width:100%;height:12px;border-radius:6px;margin:20px 0;display:none}
footer{margin-top:20px;font-size:12px;color:#666}
</style></head><body><div class="card"><h1>OTA Update</h1><p>Current: <strong>%FIRMWARE_VERSION%</strong></p>
<form id="f" enctype="multipart/form-data"><input type="password" id="p" placeholder="Password (******)" required>
<input type="file" id="file" accept=".bin" required style="display:none"><label for="file" style="background:#1e40af;color:#fff;padding:14px;border-radius:10px;cursor:pointer;display:block">Choose .bin File</label>
<div id="n" style="margin:8px 0;color:#555">No file selected</div><progress id="pr"></progress>
<button type="submit" id="b" disabled>Update Firmware</button></form><div id="m"></div>
<footer>© NIELIT ROPAR 2025 • <a href="/" style="color:#1e40af">Back to Dashboard</a></footer>
<script>
const fi=document.getElementById('file'),lb=document.querySelector('label[for=file]'),fn=document.getElementById('n'),
b=document.getElementById('b'),pr=document.getElementById('pr'),m=document.getElementById('m'),pwd=document.getElementById('p');

// File selection handler
fi.addEventListener('change',()=>{const f=fi.files[0];if(f){if(!f.name.endsWith('.bin')){m.textContent='⚠️ Only .bin files allowed';m.style.color='#dc2626';b.disabled=true;return;}
fn.textContent=f.name;lb.textContent='✓ Change File';b.disabled=!pwd.value;m.textContent='';}else{fn.textContent='No file selected';lb.textContent='Choose .bin File';b.disabled=true;}});

// Password input handler
pwd.addEventListener('input',()=>{b.disabled=!(fi.files[0] && pwd.value);});

// Form submission handler
document.getElementById('f').addEventListener('submit',e=>{e.preventDefault();const file=fi.files[0];if(!file||!pwd.value)return;
const x=new XMLHttpRequest(),fd=new FormData();fd.append('update',file);pr.style.display='block';pr.value=0;b.disabled=true;b.textContent='Uploading...';m.textContent='';

// Upload progress tracking
x.upload.addEventListener('progress',ev=>{if(ev.lengthComputable){pr.value=(ev.loaded/ev.total)*100;m.textContent=Math.round(pr.value)+'%';}});

// Upload completion handler
x.addEventListener('load',()=>{if(x.status===200){m.textContent='✅ Success! Rebooting...';m.style.color='#10b981';setTimeout(()=>{location.href='/'},3000);}
else{m.textContent='❌ Upload failed: '+x.statusText;m.style.color='#dc2626';b.disabled=false;b.textContent='Retry';pr.style.display='none';}});

// Network error handler
x.addEventListener('error',()=>{m.textContent='❌ Network error';m.style.color='#dc2626';b.disabled=false;b.textContent='Retry';pr.style.display='none';});

// Send upload request with authentication
x.open('POST','/update');x.setRequestHeader('Authorization','Basic '+btoa('admin:'+pwd.value));x.send(fd);
});
</script></body></html>)=====";

/**
 * System Logs Page
 * Displays recent system events with auto-refresh
 */
const char LOGS_PAGE[] PROGMEM = R"=====(<!DOCTYPE html><html><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Logs – NIELIT ROPAR</title><style>body{font-family:monospace;background:#1f2937;color:#e5e7eb;padding:20px}
.container{max-width:900px;margin:auto;background:#111827;padding:20px;border-radius:10px}h1{color:#10b981}
.log{padding:8px 12px;margin:5px 0;background:#1f2937;border-left:3px solid #1e40af;border-radius:4px}
.btn{background:#1e40af;color:#fff;padding:8px 16px;border:none;border-radius:6px;margin:5px;cursor:pointer}
footer{text-align:center;margin-top:20px;font-size:12px;color:#9ca3af}
</style></head><body><div class="container"><h1>System Logs</h1>
<a href="/" class="btn">🏠 Home</a><button onclick="refresh()" class="btn">🔄 Refresh</button>
<div id="logs">Loading...</div></div>
<footer>© NIELIT ROPAR 2025 • Auto-refresh every 3 seconds</footer>
<script>
function refresh(){fetch('/api/logs').then(r=>r.json()).then(d=>{document.getElementById('logs').innerHTML=
d.logs.map(l=>`<div class="log">${l||'--'}</div>`).join('');}).catch(e=>console.error('Log fetch error:',e));}
refresh();setInterval(refresh,3000);
</script></body></html>)=====";

/*********************************************************************
  API ENDPOINT HANDLERS
  - RESTful JSON API for system status and control
*********************************************************************/

/**
 * GET /api/status
 * Returns current system status as JSON
 */
void handleStatus() {
  StaticJsonDocument<512> doc;
  doc["version"] = FIRMWARE_VERSION;
  doc["r1"] = state.relay1;
  doc["r2"] = state.relay2;
  doc["temp"] = round(state.temperature * 10) / 10.0;  // Round to 1 decimal
  doc["hum"] = round(state.humidity * 10) / 10.0;
  doc["uptime"] = millis();
  doc["ip"] = WiFi.localIP().toString();
  doc["heap"] = ESP.getFreeHeap();
  
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

/**
 * GET /api/logs
 * Returns system log buffer as JSON array
 */
void handleLogs() {
  StaticJsonDocument<2048> doc;
  JsonArray arr = doc.createNestedArray("logs");
  
  // Add logs in chronological order (oldest first)
  for (int i = 0; i < LOG_BUFFER_SIZE; i++) {
    int idx = (logIndex + i) % LOG_BUFFER_SIZE;
    arr.add(logBuffer[idx]);
  }
  
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

/**
 * Handle relay control endpoints
 * @param r: Relay number (1 or 2)
 * @param on: Desired state (true = ON, false = OFF)
 * Redirects to home page on success, returns 429 if rate limited
 */
void handleRelay(int r, bool on) {
  if (setRelay(r, on)) {
    // Success - redirect to dashboard
    server.sendHeader("Location", "/");
    server.send(303);  // 303 See Other redirect
  } else {
    // Rate limit exceeded
    server.send(429, "text/plain", "Rate limit exceeded: Max 20 toggles/min");
  }
}

/**
 * GET /api/reset
 * Soft reset: Clear EEPROM and restart device
 * Does NOT clear WiFi credentials
 */
void handleReset() {
  server.send(200, "text/html", "<h2>Resetting device... © NIELIT ROPAR</h2>");
  delay(1000);
  resetEEPROM();
  delay(500);
  ESP.restart();
}

/**
 * GET /api/factory-reset
 * Factory reset: Clear EEPROM + WiFi credentials, restart into AP mode
 * Requires HTTP Basic Authentication (admin/rccrcc)
 */
void handleWebFactoryReset() {
  // Check HTTP Basic Authentication
  if (!server.authenticate(ADMIN_USER, ADMIN_PASS)) {
    server.requestAuthentication(BASIC_AUTH, "NIELIT Smart Lab", "Authentication required");
    logEvent("Factory reset: Authentication failed from " + server.client().remoteIP().toString());
    return;
  }
  
  logEvent("Factory reset initiated by authenticated user");
  
  // Audio feedback - 5 beeps to indicate serious action
  beepPattern(5);
  
  // Send response BEFORE restarting (critical!)
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", 
    "{\"status\":\"success\",\"message\":\"Factory reset complete. Rebooting into AP mode...\"}");
  
  // Allow time for response to be transmitted
  delay(500);
  
  // Clear EEPROM data
  resetEEPROM();
  delay(200);
  
  // Clear WiFi Manager stored credentials
  wifiManager.resetSettings();
  logEvent("WiFi credentials cleared");
  delay(200);
  
  // Final beep and restart
  beepStart(100);
  delay(500);
  
  // Device will restart in AP mode for reconfiguration
  ESP.restart();
}

/**
 * 404 Not Found handler
 */
void handleNotFound() {
  server.send(404, "text/plain", "404 Not Found – NIELIT ROPAR");
}

/*********************************************************************
  WEB SERVER SETUP
  - Configures all HTTP routes and starts server
  - Sets up OTA update handler with authentication
*********************************************************************/
void setupWebServer() {
  // ===== HTML PAGE ROUTES =====
  
  // Main dashboard
  server.on("/", []() {
    server.send_P(200, "text/html", HTML_PAGE);
  });
  
  // OTA Update page (with version replacement)
  server.on("/update", HTTP_GET, []() {
    String page = FPSTR(UPDATE_PAGE);  // Read from PROGMEM to String
    page.replace("%FIRMWARE_VERSION%", FIRMWARE_VERSION);  // Replace placeholder
    server.send(200, "text/html", page);
  });
  
  // System logs page
  server.on("/logs", []() {
    server.send_P(200, "text/html", LOGS_PAGE);
  });
  
  // ===== JSON API ROUTES =====
  
  server.on("/api/status", handleStatus);
  server.on("/api/logs", handleLogs);
  server.on("/api/relay1/on", []() { handleRelay(1, true); });
  server.on("/api/relay1/off", []() { handleRelay(1, false); });
  server.on("/api/relay2/on", []() { handleRelay(2, true); });
  server.on("/api/relay2/off", []() { handleRelay(2, false); });
  server.on("/api/reset", handleReset);
  server.on("/api/factory-reset", HTTP_GET, handleWebFactoryReset);

  // ===== OTA UPDATE HANDLER =====
  // Handles POST requests to /update with authentication
  httpUpdater.setup(&server, "/update", ADMIN_USER, ADMIN_PASS);
  
  // ===== 404 HANDLER =====
  server.onNotFound(handleNotFound);
  
  // Start HTTP server
  server.begin();
  logEvent("Web server started on port 80");
}

/*********************************************************************
  PHYSICAL BUTTON HANDLING
  - Monitors two physical buttons with proper debouncing
  - Buttons are ACTIVE-LOW with internal pull-up resistors
  - Each button toggles its corresponding relay
*********************************************************************/
void handleButtons() {
  // Static variables persist between function calls
  static uint8_t button1State = HIGH, button2State = HIGH;
  static uint8_t lastButton1Reading = HIGH, lastButton2Reading = HIGH;
  static unsigned long lastDebounceTime1 = 0, lastDebounceTime2 = 0;
  
  // Read current button states
  uint8_t reading1 = digitalRead(BUTTON1);
  uint8_t reading2 = digitalRead(BUTTON2);
  unsigned long currentTime = millis();

  // ===== BUTTON 1 DEBOUNCING =====
  // If reading changed, reset debounce timer
  if (reading1 != lastButton1Reading) {
    lastDebounceTime1 = currentTime;
  }
  
  // Only process if stable for DEBOUNCE_DELAY milliseconds
  if ((currentTime - lastDebounceTime1) > DEBOUNCE_DELAY) {
    if (reading1 != button1State) {
      button1State = reading1;
      // Trigger on button press (HIGH -> LOW transition)
      if (button1State == LOW) {
        setRelay(1, !state.relay1);  // Toggle relay 1
      }
    }
  }
  lastButton1Reading = reading1;

  // ===== BUTTON 2 DEBOUNCING =====
  if (reading2 != lastButton2Reading) {
    lastDebounceTime2 = currentTime;
  }
  
  if ((currentTime - lastDebounceTime2) > DEBOUNCE_DELAY) {
    if (reading2 != button2State) {
      button2State = reading2;
      // Trigger on button press (HIGH -> LOW transition)
      if (button2State == LOW) {
        setRelay(2, !state.relay2);  // Toggle relay 2
      }
    }
  }
  lastButton2Reading = reading2;
}

/*********************************************************************
  SETUP FUNCTION
  - Runs once at boot
  - Initializes hardware, loads state, connects WiFi, starts services
*********************************************************************/
void setup() {
  // ===== SERIAL COMMUNICATION =====
  Serial.begin(115200);
  Serial.println("\n\n=================================");
  Serial.println("NIELIT ROPAR SMART LAB v" + String(FIRMWARE_VERSION));
  Serial.println("=================================\n");

  // ===== GPIO INITIALIZATION =====
  // Configure pin modes
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);  // Internal pull-up for ACTIVE-LOW buttons
  pinMode(BUTTON2, INPUT_PULLUP);

  // Set all outputs to safe OFF state
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(BUZZER, LOW);

  logEvent("System boot – NIELIT ROPAR v" + String(FIRMWARE_VERSION));

  // ===== STATE RESTORATION =====
  // Try to load saved state from EEPROM
  bool loaded = loadState();
  
  if (loaded) {
    // Valid state found - restore relay states
    setRelay(1, state.relay1, false);  // false = don't save back to EEPROM
    setRelay(2, state.relay2, false);
    beepPattern(2);  // 2 beeps = state restored
    logEvent("State restored from EEPROM");
  } else {
    // No valid state - initialize defaults
    state.relay1 = false;
    state.relay2 = false;
    state.relay1Toggles = 0;
    state.relay2Toggles = 0;
    state.lastToggleReset = millis();
    strncpy(state.deviceName, DEVICE_NAME, sizeof(state.deviceName) - 1);
    state.magic = EEPROM_MAGIC;
    saveState();
    beepPattern(3);  // 3 beeps = fresh initialization
    logEvent("Fresh boot - initialized defaults");
  }

  // ===== DHT SENSOR INITIALIZATION =====
  dht.begin();
  delay(2000);  // DHT sensor needs 2 seconds to stabilize
  updateDHT();
  logEvent("DHT11 sensor initialized");

  // ===== WIFI MANAGER CONFIGURATION =====
  // Set timeout for captive portal (3 minutes)
  wifiManager.setConfigPortalTimeout(180);
  
  // Callback when entering AP mode
  wifiManager.setAPCallback([](WiFiManager *wm) {
    logEvent("AP Mode: " + String(DEVICE_NAME));
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║   WIFI SETUP MODE ACTIVATED           ║");
    Serial.println("╠════════════════════════════════════════╣");
    Serial.println("║ 1. Connect to WiFi:                   ║");
    Serial.println("║    SSID: " + String(DEVICE_NAME) + "              ║");
    Serial.println("║    Password: " + String(AP_PASSWORD) + "                ║");
    Serial.println("║ 2. Browser will open automatically     ║");
    Serial.println("║ 3. Select your WiFi network            ║");
    Serial.println("║ 4. Enter WiFi password                 ║");
    Serial.println("╚════════════════════════════════════════╝\n");
    beepPattern(4);  // 4 beeps = AP mode active
  });

  // ===== WIFI CONNECTION =====
  // Auto-connect to saved network or start AP mode
  if (!wifiManager.autoConnect(DEVICE_NAME, AP_PASSWORD)) {
    logEvent("WiFi connection failed - rebooting");
    Serial.println("❌ WiFi connection timeout - restarting...");
    delay(3000);
    ESP.restart();
  }

  // Successfully connected
  logEvent("WiFi connected: " + WiFi.localIP().toString());
  Serial.println("\n✓ WiFi Connected");
  Serial.println("✓ IP Address: " + WiFi.localIP().toString());
  Serial.println("✓ Signal Strength: " + String(WiFi.RSSI()) + " dBm");
  beepPattern(1);  // 1 beep = WiFi connected

  // ===== mDNS SETUP =====
  // Allows access via http://nielit-ropar.local instead of IP
  if (MDNS.begin("nielit-ropar")) {
    logEvent("mDNS started: nielit-ropar.local");
    MDNS.addService("http", "tcp", 80);
    Serial.println("✓ mDNS: http://nielit-ropar.local");
  } else {
    logEvent("mDNS failed to start");
    Serial.println("⚠️ mDNS failed");
  }

  // ===== WEB SERVER START =====
  setupWebServer();

  // ===== WATCHDOG TIMER =====
  // Auto-restart if system hangs (8 second timeout)
  ESP.wdtEnable(WDTO_8S);
  logEvent("Watchdog timer enabled (8s)");

  // ===== STARTUP COMPLETE =====
  beepStart(200);  // Long beep = ready
  logEvent("System ready – All services running");
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   SYSTEM READY                         ║");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.println("║ Web Interface:                         ║");
  Serial.println("║   http://" + WiFi.localIP().toString() + "            ║");
  Serial.println("║   http://nielit-ropar.local            ║");
  Serial.println("║                                        ║");
  Serial.println("║ Credentials:                           ║");
  Serial.println("║   Username: " + String(ADMIN_USER) + "                     ║");
  Serial.println("║   Password: " + String(ADMIN_PASS) + "                    ║");
  Serial.println("╚════════════════════════════════════════╝\n");
}

/*********************************************************************
  MAIN LOOP
  - Runs continuously after setup()
  - Handles web requests, button inputs, sensor updates, health checks
*********************************************************************/
void loop() {
  // ===== WATCHDOG FEED =====
  // Reset watchdog timer to prevent auto-restart
  ESP.wdtFeed();

  // ===== WEB SERVER PROCESSING =====
  // Handle incoming HTTP requests
  server.handleClient();

  // ===== mDNS UPDATE =====
  // Process mDNS queries
  MDNS.update();

  // ===== PHYSICAL BUTTON HANDLING =====
  // Check and process button presses
  handleButtons();

  // ===== PERIODIC DHT SENSOR UPDATE =====
  // Read temperature/humidity every 5 seconds
  static unsigned long lastDHTUpdate = 0;
  if (millis() - lastDHTUpdate > DHT_UPDATE_INTERVAL) {
    lastDHTUpdate = millis();
    updateDHT();
  }

  // ===== MEMORY HEALTH CHECK =====
  // Monitor free heap and auto-restart if critically low
  static unsigned long lastHeapCheck = 0;
  if (millis() - lastHeapCheck > HEAP_CHECK_INTERVAL) {
    lastHeapCheck = millis();
    uint32_t freeHeap = ESP.getFreeHeap();
    
    // Emergency restart if memory below 8KB
    if (freeHeap < MIN_FREE_HEAP) {
      logEvent("CRITICAL: Low memory (" + String(freeHeap) + " bytes) - emergency reboot");
      Serial.println("\n⚠️⚠️⚠️ CRITICAL LOW MEMORY ⚠️⚠️⚠️");
      Serial.println("Free Heap: " + String(freeHeap) + " bytes");
      Serial.println("Threshold: " + String(MIN_FREE_HEAP) + " bytes");
      Serial.println("Initiating emergency restart...\n");
      
      beepPattern(10);  // Rapid beeps = critical error
      delay(1000);
      ESP.restart();
    }
  }

  // ===== BACKGROUND TASKS =====
  // Allow ESP8266 to handle WiFi and other background processes
  yield();
}

/*********************************************************************
  END OF FIRMWARE
  
  TROUBLESHOOTING TIPS:
  
  1. OTA Update Not Working:
     - Verify password is correct (rccrcc)
     - Check .bin file is for ESP8266
     - Ensure stable WiFi connection
     - Try rebooting device and retry
  
  2. Factory Reset Not Working:
     - Use exact credentials: admin / rccrcc
     - Wait 5 seconds after clicking reset
     - Device should reboot into AP mode automatically
     - If stuck, power cycle the device
  
  3. Web Interface Not Loading:
     - Check IP address in Serial Monitor
     - Try http://nielit-ropar.local if on same network
     - Verify device is connected to WiFi (not AP mode)
     - Clear browser cache
  
  4. Relays Not Responding:
     - Check rate limiting (max 20 toggles/minute)
     - Verify wiring to relay module
     - Test physical buttons
     - Check EEPROM state via logs
  
  5. DHT Sensor Shows Invalid Data:
     - Verify DHT11 wiring (VCC, GND, Data to D5)
     - Check for loose connections
     - DHT sensors can fail - try replacement
     - Wait 2 seconds after boot for stabilization
  
  SUPPORT:
  For technical support, contact NIELIT Ropar
  Documentation: Include serial monitor output for debugging
  
*********************************************************************/