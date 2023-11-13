#pragma once
#include <Arduino.h>
#include <GyverIO.h>

#include "utils/SegDuty.h"

// задержка клока в мкс
#define DISP595_CLK_DELAY 0

class Driver595 : public SegDuty {
   public:
    Driver595(uint8_t DIO, uint8_t SCLK, uint8_t RCLK) {
        _dio = DIO;
        _clk = SCLK;
        _latch = RCLK;
        pinMode(_dio, OUTPUT);
        pinMode(_clk, OUTPUT);
        pinMode(_latch, OUTPUT);
    }

    // управление питанием (true вкл, false выкл)
    void power(bool state) {
        if (!state) write(0xff, 0);
        _power = state;
    }

   protected:
    void write(uint8_t data, uint8_t mask) {
        uint8_t packet[2] = {data, mask};
        gio::shift::send(_dio, _clk, MSBFIRST, packet, 2, DISP595_CLK_DELAY);
        gio::high(_latch);
#if (DISP595_CLK_DELAY > 0)
        delayMicroseconds(DISP595_CLK_DELAY);
#endif
        gio::low(_latch);
    }

    void tick(uint8_t* buffer, uint8_t size) {
        if (!_power) return;

        // no bright
        if (!getMaxDuty()) {
            if (++_count >= size) _count = 0;
            write(buffer[_count], (1 << _count));
            return;
        }

        // bright on
        uint8_t br = SegDuty::tick();
        if (!br) return;

        if (br == 1) {  // next + on
            if (++_count >= size) _count = 0;
            write(buffer[_count], (1 << _count));
        } else {  // all off
            write(0xff, 0);
        }
    }

   private:
    uint8_t _dio, _clk, _latch;
    uint8_t _count = 0;
    bool _power = 1;
};