# Bug Fix: Manual Control Logic (v2.1)

## Issues Fixed

### 1. **Manual Pump Control Being Overridden**
**Problem**: When user manually turned ON a pump, the automatic irrigation task would turn it OFF after ~5 seconds.

**Root Cause**: 
- Manual flag was cleared when pump was turned OFF manually
- Automatic task would run every 5 seconds and could control pumps even in manual mode
- Manual control was not persistent

**Solution**:
- Changed `web_server.c` pump handler to **always** set manual flag when user controls a pump (ON or OFF)
- Manual flag now persists until user explicitly enables auto mode
- Automatic irrigation task checks for manual flags before controlling pumps

### 2. **Auto Mode Turning Back On Automatically**
**Problem**: After disabling auto mode, it would turn back on by itself after a few minutes.

**Root Cause**: 
- `auto_mode` variable initialized to `true` in main.c
- No state persistence (would reset on any crash/restart)
- Dashboard polling every 2 seconds syncs checkbox with server state

**Solution**:
- Modified auto mode handler to clear manual flags when auto mode is enabled
- This ensures clean transition between manual and auto modes
- State now properly maintained during runtime

### 3. **State Synchronization Issues**
**Problem**: Dashboard not properly reflecting current system state.

**Root Cause**:
- Manual flags not properly communicated between web server and irrigation task
- Race conditions between automatic task and manual control

**Solution**:
- Simplified logic: automatic task only runs if `auto_mode == true AND pump1_manual == false AND pump2_manual == false`
- Manual control always takes priority over automatic control
- Clear logging messages indicate current mode

## Code Changes

### `components/webserver/web_server.c`

**Before** (pump handler):
```c
if (state) {
    pump1_manual = true;  // Enable manual mode
} else {
    pump1_manual = false; // Disable manual mode (return to auto) <-- PROBLEM!
}
```

**After** (pump handler):
```c
// Manual mode stays ON regardless of pump state (ON/OFF)
// User must use auto mode toggle to return to automatic control
pump1_manual = true;  // Always enable manual mode when user controls pump
```

**Before** (auto handler):
```c
auto_mode = cJSON_GetObjectItem(json, "enabled")->valueint;
ESP_LOGI(TAG, "Auto mode: %s", auto_mode ? "ENABLED" : "DISABLED");
```

**After** (auto handler):
```c
auto_mode = cJSON_GetObjectItem(json, "enabled")->valueint;

// When auto mode is enabled, clear manual flags to allow automatic control
if (auto_mode) {
    pump1_manual = false;
    pump2_manual = false;
    ESP_LOGI(TAG, "Auto mode ENABLED - clearing manual control flags");
} else {
    ESP_LOGI(TAG, "Auto mode DISABLED - manual control only");
}
```

### `components/irrigation/irrigation_control.c`

**Before**:
```c
if (auto_mode) {
    // Complex logic with individual pump checks
    if (!pump1_manual) {
        // control pump1
    }
    if (!pump2_manual) {
        // control pump2
    }
}
```

**After**:
```c
// Automatic irrigation logic (only if auto mode enabled AND not in manual control)
if (auto_mode && !pump1_manual && !pump2_manual) {
    // Simplified automatic control for both pumps
    // Manual control has absolute priority
}
```

## Usage Instructions

### To Use Manual Control:
1. **Disable** automatic mode using the toggle switch
2. Click "Turn ON" or "Turn OFF" buttons for individual pumps
3. Pumps will stay in the state you set them to
4. Automatic irrigation will not interfere

### To Return to Automatic Mode:
1. **Enable** automatic mode using the toggle switch
2. This automatically clears manual control flags
3. System resumes automatic irrigation based on soil moisture

## Testing Checklist

- [ ] Turn off auto mode
- [ ] Manually turn ON water pump
- [ ] Wait 10+ seconds (multiple irrigation task cycles)
- [ ] Verify pump stays ON (not turned off by automatic task)
- [ ] Manually turn OFF water pump
- [ ] Verify pump stays OFF
- [ ] Turn on auto mode
- [ ] Verify system returns to automatic control
- [ ] Verify auto mode stays enabled (doesn't revert)

## Notes

- **State Persistence**: Current implementation does not persist state across reboots. On restart, system defaults to auto mode enabled.
- **Future Enhancement**: Could add NVS storage to persist auto_mode, manual flags across reboots.
- **Priority**: Manual control ALWAYS takes priority over automatic control when both are active.

---
**Date**: November 17, 2025  
**Version**: 2.1  
**Previous Version**: 2.0 (Modular restructure)
