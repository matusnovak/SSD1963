#pragma once

#include <Arduino.h>
#include <SPI.h>

class HR2046
{
public:
    explicit HR2046(SPIClass &spi, uint8_t cs);
    ~HR2046() = default;

    void init();
    void powerDown();
    uint16_t readRaw(uint8_t target);
    void readRaw(uint16_t &x, uint16_t &y, uint32_t &z);

private:
    SPIClass &spi;
    uint8_t cs;
};
