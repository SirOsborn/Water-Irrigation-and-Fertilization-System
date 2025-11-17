# 🌱 Smart Irrigation & Fertilization System

ESP32-based IoT irrigation system with web dashboard for remote monitoring and control.

## ✨ Key Features

✅ **Modular Architecture** - Professional component-based structure  
✅ **Web Dashboard** - Real-time monitoring and control from any device  
✅ **Auto + Manual Modes** - Intelligent automation with manual override  
✅ **HTTP Polling** - Efficient updates every 2 seconds  
✅ **Dual Pump Control** - Water + fertilizer with independent control  
✅ **Tank Monitoring** - Alert LEDs for empty tanks  
✅ **Clean Code** - Separate HTML/CSS/JS for easy customization  
✅ **Local IoT** - Works without internet connection  

## 🎯 Recent Updates (v2.0)

- 🔧 **Fixed Manual Control** - Pumps now stay ON/OFF as expected
- 📁 **Restructured Project** - Clean modular organization
- 🎨 **Separated Dashboard** - HTML in separate file with syntax highlighting
- 📝 **Comprehensive Docs** - Full documentation in `docs/` folder
- ⚡ **HTTP Polling** - Replaced WebSocket for better compatibility

## 📁 Project Structure (Updated - Modular Architecture)

```
final/
├── main/                          # Application entry point (2 files only!)
│   ├── main.c                    # Main application initialization
│   └── CMakeLists.txt            # Build configuration
│
├── components/                    # Reusable components
│   ├── sensors/                  # Hardware abstraction layer
│   │   ├── sensors.c            # GPIO/ADC initialization, sensor reading
│   │   ├── sensors.h            # Sensor API & pin definitions
│   │   └── CMakeLists.txt       # Component build config
│   │
│   ├── irrigation/              # Irrigation control logic
│   │   ├── irrigation_control.c # Auto/manual irrigation logic
│   │   ├── irrigation_control.h # Irrigation API
│   │   └── CMakeLists.txt       # Component build config
│   │
│   ├── wifi/                    # WiFi connectivity
│   │   ├── wifi_config.c       # WiFi station mode setup
│   │   ├── wifi_config.h       # WiFi credentials & API
│   │   └── CMakeLists.txt      # Component build config
│   │
│   └── webserver/               # HTTP server & API
│       ├── web_server.c        # REST API endpoints
│       ├── web_server.h        # Web server API
│       └── CMakeLists.txt      # Component build config
│
├── web/                         # Web dashboard UI
│   ├── dashboard.html          # HTML/CSS/JS dashboard (EDIT THIS!)
│   ├── dashboard.h             # Auto-generated C header
│   └── html_to_header.py       # HTML → C converter script
│
├── docs/                        # Documentation
│   ├── README.md               # Comprehensive project docs
│   └── STRUCTURE_GUIDE.md      # Directory structure guide
│
├── build/                       # Build output (auto-generated)
├── CMakeLists.txt              # Root project configuration
├── sdkconfig                    # ESP-IDF configuration
└── RESTRUCTURE_SUMMARY.md       # Migration guide
```

## 🗂️ Component Descriptions

### **main/** (Application Entry)
- **main.c** - Initializes all components, creates tasks, manages global state
- Minimal code - just coordinates components

### **components/sensors/** (Hardware Layer)
- GPIO initialization for pumps, LEDs, and digital sensors
- ADC initialization for analog soil moisture sensor
- Functions: `init_gpio()`, `init_adc()`, `read_soil_moisture()`, `read_water_level_digital()`
- Pin definitions: All GPIO pins defined in `sensors.h`

### **components/irrigation/** (Business Logic)
- Main irrigation task with automatic/manual modes
- Pump control with active-low relay support
- LED alert system for empty tanks
- Respects manual override flags per pump
- Functions: `control_pump()`, `control_water_alert_led()`, `irrigation_task()`

### **components/wifi/** (Connectivity)
- WiFi station mode with automatic reconnection
- Retry logic (5 attempts)
- Event handlers for connection/disconnection
- **Edit `wifi_config.h`** to set your WiFi credentials

