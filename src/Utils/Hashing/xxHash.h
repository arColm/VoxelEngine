#pragma once
#include <cstdint>


class xxHash {
public:
    xxHash() {
        xxHash::accumulator = 0;
    }
    xxHash(uint32_t accumulator) {
        xxHash::accumulator = accumulator;
    }
    operator uint32_t() const {
        uint32_t avalanche = xxHash::accumulator;
        avalanche ^= avalanche >> 15;
        avalanche *= primeB;
        avalanche ^= avalanche >> 13;
        avalanche *= primeC;
        avalanche ^= avalanche >> 16;
        return avalanche;
    }

    xxHash operator+ (const int& v) const {
        return accumulator + (uint32_t)v;
    }

    float Floats01A() { return (float)((uint32_t)(*this) & 255) / 255.f; };
    float Floats01B() { return (float)(((uint32_t)(*this) >> 8) & 255) / 255.f; };
    float Floats01C() { return (float)(((uint32_t)(*this) >> 16) & 255) / 255.f; };
    float Floats01D() { return (float)(((uint32_t)(*this) >> 24) & 255) / 255.f; };

    xxHash eat(int data) { return rotateLeft(accumulator + (uint32_t)data * primeC, 11) * primeD; };
    xxHash eat(float data) { return rotateLeft(accumulator + (uint32_t)(data * primeC), 11) * primeD; };
    static xxHash seed(int seed) { return (uint32_t)seed + primeE; };
    static uint32_t rotateLeft(uint32_t data, int steps) { return (data << steps) | (data >> 32 - steps); };

    
private:
    uint32_t accumulator;
    static const uint32_t primeA = 0b10011110001101110111100110110001;
    static const uint32_t primeB = 0b10000101111010111100101001110111;
    static const uint32_t primeC = 0b11000010101100101010111000111101;
    static const uint32_t primeD = 0b00100111110101001110101100101111;
    static const uint32_t primeE = 0b00010110010101100110011110110001;

};