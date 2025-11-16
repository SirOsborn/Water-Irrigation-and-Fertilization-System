# Smart Irrigation System with Web Dashboard

## 📁 Project Structure

```
final/
├── main/
│   ├── main.c                    # Main entry point, initializes all modules
│   ├── sensors.c/h               # Hardware initialization & sensor reading
│   ├── irrigation_control.c/h    # Irrigation logic & pump control
│   ├── wifi_config.c/h           # WiFi connection management
│   ├── web_server.c/h            # Web server & dashboard (WebSocket)
│   └── CMakeLists.txt            # Build configuration
├── CMakeLists.txt
└── sdkconfig
```

## 🗂️ File Descriptions

### **main.c**
- Entry point of the application
- Initializes NVS, WiFi, sensors, and web server
- Creates the irrigation task
- Contains global shared state variables

### **sensors.c / sensors.h**
- GPIO initialization (pumps, LEDs, sensors)
- ADC initialization for soil moisture sensor
- Functions to read soil moisture and water level sensors

### **irrigation_control.c / irrigation_control.h**
- Main irrigation logic task
- Pump control functions
- LED alert control
- Automatic irrigation based on soil moisture
- Respects manual override modes

### **wifi_config.c / wifi_config.h**
- WiFi station mode initialization
- Connection handling and retry logic
- Event handlers for WiFi events

### **web_server.c / web_server.h**
- HTTP web server with embedded HTML dashboard
- WebSocket for real-time data updates
- REST API endpoints for control:
  - `/` - Main dashboard (HTML)
  - `/ws` - WebSocket connection
  - `/api/pump` - Control pumps manually
  - `/api/auto` - Toggle automatic mode
  - `/api/settings` - Update system settings

## ⚙️ Configuration

### **WiFi Settings** (wifi_config.h)
```c
#define WIFI_SSID      "YOUR_WIFI_SSID"
#define WIFI_PASS      "YOUR_WIFI_PASSWORD"
```

### **GPIO Pins** (irrigation_control.h)
- RELAY_PUMP1: GPIO27 (Water pump)
- RELAY_PUMP2: GPIO26 (Fertilizer pump)
- WATER_LEVEL1: GPIO34 (Water tank sensor)
- WATER_LEVEL2: GPIO35 (Fertilizer tank sensor)
- SOIL_MOISTURE: GPIO36/VP (Soil moisture sensor)
- ALERT_LED_WATER: GPIO22 (Water tank empty LED)
- ALERT_LED_FERT: GPIO23 (Fertilizer tank empty LED)

## 🚀 How to Build & Flash

1. **Configure WiFi credentials** in `wifi_config.h`
2. **Build the project:**
   ```bash
   idf.py build
   ```
3. **Flash to ESP32:**
   ```bash
   idf.py -p COM5 flash monitor
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
- Real-time updates via WebSocket

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

## 📝 Notes

- WebSocket provides real-time updates every 2 seconds
- Settings are stored in RAM (reset on reboot)
- System logs all actions to serial monitor
- Dashboard is mobile-responsive

## 🌟 Future Enhancements

- Save settings to NVS (persistent storage)
- Add scheduling (water at specific times)
- Email/SMS notifications
- Data logging and charts
- OTA (Over-The-Air) updates
