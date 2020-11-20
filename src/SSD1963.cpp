#include "SSD1963.hpp"

#define RST_HIGH digitalWrite(rst, HIGH)
#define RST_LOW digitalWrite(rst, LOW)

#define RS_HIGH digitalWrite(rs, HIGH)
#define RS_LOW digitalWrite(rs, LOW)

#define WR_HIGH digitalWrite(wr, HIGH)
#define WR_LOW digitalWrite(wr, LOW)

#define CS_HIGH digitalWrite(cs, HIGH)
#define CS_LOW digitalWrite(cs, LOW)

#define PULSE \
    WR_LOW;   \
    WR_HIGH;

SSD1963::SSD1963(N4094 &sreg, uint8_t rs,
                 uint8_t wr, uint8_t cs, uint8_t rst) : sreg(sreg), rs(rs), wr(wr), cs(cs), rst(rst), portait(false), rotate(false)
{
    pinMode(rs, OUTPUT);
    pinMode(wr, OUTPUT);
    pinMode(cs, OUTPUT);
    pinMode(rst, OUTPUT);
}

template <typename T>
static void swap(T &a, T &b)
{
    const auto temp = a;
    a = b;
    b = temp;
}

void SSD1963::reset()
{
    RST_HIGH;
    delay(5);
    RST_LOW;
    delay(15);
    RST_HIGH;
    delay(15);

    CS_HIGH;
    RS_HIGH;
    WR_HIGH;

    sreg.write(0xffff);
}

void SSD1963::init()
{
    reset();

    writeCommand(0xE2); //PLL multiplier, set PLL clock to 120M
    writeData(0x23);    //N=0x36 for 6.5M, 0x23 for 10M crystal
    writeData(0x02);
    writeData(0x04);
    writeCommand(0xE0); // PLL enable
    writeData(0x01);
    delay(10);
    writeCommand(0xE0);
    writeData(0x03);
    delay(10);
    writeCommand(0x01); // software reset
    delay(100);
    writeCommand(0xE6); //PLL setting for PCLK, depends on resolution
    writeData(0x04);
    writeData(0x93);
    writeData(0xE0);

    writeCommand(0xB0); //LCD SPECIFICATION
    writeData(0x00);    // 0x24
    writeData(0x00);
    writeData(0x03); //Set HDP	799
    writeData(0x1F);
    writeData(0x01); //Set VDP	479
    writeData(0xDF);
    writeData(0x00);

    writeCommand(0xB4); //HSYNC
    writeData(0x03);    //Set HT	928
    writeData(0xA0);
    writeData(0x00); //Set HPS	46
    writeData(0x2E);
    writeData(0x30); //Set HPW	48
    writeData(0x00); //Set LPS	15
    writeData(0x0F);
    writeData(0x00);

    writeCommand(0xB6); //VSYNC
    writeData(0x02);    //Set VT	525
    writeData(0x0D);
    writeData(0x00); //Set VPS	16
    writeData(0x10);
    writeData(0x10); //Set VPW	16
    writeData(0x00); //Set FPS	8
    writeData(0x08);

    writeCommand(0xBA);
    writeData(0x05); //GPIO[3:0] out 1

    writeCommand(0xB8);
    writeData(0x07); //GPIO3=input, GPIO[2:0]=output
    writeData(0x01); //GPIO0 normal

    writeCommand(0x36); //rotation
    if (portait)
    {
        if (rotate)
        {
            writeData(0x22);
        }
        else
        {
            writeData(0x21);
        }
    }
    else
    {
        if (rotate)
        {
            writeData(0x00);
        }
        else
        {
            writeData(0x03);
        }
    }
    //writeData(0x50);	// 0x50 - landscape #1 // 0x40
    //writeData(0x01); // 0x01 - landscape #2
    //writeData(0x22);

    writeCommand(0xF0); //pixel data interface
    writeData(0x03);

    delay(10);

    setXY(0, 0, 799, 479);
    writeCommand(0x29); //display on

    writeCommand(0xBE); //set PWM for B/L
    writeData(0x06);
    writeData(0x80); // Initial brightness to 100%
    writeData(0x01);
    writeData(0xF0);
    writeData(0x00);
    writeData(0x00);

    writeCommand(0xD0);
    writeData(0x0D);
    writeCommand(0x2C);

    fill(0, 0, 800, 480, 0xffff);
}

void SSD1963::setXY(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    if (portait)
    {
        ::swap(x, y);
        ::swap(w, h);
    }
    w = w - 1 + x;
    h = h - 1 + y;
    writeCommand(0x2a);
    writeData(x >> 8);
    writeData(x);
    writeData(w >> 8);
    writeData(w);
    writeCommand(0x2b);
    writeData(y >> 8);
    writeData(y);
    writeData(h >> 8);
    writeData(h);
    writeCommand(0x2c);
}

void SSD1963::fill(uint32_t x, uint32_t y, uint32_t w, uint32_t h, Color color)
{
    setXY(x, y, w, h);

    CS_LOW;
    RS_HIGH;
    WR_HIGH;
    sreg.write(color);
    for (uint32_t i = 0; i < w * h; i++)
    {
        PULSE;
    }
    CS_HIGH;
}

void SSD1963::writeCommand(uint8_t value)
{
    CS_LOW;
    RS_LOW;
    WR_HIGH;
    sreg.write(value);
    PULSE;
    CS_HIGH;
}

void SSD1963::writeData(uint8_t value)
{
    CS_LOW;
    RS_HIGH;
    WR_HIGH;
    sreg.write(value);
    PULSE;
    CS_HIGH;
}
