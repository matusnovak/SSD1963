#include "HR2046.hpp"

#define CMD_START 0x80
#define CMD_12BIT 0x00
#define CMD_8BIT 0x04
#define CMD_DFR 0x00
#define ADS_SER (1 << 2)

#define CMD_VAUX (0x6 << 4)
#define CMD_POS_X (0x5 << 4)
#define CMD_POS_Y (0x1 << 4)
#define CMD_POS_Z1 (0x3 << 4)
#define CMD_POS_Z2 (0x4 << 4)

#define CMD_ADC_ON 0x1
#define CMD_REF_ON 0x2
#define CMD_ALL_ON 0x3

#define REF_ON (CMD_START | CMD_12BIT | CMD_DFR | CMD_POS_X | CMD_ADC_ON | CMD_REF_ON)
#define PWRDOWN (CMD_START | CMD_12BIT | CMD_DFR | CMD_POS_X)

HR2046::HR2046(SPIClass &spi, uint8_t cs) : spi(spi), cs(cs)
{
}

void HR2046::init()
{
    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH);

    (void)readRaw(CMD_POS_X);
    powerDown();
}

void HR2046::powerDown()
{
    digitalWrite(cs, LOW);
    spi.transfer(PWRDOWN);
    digitalWrite(cs, HIGH);
}

uint16_t HR2046::readRaw(uint8_t target)
{
    uint8_t buf[3] = {0, 0, 0};
    uint8_t out[3] = {};
    buf[0] = CMD_START | CMD_12BIT | CMD_DFR | target | CMD_ALL_ON;

    digitalWrite(cs, LOW);
    spi.transferBytes(buf, out, 3);
    digitalWrite(cs, HIGH);
    return ((out[1] << 8) | out[2]) >> 3;
}

template <typename T>
static T median(T a, T b, T c)
{
    if ((a <= b) && (b <= c))
        return b; // a b c
    if ((a <= c) && (c <= b))
        return c; // a c b
    if ((b <= a) && (a <= c))
        return a; // b a c
    if ((b <= c) && (c <= a))
        return c; // b c a
    if ((c <= a) && (a <= b))
        return a; // c a b
    return b;     // c b a
}

void HR2046::readRaw(uint16_t &x, uint16_t &y, uint32_t &z)
{
    uint32_t values[3][3] = {0};

    // Skip first read and then do 3 reads
    for (int i = -1; i < 3; i++)
    {
        uint16_t raw[4];

        raw[0] = readRaw(CMD_POS_X);
        raw[1] = readRaw(CMD_POS_Y);
        raw[2] = readRaw(CMD_POS_Z1);
        raw[3] = readRaw(CMD_POS_Z2);

        if (i >= 0)
        {
            auto zt = (raw[2] + 4095) - raw[3];
            values[0][i] = raw[0];
            values[1][i] = raw[2];
            values[2][i] = zt;

            // printf("Raw: [%d, %d, %d]\n", raw[0], raw[1], zt);
        }

        delay(1);
    }

    x = ::median(values[0][0], values[0][1], values[0][2]);
    y = ::median(values[1][0], values[1][1], values[1][2]);
    z = ::median(values[2][0], values[2][1], values[2][2]);

    delay(1);

    powerDown();
}
