#pragma once

#include "Hotkey.hpp"

struct HealRule
{
    size_t minHp, maxHp, minMana, maxMana;
    Hotkey hotkey;

    bool passed(size_t characterHp, size_t characterMana) const
    {
        return minHp <= characterHp && characterHp <= maxHp
            && minMana <= characterMana && characterMana <= maxMana;
    }
};
