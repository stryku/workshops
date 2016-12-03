#include "TibiaStuffReader.hpp"
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

void prepare(TibiaStuffReader &tibiaStuffReader)
{
    TibiaFinder tibiaFinder;

    const auto foundTitles = tibiaFinder.findAllClientsTitles();
    const auto title = foundTitles[0];
    const auto pid = tibiaFinder.findProcessId(title);

    tibiaStuffReader.attachNewProcess(pid);
}

Stats readStats(TibiaStuffReader &tibiaStuffReader)
{
    Stats stats;

    stats.level = tibiaStuffReader.readLvl();
    stats.experience = tibiaStuffReader.readExp();
    stats.magicLevel = tibiaStuffReader.readMagicLvl();
    stats.magicLevelToGo = tibiaStuffReader.readMagicLvlPercentLeft();
    stats.fistFighting = tibiaStuffReader.readFistFighting();
    stats.clubFighting = tibiaStuffReader.readClubFighting();
    stats.swordFighting = tibiaStuffReader.readSwordFighting();
    stats.axeFighting = tibiaStuffReader.readAxeFighting();
    stats.distanceFighting = tibiaStuffReader.readDistanceFighting();
    stats.shielding = tibiaStuffReader.readShielding();

    return stats;
}

int main()
{
    TibiaStuffReader tibiaStuffReader;

    prepare(tibiaStuffReader);

    const auto stats = readStats(tibiaStuffReader);
    std::cout << stats;
    
    int dummy;
    std::cin >> dummy;

    return 0;
}
