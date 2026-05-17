# 🌦️ IoT Weather Station  
### ESP32 / ESP8266 + BME280  
**Version 1.0.0 • Production Grade • MIT License**

---

## ✨ Features

- ✅ Glassmorphism Dark/Light Web Dashboard  
- ✅ Real-time WebSocket Updates (No Page Refresh)  
- ✅ NTP Time Sync with 12-hour AM/PM Format  
- ✅ Temperature Sparkline Chart (Last 60 Readings)  
- ✅ Dark / Light Mode Toggle  
- ✅ REST API Endpoint → `/api/data`  
- ✅ OTA (Over-The-Air) Firmware Updates  
- ✅ mDNS Support → `http://weatherstation.local`  
- ✅ WiFi Signal Strength Monitoring  
- ✅ Auto WiFi Reconnect  
- ✅ Device Uptime Tracker  
- ✅ PWA Manifest Support  
- ✅ Pressure-to-Weather Condition Mapping  
- ✅ Heat Index (“Feels Like”) Calculation  

---

## 🔌 Wiring Diagram

### ESP32 → BME280

| ESP32 Pin | BME280 Pin |
|-----------|------------|
| 3.3V      | VCC        |
| GND       | GND        |
| GPIO21    | SDA        |
| GPIO22    | SCL        |

---

### ESP8266 / NodeMCU → BME280

| ESP8266 Pin | BME280 Pin |
|-------------|------------|
| 3.3V        | VCC        |
| GND         | GND        |
| D2 / GPIO4  | SDA        |
| D1 / GPIO5  | SCL        |

---

## ⚠ Important Notes

- ⚠ Use **3.3V ONLY** — BME280 is **NOT 5V tolerant**
- ⚠ Default I2C Address = `0x76`
- ⚠ If sensor not detected, try `0x77`

---

## 📚 Required Libraries

Install from **Arduino Library Manager**

| Library | Description |
|---------|-------------|
| ESPAsyncWebServer | Async Web Server |
| AsyncTCP (ESP32) / ESPAsyncTCP (ESP8266) | TCP Backend |
| Adafruit BME280 | Sensor Library |
| Adafruit Unified Sensor | Required Dependency |
| NTPClient | NTP Time Sync |

---

## ⚙ Arduino IDE Board Settings

### ESP32

| Setting | Value |
|--------|--------|
| Board | ESP32 Dev Module |
| CPU Speed | 240 MHz |
| Flash Size | 4MB |
| Upload Speed | 921600 |
| Partition Scheme | Default 4MB with SPIFFS |

---

### ESP8266

| Setting | Value |
|--------|--------|
| Board | NodeMCU 1.0 |
| CPU Speed | 160 MHz |
| Flash Size | 4MB |
| Upload Speed | 921600 |

---

## 🚀 Upload Steps

1. Install all required libraries  
2. Edit WiFi credentials:
   ```cpp
   WIFI_SSID
   WIFI_PASSWORD
3. Set UTC_OFFSET_SEC for your timezone  

Bangladesh (**UTC**+6) = 6 * **3600** = **21600**  
India      (**UTC**+5:30) = **19800**  
UK         (**UTC**+0)   = 0  
US/New York (**UTC**-5)  = -**18000**  

1.  Select board + COM port
    
2.  Click Upload
    
3.  Open Serial Monitor → 115200 baud
    
4.  Open the IP address shown in serial monitor
    

Or use:

`   http://weatherstation.local   `

🛠 Troubleshooting
------------------

### ❌ BME280 Not Found

*   Check wiring
    
*   Try address 0x77 instead of 0x76
    
*   Ensure 3.3V power only
    

### ❌ WiFi Not Connecting

*   Verify SSID/password
    
*   Ensure 2.4 GHz WiFi support
    
*   Avoid special characters in password
    

### ❌ weatherstation.local Not Working

**Windows:**

*   Install Apple Bonjour Service
    

**Android:**

*   mDNS may not work → use IP address
    

### ❌ Dashboard Not Updating

*   Check browser console (F12)
    
*   Disable ad blockers
    
*   Try Incognito mode
    

### ❌ OTA Update Failed

*   Ensure OTA partition enabled
    
*   Check partition scheme
    
*   Review Serial Monitor logs
    

### ❌ ESP8266 Compile Errors

*   Install ESPAsyncTCP (not AsyncTCP)
    
*   Use ESP8266 Core ≥ 3.1.x
    

🌐 Dashboard Overview
---------------------

*   ⏰ Live Clock
    
*   📅 Date Display
    
*   🌡 Temperature
    
*   💧 Humidity
    
*   🌪 Air Pressure
    
*   🏔 Altitude Estimation
    
*   📶 WiFi Signal
    
*   ⏳ Device Uptime
    
*   🌗 Dark/Light Theme Toggle
    
*   📈 Real-time Charts
