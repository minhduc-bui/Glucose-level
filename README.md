# Escreen SPO2 Monitor for ESP32

This project demonstrates how to build an embedded SpO₂ (oxygen saturation) and heart rate monitor using an ESP32, Escreen display, and the MAX30102 pulse oximeter sensor. It leverages the LVGL graphics library for a rich UI, and uses an EEZ Studio .eez project for designing screens. The firmware is based on PlatformIO and FreeRTOS.

## Features
- Real-time SpO₂ and heart rate measurement with MAX30102 sensor
- Custom 25 Hz sampling rate for increased measurement accuracy
- Responsive touchscreen UI built with LVGL
- Screen designs and layouts managed in EEZ Studio (.eez files)
- Dual-task architecture using FreeRTOS:
  - **lvgl_task**: handles LVGL tick, input, and drawing updates
  - **spo2_task**: reads sensor data, processes signals, and computes SpO₂ and heart rate

## Hardware
- Escreen (e.g., 2.8" TFT touch display) 
- MAX30102 pulse oximeter and heart-rate sensor module
- Wiring:
  - `SDA` → ESP32 I2C SDA pin
  - `SCL` → ESP32 I2C SCL pin
  - `VIN` → 3.3 V
  - `GND` → GND

## Software Requirements
- [PlatformIO](https://platformio.org/) (installed in VS Code or CLI)
- ESP32 Arduino framework
- LVGL graphics library
- MAX30102 driver (modified SparkFun library)
- FreeRTOS (bundled with ESP32 Arduino core)
- EEZ Studio (for editing `.eez` UI design files)

## Getting Started
1. Clone this repository:
   ```bash
   git clone https://github.com/leybme/SPO2_Escreen.git
   cd SPO2_Escreen
   ```
2. Open in VS Code with PlatformIO extension.
3. Adjust display settings in `platformio.ini` if needed (pin mapping, screen resolution).
4. Build and upload firmware:
   ```bash
   pio run --target upload
   ```
5. Open serial monitor to view log output:
   ```bash
   pio device monitor
   ```

## Project Structure
```
include/           # Header files for tasks and drivers
lib/               # Third-party libraries (LVGL, MAX30102, Escreen)
src/               # Application source code
  ├── main.cpp     # Entry point, RTOS init
  ├── lvgl_task.cpp# LVGL update loop task
  ├── spo2_task.cpp# Sensor read & signal processing task
  ├── ui/          # Generated UI code from EEZ Studio (.c/.h files)
test/              # Unit tests and examples (optional)
platformio.ini     # PlatformIO project configuration
```

## UI Design
All screen layouts and styles are created in EEZ Studio and exported to the `ui/` folder. Modify `.eez` files in EEZ Studio and re-export C code to update graphics.

## Sampling & Signal Processing
- Sensor driver is modified from the SparkFun MAX3010x library to enforce a precise 25 Hz sampling rate, which is critical for accurate SpO₂ and heart rate calculations.
- Raw data is filtered and processed in `spo2_task.cpp`. Further improvement on SpO₂ algorithm accuracy is a work in progress.

## License
This project is released under the MIT License. See [LICENSE](./LICENSE) for details.
