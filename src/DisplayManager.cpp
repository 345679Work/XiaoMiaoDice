// DisplayManager.cpp
#include "DisplayManager.h"

TFT_eSPI DisplayManager::tft = TFT_eSPI();

void DisplayManager::init() {
    tft.init();
    tft.setRotation(3);  // 与demo一致
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
}

void DisplayManager::clear() {
    tft.fillScreen(TFT_BLACK);
}

void DisplayManager::drawText(int x, int y, int fontSize, const String& text,
                              uint16_t fgColor, uint16_t bgColor) {
    tft.setTextColor(fgColor, bgColor);
    tft.setTextSize(fontSize);
    // tft.setTextSize(1);
    tft.setCursor(x, y);
    tft.print(text);
}

void DisplayManager::drawLine(int x1, int y1, int x2, int y2, uint16_t color) {
    tft.drawLine(x1, y1, x2, y2, color);
}

void DisplayManager::drawRect(int x, int y, int w, int h, uint16_t color) {
    tft.drawRect(x, y, w, h, color);
}

void DisplayManager::fillRect(int x, int y, int w, int h, uint16_t color) {
    tft.fillRect(x, y, w, h, color);
}

void DisplayManager::drawHighlightedText(int x, int y, int fontSize, const String& text,
                                         bool highlight,
                                         uint16_t fgColor, uint16_t bgColor) {
    if (highlight) {
        // 反色：背景白，文字黑
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
    } else {
        tft.setTextColor(fgColor, bgColor);
    }
    tft.setTextSize(fontSize);
    // tft.setTextSize(1);
    tft.setCursor(x, y);
    tft.print(text);
}

int DisplayManager::getTextWidth(const String& text, int fontSize) {
    // 字体1: 每个字符约 6x8 像素，字体2: 约 12x16 像素
    int charWidth = (fontSize == 1) ? 6 : 12;
    return text.length() * charWidth + 2; // 额外留一点余量
}

int DisplayManager::getTextHeight(int fontSize) {
    return (fontSize == 1) ? 8 : 16;
}