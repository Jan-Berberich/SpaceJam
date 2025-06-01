#include "utils.h"

float fsgnf(float f) {
    return f != 0.0f ? -2.0f * signbit(f) + 1.0f : 0.0f;
}