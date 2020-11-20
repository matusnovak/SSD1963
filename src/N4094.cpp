#include "N4094.hpp"

N4094::N4094(SPIClass &spi, uint8_t cs) : spi(spi), cs(cs)
{
}

void N4094::init()
{
    pinMode(cs, OUTPUT);
    delay(10);
}

void N4094::write(uint16_t value)
{
    static uint8_t dummy[2] = {};
    digitalWrite(cs, HIGH);
    //spi.transferBytes(reinterpret_cast<uint8_t *>(&value), dummy, sizeof(dummy));
    spi.transfer((value & 0xff00) >> 8);
    spi.transfer(value & 0xff);
    digitalWrite(cs, LOW);
}
