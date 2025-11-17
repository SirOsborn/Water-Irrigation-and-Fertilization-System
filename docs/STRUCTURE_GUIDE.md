# 🗂️ Directory Structure Guide

## Quick Reference

### Where to Find Things

| What You Want | Location | File |
|--------------|----------|------|
| **Main entry point** | `main/` | `main.c` |
| **Sensor functions** | `components/sensors/` | `sensors.c/h` |
| **Irrigation logic** | `components/irrigation/` | `irrigation_control.c/h` |
| **WiFi configuration** | `components/wifi/` | `wifi_config.c/h` |
| **Web server & API** | `components/webserver/` | `web_server.c/h` |
| **Dashboard UI** | `web/` | `dashboard.html` |
| **Documentation** | `docs/` | `README.md` |

### Editing Common Files

#### Change WiFi Credentials:
```
📁 components/wifi/wifi_config.h
```

#### Modify Dashboard:
```
📁 web/dashboard.html
   ↓ (then run)
📁 web/html_to_header.py
```

#### Adjust Sensor Pins:
```
📁 components/sensors/sensors.h
   (Update GPIO pin definitions)
```

#### Change Irrigation Logic:
```
📁 components/irrigation/irrigation_control.c
```

## Component Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                         main.c                               │
│  (Application Entry Point - Initializes Everything)         │
└────────┬─────────────┬─────────────┬─────────────┬──────────┘
         │             │             │             │
    ┌────▼────┐   ┌────▼────┐   ┌────▼────┐   ┌────▼────┐
    │ sensors │   │irrigation│   │  wifi   │   │webserver│
    │         │   │          │   │         │   │         │
    │ Read HW │◄──│ Control  │   │ Connect │◄──│ HTTP    │
    │ Sensors │   │ Pumps    │   │ Network │   │ Server  │
    └─────────┘   └──────────┘   └─────────┘   └─────────┘
                                                      │
                                                 ┌────▼────┐
                                                 │   web   │
                                                 │Dashboard│
                                                 └─────────┘
```

## Building & Flashing

### From Root Directory:
```bash
cd C:\Users\PureGoat\embedPract\final
idf.py build flash monitor -p COM5
```

### After Editing Dashboard:
```bash
python web/html_to_header.py
idf.py build flash -p COM5
```

## Benefits of This Structure

✅ **Modular** - Each component is independent  
✅ **Reusable** - Components can be used in other projects  
✅ **Maintainable** - Easy to find and update code  
✅ **Scalable** - Add new components without mess  
✅ **Professional** - Industry-standard organization  
✅ **Team-Friendly** - Multiple developers can work separately  

## Component Dependencies

Each component declares its dependencies in `CMakeLists.txt`:

```cmake
# Example: webserver depends on irrigation
idf_component_register(
    SRCS "web_server.c"
    INCLUDE_DIRS "." "../../../web"
    REQUIRES esp_http_server json irrigation
)
```

ESP-IDF automatically handles the dependency tree!

## Adding New Features

### Want to Add a Temperature Sensor?

1. **Create Component:**
   ```
   components/temperature/
   ├── temperature.c
   ├── temperature.h
   └── CMakeLists.txt
   ```

2. **Implement Functions:**
   ```c
   // temperature.c
   float read_temperature(void);
   ```

3. **Update Dependencies:**
   ```cmake
   # irrigation/CMakeLists.txt
   REQUIRES freertos sensors temperature
   ```

4. **Use in Code:**
   ```c
   #include "temperature.h"
   float temp = read_temperature();
   ```

### Want to Add MQTT?

1. **Create Component:**
   ```
   components/mqtt/
   ├── mqtt_client.c
   ├── mqtt_client.h
   └── CMakeLists.txt
   ```

2. **Link Dependencies:**
   ```cmake
   REQUIRES mqtt esp_wifi
   ```

3. **Initialize in main.c:**
   ```c
   #include "mqtt_client.h"
   mqtt_init();
   ```

## File Navigation Tips

### VS Code Quick Open:
- Press `Ctrl+P`
- Type filename: `sensors.c`
- Instantly opens the file!

### Find in Files:
- Press `Ctrl+Shift+F`
- Search across all components

### Go to Definition:
- Right-click function name
- "Go to Definition"
- Jumps to source file

## Summary

### Old Structure (Messy):
```
main/
├── main.c
├── sensors.c
├── sensors.h
├── irrigation_control.c
├── irrigation_control.h
├── wifi_config.c
├── wifi_config.h
├── web_server.c
├── web_server.h
├── dashboard.html
├── dashboard.h
└── html_to_header.py   ← Everything mixed!
```

### New Structure (Clean):
```
main/                     ← Entry point only
components/
├── sensors/             ← Hardware layer
├── irrigation/          ← Business logic
├── wifi/               ← Connectivity
└── webserver/          ← API layer
web/                    ← UI layer
docs/                   ← Documentation
```

**Much better!** 🎉

---

**Next:** Run `idf.py build` to verify the new structure works!
