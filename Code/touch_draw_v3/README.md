# Touch Draw V3

A drawing application for ESP32 with TFT display and touch input support. This project allows users to draw on a TFT display using either a capacitive (FT6236) or resistive (NS2009) touch controller.

This code is an adaptation from the [Makerfabs/Project_Touch-Screen-Camera](https://github.com/Makerfabs/Project_Touch-Screen-Camera) touch_draw_v2 project.

**Author:** Lucienne Swart  
**Date:** March 18, 2025  
**Version:** 1.0  
**License:** MIT

## Features

- Support for both FT6236 (capacitive) and NS2009 (resistive) touch controllers
- Color palette for drawing with different colors
- Real-time touch input processing
- Jitter filtering for smooth drawing
- Compatible with ILI9488 TFT display (320x480)

## Hardware Requirements

- ESP32 development board
- ILI9488 TFT display (320x480)
- FT6236 or NS2009 touch controller
- I2C and SPI connections as defined in `makerfabs_pin.h`

## Pin Configuration

The pin configuration is defined in `makerfabs_pin.h`:

- I2C pins for touch controller:
  - SDA: GPIO26
  - SCL: GPIO27

- SPI pins for display:
  - MOSI: GPIO13
  - MISO: GPIO12
  - SCK: GPIO14
  - CS: GPIO15
  - RST: GPIO26
  - DC: GPIO33

## Building and Running

1. Install the required libraries:
   - LovyanGFX
   - Wire
   - SPI

2. Select your touch controller by uncommenting the appropriate define in `touch_draw_v3.ino`:
   ```cpp
   //#define NS2009_TOUCH
   #define FT6236_TOUCH
   ```

3. Upload the code to your ESP32 board

## Usage

1. Power on the device
2. The display will show a color palette at the top
3. Touch a color to select it
4. Draw on the screen by touching and dragging
5. The selected color will be used for drawing

## Documentation

The project is documented using Doxygen. To generate the documentation:

1. Install Doxygen
2. Run `doxygen Doxyfile` in the project directory
3. Open `docs/html/index.html` in a web browser

## License

This project is licensed under the MIT License - see the LICENSE file for details. 