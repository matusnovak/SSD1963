#pragma once

#include <Arduino.h>
#include <SPI.h>

class N4094
{
public:
    explicit N4094(SPIClass &spi, uint8_t cs);
    ~N4094() = default;

    void init();
    void write(uint16_t value);

private:
    SPIClass &spi;
    uint8_t cs;
};
