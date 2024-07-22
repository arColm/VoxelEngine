#pragma once
#include <cstdint>


class xxHash {
public:
    xxHash();
    xxHash(uint32_t accumulator);
    operator uint32_t() const;
    xxHash eat(int data) { return rotateLeft(accumulator + (uint32_t)data * primeC, 11) * primeD; };
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