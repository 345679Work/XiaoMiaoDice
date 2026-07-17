// AppState.h
#ifndef APP_STATE_H
#define APP_STATE_H

#include <vector>
#include <Arduino.h>
#include "constants.h"

// 单个骰子
struct Die
{
    uint8_t sides; // 面数: 2,4,6,8,10,12,20,100
};

// 配置 = 一组骰子
struct DiceConfig
{
    String name; // 内部标识，如 "cfg1"
    std::vector<Die> dice;
};

// 计数方式
enum class CountMethod
{
    Max,
    Min,
    Sum,
    Avg
};

// 应用模式
enum class AppMode
{
    Roll,
    ConfigList,
    ConfigDetail
};

// 配置详情中的焦点区域
enum class FocusArea
{
    DiceList, // 骰子区（左右切换骰子）
    DiceOps,  // 骰子操作区（Sides/Del/Add）
    ConfigOps // 配置操作区（Save/Use/Remove/Quit）
};

// 全局状态
struct AppState
{
    AppMode mode = AppMode::Roll;
    CountMethod method = CountMethod::Sum;

    // 配置列表
    std::vector<DiceConfig> configs;
    int currentConfigIndex = 0;  // 当前使用的配置索引（Roll模式）
    int selectedConfigIndex = 0; // 配置列表中选中的索引

    // 配置详情状态
    int detailDiceIndex = 0;     // 骰子区选中的骰子索引
    int detailDiceOpIndex = 0;   // 骰子操作区选中: 0=Sides, 1=Del, 2=Add
    int detailConfigOpIndex = 0; // 配置操作区选中: 0=Save, 1=Use, 2=Remove, 3=Quit
    FocusArea focus = FocusArea::DiceList;

    // 存储每个骰子的具体结果
    std::vector<int> lastRollDetails;

    // 投掷结果
    int lastRollResult = 0;
    bool hasRolled = false; // 是否已投掷过，用于显示“--”

    // 当前正在编辑的配置（在ConfigDetail中）的索引
    int editingConfigIndex = -1; // -1 表示新建
};

// 全局状态实例（extern）
extern AppState g_appState;

#endif