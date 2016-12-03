#pragma once

#include "ProcessMemoryReader.hpp"

#include <stdint.h>

class TibiaStuffReader
{
public:
    uint32_t hp() const
    {
        return memoryReader.read<uint32_t>(HpAddress) ^ memoryReader.read<uint32_t>(XorAddress);
    }
    uint32_t mana() const
    {
        return memoryReader.read<uint32_t>(ManaAddress) ^ memoryReader.read<uint32_t>(XorAddress);
    }

    void attachNewProcess(DWORD pid)
    {
        memoryReader.attachNewProcess(pid);
    }

private:
    static const DWORD HpAddress = 0x70E000;
    static const DWORD ManaAddress = 0x57048C;
    static const DWORD XorAddress = 0x570458;

    ProcessMemoryReader memoryReader;
};
