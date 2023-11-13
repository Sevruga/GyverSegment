#pragma once
#include <Arduino.h>
#include <GyverIO.h>

#include "Driver595.h"
#include "utils/SegBuffer.h"

class Disp595_8 : public Driver595, public SegBuffer {
   public:
    // пины DIO, SCLK, RCLK
    Disp595_8(uint8_t DIO, uint8_t SCLK, uint8_t RCLK) : Driver595(DIO, SCLK, RCLK), SegBuffer(buffer, 8, 1) {
        update();
    }

    // обновить дисплей
    void update() {
        for (uint8_t i = 0; i < 4; i++) {
            _buf[i] = ~buffer[i + 4];
            _buf[i + 4] = ~buffer[i];
        }
    }

    // тикер динамической индикации, вызывать в loop постоянно или по таймеру
    void tick() {
        Driver595::tick(_buf, 8);
    }

    uint8_t buffer[8] = {0};

   private:
    uint8_t _buf[8] = {0};
};