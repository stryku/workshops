#pragma once

#include "HealRule.hpp"
#include "TibiaFinder.hpp"
#include "TibiaStuffReader.hpp"
#include "Simulator.hpp"

#include <vector>
#include <string>

class Bot
{
public:
    void run(const std::vector<HealRule> &rules)
    {
        prepare();
        runHealer(rules);
    }

private:
    void prepare()
    {
        const auto foundTitles = tibiaFinder.findAllClientsTitles();
        const auto title = foundTitles[0];
        const auto pid = tibiaFinder.findProcessId(title);
        const auto windowHandle = tibiaFinder.pidToHwnd(pid);

        tibiaStuffReader.attachNewProcess(pid);
        simulator.attachNewWindow(windowHandle);
    }

    void runHealer(const std::vector<HealRule> &rules)
    {
        while (true)
        {
            const auto hp = tibiaStuffReader.hp();
            const auto mana = tibiaStuffReader.mana();

            for (const auto &rule : rules)
                if (rule.passed(hp, mana))
                    simulator.hotkey(rule.hotkey);

            std::this_thread::sleep_for( std::chrono::seconds{ 1 } );
        }
    }

    TibiaFinder tibiaFinder;
    TibiaStuffReader tibiaStuffReader;
    Simulator simulator;
};