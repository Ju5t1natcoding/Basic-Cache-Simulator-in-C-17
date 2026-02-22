#include "MainMemory.hpp"
#include <array>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

MainMemory::MainMemory() {
    ///Allocate memory region
    memory = std::make_unique<std::array<uint8_t, MAIN_DIM>>();

    ///Fill main memory with zeros
    memory.get()->fill(0x00);
}

MainMemory::~MainMemory() {
}

void MainMemory::Read(uint32_t startAddress, uint8_t size, uint8_t* destination) {
    std::stringstream ss;
    ss << "0x" << std::hex << startAddress;
    std::string message = "Reading from main memory (address: " + ss.str() + ")\n";
    std::cout << message << std::flush;

    ///Read the requested amount of memory and copy to destination
    memcpy(destination, &memory.get()->at(startAddress), size);
}

void MainMemory::Write(uint32_t startAddress, uint8_t size, uint8_t* source) {
    std::stringstream ss;
    ss << "0x" << std::hex << startAddress;
    std::string message = "Writing to main memory (address: " + ss.str() + ")\n";
    std::cout << message << std::flush;

    ///Copy the requested amount of memory form the source to the main memory
    memcpy(&memory.get()->at(startAddress), source, size);
}

void MainMemory::Print() {
    ///Print a slice of the memory region (for debugging)
    const uint32_t ROWS = 24;
    const uint32_t COLUMNS = 12;

    for (uint32_t dash = 0; dash < 95; ++dash) {
        std::cout << "-\n"[dash == 94];
    }
    
    std::cout << std::flush;

    for (uint32_t row = 0; row < ROWS; ++row) {
        std::cout << " ";
        for (uint32_t col = 0; col < COLUMNS; ++col) {
            ///I use stringstream instead of format because format was added in C++20 and stringstream is accepted by C++11/14/17
            std::stringstream ss;
            ss << "0x" << std::hex << static_cast<uint32_t>(memory.get()->at(row * COLUMNS + col));
            std::string value = ss.str();

            std::cout << value << (value.size() == 3 ? "     " : "    ");
        }

        std::cout << "\n" << std::flush;
    }

    for (uint32_t dash = 0; dash < 95; ++dash) {
        std::cout << "-\n"[dash == 94];
    }

    std::cout << "\n" << std::flush;
}