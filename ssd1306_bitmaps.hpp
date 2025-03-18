#pragma once

#include "stdint.h"


class Bitmap
{
public:

const uint8_t w;
const uint8_t h;
const uint8_t* data;

Bitmap(uint8_t width_px, uint8_t height_px, const uint8_t* bitmap_ptr);
};



extern Bitmap big_car;
extern Bitmap small_car;
extern Bitmap accum[];





