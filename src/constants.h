// constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// ===== 按键引脚 =====
#define PIN_UP 2
#define PIN_DOWN 13
#define PIN_LEFT 27
#define PIN_RIGHT 35 // 仅输入
#define PIN_A 34     // 仅输入
#define PIN_B 12

// ===== 蜂鸣器 =====
#define PIN_BUZZER 14

// ===== 屏幕尺寸 =====
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128

// ===== 颜色定义（TFT_eSPI 颜色宏）=====
#define COLOR_BG TFT_BLACK
#define COLOR_FG TFT_WHITE
#define COLOR_HIGHLIGHT_BG TFT_WHITE
#define COLOR_HIGHLIGHT_FG TFT_BLACK
#define COLOR_LINE TFT_DARKGREY
#define COLOR_TITLE TFT_CYAN
#define COLOR_RESULT TFT_YELLOW

// ===== 字体大小 =====
// 字体大小：实际生效的只有 1 和 2
#define FONT_SIZE_TITLE   2   // 结果区、骰子区
#define FONT_SIZE_HEADING 2   // 骰子区（与标题同尺寸）
#define FONT_SIZE_NORMAL  1   // 操作区、列表项
#define FONT_SIZE_SMALL   1   // 辅助文字

// ===== 骰子面数预设 =====
#define SIDES_PRESET_COUNT 8
extern const uint8_t SIDES_PRESET[SIDES_PRESET_COUNT];

// ===== 配置存储命名空间 =====
#define PREF_NAMESPACE "dice_cfg"

// ===== 按键事件 =====
enum KeyCode
{
    KEY_NONE,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_A,
    KEY_B
};

enum KeyAction
{
    ACTION_NONE,
    ACTION_PRESS,
    ACTION_RELEASE
};

struct InputEvent
{
    KeyCode key;
    KeyAction action;
};

#endif