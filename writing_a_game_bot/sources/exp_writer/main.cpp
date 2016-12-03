#include "ProcessMemoryWriter.hpp"
#include "TibiaFinder.hpp"

#include <iostream>

struct Stats
{
    size_t level;
    size_t experience;
    size_t magicLevel;
    size_t magicLevelToGo;
    size_t fistFighting;
    size_t clubFighting;
    size_t swordFighting;
    size_t axeFighting;
    size_t distanceFighting;
    size_t shielding;

    friend std::ostream& operator<<(std::ostream &out, const Stats &stats)
    {
        out <<
            "Level: " << stats.level << "\n" <<
            "Experience: " << stats.experience << "\n" <<
            "Magic lvl %: " << stats.magicLevelToGo << "\n" <<
            "Fist fighting: " << stats.fistFighting << "\n" <<
            "Club fighting: " << stats.clubFighting << "\n" <<
            "Sword fighting: " << stats.swordFighting << "\n" <<
            "Axe fighting: " << stats.axeFighting << "\n" <<
            "Distance fighting: " << stats.distanceFighting << "\n" <<
            "Shielding: " << stats.shielding << "\n";

        return out;
    }
};

void prepare(ProcessMemoryWriter &processWriter)
{
    TibiaFinder tibiaFinder;

    const auto foundTitles = tibiaFinder.findAllClientsTitles();
    const auto title = foundTitles[0];
    const auto pid = tibiaFinder.findProcessId(title);

    processWriter.attachNewProcess(pid);
}

int main()
{
    static const DWORD ExpAddress = 0x570460;
    ProcessMemoryWriter processWriter;

    prepare(processWriter);

    while (true)
    {
        uint32_t exp;
        std::cout << "Experience to write: ";
        std::cin >> exp;

        processWriter.write(ExpAddress, exp);
    }

    return 0;
}
