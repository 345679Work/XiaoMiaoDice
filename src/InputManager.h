// InputManager.h
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "constants.h"

class InputManager {
public:
    static void init();
    static InputEvent getEvent();   // 阻塞等待事件？我们采用轮询方式，返回最新事件
    static void scan();             // 扫描并更新内部事件队列（简单起见只保留最新一个事件）
    static bool isKeyPressed(KeyCode key);
    
private:
    static bool debounce(KeyCode key);
    static bool lastStates[6];
    static unsigned long lastDebounceTime[6];
    static const unsigned long DEBOUNCE_DELAY = 30; // ms
};

#endif