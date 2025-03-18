/**
 * @file touch_draw_v3.ino
 * @brief Touch Drawing Application for ESP32 with TFT Display
 * 
 * This application implements a simple drawing program using a TFT display
 * and touch input. It supports both NS2009 and FT6236 touch controllers.
 * The program allows users to draw on the screen with different colors
 * selected from a color palette at the top of the display.
 * 
 * This code is an adaptation from the Makerfabs/Project_Touch-Screen-Camera
 * touch_draw_v2 project: https://github.com/Makerfabs/Project_Touch-Screen-Camera
 * 
 * @author Lucienne Swart
 * @date March 18, 2025
 * @version 1.0
 * @license MIT
 */

#include <Wire.h>
#include "SPI.h"
#include <LovyanGFX.hpp>
#include "makerfabs_pin.h"

//Choice your touch IC
//#define NS2009_TOUCH
#define FT6236_TOUCH

#ifdef NS2009_TOUCH
#include "NS2009.h"
const int i2c_touch_addr = NS2009_ADDR;
#endif

#ifdef FT6236_TOUCH
#include "FT6236.h"
const int i2c_touch_addr = TOUCH_I2C_ADD;
#endif

//SPI control
#define SPI_ON_TFT digitalWrite(LCD_CS, LOW)
#define SPI_OFF_TFT digitalWrite(LCD_CS, HIGH)

/**
 * @brief Custom LGFX display driver class for ILI9488 TFT panel
 * 
 * This class configures the SPI bus and display panel settings for the ILI9488 TFT.
 * It handles the initialization of the display with proper timing and pin configurations.
 */
class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9488 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

public:
    LGFX(void)
    {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = HSPI_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 60000000;
            cfg.freq_read = 16000000;
            cfg.spi_3wire = false;
            cfg.use_lock = true;
            cfg.dma_channel = 1;
            cfg.pin_sclk = LCD_SCK;
            cfg.pin_mosi = LCD_MOSI;
            cfg.pin_miso = LCD_MISO;
            cfg.pin_dc = LCD_DC;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs = LCD_CS;
            cfg.pin_rst = LCD_RST;
            cfg.pin_busy = -1;
            cfg.memory_width = LCD_WIDTH;
            cfg.memory_height = LCD_HEIGHT;
            cfg.panel_width = LCD_WIDTH;
            cfg.panel_height = LCD_HEIGHT;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = true;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;
            _panel_instance.config(cfg);
        }
        setPanel(&_panel_instance);
    }
};

static LGFX tft;
static lgfx::LGFX_Sprite sprite(&tft);

int last_pos[2] = {0, 0};
int draw_color = TFT_WHITE;

/**
 * @brief Initializes the hardware and sets up the display
 * 
 * This function:
 * - Initializes serial communication
 * - Sets up I2C for touch input
 * - Configures SPI for display communication
 * - Initializes the TFT display
 * - Creates the color palette at the top of the screen
 */
void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ; // Leonardo: wait for serial monitor
    Serial.println("\n FT6236 test");

    Wire.begin(I2C_SDA, I2C_SCL);
    byte error, address;

    Wire.beginTransmission(i2c_touch_addr);
    error = Wire.endTransmission();

    if (error == 0)
    {
        Serial.print("I2C device found at address 0x");
        Serial.print(i2c_touch_addr, HEX);
        Serial.println("  !");
    }
    else if (error == 4)
    {
        Serial.print("Unknown error at address 0x");
        Serial.println(i2c_touch_addr, HEX);
    }

    pinMode(LCD_CS, OUTPUT);
    SPI_OFF_TFT;
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    //TFT(SPI) init
    SPI_ON_TFT;
    tft.init();
    tft.fillScreen(TFT_BLACK);
    tft.fillRect(0, 0, 80, 40, TFT_RED);
    tft.fillRect(80, 0, 80, 40, TFT_GREEN);
    tft.fillRect(160, 0, 80, 40, TFT_BLUE);
    tft.fillRect(240, 0, 80, 40, TFT_YELLOW);
    SPI_OFF_TFT;
}

/**
 * @brief Main program loop
 * 
 * This function:
 * - Reads touch input position
 * - Handles color selection from the palette
 * - Draws points on the screen based on touch position
 */
void loop()
{
    int pos[2] = {0, 0};
#ifdef NS2009_TOUCH
    Serial.println(ns2009_pos(pos));
#endif
#ifdef FT6236_TOUCH
    ft6236_pos(pos);
#endif
    Serial.printf("%d,%d\n", pos[0], pos[1]);
    if (0 < pos[1] && pos[1] < 40)
    {
        if (0 < pos[0] && pos[0] < 80)
        {
            draw_color = TFT_RED;
        }
        else if (80 < pos[0] && pos[0] < 160)
        {
            draw_color = TFT_GREEN;
        }
        else if (160 < pos[0] && pos[0] < 240)
        {
            draw_color = TFT_BLUE;
        }
        else if (240 < pos[0] && pos[0] < 320)
        {
            draw_color = TFT_YELLOW;
        }
    }
    else
    {
        tft.fillRect(pos[0], pos[1], 3, 3, draw_color);
    }
}

/**
 * @brief Filters touch input to reduce jitter
 * 
 * @param last_pos Previous touch position [x, y]
 * @param pos Current touch position [x, y]
 * @param level Threshold level for filtering
 * @return int 1 if position is valid, 0 if filtered out
 */
int filter(int last_pos[2], int pos[2], int level)
{
    int temp = (last_pos[0] - pos[0]) * (last_pos[0] - pos[0]) + (last_pos[1] - pos[1]) * (last_pos[1] - pos[1]);
    last_pos[0] = pos[0];
    last_pos[1] = pos[1];
    if (temp > level)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}