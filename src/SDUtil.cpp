//
// Created by k3arj on 19/07/2025.
//

#include "Arduino.h"
#include <SdFat.h>
#include "SDUtil.h"

#define FRAME_SIZE 512

// char filename[] = "LOG0000.CSV";
SdFat SD;

SdFile frameFile;
uint8_t frameBuffer[FRAME_SIZE];

SdFile delayFile;

uint32_t setupSD(const int SD_CS) {
    if (!SD.begin(SD_CS, SD_SCK_MHZ(16))) {
        Serial.println(F("SD init failed"));
        return false;
    }

    if (!frameFile.open("cat.bin", O_READ)) {
        Serial.println(F("cat.bin missing!"));
        return false;
    }

    if (!delayFile.open("dcat.bin", O_READ)) {
        Serial.println(F("dcat.bin missing!"));
    }

    return frameFile.fileSize() / FRAME_SIZE;
}

bool loadFrame(uint8_t** buffer, const uint16_t index) {
    uint32_t offset = (uint32_t)index * FRAME_SIZE;
    if (!frameFile.seekSet(offset)) {
        return false;
    }

    size_t bytesRead = frameFile.read(frameBuffer, FRAME_SIZE);
    if (bytesRead != FRAME_SIZE) {
        return false;
    }

    *buffer = frameBuffer;
    return true;
}

uint16_t loadDelay(uint16_t index) {
    if (!delayFile) {
        return 0;
    }

    uint32_t offset = (uint32_t)index * sizeof(uint16_t);
    if (!delayFile.seekSet(offset)) {
        return 0;
    }

    uint8_t buf[2];
    if (delayFile.read(buf, 2) != 2) {
        return 0;
    }

    // Little-endian 16-bit
    return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}

