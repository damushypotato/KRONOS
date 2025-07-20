//
// Created by k3arj on 19/07/2025.
//

#ifndef SDUTIL_H
#define SDUTIL_H

uint32_t setupSD(int SD_CS = 10);

bool loadFrame(uint8_t** buffer, const uint16_t index);

uint16_t loadDelay(uint16_t index);

#endif //SDUTIL_H
