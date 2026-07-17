// main.cpp
#include <Arduino.h>
#include "AppState.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "DiceEngine.h"
#include "ConfigManager.h"
#include "UIManager.h"
#include "constants.h"

// 全局状态实例

// 定义预设面数数组
const uint8_t SIDES_PRESET[SIDES_PRESET_COUNT] = {2, 4, 6, 8, 10, 12, 20, 100};

// 前向声明处理函数
void handleRollMode(InputEvent ev);
void handleConfigList(InputEvent ev);
void handleConfigDetail(InputEvent ev);

void setup()
{
    Serial.begin(115200);
    Serial.println("Dice Roller Starting...");

    Serial.println("Init Display...");
    DisplayManager::init();
    Serial.println("Init Input...");
    InputManager::init();
    Serial.println("Init Sound...");
    SoundManager::init();
    Serial.println("Load Config...");
    ConfigManager::loadAll();
    Serial.println("Config loaded");

    // 如果配置列表为空，创建一个默认配置（但 loadAll 已经做了）
    if (g_appState.configs.empty())
    {
        Serial.println("No configs, creating default...");
        DiceConfig def;
        def.name = "cfg1";
        def.dice.push_back({6});
        g_appState.configs.push_back(def);
        ConfigManager::saveConfig("cfg1", def.dice);
    }

    // 初始状态
    g_appState.mode = AppMode::Roll;
    g_appState.currentConfigIndex = 0;
    g_appState.selectedConfigIndex = 0;
    g_appState.hasRolled = false;
    g_appState.lastRollResult = 0;

    Serial.println("Drawing initial UI...");
    UIManager::draw(g_appState);
    Serial.println("Setup done.");

    // 测试蜂鸣器
    ledcWrite(0, 128);
    delay(20);
    ledcWrite(0, 0);
}

void loop()
{
    InputManager::scan();
    InputEvent ev = InputManager::getEvent();
    if (ev.action == ACTION_PRESS)
    {
        SoundManager::beep(30);
        // 根据当前模式分发
        switch (g_appState.mode)
        {
        case AppMode::Roll:
            handleRollMode(ev);
            break;
        case AppMode::ConfigList:
            handleConfigList(ev);
            break;
        case AppMode::ConfigDetail:
            handleConfigDetail(ev);
            break;
        }
        UIManager::draw(g_appState);
    }
    delay(10);
}

void handleRollMode(InputEvent ev)
{
    int m; // ✅ 在 switch 外声明
    switch (ev.key)
    {
    case KEY_LEFT:
        m = (int)g_appState.method;
        m = (m - 1 + 4) % 4;
        g_appState.method = (CountMethod)m;
        break;
    case KEY_RIGHT:
        m = (int)g_appState.method;
        m = (m + 1) % 4;
        g_appState.method = (CountMethod)m;
        break;
    case KEY_UP:
        // 切换配置（上一个），此处无需 m
        if (!g_appState.configs.empty())
        {
            g_appState.currentConfigIndex = (g_appState.currentConfigIndex - 1 + g_appState.configs.size()) % g_appState.configs.size();
            g_appState.hasRolled = false;
        }
        break;
    case KEY_DOWN:
        // 切换配置（下一个）
        if (!g_appState.configs.empty())
        {
            g_appState.currentConfigIndex = (g_appState.currentConfigIndex + 1) % g_appState.configs.size();
            g_appState.hasRolled = false;
        }
        break;
    case KEY_A:
    {
        // 投掷
        // const auto &cfg = g_appState.configs[g_appState.currentConfigIndex];
        // if (cfg.dice.empty())
        // {
        //     g_appState.lastRollResult = 0;
        //     g_appState.hasRolled = true;
        // }
        // else
        // {
        //     std::vector<int> results = DiceEngine::roll(cfg.dice);
        //     g_appState.lastRollResult = DiceEngine::compute(results, g_appState.method);
        //     g_appState.hasRolled = true;
        // }

        const auto &cfg = g_appState.configs[g_appState.currentConfigIndex];
        if (cfg.dice.empty())
        {
            g_appState.lastRollResult = 0;
            g_appState.lastRollDetails.clear();
            g_appState.hasRolled = true;
        }
        else
        {
            std::vector<int> results = DiceEngine::roll(cfg.dice);
            g_appState.lastRollResult = DiceEngine::compute(results, g_appState.method);
            g_appState.lastRollDetails = results; // 保存细节
            g_appState.hasRolled = true;
        }
    }
    break;
    case KEY_B:
        // 进入配置列表
        g_appState.mode = AppMode::ConfigList;
        g_appState.selectedConfigIndex = 0;
        break;
    default:
        break;
    }
}

