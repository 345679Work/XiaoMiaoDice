// DiceEngine.h
#ifndef DICE_ENGINE_H
#define DICE_ENGINE_H

#include <vector>
#include "AppState.h"

class DiceEngine {
public:
    // 投掷一组骰子，返回每个骰子的结果
    static std::vector<int> roll(const std::vector<Die>& dice);
    // 根据计数方式计算最终结果
    static int compute(const std::vector<int>& results, CountMethod method);
};

#endif