#include <Arduino.h>
#include <WiFi.h>
#include "N4094.hpp"
#include "SSD1963.hpp"
#include "HR2046.hpp"

inline float randf()
{
  return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

inline float rand(int min, int max)
{
  return min + rand() % (max + 1 - min);
}

#define LCD_RS GPIO_NUM_2
#define LCD_WR GPIO_NUM_4
#define LCD_CS GPIO_NUM_27
#define LCD_RST GPIO_NUM_25

SPIClass &vspi = SPI;
SPIClass hspi(HSPI);
N4094 sreg(hspi, GPIO_NUM_15);
HR2046 touch(vspi, GPIO_NUM_5);
SSD1963 display(sreg, LCD_RS, LCD_WR, LCD_CS, LCD_RST);

static bool touchFlag = false;

void IRAM_ATTR touchIrq()
{
  if (!touchFlag)
  {
    touchFlag = true;
  }
}

void setup()
{
  Serial.begin(115200);
  printf("\n");

  vspi.begin();
  vspi.setFrequency(1000000);

  hspi.begin();
  hspi.setFrequency(10000000);

  sreg.init();
  display.init();
  touch.init();

  pinMode(GPIO_NUM_34, INPUT_PULLUP);
  attachInterrupt(GPIO_NUM_34, touchIrq, FALLING);
}

void loop()
{
  while (true)
  {
    auto x = rand(0, 800);
    auto y = rand(0, 480);
    auto w = rand(10, 150);
    auto h = rand(10, 150);
    if (x + w > 800 || y + h > 480)
    {
      continue;
    }

    auto color = hsv(randf(), 0.8f, 0.8f);
    display.fill(x, y, w, h, color);
    break;
  }

  if (touchFlag)
  {

    uint16_t x;
    uint16_t y;
    uint32_t z;
    touch.readRaw(x, y, z);
    if (z > 200)
    {
      printf("Touch: [%d, %d, %d]\n", x, y, z);
    }

    delay(10);

    touchFlag = false;
  }
}
