# NIELIT ROPAR SMART LAB – Complete Documentation

[![Firmware](https://img.shields.io/badge/Firmware-v3.0.0-blue.svg)](https://github.com/lovnishverma/nielit-smartlab)
[![Platform](https://img.shields.io/badge/Platform-ESP8266-orange.svg)](https://www.espressif.com/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

**Production-Ready IoT Smart Home System for ESP8266**

---

## 📑 Table of Contents

1. [Overview](#-overview)
2. [Features](#-features)
3. [Hardware Requirements](#-hardware-requirements)
4. [Software Requirements](#-software-requirements)
5. [Installation Guide](#-installation-guide)
6. [First-Time Setup](#-first-time-setup)
7. [Web Interface Guide](#-web-interface-guide)
8. [OTA Firmware Updates](#-ota-firmware-updates)
9. [API Documentation](#-api-documentation)
10. [Physical Controls](#-physical-controls)
11. [Troubleshooting](#-troubleshooting)
12. [Advanced Configuration](#-advanced-configuration)
13. [Security & Best Practices](#-security--best-practices)

---

## 🎯 Overview

NIELIT Smart Lab is a comprehensive IoT system built on ESP8266, designed for educational institutions and home automation. It provides:

- **Dual relay control** with web and physical interfaces
- **Environmental monitoring** via DHT11 sensor
- **Wireless firmware updates** (OTA)
- **Factory reset** with authentication
- **Real-time system monitoring** and logging

**Key Improvements in v3.0.0:**
- ✅ Fixed OTA Update page loading bug
- ✅ Fixed Factory Reset authentication
- ✅ Enhanced error handling
- ✅ Improved code documentation

---

## ✨ Features

### Core Functionality

| Feature | Description |
|---------|-------------|
| **Relay Control** | 2 independent relays (web + physical buttons) |
| **Environmental Sensing** | DHT11 temperature & humidity monitoring |
| **WiFi Manager** | Captive portal for easy network setup |
| **OTA Updates** | Upload firmware via web browser |
| **Factory Reset** | Complete device reset with password |
| **System Logs** | Real-time event tracking (20 entries) |

### Advanced Features

- ⚡ **Rate Limiting:** 20 toggles/minute per relay
- 💾 **EEPROM Persistence:** Saves relay states across reboots
- 🔒 **HTTP Authentication:** Protects admin functions
- 🌐 **mDNS Support:** Access via `http://nielit-ropar.local`
- 🐕 **Watchdog Timer:** Auto-recovery from crashes (8s timeout)
- 📊 **Memory Monitoring:** Auto-restart below 8KB free heap
- 🔊 **Audio Feedback:** Buzzer confirms all actions

---

## 🔧 Hardware Requirements

### Components List

| Component | Specification | Quantity | Pin Connection |
|-----------|--------------|----------|----------------|
| **ESP8266 Board** | NodeMCU v3 / Wemos D1 Mini | 1 | - |
| **Relay Module** | 2-Channel 5V (ACTIVE-HIGH) | 1 | D1, D2 |
| **Temperature Sensor** | DHT11 | 1 | D5 |
| **Status LEDs** | 5mm Red/Green | 2 | D7, D8 |
| **Control Buttons** | Tactile Push Button | 2 | RX, TX |
| **Buzzer** | 5V Active Buzzer | 1 | D3 |
| **Resistors** | 220Ω (for LEDs) | 2 | - |
| **Power Supply** | 5V 2A USB/DC Adapter | 1 | VIN/5V |
| **Breadboard** | Half-size or full-size | 1 | - |
| **Jumper Wires** | Male-to-Male/Female | 20+ | - |

### Complete Pin Mapping

```
┌─────────────────────────────────────────────────────────┐
│              ESP8266 NodeMCU Pin Configuration           │
├─────────────┬──────────┬──────────┬────────────────────┤
│ Function    │ Board Pin│ GPIO Pin │ Connection Details │
├─────────────┼──────────┼──────────┼────────────────────┤
│ Relay 1     │ D1       │ GPIO5    │ → Relay IN1        │
│ Relay 2     │ D2       │ GPIO4    │ → Relay IN2        │
│ LED 1       │ D7       │ GPIO13   │ → 220Ω → LED → GND │
│ LED 2       │ D8       │ GPIO15   │ → 220Ω → LED → GND │
│ Button 1    │ RX       │ GPIO3    │ → Button → GND     │
│ Button 2    │ TX       │ GPIO1    │ → Button → GND     │
│ Buzzer      │ D3       │ GPIO0    │ → Buzzer+ → GND    │
│ DHT11 Data  │ D5       │ GPIO14   │ → DHT11 Data Pin   │
│ DHT11 VCC   │ 3.3V     │ -        │ → DHT11 VCC        │
│ DHT11 GND   │ GND      │ -        │ → DHT11 GND        │
│ Power In    │ VIN/5V   │ -        │ ← 5V 2A Supply     │
│ Ground      │ GND      │ -        │ ← Common Ground    │
└─────────────┴──────────┴──────────┴────────────────────┘
```

### Wiring Diagram

```
                    ┌──────────────────────┐
                    │   ESP8266 NodeMCU    │
                    │                      │
    5V Power ───────┤ VIN              3V3 ├───── DHT11 VCC
    Ground ─────────┤ GND              GND ├───── Common GND
                    │                      │
    Relay IN1 ──────┤ D1  (GPIO5)       D5 ├───── DHT11 Data
    Relay IN2 ──────┤ D2  (GPIO4)       D7 ├───[220Ω]─── LED1 ─── GND
                    │                   D8 ├───[220Ω]─── LED2 ─── GND
    Button1 ────────┤ RX  (GPIO3)       D3 ├───── Buzzer+ ─── GND
    Button2 ────────┤ TX  (GPIO1)          │
                    └──────────────────────┘

    ┌──────────────────────────────────┐
    │    2-Channel Relay Module        │
    ├──────────────────────────────────┤
    │  VCC ←── 5V Power Supply         │
    │  GND ←── Common Ground           │
    │  IN1 ←── D1 (GPIO5)              │
    │  IN2 ←── D2 (GPIO4)              │
    │                                  │
    │  Relay 1: COM1 ──┬── NO1        │
    │                  └── NC1         │
    │  Relay 2: COM2 ──┬── NO2        │
    │                  └── NC2         │
    └──────────────────────────────────┘
```

**⚠️ IMPORTANT NOTES:**
- **Relay Logic:** ACTIVE-HIGH (HIGH = ON, LOW = OFF)
- **Button Logic:** ACTIVE-LOW with internal pullup (press = connect to GND)
- **DHT11 Power:** Use 3.3V for stability (5V can work but may cause errors)
- **Common Ground:** All components must share common ground with ESP8266
- **Power Supply:** 2A minimum to handle relay switching current

---

## 💻 Software Requirements

### Arduino IDE Setup

#### 1. Install Arduino IDE
- **Version Required:** 1.8.19 or newer
- **Download:** [arduino.cc/en/software](https://www.arduino.cc/en/software)

#### 2. Add ESP8266 Board Support

1. Open Arduino IDE
2. Go to **File → Preferences**
3. In "Additional Board Manager URLs", add:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Click **OK**
5. Go to **Tools → Board → Boards Manager**
6. Search for `esp8266`
7. Install **ESP8266 by ESP8266 Community** (version 3.0.0+)
8. Click **Close**

#### 3. Install Required Libraries

Open **Sketch → Include Library → Manage Libraries** and install:

| Library Name | Version | Author | Purpose |
|-------------|---------|--------|---------|
| **WiFiManager** | 2.0.16+ | tzapu | WiFi configuration portal |
| **ArduinoJson** | 6.21.0+ | Benoit Blanchon | JSON parsing for API |
| **DHT sensor library** | 1.4.4+ | Adafruit | DHT11 temperature/humidity |
| **Adafruit Unified Sensor** | 1.1.9+ | Adafruit | Required by DHT library |

**Built-in Libraries (No Installation Needed):**
- ESP8266WiFi
- ESP8266WebServer
- ESP8266mDNS
- ESP8266HTTPUpdateServer
- EEPROM

### Verify Installation

Create a test sketch:
```cpp
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <DHT.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Libraries loaded successfully!");
}

void loop() {}
```

Click **Verify (✓)**. If it compiles without errors, you're ready!

---

## 📥 Installation Guide

### Step 1: Connect ESP8266

1. **Connect via USB** to your computer
2. **Wait for drivers** to install (Windows: Check Device Manager)
3. **Note the COM port** (e.g., COM3, COM5)
   - Windows: Device Manager → Ports (COM & LPT)
   - Mac: `/dev/cu.usbserial-*`
   - Linux: `/dev/ttyUSB*`

### Step 2: Configure Arduino IDE

1. **Open** `NIELIT_SmartHome.ino`
2. Go to **Tools** and configure:

| Setting | Value |
|---------|-------|
| **Board** | NodeMCU 1.0 (ESP-12E Module) |
| **Upload Speed** | 115200 |
| **CPU Frequency** | 80 MHz |
| **Flash Size** | **4MB (FS:2MB OTA:~1019KB)** ⚠️ Critical! |
| **Port** | Your COM port (e.g., COM3) |
| **Erase Flash** | Only Sketch (or "All" for first upload) |
| **Debug Level** | None |

### Step 3: Compile Firmware

1. Click **Verify (✓)** button
2. Wait for compilation (30-60 seconds)
3. **Check for errors** in the output window
4. **Expected output:**
   ```
   Sketch uses 357,232 bytes (34%) of program storage space.
   Global variables use 31,416 bytes (38%) of dynamic memory.
   ```

### Step 4: Upload Firmware

1. **Ensure ESP8266 is connected** via USB
2. Click **Upload (→)** button
3. **Monitor progress** in the output window:
   ```
   Uploading...
   Writing at 0x00000000... (10%)
   Writing at 0x00004000... (20%)
   ...
   Writing at 0x00074000... (100%)
   Hard resetting via RTS pin...
   ```
4. **Success message:** `Done uploading.`

### Step 5: Verify Upload

**⚠️ IMPORTANT:** Serial Monitor is **DISABLED** in this firmware due to use of RX/TX pins for physical buttons!

**Instead, verify upload by:**
1. **Audio feedback:**
   - 3 beeps = Fresh boot (no saved state)
   - 2 beeps = State restored from EEPROM
2. **LED indicators:**
   - LEDs should be OFF (or match last saved state)
3. **WiFi network:**
   - Look for `NIELIT-SmartLab` WiFi network (AP mode)

**Why No Serial Monitor?**
- GPIO3 (RX) and GPIO1 (TX) are used for physical buttons
- Serial communication would interfere with button detection
- All diagnostics available via web interface (`/logs` page)

---

## 🚀 First-Time Setup

### WiFi Configuration Process

When powered on for the first time, the device enters **Access Point (AP) Mode**:

#### Audio/Visual Indicators

| Beep Pattern | Meaning |
|--------------|---------|
| 3 beeps | Fresh boot (no EEPROM data) |
| 4 beeps | AP mode activated |
| 1 beep | WiFi connected successfully |
| 2 beeps | State restored from EEPROM |

#### Step-by-Step WiFi Setup

**Step 1: Power ON Device**
```
▪️ Connect 5V power supply
▪️ Wait 3-5 seconds for boot
▪️ Listen for 4 beeps (AP mode active)
```

**Step 2: Connect to AP**
```
📱 On your phone/laptop:
   └─ WiFi Settings
      └─ Select: NIELIT-SmartLab
         └─ Password: nielit2025
```

**Step 3: Configure WiFi**
```
🌐 Captive portal opens automatically
   (If not, browse to: http://192.168.4.1)

   1. Click "Configure WiFi"
   2. Select your home WiFi network
   3. Enter WiFi password
   4. Click "Save"
```

**Step 4: Wait for Connection**
```
⏳ Device will:
   1. Save credentials to memory
   2. Reboot automatically
   3. Connect to your WiFi
   4. Beep once (success)
```

**Step 5: Find Device IP**
```
Since Serial Monitor is disabled, use:

Option A: Router Admin Panel
   └─ DHCP Client List
      └─ Look for "ESP_XXXXXX" or "NIELIT-SmartLab"

Option B: mDNS (if supported)
   └─ Browse to: http://nielit-ropar.local

Option C: Network Scanner App
   └─ Use: Fing (iOS/Android) or Advanced IP Scanner (Windows)
      └─ Scan for device on port 80
```

### Troubleshooting First Setup

| Problem | Solution |
|---------|----------|
| **Can't see AP network** | • Power cycle device<br>• Check 2.4GHz WiFi enabled<br>• Move closer to device |
| **Captive portal doesn't open** | • Manually go to `192.168.4.1`<br>• Disable mobile data<br>• Try different browser |
| **Can't connect to home WiFi** | • Verify password (case-sensitive)<br>• Check router is 2.4GHz<br>• Ensure DHCP is enabled |
| **No beep after save** | • Wait 30 seconds<br>• Check power supply (2A needed)<br>• Try factory reset |

---

## 🌐 Web Interface Guide

### Accessing Dashboard

**Primary Methods:**
```
🔗 http://DEVICE_IP_ADDRESS
   Example: http://192.168.1.100

🔗 http://nielit-ropar.local (mDNS)
   Note: May not work on all networks
```

**Default Credentials:**
```
👤 Username: admin
🔐 Password: rccrcc
```

### Dashboard Layout

```
╔════════════════════════════════════════════════════════╗
║            NIELIT Ropar Smart Lab                      ║
║         Firmware v3.0.0 • ● Online                     ║
╠════════════════════════════════════════════════════════╣
║  [RELAY CONTROL]                                       ║
║  ┌────────────────────────────────────────────┐       ║
║  │ Relay 1              [ON]  [Turn OFF]      │       ║
║  │ Relay 2              [OFF] [Turn ON]       │       ║
║  └────────────────────────────────────────────┘       ║
╠════════════════════════════════════════════════════════╣
║  [ENVIRONMENT SENSORS]                                 ║
║  ┌─────────────────┬─────────────────┐                ║
║  │  Temperature    │    Humidity     │                ║
║  │     28.5°C      │      65.2%      │                ║
║  └─────────────────┴─────────────────┘                ║
╠════════════════════════════════════════════════════════╣
║  [SYSTEM INFO]                                         ║
║  Uptime:      2h 15m                                   ║
║  IP Address:  192.168.1.100                            ║
║  Free Heap:   28.5 KB                                  ║
║  Local URL:   http://nielit-ropar.local/               ║
╠════════════════════════════════════════════════════════╣
║  [FACTORY RESET]                                       ║
║  [ Factory Reset (Erase Wi-Fi & Settings) ]            ║
║  ⚠️ Requires password: admin / ******                  ║
╠════════════════════════════════════════════════════════╣
║  © NIELIT Ropar 2025 • OTA Update • System Logs       ║
╚════════════════════════════════════════════════════════╝
```

### Dashboard Features Explained

#### 1. Relay Control Section

**Relay Status Badge:**
- 🟢 **ON** (Green) = Relay is active
- 🔴 **OFF** (Red) = Relay is inactive

**Control Buttons:**
- **Turn ON** = Activates relay (green button)
- **Turn OFF** = Deactivates relay (red button)

**Rate Limiting:**
- Maximum: 20 toggles per minute per relay
- Counter resets every 60 seconds
- Exceeded limit shows error message

**Audio Feedback:**
- Single beep = Toggle successful
- No beep = Rate limit exceeded

#### 2. Environment Sensors

**Temperature Display:**
- Range: -40°C to 80°C
- Updates every 5 seconds
- Shows `--` if sensor error

**Humidity Display:**
- Range: 0% to 100%
- Updates every 5 seconds
- Shows `--` if sensor error

#### 3. System Information

**Uptime:**
- Format: `Xd Yh Zm` (days, hours, minutes)
- Example: `2d 5h 30m`
- Resets on device reboot

**IP Address:**
- Shows current local IP
- Click to copy (depends on browser)

**Free Heap:**
- Current available RAM
- Should stay above 8 KB
- Auto-restart if below 8 KB

**Local URL:**
- mDNS address (if supported)
- Alternative to IP address

#### 4. Factory Reset

**Function:**
- Clears all WiFi credentials
- Erases EEPROM data
- Resets to factory defaults
- Reboots into AP mode

**Process:**
1. Click "Factory Reset" button
2. Confirm warning dialog
3. Enter username: `admin`
4. Enter password: `rccrcc`
5. Wait 5-10 seconds for reboot

### System Logs Page

**Access:** `http://DEVICE_IP/logs`

**Features:**
- Shows last 20 system events
- Auto-refreshes every 3 seconds
- Includes timestamps (in seconds since boot)
- No authentication required

**Example Log Entries:**
```
15s: System boot – NIELIT ROPAR v3.0.0
17s: EEPROM invalid or empty - using defaults
20s: DHT11 sensor initialized
25s: WiFi connected: 192.168.1.100
26s: mDNS started: nielit-ropar.local
27s: Web server started on port 80
28s: System ready – All services running
45s: Relay1 → ON
50s: Relay2 → ON
```

---

## 🔄 OTA Firmware Updates

### Preparing for OTA Update

#### Step 1: Generate .BIN File

**Using Arduino IDE:**

1. **Open** your modified `NIELIT_SmartHome.ino`
2. **Increment version** if you made changes:
   ```cpp
   #define FIRMWARE_VERSION "3.0.1"  // Change from 3.0.0
   ```
3. **Go to:** Sketch → Export Compiled Binary
   - Or press `Ctrl+Alt+S` (Windows/Linux)
   - Or press `Cmd+Alt+S` (Mac)
4. **Wait** for compilation (output shows "Done")
5. **Locate .BIN file:**
   - Windows: `Documents\Arduino\NIELIT_SmartHome\`
   - Mac: `~/Documents/Arduino/NIELIT_SmartHome/`
   - Linux: `~/Arduino/NIELIT_SmartHome/`
   - Filename: `NIELIT_SmartHome.ino.nodemcu.bin`

**Verify .BIN File:**
```
✅ File size: 350-450 KB
✅ Extension: .bin
✅ No spaces in filename (if renaming)
```

#### Step 2: Perform OTA Update

**Access Update Page:**
```
🔗 http://DEVICE_IP/update
   Example: http://192.168.1.100/update

🔗 http://nielit-ropar.local/update
```

**Update Process:**

```
┌──────────────────────────────────────────┐
│        OTA Update – NIELIT ROPAR         │
│                                          │
│  Current: v3.0.0                         │
│                                          │
│  [Password: ******]                      │
│  [Choose .bin File]                      │
│                                          │
│  ⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜ 0%                     │
│                                          │
│  [Update Firmware] (disabled)            │
│                                          │
│  © NIELIT ROPAR 2025 • Back to Dashboard │
└──────────────────────────────────────────┘
```

**Steps:**
1. **Enter password:** `rccrcc`
2. **Click "Choose .bin File"**
3. **Select your .bin file**
4. **Click "Update Firmware"**
5. **Monitor progress bar**
6. **Wait for success message**
7. **Device reboots automatically** (20-30 seconds)
8. **Verify new version** on dashboard

**Upload Progress:**
```
⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜ 0%   → Starting upload...
⬛⬛⬛⬜⬜⬜⬜⬜⬜⬜ 25%  → Uploading...
⬛⬛⬛⬛⬛⬛⬜⬜⬜⬜ 50%  → Halfway there...
⬛⬛⬛⬛⬛⬛⬛⬛⬛⬜ 75%  → Almost done...
⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛ 100% → ✅ Success! Rebooting...
```

### OTA Update Troubleshooting

| Error | Cause | Solution |
|-------|-------|----------|
| **"Authentication failed"** | Wrong password | Use `rccrcc` exactly |
| **"Only .bin files allowed"** | Wrong file type | Export compiled binary, not .ino |
| **"Upload failed"** | Network issue | Move closer to router, retry |
| **"Not enough space"** | Wrong flash settings | Recompile with 4MB OTA partition |
| **Page doesn't load** | Browser cache | Hard refresh (Ctrl+F5) |
| **Device unresponsive** | Bad firmware | Recover via USB upload |

### Recovery from Bad OTA

**If device becomes unresponsive:**

1. **Connect via USB**
2. **Open Arduino IDE**
3. **Select correct COM port**
4. **Upload working firmware** (normal upload method)
5. **Wait for upload to complete**
6. **Perform factory reset** (web interface or via code)

**Why OTA might fail:**
- ❌ Wrong board type selected during compilation
- ❌ Flash size mismatch (must have OTA partition)
- ❌ Corrupted .bin file
- ❌ Power loss during upload
- ❌ WiFi connection dropped mid-upload

---

## 📡 API Documentation

### Overview

All API endpoints return JSON responses and use RESTful conventions.

**Base URL:** `http://DEVICE_IP/api/`

**Authentication:** Required for `/factory-reset` only (HTTP Basic Auth)

### Endpoints Reference

#### GET /api/status

**Description:** Retrieve current system status

**Authentication:** None

**Response:**
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

**Response Fields:**

| Field | Type | Description |
|-------|------|-------------|
| `version` | string | Firmware version |
| `r1` | boolean | Relay 1 state (true=ON) |
| `r2` | boolean | Relay 2 state (true=ON) |
| `temp` | float | Temperature in Celsius |
| `hum` | float | Relative humidity (%) |
| `uptime` | integer | Milliseconds since boot |
| `ip` | string | Device IP address |
| `heap` | integer | Free heap in bytes |

**Example cURL:**
```bash
curl http://192.168.1.100/api/status
```

---

#### GET /api/logs

**Description:** Retrieve system event logs

**Authentication:** None

**Response:**
```json
{
  "logs": [
    "15s: System boot – NIELIT ROPAR v3.0.0",
    "20s: WiFi connected: 192.168.1.100",
    "45s: Relay1 → ON",
    "50s: Relay2 → OFF",
    ""
  ]
}
```

**Response Fields:**
- `logs`: Array of 20 log entries (oldest first)
- Empty strings indicate unused buffer slots

**Example cURL:**
```bash
curl http://192.168.1.100/api/logs
```

---

#### GET /api/relay1/on

**Description:** Turn Relay 1 ON

**Authentication:** None

**Response:** 303 redirect to `/` (dashboard)

**Rate Limit:** 20 toggles/minute

**Error Response (429):**
```
Rate limit exceeded: Max 20 toggles/min
```

**Example cURL:**
```bash
curl http://192.168.1.100/api/relay1/on
```

---

#### GET /api/relay1/off

**Description:** Turn Relay 1 OFF

**Authentication:** None

**Response:** 303 redirect to `/` (dashboard)

**Example cURL:**
```bash
curl http://192.168.1.100/api/relay1/off
```

---

#### GET /api/relay2/on

**Description:** Turn Relay 2 ON

**Authentication:** None

**Response:** 303 redirect to `/` (dashboard)

**Example cURL:**
```bash
curl http://192.168.1.100/api/relay2/on
```

---

#### GET /api/relay2/off

**Description:** Turn Relay 2 OFF

**Authentication:** None

**Response:** 303 redirect to `/` (dashboard)

**Example cURL:**
```bash
curl http://192.168.1.100/api/relay2/off
```

---

#### GET /api/reset

**Description:** Soft reset (clears EEPROM only, keeps WiFi)

**Authentication:** None

**Response:**
```html
<h2>Resetting device... © NIELIT ROPAR</h2>
```

**Effect:**
- Clears EEPROM data
- Keeps WiFi credentials
- Reboots device

**Example cURL:**
```bash
curl http://192.168.1.100/api/reset
```

---

#### GET /api/factory-reset

**Description:** Complete factory reset (WiFi + EEPROM)

**Authentication:** HTTP Basic Auth (`admin` / `rccrcc`)

**Response:**
```json
{
  "status": "success",
  "message": "Factory reset complete. Rebooting into AP mode..."
}
```

**Effect:**
- Clears EEPROM data
- Erases WiFi credentials
- Reboots into AP mode
- Audio: 5 beeps before reboot

**Example cURL:**
```bash
curl -u admin:rccrcc http://192.168.1.100/api/factory-reset
```

**Error Response (401):**
```
Authentication required
```

---

### API Usage Examples

#### Python Example

```python
import requests

BASE_URL = "http://192.168.1.100/api"

# Get status
response = requests.get(f"{BASE_URL}/status")
data = response.json()
print(f"Temperature: {data['temp']}°C")
print(f"Relay 1: {'ON' if data['r1'] else 'OFF'}")

# Toggle relay
requests.get(f"{BASE_URL}/relay1/on")

# Factory reset (requires auth)
requests.get(
    f"{BASE_URL}/factory-reset",
    auth=('admin', 'rccrcc')
)
```

#### JavaScript Example

```javascript
// Get status
fetch('http://192.168.1.100/api/status')
  .then(r => r.json())
  .then(d => {
    console.log(`Temperature: ${d.temp}°C`);
    console.log(`Relay 1: ${d.r1 ? 'ON' : 'OFF'}`);
  });

// Toggle relay
fetch('http://192.168.1.100/api/relay1/on');

// Factory reset (with auth)
fetch('http://192.168.1.100/api/factory-reset', {
  method: 'GET',
  headers: {
    'Authorization': 'Basic ' + btoa('admin:rccrcc')
  }
});
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
- NOCE MCU Blynk board

## 📄 Screenshots

<img width="1852" height="2356" alt="nielit-ropar local_" src="https://github.com/user-attachments/assets/51825530-ebf0-4353-b7d1-761c795afb78" />

---

<img width="1852" height="1396" alt="nielit-ropar local_update" src="https://github.com/user-attachments/assets/d6c1765f-f483-4849-b1f7-6f7db36924b5" />

---

# The hardware used includes a NOCE MCU Blynk board, an ESP8266, one buzzer, two LEDs, some resistors, and connecting wires.

![hw](https://github.com/user-attachments/assets/0bf78c01-fab9-490a-a46f-741837d432d4)


