// UIManager.h
#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "AppState.h"

class UIManager {
public:
    static void draw(const AppState& state);
    static void drawRollMode(const AppState& state);
    static void drawConfigList(const AppState& state);
    static void drawConfigDetail(const AppState& state);
    
private:
    static void drawDivider(int y);
    static void drawDiceList(const std::vector<Die>& dice, int selectedIndex, int x, int y, int fontSize);
};

#endif