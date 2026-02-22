#pragma once
#include "MainMemory.hpp"
#include "ReplacementAlgorithm.hpp"
#include <array>

const uint8_t MEMORY_ADDRESS_SIZE = 32; ///32-bit memory addresses
const uint8_t CACHE_LINE_SIZE = 64;     ///64-byte cache lines
const uint8_t CACHE_SETS = 64;          ///Cache with 64 sets
const uint8_t CACHE_WAYS = 4;           ///4-way set-associative cache

const uint8_t CACHE_LINE_BYTE_OFFSET_SIZE = 6;
const uint8_t CACHE_LINE_SET_INDEX_SIZE = 6;
const uint8_t CACHE_LINE_TAG_SIZE = 20;

struct CacheLine {
    uint32_t tag = 0;
    std::array<uint8_t, CACHE_LINE_SIZE> data;
    bool valid = false;
};

struct AddressParts {
    uint32_t tag;
    uint8_t setIndex, byteOffset;

    AddressParts(uint32_t address) {
        byteOffset = address & (CACHE_LINE_SIZE - 1);
        setIndex = (address >> CACHE_LINE_BYTE_OFFSET_SIZE) & ((1 << CACHE_LINE_SET_INDEX_SIZE) - 1);
        tag = address >> (CACHE_LINE_BYTE_OFFSET_SIZE + CACHE_LINE_SET_INDEX_SIZE);
    }
};

struct CacheStatistics {
    uint32_t cacheAccesses; ///Number of times the cache was accessed
    uint32_t cacheHits;     ///Number of cache hits

    float_t GetHitRate() {
        return static_cast<float_t>(cacheHits) / cacheAccesses;
    }
};

class CacheSet {
public:
    CacheSet();
    ~CacheSet();

    CacheLine* Find(uint32_t tag);
    CacheLine* Replace(uint32_t tag, uint8_t* sourceData);

private:
    std::array<CacheLine, CACHE_WAYS> lines;    ///Array of cache lines
    RandomReplacement replacement;  ///RRA (Random Replacement Algorithm)
};

class Cache {
public:
    void Initialize(MainMemory* memory);
    uint32_t Read(uint32_t address);
    void Write(uint32_t address, uint32_t data);

    CacheStatistics GetStatistics() const;
    void ResetStatistics();

private:
    std::array<CacheSet, CACHE_SETS> sets;  ///Array of cache sets
    MainMemory* mainMemory;                 ///Link to main memory
    CacheStatistics statistics;
};