### **components/webserver/** (HTTP API)
- HTTP web server (no WebSocket - uses HTTP polling instead)
- Serves embedded HTML dashboard
- REST API endpoints:
  - `GET /` - Main dashboard (HTML)
  - `GET /api/data` - Get all sensor data (JSON)
  - `POST /api/pump` - Control pumps manually
  - `POST /api/auto` - Toggle automatic mode
  - `POST /api/settings` - Update system settings

### **web/** (UI Layer)
- **dashboard.html** - Clean HTML/CSS/JS with proper syntax highlighting
- **html_to_header.py** - Converts HTML to C header for embedding
- Dashboard updates every 2 seconds via HTTP polling (efficient for irrigation)
  - `POST /api/settings` - Update system settings

## 🔧 Hardware Connections

| Component | ESP32 Pin | Type | Notes |
|-----------|-----------|------|-------|
| **Water Pump Relay** | GPIO 27 | Output | Active LOW (LOW = ON) |
| **Fertilizer Pump Relay** | GPIO 26 | Output | Active LOW (LOW = ON) |
| **Soil Moisture Sensor** | GPIO 36 (ADC1_CH0) | Analog Input | 0-4095 range |
| **Water Tank Sensor** | GPIO 34 | Digital Input | HIGH = has water |
| **Fertilizer Tank Sensor** | GPIO 35 | Digital Input | HIGH = has liquid |
| **Water Alert LED** | GPIO 22 | Output | HIGH = tank empty |
| **Fertilizer Alert LED** | GPIO 23 | Output | HIGH = tank empty |

## ⚙️ Configuration

### **WiFi Settings** (`components/wifi/wifi_config.h`)
```c
#define WIFI_SSID      "YOUR_WIFI_SSID"
#define WIFI_PASS      "YOUR_WIFI_PASSWORD"
```

### **Pin Definitions** (`components/sensors/sensors.h`)
All GPIO pins are defined in the sensors header - edit there to change hardware connections.

## 🚀 How to Build & Flash

### **Prerequisites**
- ESP-IDF v5.5.1 installed
- Python 3.x
- ESP32 board connected via USB

### **Quick Start**
1. **Configure WiFi credentials** in `components/wifi/wifi_config.h`

2. **Build the project:**
   ```bash
   cd C:\Users\PureGoat\embedPract\final
   idf.py build
   ```

3. **Flash to ESP32:**
   ```bash
   idf.py -p COM5 flash monitor
   ```

### **Updating the Dashboard**
1. Edit `web/dashboard.html` with your changes
2. Run the converter:
   ```bash
   python web/html_to_header.py
   ```
3. Rebuild and flash:
   ```bash
   idf.py build flash -p COM5
   ```

## 📱 Accessing the Dashboard

1. After flashing, check the serial monitor for the ESP32's IP address
2. Open a web browser on any device connected to the same WiFi
3. Navigate to: `http://<ESP32_IP_ADDRESS>`
4. You'll see the real-time dashboard with:
   - Live sensor readings
   - Manual pump controls
   - Automatic mode toggle
   - Adjustable settings

## 🎯 Features

### **Real-Time Monitoring**
- Soil moisture level (ADC value)
- Water tank status (FULL/EMPTY)
- Fertilizer tank status (FULL/EMPTY)
- Pump status (ON/OFF)

### **Manual Control**
- Turn pumps ON/OFF manually
- Override automatic mode
- Independent control for each pump

### **Settings (Adjustable via Web)**
- Soil moisture threshold
- Water pump duration
- Fertilizer pump duration
- Sensor check interval

### **Automatic Irrigation**
- Triggers when soil moisture exceeds threshold
- Only runs if tanks have liquid
- Respects manual override
- Pumps water first, then fertilizer

### **Visual Alerts**
- LED indicators for empty tanks
- Color-coded status on dashboard
- Real-time updates every 2 seconds via HTTP polling

## 🎨 Modifying the Dashboard

The dashboard is now a **separate HTML file** for easy editing!

**Old way (messy):**
```c
// Everything in C strings - hard to edit!
static const char* html = "<!DOCTYPE html><html>...";
```

**New way (clean):**
1. Edit `web/dashboard.html` with proper HTML syntax highlighting
2. Run: `python web/html_to_header.py`
3. Rebuild: `idf.py build flash -p COM5`

