#pragma once

#include "N4094.hpp"
#include "Color.hpp"

class SSD1963
{
public:
    explicit SSD1963(N4094 &sreg, uint8_t rs, uint8_t wr, uint8_t cs, uint8_t rst);
    ~SSD1963() = default;

    void init();
    void reset();
    void setXY(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    void fill(uint32_t x, uint32_t y, uint32_t w, uint32_t h, Color color);

private:
    void writeCommand(uint8_t value);
    void writeData(uint8_t value);

    N4094 &sreg;
    uint8_t rs;
    uint8_t wr;
    uint8_t cs;
    uint8_t rst;

    bool portait;
    bool rotate;
};
