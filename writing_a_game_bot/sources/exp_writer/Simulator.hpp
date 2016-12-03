#pragma once

#include "Hotkey.hpp"
#include "TibiaFinder.hpp"

#include <Windows.h>

#include <thread>
#include <random>

class Simulator
{
private:
    struct RandomBetween
    {
        size_t min, max;

        size_t get() const
        {
            static std::random_device rd;
            static std::mt19937 mt(rd());
            std::uniform_int_distribution<size_t> dist(min, max);

            return dist(mt);
        }
    };

public:
    void hotkey(Hotkey hotkey,
                RandomBetween randomBetween = { defMinRandomBetween, defMaxRandomBetween })
    {
        keyDownAndUp(hotkeyToWparam(hotkey),
                     randomBetween);
    }

    void keyDownAndUp(WPARAM key,
                      RandomBetween randomBetween = { defMinRandomBetween, defMaxRandomBetween })
    {
        PostMessage(windowHwnd, WM_KEYDOWN, key, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(randomBetween.get()));

        PostMessage(windowHwnd, WM_KEYUP, key, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(randomBetween.get()));
    }

    void attachNewWindow(HWND hwnd)
    {
        windowHwnd = hwnd;
    }

private:
    static const size_t defMinRandomBetween = 15;
    static const size_t defMaxRandomBetween = 50;

    HWND windowHwnd;

    static WPARAM hotkeyToWparam(Hotkey hotkey)
    {
        switch (hotkey)
        {
            case Hotkey::F1: return VK_F1;
            case Hotkey::F2: return VK_F2;
            case Hotkey::F3: return VK_F3;
            case Hotkey::F4: return VK_F4;
            case Hotkey::F5: return VK_F5;
            case Hotkey::F6: return VK_F6;
            case Hotkey::F7: return VK_F7;
            case Hotkey::F8: return VK_F8;
            case Hotkey::F9: return VK_F9;
            case Hotkey::F10: return VK_F10;
            case Hotkey::F11: return VK_F11;
            case Hotkey::F12: return VK_F12;
        }
    }
};
