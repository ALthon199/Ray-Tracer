#pragma once

#include "Vector.h"

rt::Vec3 random_unit_vec();

float random_num(float min, float max);

float clamp(float val, float min, float max);

void color_clamp(rt::Color& color, float min, float max);