**Benefits:**
- ✅ Full HTML/CSS/JavaScript syntax highlighting
- ✅ Easy to debug and preview in browser
- ✅ Professional development workflow
- ✅ Still embedded in ESP32 flash (fast loading)

## 📊 System Behavior

### **Automatic Mode (Default)**
- Checks soil moisture every 5 seconds
- If moisture > 2800 (dry soil):
  - Activates water pump for 3 seconds
  - Waits 1 second
  - Activates fertilizer pump for 1.5 seconds
- Skips irrigation if tanks are empty
- Respects manual mode flags

### **Manual Mode**
- Click "Turn ON" → Pump stays ON indefinitely ✅
- Click "Turn OFF" → Pump turns OFF
- Each pump controlled independently
- Automatic control paused while manual active
- Fixed issue: Pumps no longer auto-turn-off in manual mode!

### **Alert System**
- LEDs turn ON when tanks are empty
- Dashboard shows EMPTY status in red
- Automatic irrigation skipped if no liquid available

## 🔧 Troubleshooting

**WiFi won't connect:**
- Check SSID and password in `wifi_config.h`
- Ensure ESP32 is in range of router
- Check serial monitor for connection status

**Dashboard not loading:**
- Verify ESP32 IP address from serial monitor
- Ensure device is on same WiFi network
- Try accessing: `http://192.168.x.x` (your ESP32's IP)

**Sensors reading incorrectly:**
- Check wiring connections
- Verify sensor orientation (some are inverted)
- Adjust thresholds via dashboard

**Pumps not working:**
- Verify relay connections
- Check if in automatic or manual mode
- Ensure tanks are not empty

## 📝 Technical Notes

### **Communication Protocol**
- HTTP polling - Updates every 2 seconds
- More compatible with ESP-IDF v5.5.1
- Sufficient for irrigation monitoring (soil changes slowly)

### **Data Storage**
- Settings stored in RAM (reset on reboot)
- WiFi credentials in code (recompile to change)
- All actions logged to serial monitor for debugging

### **IoT Classification**
- **Local IoT / Edge IoT** - Works on local network
- No internet connection required
- Access dashboard from any device on same WiFi
- Can be upgraded to Cloud IoT with MQTT/Firebase

### **Component Architecture**
- Modular design for easy maintenance
- Each component can be reused in other projects
- Clear separation of concerns (Hardware → Logic → UI)
- ESP-IDF best practices followed

## 🔐 Security Considerations

⚠️ **Current Implementation:**
- No authentication on web dashboard
- HTTP only (not HTTPS)
- Local network access only

**For Production Use:**
- Add login system
- Implement HTTPS
- Use WPA3 WiFi encryption
- Enable ESP32 secure boot

## 📚 Additional Documentation

- **Full Architecture:** `docs/README.md`
- **Structure Guide:** `docs/STRUCTURE_GUIDE.md`
- **Migration Guide:** `RESTRUCTURE_SUMMARY.md`
- **Dashboard Guide:** Edit `web/dashboard.html` comments

## 🌟 Future Enhancements

- [ ] Save settings to NVS (persistent storage)
- [ ] Add scheduling (water at specific times)
- [ ] Email/SMS notifications via Telegram Bot
- [ ] Data logging to SD card
- [ ] Historical charts and analytics
- [ ] OTA (Over-The-Air) updates
- [ ] MQTT cloud integration
- [ ] Mobile app (React Native)
- [ ] Multi-zone irrigation support
- [ ] Weather API integration

## 👨‍💻 Development

### **Project Info**
- **ESP-IDF Version:** v5.5.1
- **Target:** ESP32 (all variants)
- **Language:** C (components), HTML/CSS/JS (dashboard)
- **Architecture:** Modular component-based

### **Repository**
- **GitHub:** [Water-Irrigation-and-Fertilization-System](https://github.com/SirOsborn/Water-Irrigation-and-Fertilization-System)
- **License:** MIT (free for personal use)

### **Credits**
- ESP-IDF Framework by Espressif Systems
- FreeRTOS Real-Time Operating System
- cJSON library for JSON parsing

---

**Need Help?** Check the serial monitor output, review `docs/README.md`, or open an issue on GitHub! 🚀
