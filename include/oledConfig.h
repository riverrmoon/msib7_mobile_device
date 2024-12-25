#ifndef OLED_CONFIG_H
#define OLED_CONFIG_H
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Ukuran layar OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

class MyOLED
{
public:
    MyOLED(uint8_t i2cAddress = 0x3C)
        : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1), i2cAddress(i2cAddress) {}

    bool begin()
    {
        if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
        {
            return false; // Gagal menginisialisasi
        }
        display.clearDisplay();
        display.display();
        return true; // Berhasil
    }

    void displayText(const char *text, uint8_t textSize = 2, uint8_t YCursor = 0, uint8_t cls = 0)
    {
        if (cls == 0)
        {
            display.clearDisplay();
        }
        display.setTextSize(textSize);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, YCursor); // x dari 0-127 dan y dari 0-63
        display.println(text);
        display.display();
    }

private:
    Adafruit_SSD1306 display;
    uint8_t i2cAddress;
};

#endif
