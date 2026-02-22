#include "ReplacementAlgorithm.hpp"

void RandomReplacement::SetWays(uint8_t num) {
    ///Set the number of ways in the cache
    ways = num;
}

uint8_t RandomReplacement::GetVictim() const {
    ///Choose a random victim
    std::uniform_int_distribution<int> dist(0, ways - 1);
    return dist(rng);
}
