#include "Bot.hpp"

#include <iostream>

Hotkey size_tToHotkey(size_t hotkey)
{
    switch (hotkey)
    {
        case 1: return Hotkey::F1;
        case 2: return Hotkey::F2;
        case 3: return Hotkey::F3;
        case 4: return Hotkey::F4;
        case 5: return Hotkey::F5;
        case 6: return Hotkey::F6;
        case 7: return Hotkey::F7;
        case 8: return Hotkey::F8;
        case 9: return Hotkey::F9;
        case 10: return Hotkey::F10;
        case 11: return Hotkey::F11;
        case 12: return Hotkey::F12;

        default: return Hotkey::UNKNOWN;
    }
}

std::vector<HealRule> getRules()
{
    HealRule rule;
    int rulesCount;
    size_t hotkey;
    std::vector<HealRule> rules;

    std::cout << "How many rules: ";
    std::cin >> rulesCount;

    while (rulesCount --> 0)
    {
        std::cout << "Min hp: ";
        std::cin >> rule.minHp;

        std::cout << "Max hp: ";
        std::cin >> rule.maxHp;

        std::cout << "Min mana: ";
        std::cin >> rule.minMana;

        std::cout << "Max mana: ";
        std::cin >> rule.maxMana;

        std::cout << "Hotkey: F";
        std::cin >> hotkey;

        rule.hotkey = size_tToHotkey(hotkey);

        rules.push_back(rule);
    }

    return rules;
}

int main()
{
    Bot bot;

    const auto rules = getRules();
    bot.run(rules);

    return 0;
}
