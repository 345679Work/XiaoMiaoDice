// SoundManager.h
#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <Arduino.h>

class SoundManager {
public:
    static void init();
    static void beep(int durationMs = 50);
    static void beepNonBlocking(int durationMs); // 可选，但简单起见用阻塞
};

#endif