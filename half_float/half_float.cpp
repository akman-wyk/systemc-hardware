#include "half_float.h"

half ParseHalfFloat(uint16_t v) {
    return half((v & 0x3ff), ((v >> 10) & 0x1f), ((v >> 15) & 0x1));
}

uint16_t HalfFloatAdd(uint16_t h1, uint16_t h2) {
    half result = ParseHalfFloat(h1) + ParseHalfFloat(h2);
    return result.GetBits(); 
}

uint16_t HalfFloatSub(uint16_t h1, uint16_t h2) {
    half result = ParseHalfFloat(h1) - ParseHalfFloat(h2);
    return result.GetBits(); 
}

uint16_t HalfFloatMul(uint16_t h1, uint16_t h2) {
    half result = ParseHalfFloat(h1) * ParseHalfFloat(h2);
    return result.GetBits(); 
}

uint16_t HalfFloatDiv(uint16_t h1, uint16_t h2) {
    half result = ParseHalfFloat(h1) / ParseHalfFloat(h2);
    return result.GetBits(); 
}

int HalfFloatCompare(uint16_t h1, uint16_t h2) {
    half hh1 = ParseHalfFloat(h1);
    half hh2 = ParseHalfFloat(h2);
    if (hh1 > hh2) {
        return 1;
    } else if (hh1 < hh2) {
        return -1;
    } else {
        return 0;
    }
}