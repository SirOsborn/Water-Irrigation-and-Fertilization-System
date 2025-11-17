# 🌱 Smart Irrigation & Fertilization System

ESP32-based IoT irrigation system with web dashboard for remote monitoring and control.

## 📁 Project Structure

```
final/
├── main/                          # Application entry point
│   ├── main.c                    # Main application
│   └── CMakeLists.txt            # Build configuration
│
├── components/                    # Reusable components
│   ├── sensors/                  # Hardware abstraction layer
│   │   ├── sensors.c            # Sensor reading functions
│   │   ├── sensors.h            # Sensor API
│   │   └── CMakeLists.txt       # Component build
│   │
│   ├── irrigation/              # Irrigation control logic
│   │   ├── irrigation_control.c # Auto/manual irrigation
│   │   ├── irrigation_control.h # Irrigation API
│   │   └── CMakeLists.txt       # Component build
│   │
│   ├── wifi/                    # WiFi connectivity
│   │   ├── wifi_config.c       # WiFi station mode
│   │   ├── wifi_config.h       # WiFi API
│   │   └── CMakeLists.txt      # Component build
│   │
│   └── webserver/               # HTTP server
│       ├── web_server.c        # REST API endpoints
│       ├── web_server.h        # Web server API
│       └── CMakeLists.txt      # Component build
│
├── web/                         # Web dashboard
│   ├── dashboard.html          # Main UI (EDIT THIS)
│   ├── dashboard.h             # Auto-generated C header
│   └── html_to_header.py       # HTML → C converter
│
├── docs/                        # Documentation
│   └── README.md               # This file
│
├── build/                       # Build output (auto-generated)
├── CMakeLists.txt              # Root project configuration
└── sdkconfig                    # ESP-IDF configuration
```

## 🔧 Hardware Connections

| Component | ESP32 Pin | Type | Notes |
|-----------|-----------|------|-------|
| **Water Pump Relay** | GPIO 27 | Output | Active LOW |
| **Fertilizer Pump Relay** | GPIO 26 | Output | Active LOW |
| **Soil Moisture Sensor** | GPIO 36 (ADC1_CH0) | Analog Input | 0-4095 range |
| **Water Tank Sensor** | GPIO 34 | Digital Input | HIGH = has water |
| **Fertilizer Tank Sensor** | GPIO 35 | Digital Input | HIGH = has liquid |
| **Water Alert LED** | GPIO 22 | Output | HIGH = tank empty |
| **Fertilizer Alert LED** | GPIO 23 | Output | HIGH = tank empty |

### Circuit Requirements:
- 5V relay modules (active-low trigger)
- Analog soil moisture sensor
- Digital water level sensors
- LEDs with appropriate resistors (220Ω recommended)

## 🚀 Quick Start

### Prerequisites
- ESP-IDF v5.5.1 installed
- Python 3.x
- ESP32 board
- USB cable

### 1. Configure WiFi
Edit `components/wifi/wifi_config.h`:
```c
#define WIFI_SSID      "YourWiFiName"
#define WIFI_PASS      "YourPassword"
```

### 2. Build & Flash
```bash
cd final
idf.py build flash -p COM5 monitor
```

### 3. Access Dashboard
Once WiFi connects, note the IP address from serial monitor:
```
I (xxx) WIFI: 📡 Got IP: 192.168.1.100
```

Open browser and go to: `http://192.168.1.100`

## 📱 Web Dashboard Features

### Real-Time Monitoring
- 💧 **Soil Moisture** - ADC reading (0-4095)
- 🚰 **Water Tank Status** - FULL/EMPTY
- 🧪 **Fertilizer Tank Status** - FULL/EMPTY
- Updates every 2 seconds via HTTP polling

### Manual Control
- Toggle automatic/manual mode
- Individual pump ON/OFF buttons
- Pump status indicators

### System Settings
- Soil moisture threshold adjustment
- Pump duration configuration
- Check interval timing
- Persistent settings

## 🔄 Development Workflow

### Modifying the Dashboard

1. **Edit HTML** (`web/dashboard.html`):
   ```html
   <h1>🌱 My Custom Title</h1>
   ```

2. **Convert to C header**:
   ```bash
   python web/html_to_header.py
   ```

