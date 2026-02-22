#pragma once
#include "MainMemory.hpp"
#include "Cache.hpp"

class MemorySystem {
public:
    MemorySystem();
    ~MemorySystem();

    uint32_t Read(uint32_t address);
    void Write(uint32_t address, uint32_t data);
    void PrintMainMemory();

    CacheStatistics GetCacheStatistics() const;
    void ResetCacheStatistics();

private:
    MainMemory mainMemory;
    Cache cache;
};