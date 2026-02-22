#include "Processor.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>

MemorySystem::MemorySystem() {
    cache.Initialize(&mainMemory);
}

MemorySystem::~MemorySystem() {
}

uint32_t MemorySystem::Read(uint32_t address) {
    return cache.Read(address);
}

void MemorySystem::Write(uint32_t address, uint32_t data) {
    cache.Write(address, data);
}

void MemorySystem::PrintMainMemory() {
    mainMemory.Print();
}

CacheStatistics MemorySystem::GetCacheStatistics() const {
    return cache.GetStatistics();
}

void MemorySystem::ResetCacheStatistics() {
    cache.ResetStatistics();
}

signed main() {
    MemorySystem memory;

    /**
        Test 1 - functionality:

    ///Print a slice of the main memory
    memory.PrintMainMemory();

    ///Expected: read from main memory at address 0x00 (64 bytes) the value 0x00
    uint32_t data1 = memory.Read(0x20);
    std::cout << "Value: 0x" << std::hex << data1 << "\n" << std::flush;

    ///Expected: read from cache address 0x20 the value 0x00
    data1 = memory.Read(0x20);
    std::cout << "Value: 0x" << std::hex << data1 << "\n" << std::flush;

    ///Expected: write to both cache and main memory (write-through) the value 0x6139 at address 0x20
    memory.Write(0x20, 0x6139);

    ///Expected: read from cache the value 0x6139 at address 0x20
    uint32_t data2 = memory.Read(0x20);
    std::cout << "Value: 0x" << std::hex << data2 << "\n" << std::flush;

    ///Expected: write to both cache and main memory
    memory.Write(0x10, 0x12345678);
    memory.Write(0x10, 0x77777777);
    memory.Write(0x10, 0x52690723);

    ///Expected: read from cache at address 0x10 the value 0x52690723
    uint32_t data3 = memory.Read(0x10);
    std::cout << "Value: 0x" << std::hex << data3 << "\n" << std::flush;

    ///Thrashing
    for (uint8_t i = 0; i < 4; ++i) {
        ///Expected: first loop reads from main memory and updates cache
        ///From second loops reads from cache only
        memory.Read(0x0040);
        memory.Read(0x1040);
        memory.Read(0x2040);
        memory.Read(0x3040);
        memory.Read(0x4040);    ///Causes an eviction and cache miss (reads from main memory)
    }
    **/

    /**
        Test 2 - cache hit rate:
    **/

    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint32_t> dist_address1(0x0u, 0x4000u), dist_value(0x0u, 0xFFFFFFFFu), dist_address2(0x1000u, 0x3000u);

    ///Random write (initially fill memory)
    for (uint32_t writeCommand = 0; writeCommand < 128; ++writeCommand) {
        ///Write a random value to a random address (up to 0x4000)
        memory.Write(dist_address1(rng), dist_value(rng));
    }

    ///Sequential read cycles
    ///Expected: first cycle with lower hit rate. Following cycles with higher hit rate
    for (uint8_t cycle = 0; cycle < 4; ++cycle) {
        memory.ResetCacheStatistics();

        ///Sequential read
        for (uint32_t readCommand = 0; readCommand < 128; ++readCommand) {
            memory.Read(readCommand * 4);
        }

        std::cout << "Cache hit rate: " << memory.GetCacheStatistics().GetHitRate() * 100 << "% (sequential read " << cycle + 1 << ")\n\n" << std::flush;
    }

    ///Random read cycles
    ///Expected: cache hit rate increases with each cycle
    for (uint8_t cycle = 0; cycle < 4; ++cycle) {
        memory.ResetCacheStatistics();

        ///Random read
        for (uint32_t readCommand = 0; readCommand < 128; ++readCommand) {
            memory.Read(dist_address2(rng));
        }

        std::cout << "Cache hit rate: " << memory.GetCacheStatistics().GetHitRate() * 100 << "% (random read " << cycle + 1 << ")\n\n" << std::flush;
    }

    memory.PrintMainMemory();

    system("PAUSE");
    exit(0);
}