3. **Rebuild & Flash**:
   ```bash
   idf.py build flash -p COM5
   ```

### Adding New Components

1. Create directory: `components/mycomponent/`
2. Add files: `mycomponent.c`, `mycomponent.h`
3. Create `CMakeLists.txt`:
   ```cmake
   idf_component_register(
       SRCS "mycomponent.c"
       INCLUDE_DIRS "."
       REQUIRES <dependencies>
   )
   ```
4. Add to main's `CMakeLists.txt`: `REQUIRES mycomponent`

## 📊 System Behavior

### Automatic Mode (Default)
- Checks soil moisture every 5 seconds
- If moisture > 2800 (dry):
  - Activates water pump for 3 seconds
  - Waits 1 second
  - Activates fertilizer pump for 1.5 seconds
- Skips pumps in manual mode
- Respects tank levels (won't pump if empty)

### Manual Mode
- Click "Turn ON" → Pump stays ON indefinitely
- Click "Turn OFF" → Pump turns OFF
- Automatic control paused while manual active
- Pumps can be controlled independently

### Alert System
- LEDs indicate empty tanks
- Dashboard shows tank status
- Automatic irrigation skipped if tanks empty

## 🛠️ Configuration

### Sensor Thresholds
Edit in dashboard or modify `main.c`:
```c
int soil_dry_threshold = 2800;  // Higher = drier soil
```

### Pump Timings
```c
int pump_duration_ms = 3000;      // Water pump (3s)
int fertilizer_duration_ms = 1500; // Fertilizer pump (1.5s)
int check_interval_ms = 5000;     // Check every 5s
```

### WiFi Settings
```c
#define WIFI_MAXIMUM_RETRY  5
```

## 📡 API Endpoints

All endpoints return JSON responses:

- `GET /` - Serve HTML dashboard
- `GET /api/data` - Get all sensor data
- `POST /api/pump` - Control pumps manually
  ```json
  {"pump": 1, "state": true}
  ```
- `POST /api/auto` - Toggle automatic mode
  ```json
  {"enabled": true}
  ```
- `POST /api/settings` - Update configuration
  ```json
  {
    "threshold": 2800,
    "pump_duration": 3000,
    "fert_duration": 1500,
    "interval": 5000
  }
  ```

## 🐛 Troubleshooting

### WiFi Won't Connect
- Check SSID/password in `wifi_config.h`
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)
- Move ESP32 closer to router
- Check serial monitor for error messages

### Sensors Read Zero
- Verify hardware connections
- Check power supply to sensors
- Test with multimeter

### Dashboard Not Loading
- Verify ESP32 has IP address
- Check firewall settings
- Try `http://esp32-ip-address:80`

### Pumps Won't Turn On
- Check relay power supply (5V)
- Verify relay module type (active-low/high)
- Test relays with external power
- Check GPIO connections

## 📝 Component Dependencies

```
main
├── sensors
│   └── driver, esp_adc
├── irrigation
│   └── freertos, sensors
├── wifi
│   └── esp_wifi, esp_netif, nvs_flash
└── webserver
    └── esp_http_server, json, irrigation
```

## 🔐 Security Notes

⚠️ **Current Implementation:**
- No authentication/authorization
- Local network only
- HTTP (not HTTPS)

**For Production:**
- Add login system
- Implement HTTPS
- Use secure WiFi (WPA3)
- Enable ESP32 secure boot

## 📈 Future Enhancements

- [ ] Data logging to SD card
- [ ] MQTT cloud integration
- [ ] Mobile app (React Native)
- [ ] Weather API integration
- [ ] Soil pH sensor support
- [ ] Multi-zone irrigation
- [ ] Scheduling/timers
- [ ] Email/SMS alerts

## 📄 License

MIT License - Free for personal and commercial use

## 👨‍💻 Author

**Water Irrigation & Fertilization System**  
ESP32-IDF v5.5.1  
Repository: [Water-Irrigation---Fertilization-System](https://github.com/SirOsborn/Water-Irrigation---Fertilization-System)

## 🙏 Credits

- ESP-IDF Framework by Espressif
- FreeRTOS
- cJSON library

---

**Need Help?** Check the serial monitor output or open an issue on GitHub!
