#pragma once
#include <cstdint>
#include <random>

class RandomReplacement {
public:
    RandomReplacement(): rng(std::random_device{}()) {}

    void SetWays(uint8_t num);
    uint8_t GetVictim() const;

private:
    uint8_t ways = 0; ///Number of ways in the cache
    mutable std::mt19937 rng;
};