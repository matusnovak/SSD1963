#pragma once

#include <Arduino.h>

using Color = uint16_t;

struct RGB
{
    float r;
    float g;
    float b;
};

inline Color rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r >> 3) << 11) | ((g >> 2) << 5) | b >> 3;
}

inline Color rgb(float r, float g, float b)
{
    return rgb(
        static_cast<uint8_t>(r * 255),
        static_cast<uint8_t>(g * 255),
        static_cast<uint8_t>(b * 255));
}

// Source: https://stackoverflow.com/a/6930407
inline Color hsv(float h, float s, float v)
{
    h *= 360.0f;
    float hh, p, q, t, ff;
    long i;
    RGB out;

    if (s <= 0.0)
    { // < is bogus, just shuts up warnings
        return rgb(v, v, v);
    }

    hh = h;
    if (hh >= 360.0f)
    {
        hh = 0.0;
    }

    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch (i)
    {
    case 0:
        out.r = v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = v;
        break;
    case 5:
    default:
        out.r = v;
        out.g = p;
        out.b = q;
        break;
    }

    return rgb(out.r, out.g, out.b);
}
