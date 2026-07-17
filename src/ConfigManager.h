// ConfigManager.h
#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Preferences.h>
#include <vector>
#include "AppState.h"

class ConfigManager {
public:
    static void init();
    // 加载所有配置到 g_appState.configs
    static void loadAll();
    // 保存单个配置
    static void saveConfig(const String& name, const std::vector<Die>& dice);
    // 删除配置
    static void deleteConfig(const String& name);
    // 生成唯一名称，如 "cfg1", "cfg2" ...
    static String generateName();
    // 将骰子列表转为显示字符串，如 "D6 D20"
    static String diceToString(const std::vector<Die>& dice);
    // 从字符串解析骰子列表（用于加载）
    static std::vector<Die> stringToDice(const String& str);
    
private:
    static Preferences preferences;
};

#endif