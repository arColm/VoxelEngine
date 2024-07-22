#include "xxHash.h"


xxHash::xxHash() {
    xxHash::accumulator = 0;
}
xxHash::xxHash(uint32_t accumulator) {
	xxHash::accumulator = accumulator;
}

xxHash::operator uint32_t() const {
    uint32_t avalanche = xxHash::accumulator;
    avalanche ^= avalanche >> 15;
    avalanche *= primeB;
    avalanche ^= avalanche >> 13;
    avalanche *= primeC;
    avalanche ^= avalanche >> 16;
    return avalanche;
}
