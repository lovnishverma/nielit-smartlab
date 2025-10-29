# NIELIT ROPAR SMART LAB – ESP8266 Smart Home System

[![Firmware Version](https://img.shields.io/badge/Firmware-v3.0.0-blue)](https://github.com/lovnishverma/nielit-smartlab)
[![License](https://img.shields.io/badge/License-MIT-green)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-ESP8266-orange)](https://www.espressif.com/)

A production-ready IoT smart home system built for ESP8266, featuring dual relay control, environmental monitoring, and comprehensive web-based management.

---

## 🚀 Features

### Core Functionality
- **Dual Relay Control** – Web interface + physical button control
- **DHT11 Sensor** – Real-time temperature & humidity monitoring
- **WiFi Manager** – Easy network configuration with captive portal
- **OTA Updates** – Wireless firmware updates via web interface
- **Factory Reset** – Complete device reset with authentication
- **System Logging** – Real-time event monitoring and diagnostics

### Advanced Features
- ⚡ Rate limiting (20 toggles/minute per relay)
- 💾 EEPROM state persistence across reboots
- 🔒 HTTP Basic Authentication for admin functions
- 🌐 mDNS support (http://nielit-ropar.local)
- 🐕 Watchdog timer protection
- 📊 Memory health monitoring with auto-recovery
- 🔊 Audio feedback for all interactions

---

## 📋 Hardware Requirements

| Component | Specification | Pin |
|-----------|--------------|-----|
| **Microcontroller** | ESP8266 (NodeMCU/Wemos D1) | - |
| **Relay Module** | 2-Channel 5V Relay | D1 (GPIO5), D2 (GPIO4) |
| **Temperature Sensor** | DHT11 | D5 (GPIO14) |
| **Status LEDs** | Standard LEDs | D7 (GPIO13), D8 (GPIO15) |
| **Control Buttons** | Push buttons | RX (GPIO3), TX (GPIO1) |
| **Buzzer** | 5V Active Buzzer | D3 (GPIO0) |
| **Power Supply** | 5V 2A | VIN/5V |

### Pin Configuration
```
RELAY1  → D1 (GPIO5)   |  LED1    → D7 (GPIO13)
RELAY2  → D2 (GPIO4)   |  LED2    → D8 (GPIO15)
BUTTON1 → RX (GPIO3)   |  BUZZER  → D3 (GPIO0)
BUTTON2 → TX (GPIO1)   |  DHT11   → D5 (GPIO14)
```

---

## 🔧 Installation

### 1. Prerequisites
- **Arduino IDE** 1.8.19+ or **PlatformIO**
- **ESP8266 Board Package** (3.0.0+)
- **USB to Serial Driver** (CH340/CP2102 based on your board)

#### Installing ESP8266 Board Package
1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add to "Additional Board Manager URLs":
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search for "esp8266" and install **ESP8266 by ESP8266 Community**

### 2. Required Libraries
Install via **Sketch → Include Library → Manage Libraries**:

| Library | Version | Author |
|---------|---------|--------|
| WiFiManager | v2.0.16+ | tzapu |
| ArduinoJson | v6.21+ | Benoit Blanchon |
| DHT sensor library | v1.4.4+ | Adafruit |
| Adafruit Unified Sensor | v1.1.9+ | Adafruit |

**Pre-installed with ESP8266 package:**
- ESP8266WiFi
- ESP8266WebServer
- ESP8266mDNS
- ESP8266HTTPUpdateServer
- EEPROM

### 3. Upload Initial Firmware

#### Step 1: Connect Hardware
1. Connect ESP8266 to computer via USB
2. Wait for drivers to install (check Device Manager on Windows)
3. Note the COM port (e.g., COM3, COM5)

#### Step 2: Configure Arduino IDE
1. Open `NIELIT_SmartHome.ino`
2. **Tools → Board** → Select **"NodeMCU 1.0 (ESP-12E Module)"**
3. **Tools → Upload Speed** → **115200**
4. **Tools → CPU Frequency** → **80 MHz**
5. **Tools → Flash Size** → **4MB (FS:2MB OTA:~1019KB)**
6. **Tools → Port** → Select your COM port
7. **Tools → Erase Flash** → **Only Sketch** (first time use "All Flash Contents")

#### Step 3: Upload
1. Click **Verify (✓)** to compile (ensure no errors)
2. Click **Upload (→)**
3. Wait for "Done uploading" message
4. Open **Tools → Serial Monitor** (115200 baud)

### 4. First-Time Setup (WiFi Configuration)

When the device boots for the first time, it enters **Access Point (AP) Mode**:

#### Connection Details
```
📶 WiFi Network Name (SSID): NIELIT-SmartLab
🔐 Password: nielit2025
```

#### Setup Steps
1. **Power ON** the ESP8266
2. **Listen for audio feedback**:
   - 3 beeps = Fresh boot
   - 4 beeps = AP mode activated
3. **Connect your phone/laptop** to WiFi: `NIELIT-SmartLab`
4. **Enter password**: `nielit2025`
5. **Captive portal opens automatically** (if not, go to `http://192.168.4.1`)
6. **Click "Configure WiFi"**
7. **Select your home WiFi** from the list
8. **Enter your WiFi password**
9. **Click "Save"**
10. Device will **reboot and connect** to your network (1 beep = success)

#### Finding Device IP Address
After successful connection, check Serial Monitor for:
```
✓ WiFi Connected
✓ IP Address: 192.168.1.100
✓ mDNS: http://nielit-ropar.local
```

### 5. Generating .BIN File for OTA Updates

#### Method 1: Arduino IDE (Recommended)
1. Open your modified `NIELIT_SmartHome.ino`
2. Go to **Sketch → Export Compiled Binary** (or press `Ctrl+Alt+S`)
3. Wait for compilation to complete
4. Find the `.bin` file in the sketch folder:
   - Windows: `Documents\Arduino\NIELIT_SmartHome\`
   - Look for: `NIELIT_SmartHome.ino.nodemcu.bin`

#### Method 2: Manual Export
1. Click **Verify (✓)** to compile
2. Look for build output in Arduino IDE (enable verbose output in Preferences)
3. Find line: `Copying output to...`
4. Copy the `.bin` file from temp directory to your desired location

#### Method 3: Using arduino-cli (Advanced)
```bash
arduino-cli compile --fqbn esp8266:esp8266:nodemcu NIELIT_SmartHome.ino
```

#### Verifying .BIN File
- **File size**: Should be ~350-450 KB
- **Filename**: Must end with `.bin`
- **Version check**: Increment `FIRMWARE_VERSION` in code before compiling

### 6. Performing OTA Update

#### Prerequisites
- Device must be connected to WiFi
- `.bin` file ready
- Admin password known (`rccrcc`)

#### Update Steps
1. Open web interface: `http://nielit-ropar.local` or IP address
2. Click **"OTA Update"** link in footer
3. Enter password: `rccrcc`
4. Click **"Choose .bin File"**
5. Select your compiled `.bin` file
6. Click **"Update Firmware"**
7. **Wait for upload** (progress bar shows percentage)
8. Device will **automatically reboot** with new firmware
9. **Verify version** on dashboard after reboot

#### OTA Update Troubleshooting
- ❌ **"Authentication failed"** → Check password is `rccrcc`
- ❌ **"Upload failed"** → Ensure file is valid `.bin` for ESP8266
- ❌ **Device unresponsive** → Power cycle and retry
- ❌ **"Not enough space"** → Use correct Flash Size setting (4MB with OTA)

---

## 🌐 Usage

### Web Interface
Access via browser:
- **IP Address**: `http://192.168.x.x` (check Serial Monitor)
- **mDNS**: `http://nielit-ropar.local`

### Default Credentials
```
Username: admin
Password: rccrcc
```

### Dashboard Features
- **Relay Control** – Toggle relays ON/OFF with rate limiting
- **Live Sensors** – Real-time temperature and humidity display
- **System Info** – Uptime, IP address, memory usage
- **OTA Update** – Upload new firmware (.bin files)
- **System Logs** – View recent events and diagnostics
- **Factory Reset** – Complete device reset (requires password)

### Physical Controls
- **Button 1** – Toggle Relay 1
- **Button 2** – Toggle Relay 2
- Both buttons are debounced and include audio feedback

---

## 📡 API Endpoints

### Status & Control
```
GET  /api/status          → System status (JSON)
GET  /api/logs            → System logs (JSON)
GET  /api/relay1/on       → Turn Relay 1 ON
GET  /api/relay1/off      → Turn Relay 1 OFF
GET  /api/relay2/on       → Turn Relay 2 ON
GET  /api/relay2/off      → Turn Relay 2 OFF
GET  /api/reset           → Soft reset (EEPROM only)
GET  /api/factory-reset   → Full reset (requires auth)
```

### Example Response (`/api/status`)
```json
{
  "version": "3.0.0",
  "r1": true,
  "r2": false,
  "temp": 28.5,
  "hum": 65.2,
  "uptime": 125430,
  "ip": "192.168.1.100",
  "heap": 28672
}
```

---

## 🔐 Security Features

- **HTTP Basic Authentication** for OTA updates and factory reset
- **Rate Limiting** prevents relay abuse (20 toggles/min)
- **Input Validation** for all API endpoints
- **WiFi Encryption** with configurable AP password

---

---

## 🛠️ Troubleshooting

### 🔴 First Boot Issues

#### Device Stuck in AP Mode
**Symptoms:** Can't connect to home WiFi, keeps showing `NIELIT-SmartLab` network
- **Solution 1:** Reset WiFi credentials
  - Access device via AP mode (`NIELIT-SmartLab` / `nielit2025`)
  - Open `http://192.168.4.1`
  - Reconfigure WiFi with correct credentials
- **Solution 2:** Factory reset
  - Power off device
  - Press and hold Button 1
  - Power on while holding (release after 5 seconds)
  - Device resets to AP mode

#### Can't Find AP Network
- **Check:** WiFi is enabled on your phone/laptop
- **Check:** You're within 10m of the device
- **Check:** 2.4GHz WiFi is supported (5GHz won't work)
- **Try:** Restart the device (power cycle)

#### Captive Portal Doesn't Open
- **Manual access:** Open browser and go to `http://192.168.4.1`
- **Disable mobile data:** Turn off cellular data on phone
- **Try different browser:** Use Chrome/Firefox instead of in-app browsers

---

### 🌐 WiFi Connection Problems

#### Device Not Connecting to Home WiFi
- **Verify:** SSID and password are correct (case-sensitive)
- **Check:** Router is 2.4GHz capable (ESP8266 doesn't support 5GHz)
- **Check:** Router allows new device connections (not MAC filtered)
- **Try:** Move device closer to router
- **Reset:** Hold power button for 10+ seconds to force AP mode

#### Lost IP Address / Can't Access Web Interface
1. Check Serial Monitor for IP address (115200 baud)
2. Use mDNS: `http://nielit-ropar.local`
3. Check router's DHCP client list
4. Use IP scanner app (Fing, Advanced IP Scanner)

---

### ⚙️ Hardware Issues

#### Relays Not Responding
- **Rate Limit:** Wait 60 seconds if you hit 20 toggles/minute limit
- **Power Check:** Verify relay module has separate 5V supply
- **Wiring Check:** 
  - Relay 1 → D1 (GPIO5)
  - Relay 2 → D2 (GPIO4)
  - Common ground between ESP8266 and relay module
- **Test:** Use physical buttons to rule out software issues
- **Logs:** Check `/logs` page for error messages

#### Physical Buttons Not Working
- **Wiring Check:**
  - Button 1 → RX (GPIO3) → Ground
  - Button 2 → TX (GPIO1) → Ground
- **Note:** Buttons are ACTIVE-LOW (press connects to GND)
- **Test:** Buttons should trigger audio feedback (beep)
- **Warning:** RX/TX buttons may interfere with Serial Monitor during boot

#### DHT Sensor Shows `--` or Invalid Data
- **Wiring Verification:**
  ```
  DHT11 VCC  → 3.3V (NOT 5V for stability)
  DHT11 GND  → GND
  DHT11 DATA → D5 (GPIO14)
  ```
- **Pull-up Resistor:** Add 10kΩ resistor between DATA and VCC if using raw DHT11
- **Wait Time:** Sensor needs 2 seconds after boot to stabilize
- **Test:** Replace with known-good DHT11 module
- **Valid Range:** Temperature: -40°C to 80°C, Humidity: 0-100%

#### LEDs Not Illuminating
- **Check:** LED polarity (long leg = anode = positive)
- **Resistor:** Use 220Ω-330Ω current-limiting resistor
- **Pins:** LED1 → D7 (GPIO13), LED2 → D8 (GPIO15)
- **Note:** LEDs mirror relay states (ON = HIGH = lit)

#### Buzzer Not Working
- **Type:** Use ACTIVE buzzer (built-in oscillator)
- **Pin:** Connect to D3 (GPIO0)
- **Polarity:** Red/+ to GPIO0, Black/- to GND
- **Test:** Should beep on boot and button presses

---

### 🔄 OTA Update Problems

#### OTA Page Not Loading
- **Clear Cache:** Force refresh (`Ctrl+F5` or `Cmd+Shift+R`)
- **Try:** Use incognito/private browsing mode
- **Direct URL:** `http://YOUR_IP/update`
- **Check:** Firmware version in code is v3.0.0+ (older versions had bug)

#### Authentication Failed
- **Credentials:**
  ```
  Username: admin
  Password: rccrcc
  ```
- **Case Sensitive:** Enter exactly as shown
- **Browser:** Avoid auto-fill, type manually

#### Upload Fails / Device Unresponsive
- **File Check:** Ensure `.bin` file is for ESP8266 (not ESP32)
- **Size Check:** File should be 300-500 KB
- **Flash Size:** Compiled with correct settings (4MB with OTA partition)
- **WiFi:** Ensure stable connection during upload (stay within 5m of router)
- **Recovery:** If device becomes unresponsive:
  1. Power off
  2. Connect via USB
  3. Upload firmware via serial (not OTA)

#### "Not Enough Space" Error
- **Flash Settings:** Recompile with: **4MB (FS:2MB OTA:~1019KB)**
- **Clear EEPROM:** Use factory reset before OTA update
- **Last Resort:** Re-upload via USB with "Erase Flash: All Flash Contents"

---

### 🔄 System Behavior Issues

#### Device Keeps Rebooting
- **Serial Monitor:** Check for error messages
- **Memory:** Low heap warning? (check `/api/status` → heap)
- **Power Supply:** Insufficient current (need 2A minimum)
- **Watchdog:** Triggering due to blocked operations (check logs)
- **Fix:** Factory reset and re-upload firmware

#### Relay States Not Persisting
- **EEPROM Check:** Look for "State saved to EEPROM" in logs
- **Fix:** Perform soft reset (`/api/reset`)
- **Verify:** Check `state.magic` value in logs (should be 0xAA)

#### High Rate Limit Warnings
- **Normal:** Max 20 toggles per relay per minute
- **Counter Reset:** Automatically resets every 60 seconds
- **Workaround:** Wait 1 minute or perform soft reset
- **Check:** Review `/logs` for excessive toggle attempts

---

### 📱 Mobile/Browser Issues

#### Dashboard Not Auto-Updating
- **JavaScript:** Ensure JS is enabled in browser
- **Console:** Check browser console (F12) for errors
- **Network:** Verify `/api/status` endpoint returns valid JSON
- **Try:** Hard refresh or clear browser cache

#### Factory Reset Button Doesn't Work
- **Authentication:** Must enter credentials in popup
  - Username: `admin`
  - Password: `rccrcc`
- **Confirmation:** Must click "OK" in confirmation dialog
- **Browser:** Some browsers block popups - check for blocked popup icon
- **Manual Method:** Use API endpoint with curl:
  ```bash
  curl -u admin:rccrcc http://YOUR_IP/api/factory-reset
  ```

---

### 🔍 Diagnostic Tools

#### Serial Monitor Output
**Access:** Tools → Serial Monitor (115200 baud)
**Useful Commands:** Look for:
- Boot messages with version number
- IP address assignment
- Error messages
- EEPROM state
- Relay toggle events

#### System Logs Page
**Access:** `http://YOUR_IP/logs`
- Shows last 20 events
- Auto-refreshes every 3 seconds
- Includes timestamps
- Helps diagnose issues without USB connection

#### Status API Endpoint
**Access:** `http://YOUR_IP/api/status`
**Returns:**
```json
{
  "version": "3.0.0",
  "r1": true,
  "r2": false,
  "temp": 28.5,
  "hum": 65.2,
  "uptime": 125430,
  "ip": "192.168.1.100",
  "heap": 28672  ← Monitor this for memory issues
}
```

---

### 🆘 Emergency Recovery

#### Complete Factory Reset (Nuclear Option)
1. **Via Web Interface:**
   - Go to dashboard
   - Scroll to "Factory Reset" section
   - Enter credentials (`admin` / `rccrcc`)
   - Confirm reset
   - Wait for reboot into AP mode

2. **Via API (if web interface not working):**
   ```bash
   curl -u admin:rccrcc http://YOUR_IP/api/factory-reset
   ```

3. **Hardware Method (if completely unresponsive):**
   - Power off device
   - Connect to computer via USB
   - In Arduino IDE: **Tools → Erase Flash → All Flash Contents**
   - Re-upload firmware
   - Reconfigure WiFi from scratch

#### Recovering from Bad OTA Update
1. Connect device via USB
2. Open Arduino IDE
3. Upload working firmware via serial
4. Check Serial Monitor for errors
5. Perform factory reset if needed

---

## 📖 Advanced Configuration

### Customizing Device Settings

You can modify these constants in the firmware before uploading:

```cpp
// In NIELIT_SmartHome.ino

#define DEVICE_NAME      "NIELIT-SmartLab"   // AP name and mDNS hostname
#define AP_PASSWORD      "nielit2025"         // AP mode password
#define ADMIN_USER       "admin"              // Web interface username
#define ADMIN_PASS       "rccrcc"             // Web interface password

#define MAX_TOGGLES_PER_MIN      20          // Rate limit per relay
#define DHT_UPDATE_INTERVAL      5000        // Sensor read interval (ms)
#define MIN_FREE_HEAP            8192        // Auto-restart threshold (bytes)
```

After making changes:
1. Save the file
2. Recompile and upload via USB or OTA
3. Device will use new settings after reboot

---

## 🔌 Wiring Diagram

### Complete Hardware Setup

```
┌─────────────────────────────────────────────────────────────┐
│                     ESP8266 NodeMCU                          │
│                                                              │
│  3V3 ●────────────● VCC (DHT11)                             │
│  GND ●────────────● GND (DHT11, Relay, LEDs, Buzzer)       │
│                                                              │
│   D1 ●────────────● IN1 (Relay Channel 1)                  │
│   D2 ●────────────● IN2 (Relay Channel 2)                  │
│                                                              │
│   D5 ●────────────● DATA (DHT11)                            │
│                                                              │
│   D7 ●────[220Ω]──● LED1 (Anode) → Cathode to GND         │
│   D8 ●────[220Ω]──● LED2 (Anode) → Cathode to GND         │
│                                                              │
│   D3 ●────────────● BUZZER (+) → BUZZER (-) to GND        │
│                                                              │
│   RX ●────[BTN1]──● GND (Press to toggle Relay 1)         │
│   TX ●────[BTN2]──● GND (Press to toggle Relay 2)         │
│                                                              │
│  VIN ●────────────● 5V Power Supply                         │
│  GND ●────────────● Power Supply GND                        │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                    2-Channel Relay Module                    │
│                                                              │
│  VCC ●────────────● 5V Power Supply                         │
│  GND ●────────────● Common GND with ESP8266                 │
│  IN1 ●────────────● From D1 (GPIO5)                         │
│  IN2 ●────────────● From D2 (GPIO4)                         │
│                                                              │
│  COM1 ●───────────● AC/DC Load Common                       │
│  NO1  ●───────────● AC/DC Load (Normally Open)              │
│  NC1  ●   (unused - Normally Closed)                        │
│                                                              │
│  COM2 ●───────────● AC/DC Load Common                       │
│  NO2  ●───────────● AC/DC Load (Normally Open)              │
│  NC2  ●   (unused - Normally Closed)                        │
└─────────────────────────────────────────────────────────────┘

⚠️ SAFETY WARNING:
- Never work on AC circuits while powered
- Use proper insulation and enclosures
- Follow local electrical codes
- Test with low voltage loads first (LED strips, DC fans)
```

### Pin Mapping Summary

| Function | ESP8266 Pin | GPIO | Notes |
|----------|-------------|------|-------|
| Relay 1 | D1 | GPIO5 | Output (ACTIVE-HIGH) |
| Relay 2 | D2 | GPIO4 | Output (ACTIVE-HIGH) |
| LED 1 | D7 | GPIO13 | Mirrors Relay 1 state |
| LED 2 | D8 | GPIO15 | Mirrors Relay 2 state |
| Button 1 | RX | GPIO3 | Input (ACTIVE-LOW) |
| Button 2 | TX | GPIO1 | Input (ACTIVE-LOW) |
| Buzzer | D3 | GPIO0 | Output (ACTIVE-HIGH) |
| DHT11 | D5 | GPIO14 | Data pin |

---

## 🔐 Security Best Practices

### Change Default Credentials
**Before deploying in production:**

1. Modify credentials in code:
   ```cpp
   #define ADMIN_USER       "your_username"
   #define ADMIN_PASS       "your_strong_password"
   #define AP_PASSWORD      "your_ap_password"
   ```

2. Use strong passwords:
   - Minimum 8 characters
   - Mix of letters, numbers, symbols
   - Avoid common words

### Network Security
- **Use WPA2/WPA3** encryption on your WiFi router
- **Change AP password** from default `nielit2025`
- **Disable AP mode** after initial setup (requires code modification)
- **Use VPN** if accessing from outside local network
- **Firewall rules** to restrict access to device IP

### Firmware Security
- **Verify `.bin` files** before OTA updates
- **Keep backups** of working firmware versions
- **Test updates** on development device first
- **Monitor logs** for unauthorized access attempts

---

## 📊 Performance Specifications

| Metric | Value |
|--------|-------|
| **Boot Time** | ~5-8 seconds (WiFi dependent) |
| **Web Response Time** | <100ms (local network) |
| **Relay Switching Speed** | <50ms |
| **DHT Update Rate** | Every 5 seconds |
| **API Rate Limit** | 20 toggles/min per relay |
| **Max Concurrent Users** | 4-5 (limited by ESP8266 RAM) |
| **Memory Usage (Idle)** | ~25-30 KB free heap |
| **Power Consumption** | ~80mA (idle), ~250mA (relays ON) |
| **WiFi Range** | ~30m (indoor, 2.4GHz) |
| **OTA Update Time** | 30-60 seconds |

---

## 🎯 Use Cases

### Home Automation
- **Smart Lighting:** Control room lights remotely
- **Fan Control:** Automated temperature-based fan control
- **Appliance Scheduling:** Timer-based device control
- **Environmental Monitoring:** Track room temperature/humidity

### Educational Projects
- **IoT Learning:** Hands-on ESP8266 programming
- **Web Development:** REST API and frontend integration
- **Electronics:** Relay control and sensor interfacing
- **Network Protocols:** WiFi, HTTP, mDNS, OTA

### Laboratory Management
- **Equipment Control:** Remote lab equipment switching
- **Climate Monitoring:** Temperature/humidity logging
- **Access Control:** Authenticated remote operation
- **Energy Management:** Monitor device states and uptime

---

### v3.0.0 (2025-01-29)
- ✅ Fixed OTA Update page loading issue (PROGMEM handling)
- ✅ Fixed Factory Reset authentication and execution
- ✅ Improved code documentation and readability
- ✅ Enhanced error handling for web endpoints

### v2.0.0
- Added factory reset functionality
- Improved memory management
- Enhanced logging system

### v1.0.0
- Initial production release
- Core relay and sensor functionality

---

## 📄 License

MIT License – © NIELIT ROPAR 2025

---

## 🤝 Support

**Developed by:** NIELIT Ropar Smart Lab Team  
**Contact:** support@nielitropar.edu.in  
**Documentation:** [Wiki](https://github.com/lovnishverma/nielit-smartlab/wiki)

For bug reports and feature requests, please open an issue on GitHub.

---

## ⭐ Acknowledgments

Built with:
- ESP8266 Arduino Core
- WiFiManager by tzapu
- ArduinoJson by Benoit Blanchon
- DHT Sensor Library by Adafruit
- 

## 📄 Screenshots

<img width="1852" height="2356" alt="nielit-ropar local_" src="https://github.com/user-attachments/assets/51825530-ebf0-4353-b7d1-761c795afb78" />

---

<img width="1852" height="1396" alt="nielit-ropar local_update" src="https://github.com/user-attachments/assets/d6c1765f-f483-4849-b1f7-6f7db36924b5" />

