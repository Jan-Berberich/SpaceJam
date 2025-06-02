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