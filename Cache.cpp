#include "Cache.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

CacheSet::CacheSet() {
    ///Set the number of cache ways in the RRA
    replacement.SetWays(CACHE_WAYS);
}

CacheSet::~CacheSet() {
}

CacheLine *CacheSet::Find(uint32_t tag) {
    ///Find the cache line based on the given tag
    for (uint8_t way = 0; way < CACHE_WAYS; ++way) {
        if (lines[way].valid && lines[way].tag == tag) {
            return &lines[way];
        }
    }

    ///Cache line wasn't found
    return nullptr;
}

CacheLine *CacheSet::Replace(uint32_t tag, uint8_t *sourceData) {
    ///Choose a victim line to replace
    uint8_t victim = replacement.GetVictim();

    ///Set new cache line attributes
    lines[victim].valid = true;
    lines[victim].tag = tag;

    ///Copy 64-byte data from main memory to cache
    memcpy(lines[victim].data.data(), sourceData, CACHE_LINE_SIZE);

    return &lines[victim];
}

void Cache::Initialize(MainMemory *memory) {
    mainMemory = memory;
}

uint32_t Cache::Read(uint32_t address) {
    AddressParts addressParts(address);

    ///Find the requested cache line
    CacheLine* line = sets[addressParts.setIndex].Find(addressParts.tag);

    ///Update cache statistics (cache accesses)
    statistics.cacheAccesses++;

    if (line) { ///Cache hit
        std::stringstream ss, ss1, ss2;
        ss << "0x" << std::hex << address;
        ss1 << static_cast<uint32_t>(addressParts.setIndex);
        ss2 << addressParts.tag;
        std::string message = "Reading from cache (address: " + ss.str() + ", set: " + ss1.str() + ", tag: " + ss2.str() + ")\n";
        std::cout << message << std::flush;

        ///Update cache statistics (cache hits)
        statistics.cacheHits++;

        ///Reinterpret the memory as an 32-bit integer
        return *reinterpret_cast<uint32_t*>(&line->data[addressParts.byteOffset]);
    } else {    ///Cache miss
        ///Find the beginning og the cache line
        uint32_t lineStart = address & ~(CACHE_LINE_SIZE - 1);

        ///Allocate data buffer
        std::array<uint8_t, CACHE_LINE_SIZE> buffer;

        ///Read the requested data from main memory
        mainMemory->Read(lineStart, CACHE_LINE_SIZE, buffer.data());

        ///Replace the cache line in the set
        CacheLine* newLine = sets[addressParts.setIndex].Replace(addressParts.tag, buffer.data());

        ///Reinterpret the memory as a 32-bit integer and return it
        return *reinterpret_cast<uint32_t*>(&newLine->data[addressParts.byteOffset]);
    }

    return 0;
}

void Cache::Write(uint32_t address, uint32_t data) {
    AddressParts addressParts(address);

    ///Find the requested line in the cache
    CacheLine* line = sets[addressParts.setIndex].Find(addressParts.tag);
    
    ///Update cache statistics (cache accesses)
    statistics.cacheAccesses++;

    if (line) { ///Cache hit
        std::stringstream ss, ss1, ss2;
        ss << "0x" << std::hex << address;
        ss1 << static_cast<uint32_t>(addressParts.setIndex);
        ss2 << addressParts.tag;
        std::string message = "Writing to cache (address: " + ss.str() + ", set: " + ss1.str() + ", tag: " + ss2.str() + ")\n";
        std::cout << message << std::flush;

        ///Update cache statistics (cache hits)
        statistics.cacheHits++;

        ///Reinterpret the cache memory as a 32-bit integer and write the data
        *reinterpret_cast<uint32_t*>(&line->data[addressParts.byteOffset]) = data;
    }

    ///Write through cache (immediately write modified data to main memory)
    mainMemory->Write(address, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&data));
}

CacheStatistics Cache::GetStatistics() const {
    return statistics;
}

void Cache::ResetStatistics() {
    statistics.cacheAccesses = 0;
    statistics.cacheHits = 0;
}
