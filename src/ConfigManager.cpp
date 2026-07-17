// ConfigManager.cpp
#include "ConfigManager.h"
#include "constants.h"
#include <Arduino.h>

Preferences ConfigManager::preferences;

void ConfigManager::init() {
    // 不需要每次初始化，在 loadAll 时打开
}

void ConfigManager::loadAll() {
    preferences.begin(PREF_NAMESPACE, false); // 读写模式
    // 读取所有键值对？Preferences没有枚举所有键的API，我们用一个键来存储配置列表名称
    // 简单方案：存储一个 "list" 字符串，用逗号分隔配置名
    // 或者我们使用固定数量的配置（比如最多10个），存储为 "cfg0", "cfg1" ...
    // 这里采用存储配置名列表的方式
    String listStr = preferences.getString("list", "");
    g_appState.configs.clear();
    if (listStr.length() == 0) {
        // 如果没有配置，创建一个默认配置 D6
        DiceConfig def;
        def.name = "cfg1";
        def.dice.push_back({6});
        g_appState.configs.push_back(def);
        // 保存这个默认配置
        saveConfig("cfg1", def.dice);
        // 更新list
        listStr = "cfg1";
        preferences.putString("list", listStr);
    } else {
        // 解析 listStr，按逗号分割
        int start = 0;
        while (start < listStr.length()) {
            int comma = listStr.indexOf(',', start);
            String name;
            if (comma == -1) {
                name = listStr.substring(start);
                start = listStr.length();
            } else {
                name = listStr.substring(start, comma);
                start = comma + 1;
            }
            // 读取该配置的骰子字符串
            String diceStr = preferences.getString(name.c_str(), "");
            if (diceStr.length() > 0) {
                DiceConfig cfg;
                cfg.name = name;
                cfg.dice = stringToDice(diceStr);
                g_appState.configs.push_back(cfg);
            }
        }
    }
    preferences.end();
}

void ConfigManager::saveConfig(const String& name, const std::vector<Die>& dice) {
    preferences.begin(PREF_NAMESPACE, false);
    // 将骰子列表转为字符串保存
    String diceStr;
    for (size_t i=0; i<dice.size(); i++) {
        if (i>0) diceStr += " ";
        diceStr += "D" + String(dice[i].sides);
    }
    preferences.putString(name.c_str(), diceStr);
    // 更新列表
    String listStr = preferences.getString("list", "");
    // 如果name不在list中，添加
    if (listStr.indexOf(name) == -1) {
        if (listStr.length() > 0) listStr += ",";
        listStr += name;
        preferences.putString("list", listStr);
    }
    preferences.end();
}

void ConfigManager::deleteConfig(const String& name) {
    preferences.begin(PREF_NAMESPACE, false);
    preferences.remove(name.c_str());
    // 从list中移除
    String listStr = preferences.getString("list", "");
    int index = listStr.indexOf(name);
    if (index != -1) {
        int end = index + name.length();
        if (end < listStr.length() && listStr[end] == ',') {
            listStr.remove(index, name.length()+1);
        } else if (index > 0 && listStr[index-1] == ',') {
            listStr.remove(index-1, name.length()+1);
        } else {
            listStr.remove(index, name.length());
        }
        preferences.putString("list", listStr);
    }
    preferences.end();
}

String ConfigManager::generateName() {
    int i = 1;
    while (true) {
        String name = "cfg" + String(i);
        // 检查是否已存在
        preferences.begin(PREF_NAMESPACE, true);
        bool exists = preferences.isKey(name.c_str());
        preferences.end();
        if (!exists) return name;
        i++;
    }
}

String ConfigManager::diceToString(const std::vector<Die>& dice) {
    String s;
    for (size_t i=0; i<dice.size(); i++) {
        if (i>0) s += " ";
        s += "D" + String(dice[i].sides);
    }
    return s;
}

std::vector<Die> ConfigManager::stringToDice(const String& str) {
    std::vector<Die> dice;
    int start = 0;
    while (start < str.length()) {
        // 跳过空格
        while (start < str.length() && str[start] == ' ') start++;
        if (start >= str.length()) break;
        // 找下一个空格
        int end = str.indexOf(' ', start);
        if (end == -1) end = str.length();
        String token = str.substring(start, end);
        // token 格式为 "D6", "D20" 等
        if (token.length() >= 2 && token[0] == 'D') {
            int sides = token.substring(1).toInt();
            if (sides > 0) {
                Die d;
                d.sides = (uint8_t)sides;
                dice.push_back(d);
            }
        }
        start = end + 1;
    }
    return dice;
}