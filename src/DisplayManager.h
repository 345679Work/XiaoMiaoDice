// DisplayManager.h
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include "constants.h"

class DisplayManager {
public:
    static void init();
    static void clear();
    static void drawText(int x, int y, int fontSize, const String& text, 
                         uint16_t fgColor = COLOR_FG, uint16_t bgColor = COLOR_BG);
    static void drawLine(int x1, int y1, int x2, int y2, uint16_t color = COLOR_LINE);
    static void drawRect(int x, int y, int w, int h, uint16_t color = COLOR_FG);
    static void fillRect(int x, int y, int w, int h, uint16_t color);
    static void drawHighlightedText(int x, int y, int fontSize, const String& text,
                                    bool highlight, 
                                    uint16_t fgColor = COLOR_FG, 
                                    uint16_t bgColor = COLOR_BG);
    // 获取文本宽度（近似，用于居中）
    static int getTextWidth(const String& text, int fontSize);
    static int getTextHeight(int fontSize);
    
    // 获取屏幕尺寸
    static int width() { return SCREEN_WIDTH; }
    static int height() { return SCREEN_HEIGHT; }
    
private:
    static TFT_eSPI tft;
};

#endif