void handleConfigList(InputEvent ev)
{
    switch (ev.key)
    {
    case KEY_UP:
        if (g_appState.selectedConfigIndex > 0)
        {
            g_appState.selectedConfigIndex--;
        }
        break;
    case KEY_DOWN:
    {
        int maxIndex = g_appState.configs.size(); // new + configs
        if (g_appState.selectedConfigIndex < maxIndex)
        {
            g_appState.selectedConfigIndex++;
        }
    }
    break;
    case KEY_A:
    {
        if (g_appState.selectedConfigIndex == 0)
        {
            // 新建配置
            DiceConfig newCfg;
            newCfg.name = ConfigManager::generateName();
            newCfg.dice.push_back({6}); // 默认D6
            g_appState.configs.push_back(newCfg);
            ConfigManager::saveConfig(newCfg.name, newCfg.dice);
            // 进入编辑该配置
            g_appState.editingConfigIndex = g_appState.configs.size() - 1;
            g_appState.detailDiceIndex = 0;
            g_appState.detailDiceOpIndex = 0;
            g_appState.detailConfigOpIndex = 0;
            g_appState.focus = FocusArea::DiceList;
            g_appState.mode = AppMode::ConfigDetail;
        }
        else
        {
            // 选中已存在的配置
            int cfgIndex = g_appState.selectedConfigIndex - 1;
            if (cfgIndex < (int)g_appState.configs.size())
            {
                g_appState.editingConfigIndex = cfgIndex;
                g_appState.detailDiceIndex = 0;
                g_appState.detailDiceOpIndex = 0;
                g_appState.detailConfigOpIndex = 0;
                g_appState.focus = FocusArea::DiceList;
                g_appState.mode = AppMode::ConfigDetail;
            }
        }
    }
    break;
    case KEY_B:
        // 返回投掷模式
        g_appState.mode = AppMode::Roll;
        break;
    default:
        break;
    }
}

