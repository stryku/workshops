#pragma once

#include "ProcessMemoryReader.hpp"

#include <stdint.h>

class TibiaStuffReader
{
public:
    uint32_t readExp()
    {
        return read(ExpAddress);
    }
    uint32_t readLvl()
    {
        return read(LvlAddress);
    }
    uint32_t readMagicLvl ()
    {
        return read(MagicLvlAddress);
    }
    uint32_t readMagicLvlPercentLeft()
    {
        return read(MagicLvLPercentLeftAddress);
    }
    uint32_t readFistFighting()
    {
        return read(FistFightingAddress);
    }
    uint32_t readClubFighting()
    {
        return read(ClubFightingAddress);
    }
    uint32_t readAxeFighting()
    {
        return read(AxeFightingAddress);
    }
    uint32_t readSwordFighting()
    {
        return read(SwordFightingAddress);
    }
    uint32_t readDistanceFighting()
    {
        return read(DistanceFightingAddress);
    }
    uint32_t readFishing()
    {
        return read(FishingAddress);
    }
    uint32_t readShielding()
    {
        return read(ShieldingAddress);
    }

    void attachNewProcess(DWORD pid)
    {
        memoryReader.attachNewProcess(pid);
    }

private:
    static const DWORD ExpAddress = 0x570460;
    static const DWORD LvlAddress = 0x570470;
    static const DWORD MagicLvlAddress = 0x570478;
    static const DWORD MagicLvLPercentLeftAddress = 0x570484;
    static const DWORD SkillStep = 0x4;
    static const DWORD FistFightingAddress = 0x70E00C;
    static const DWORD ClubFightingAddress = FistFightingAddress + SkillStep;
    static const DWORD SwordFightingAddress = ClubFightingAddress + SkillStep;
    static const DWORD AxeFightingAddress = SwordFightingAddress + SkillStep;
    static const DWORD DistanceFightingAddress = AxeFightingAddress + SkillStep;
    static const DWORD ShieldingAddress = DistanceFightingAddress + SkillStep;
    static const DWORD FishingAddress = ShieldingAddress + SkillStep;

    uint32_t read(DWORD offset)
    {
        return memoryReader.read<uint32_t>(offset);
    }

    ProcessMemoryReader memoryReader;
};
