// DiceEngine.cpp
#include "DiceEngine.h"
#include <esp_random.h>

std::vector<int> DiceEngine::roll(const std::vector<Die>& dice) {
    std::vector<int> results;
    results.reserve(dice.size());
    for (const auto& d : dice) {
        // 生成 1 ~ sides 的随机数
        uint32_t r = esp_random();
        int val = (r % d.sides) + 1;
        results.push_back(val);
    }
    return results;
}

int DiceEngine::compute(const std::vector<int>& results, CountMethod method) {
    if (results.empty()) return 0;
    int sum = 0;
    int maxVal = results[0];
    int minVal = results[0];
    for (int v : results) {
        sum += v;
        if (v > maxVal) maxVal = v;
        if (v < minVal) minVal = v;
    }
    switch (method) {
        case CountMethod::Max: return maxVal;
        case CountMethod::Min: return minVal;
        case CountMethod::Sum: return sum;
        case CountMethod::Avg: return sum / results.size(); // 整数平均，截断
    }
    return sum;
}