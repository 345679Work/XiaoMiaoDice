// InputManager.cpp
#include "InputManager.h"
#include <Arduino.h>

static bool lastStates[6] = {false};
static unsigned long lastDebounceTime[6] = {0};
static InputEvent lastEvent = {KEY_NONE, ACTION_NONE};

bool InputManager::lastStates[6] = {false};
unsigned long InputManager::lastDebounceTime[6] = {0};
static unsigned long lastPressTime[6];

static int pinOfKey(KeyCode key)
{
    switch (key)
    {
    case KEY_UP:
        return PIN_UP;
    case KEY_DOWN:
        return PIN_DOWN;
    case KEY_LEFT:
        return PIN_LEFT;
    case KEY_RIGHT:
        return PIN_RIGHT;
    case KEY_A:
        return PIN_A;
    case KEY_B:
        return PIN_B;
    default:
        return -1;
    }
}

static KeyCode keyOfPin(int pin)
{
    switch (pin)
    {
    case PIN_UP:
        return KEY_UP;
    case PIN_DOWN:
        return KEY_DOWN;
    case PIN_LEFT:
        return KEY_LEFT;
    case PIN_RIGHT:
        return KEY_RIGHT;
    case PIN_A:
        return KEY_A;
    case PIN_B:
        return KEY_B;
    default:
        return KEY_NONE;
    }
}

void InputManager::init()
{
    pinMode(PIN_UP, INPUT_PULLUP);
    pinMode(PIN_DOWN, INPUT_PULLUP);
    pinMode(PIN_LEFT, INPUT_PULLUP);
    pinMode(PIN_RIGHT, INPUT_PULLUP);
    pinMode(PIN_A, INPUT_PULLUP);
    pinMode(PIN_B, INPUT_PULLUP);
    // 初始状态
    for (int i = 0; i < 6; i++)
    {
        lastStates[i] = false;
        lastDebounceTime[i] = 0;
    }
    lastEvent = {KEY_NONE, ACTION_NONE};
}

void InputManager::scan()
{
    KeyCode keys[] = {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_A, KEY_B};
    for (int i = 0; i < 6; i++)
    {
        KeyCode k = keys[i];
        int pin = pinOfKey(k);
        bool reading = digitalRead(pin) == LOW; // 按下为低
                                                // 直接检测边沿
        if (reading != lastStates[i])
        {
            lastStates[i] = reading;
            if (reading)
            {
                unsigned long now = millis();
                if (now - lastPressTime[i] > 50)
                { // 50ms 防抖
                    lastPressTime[i] = now;
                    lastEvent = {k, ACTION_PRESS};
                }
            }
            else
            {
                lastEvent = {k, ACTION_RELEASE};
            }
        }
    }
}

// void InputManager::scan()
// {
//     // 扫描所有按键，检测按下事件（低电平有效）
//     KeyCode keys[] = {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_A, KEY_B};
//     for (int i = 0; i < 6; i++)
//     {
//         KeyCode k = keys[i];
//         int pin = pinOfKey(k);
//         bool reading = digitalRead(pin) == LOW; // 按下为LOW
//         if (reading != lastStates[i])
//         {
//             lastDebounceTime[i] = millis();
//         }
//         if ((millis() - lastDebounceTime[i]) > DEBOUNCE_DELAY)
//         {
//             if (reading != lastStates[i])
//             {
//                 lastStates[i] = reading;
//                 if (reading)
//                 { // 按下瞬间
//                     lastEvent = {k, ACTION_PRESS};
//                 }
//                 else
//                 {
//                     lastEvent = {k, ACTION_RELEASE};
//                 }
//             }
//         }
//     }
// }

InputEvent InputManager::getEvent()
{
    InputEvent ev = lastEvent;
    lastEvent = {KEY_NONE, ACTION_NONE}; // 消费事件
    return ev;
}

bool InputManager::isKeyPressed(KeyCode key)
{
    int pin = pinOfKey(key);
    if (pin == -1)
        return false;
    return digitalRead(pin) == LOW;
}