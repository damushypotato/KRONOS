#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGPSPlus.h>
#include "bitmaps.h"
#include "cat.h"
#include "SDUtil.h"

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    (-1)
Adafruit_SSD1306* display;

uint32_t frameCount = 0;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// GPS
TinyGPSPlus gps;
#define GPS_SERIAL Serial1
#define GPS_BAUD 9600
const unsigned char UBLOX_INIT[] PROGMEM = {
     0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12, //(10Hz)
     // 0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A, //(5Hz)
     //   0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39 //(1Hz)
};

// SD
#define SD_CS 10
bool serial_only = false;

void setup() {
    // Init Serial
    Serial.begin(115200);
    display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    // Init OLED
    Wire.begin();
    if (!display->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        // ReSharper disable once CppDFAEndlessLoop
        for (;;);
    }
    display->clearDisplay();
    display->display();

    // Init LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("SPIN");

    // Init SD
    frameCount = setupSD(SD_CS);

    if (!frameCount)
    {
        // ReSharper disable once CppDFAEndlessLoop
        for (;;);
    }

    // Init GPS
    GPS_SERIAL.begin(GPS_BAUD);

    display->clearDisplay();
    display->drawBitmap(0,0 , epd_bitmap_k3r, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    display->display();
}

void loop() {
    static uint32_t frameIndex = 0;
    static unsigned long startTime = millis();

    uint8_t* framePtr = nullptr;

    if (loadFrame(&framePtr, frameIndex)) {
        display->clearDisplay();
        display->drawBitmap(0, 0, framePtr, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
        display->display();

        uint16_t frameDelay = loadDelay(frameIndex);
        const unsigned long elapsed = millis() - startTime;

        if (elapsed < frameDelay) {
            delay(frameDelay - elapsed);
        }

        startTime = millis();

        Serial.print("Opms: ");
        Serial.print(elapsed);
        Serial.print(", Frms: ");
        Serial.println(frameDelay);
    }

    frameIndex = (frameIndex + 1) % frameCount;
}