void handleConfigDetail(InputEvent ev)
{
    // 确保编辑索引有效
    if (g_appState.editingConfigIndex < 0 || g_appState.editingConfigIndex >= (int)g_appState.configs.size())
    {
        // 无效，返回列表
        g_appState.mode = AppMode::ConfigList;
        return;
    }
    DiceConfig &cfg = g_appState.configs[g_appState.editingConfigIndex];

    switch (ev.key)
    {
    case KEY_B:
        // 切换焦点区域
        switch (g_appState.focus)
        {
        case FocusArea::DiceList:
            g_appState.focus = FocusArea::DiceOps;
            break;
        case FocusArea::DiceOps:
            g_appState.focus = FocusArea::ConfigOps;
            break;
        case FocusArea::ConfigOps:
            g_appState.focus = FocusArea::DiceList;
            break;
        }
        break;

    case KEY_LEFT:
        if (g_appState.focus == FocusArea::DiceList)
        {
            // 切换骰子
            if (cfg.dice.size() > 0)
            {
                g_appState.detailDiceIndex = (g_appState.detailDiceIndex - 1 + cfg.dice.size()) % cfg.dice.size();
            }
        }
        else if (g_appState.focus == FocusArea::DiceOps)
        {
            // 切换骰子操作
            g_appState.detailDiceOpIndex = (g_appState.detailDiceOpIndex - 1 + 3) % 3;
        }
        else if (g_appState.focus == FocusArea::ConfigOps)
        {
            // 切换配置操作
            g_appState.detailConfigOpIndex = (g_appState.detailConfigOpIndex - 1 + 4) % 4;
        }
        break;

    case KEY_RIGHT:
        if (g_appState.focus == FocusArea::DiceList)
        {
            if (cfg.dice.size() > 0)
            {
                g_appState.detailDiceIndex = (g_appState.detailDiceIndex + 1) % cfg.dice.size();
            }
        }
        else if (g_appState.focus == FocusArea::DiceOps)
        {
            g_appState.detailDiceOpIndex = (g_appState.detailDiceOpIndex + 1) % 3;
        }
        else if (g_appState.focus == FocusArea::ConfigOps)
        {
            g_appState.detailConfigOpIndex = (g_appState.detailConfigOpIndex + 1) % 4;
        }
        break;

    case KEY_UP:
        if (g_appState.focus == FocusArea::DiceOps && g_appState.detailDiceOpIndex == 0)
        {
            // 调整面数（增加）
            if (cfg.dice.size() > 0)
            {
                uint8_t cur = cfg.dice[g_appState.detailDiceIndex].sides;
                // 找到下一个更大的预设
                int nextIdx = -1;
                for (int i = 0; i < SIDES_PRESET_COUNT; i++)
                {
                    if (SIDES_PRESET[i] > cur)
                    {
                        nextIdx = i;
                        break;
                    }
                }
                if (nextIdx != -1)
                {
                    cfg.dice[g_appState.detailDiceIndex].sides = SIDES_PRESET[nextIdx];
                } // 否则保持最大
            }
        }
        break;

    case KEY_DOWN:
        if (g_appState.focus == FocusArea::DiceOps && g_appState.detailDiceOpIndex == 0)
        {
            // 调整面数（减小）
            if (cfg.dice.size() > 0)
            {
                uint8_t cur = cfg.dice[g_appState.detailDiceIndex].sides;
                int prevIdx = -1;
                for (int i = SIDES_PRESET_COUNT - 1; i >= 0; i--)
                {
                    if (SIDES_PRESET[i] < cur)
                    {
                        prevIdx = i;
                        break;
                    }
                }
                if (prevIdx != -1)
                {
                    cfg.dice[g_appState.detailDiceIndex].sides = SIDES_PRESET[prevIdx];
                } // 否则保持最小
            }
        }
        break;

    case KEY_A:
        // 执行当前焦点下的操作
        if (g_appState.focus == FocusArea::DiceOps)
        {
            switch (g_appState.detailDiceOpIndex)
            {
            case 0: // Sides
                break;
            case 1: // Del
                if (cfg.dice.size() > 1)
                {
                    cfg.dice.erase(cfg.dice.begin() + g_appState.detailDiceIndex);
                    if (g_appState.detailDiceIndex >= (int)cfg.dice.size())
                    {
                        g_appState.detailDiceIndex = cfg.dice.size() - 1;
                    }
                }
                break;
            case 2: // Add
                if (cfg.dice.size() < 16)
                {
                    cfg.dice.push_back({6});
                    g_appState.detailDiceIndex = cfg.dice.size() - 1;
                }
                else
                {
                    // 已达上限，蜂鸣两声提示
                    SoundManager::beep(30);
                    delay(30);
                    SoundManager::beep(30);
                }
                break;
            }
        }
        else if (g_appState.focus == FocusArea::ConfigOps)
        {
            switch (g_appState.detailConfigOpIndex)
            {
            case 0: // Save
                ConfigManager::saveConfig(cfg.name, cfg.dice);
                g_appState.mode = AppMode::ConfigList;
                break;
            case 1: // Use
                ConfigManager::saveConfig(cfg.name, cfg.dice);
                // 找到该配置在列表中的索引，设置为当前使用
                for (size_t i = 0; i < g_appState.configs.size(); i++)
                {
                    if (g_appState.configs[i].name == cfg.name)
                    {
                        g_appState.currentConfigIndex = i;
                        break;
                    }
                }
                g_appState.mode = AppMode::Roll;
                g_appState.hasRolled = false;
                break;
            case 2: // Remove
                ConfigManager::deleteConfig(cfg.name);
                // 从configs中移除
                for (auto it = g_appState.configs.begin(); it != g_appState.configs.end(); ++it)
                {
                    if (it->name == cfg.name)
                    {
                        g_appState.configs.erase(it);
                        break;
                    }
                }
                g_appState.mode = AppMode::ConfigList;
                // 调整选中索引
                if (g_appState.selectedConfigIndex > (int)g_appState.configs.size())
                {
                    g_appState.selectedConfigIndex = g_appState.configs.size();
                }
                break;
            case 3: // Quit
                g_appState.mode = AppMode::ConfigList;
                break;
            }
        }
        break;

    default:
        break;
    }
}