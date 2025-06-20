#include <math.h>
#include <stdint.h>

extern float fsgnf(float f);

extern uint32_t randi(uint32_t seed);
extern float randfin(uint32_t seed, float a, float b);

/* Quake III fast inverse square root algorithm */
extern float rsqrtf(float f);

extern void rgbaSplitf(uint32_t rgba, float* r, float* g, float* b, float* a);