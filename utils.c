#include "utils.h"

float fsgnf(float f) {
    return f != 0.0f ? -2.0f * signbit(f) + 1.0f : 0.0f;
}

uint32_t  randi(uint32_t seed) {
    seed = (seed << 13) ^ seed;
    return ((seed * (seed * seed++ * 15731 + 789221) + 1376312589) & 0x7fffffff);
}

float randfin(uint32_t seed, float a, float b) {
    return (float)randi(seed) / 0x7fffffff * (b - a) + a;
}

float rsqrtf(float f) {
    long i;
    float x2, y;
    const float threehalfs = 1.5f;

    x2 = f * 0.5f;
    y  = f;
    i = *(long*)&y;                      // evil floating point bit hack
    i = 0x5f3759df - (i >> 1);           // what the fuck?
    y = *(float*)&i;
    y = y * (threehalfs - (x2 * y * y)); // 1st iteration
//  y = y * (threehalfs - (x2 * y * y)); // 2nd iteration, can be removed

    return y;
}

void ui32to4f(float rgba[4], uint32_t color) {
    rgba[0] = (float)((color >> 24) & 0xFF) / 0xFF;
    rgba[1] = (float)((color >> 16) & 0xFF) / 0xFF;
    rgba[2] = (float)((color >>  8) & 0xFF) / 0xFF;
    rgba[3] = (float)((color >>  0) & 0xFF) / 0xFF;
}