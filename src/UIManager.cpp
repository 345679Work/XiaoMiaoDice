// UIManager.cpp
#include "UIManager.h"
#include "DisplayManager.h"
#include "ConfigManager.h"
#include "constants.h"

void UIManager::draw(const AppState &state)
{
    DisplayManager::clear();
    switch (state.mode)
    {
    case AppMode::Roll:
        drawRollMode(state);
        break;
    case AppMode::ConfigList:
        drawConfigList(state);
        break;
    case AppMode::ConfigDetail:
        drawConfigDetail(state);
        break;
    }
}

void UIManager::drawDivider(int y)
{
    DisplayManager::drawLine(0, y, SCREEN_WIDTH - 1, y, COLOR_LINE);
}

void UIManager::drawDiceList(const std::vector<Die> &dice, int selectedIndex, int x, int y, int fontSize)
{
    int posX = x;
    for (size_t i = 0; i < dice.size(); i++)
    {
        String label = "D" + String(dice[i].sides);
        bool highlight = (i == (size_t)selectedIndex);
        DisplayManager::drawHighlightedText(posX, y, fontSize, label, highlight);
        posX += DisplayManager::getTextWidth(label + " ", fontSize);
    }
}

void UIManager::drawRollMode(const AppState &state)
{
    int y = 0;

    // ---- 结果区（2号字体） ----
    String resultText;
    uint16_t resultColor = COLOR_RESULT; // 默认黄色
    if (state.hasRolled)
    {
        resultText = "Result: " + String(state.lastRollResult);
        // 根据当前计数方式计算理论最大和最小值
        if (!state.configs.empty())
        {
            const auto &cfg = state.configs[state.currentConfigIndex];
            int maxVal = 0, minVal = 0;
            int maxSides = 0, minSides = 1000, sumSides = 0;
            int numDice = cfg.dice.size();
            for (const auto &d : cfg.dice)
            {
                if (d.sides > maxSides)
                    maxSides = d.sides;
                if (d.sides < minSides)
                    minSides = d.sides;
                sumSides += d.sides;
            }
            if (numDice > 0)
            {
                switch (state.method)
                {
                case CountMethod::Max:
                    maxVal = maxSides;
                    minVal = 1;
                    break;
                case CountMethod::Min:
                    maxVal = minSides;
                    minVal = 1;
                    break;
                case CountMethod::Sum:
                    maxVal = sumSides;
                    minVal = numDice;
                    break;
                case CountMethod::Avg:
                    maxVal = sumSides / numDice;
                    minVal = 1;
                    break;
                }
            }
            // 颜色判断
            if (state.lastRollResult == maxVal)
            {
                resultColor = TFT_RED;
            }
            else if (state.lastRollResult == minVal)
            {
                resultColor = TFT_BLUE;
            }
            else if (state.lastRollResult > maxVal / 2)
            {
                resultColor = TFT_YELLOW;
            }
            else if (state.lastRollResult < maxVal / 2)
            {
                resultColor = TFT_GREEN;
            }
            else
            {                             // 等于一半
                resultColor = TFT_YELLOW; // 或者 TFT_GREEN，随你
            }
        }
    }
    else
    {
        resultText = "--";
        resultColor = COLOR_RESULT;
    }
    DisplayManager::drawText(5, y, FONT_SIZE_TITLE, resultText, resultColor);
    y += DisplayManager::getTextHeight(FONT_SIZE_TITLE) + 2;

    // ---- 骰子细节（每个骰子的结果） ----
    if (state.hasRolled && !state.lastRollDetails.empty())
    {
        String detailStr;
        for (int v : state.lastRollDetails)
        {
            if (detailStr.length() > 0)
                detailStr += " ";
            detailStr += String(v);
        }
        DisplayManager::drawText(5, y, FONT_SIZE_NORMAL, detailStr);
        y += DisplayManager::getTextHeight(FONT_SIZE_NORMAL) + 4;
    }
    else
    {
        y += DisplayManager::getTextHeight(FONT_SIZE_NORMAL) + 4;
    }

    // ---- 骰子配置 ----
    if (!state.configs.empty())
    {
        const auto &cfg = state.configs[state.currentConfigIndex];
        int posX = 5;
        int yPos = y;
        int countInRow = 0;
        int maxPerRow = 4;
        for (const auto &d : cfg.dice)
        {
            String label = String(d.sides); // 只数字
            if (countInRow >= maxPerRow)
            {
                countInRow = 0;
                posX = 5;
                yPos += DisplayManager::getTextHeight(FONT_SIZE_HEADING) + 2;
            }
            DisplayManager::drawText(posX, yPos, FONT_SIZE_HEADING, label);
            posX += DisplayManager::getTextWidth(label, FONT_SIZE_HEADING) + 8;
            countInRow++;
        }
        y = yPos + DisplayManager::getTextHeight(FONT_SIZE_HEADING) + 4;
    }

    // ---- 分割线 ----
    int dividerY = SCREEN_HEIGHT - DisplayManager::getTextHeight(FONT_SIZE_NORMAL) - 6;
    drawDivider(dividerY);

    // ---- 计数方式 ----
    const char *methods[] = {"Max", "Min", "Sum", "Avg"};
    int methodIndex = (int)state.method;
    int posX = 5;
    for (int i = 0; i < 4; i++)
    {
        bool highlight = (i == methodIndex);
        String label = methods[i];
        DisplayManager::drawHighlightedText(posX, dividerY + 4, FONT_SIZE_NORMAL, label, highlight);
        posX += DisplayManager::getTextWidth(label, FONT_SIZE_NORMAL) + 8;
    }
}

void UIManager::drawConfigList(const AppState &state)
{
    int y = 0;
    DisplayManager::drawText(5, y, FONT_SIZE_NORMAL, "Config List");
    y += DisplayManager::getTextHeight(FONT_SIZE_NORMAL) + 2;
    drawDivider(y);
    y += 2;

    // 所有列表项用1号字体
    String newLabel = "[new config]";
    bool highlight = (state.selectedConfigIndex == 0);
    DisplayManager::drawHighlightedText(5, y, FONT_SIZE_NORMAL, newLabel, highlight);
    y += DisplayManager::getTextHeight(FONT_SIZE_NORMAL) + 2;

    for (size_t i = 0; i < state.configs.size(); i++)
    {
        String label = String(i + 1) + ". " + ConfigManager::diceToString(state.configs[i].dice);
        bool highlight2 = (state.selectedConfigIndex == (int)(i + 1));
        DisplayManager::drawHighlightedText(5, y, FONT_SIZE_NORMAL, label, highlight2);
        y += DisplayManager::getTextHeight(FONT_SIZE_NORMAL) + 2;
        if (y > SCREEN_HEIGHT - 20)
            break;
    }
}

void UIManager::drawConfigDetail(const AppState &state)
{
    if (state.editingConfigIndex < 0 || state.editingConfigIndex >= (int)state.configs.size())
        return;
    const DiceConfig &cfg = state.configs[state.editingConfigIndex];
    if (cfg.dice.empty())
        return;

    // ===== 骰子列表区域（每行最多4个，2号字体，只显示数字） =====
    int startY = 0;
    int posX = 5;
    int lineHeight = DisplayManager::getTextHeight(FONT_SIZE_TITLE) + 4; // 行高
    int maxPerRow = 4;
    int drawnCount = 0;

    for (size_t i = 0; i < cfg.dice.size(); i++)
    {
        // 只显示面数数字，不加"D"
        String label = String(cfg.dice[i].sides);
        bool highlight = (i == (size_t)state.detailDiceIndex);

        // 如果当前行已有4个，换行
        if (drawnCount >= maxPerRow)
        {
            drawnCount = 0;
            posX = 5;
            startY += lineHeight;
        }

        DisplayManager::drawHighlightedText(posX, startY, FONT_SIZE_TITLE, label, highlight);
        posX += DisplayManager::getTextWidth(label, FONT_SIZE_TITLE) + 8; // 间距
        drawnCount++;
    }

    // 计算骰子区域占用的总高度（用于下方分割线定位）
    int totalRows = (cfg.dice.size() + maxPerRow - 1) / maxPerRow;
    if (totalRows == 0)
        totalRows = 1;
    int diceAreaBottom = startY + lineHeight; // 最后一行底部

    // ===== 分割线（在骰子列表下方，骰子操作区上方） =====
    int dividerY = diceAreaBottom + 4;
    // 确保分割线不会与底部的操作区重叠，如果骰子太多则压缩
    int bottomOpsHeight = DisplayManager::getTextHeight(FONT_SIZE_NORMAL) * 2 + 8 + 4; // 两行操作区 + 分割线
    int maxDividerY = SCREEN_HEIGHT - bottomOpsHeight - 4;
    if (dividerY > maxDividerY)
    {
        dividerY = maxDividerY;
    }
    drawDivider(dividerY);

    // ===== 骰子操作区（1号字体） =====
    int opsStartY = dividerY + 4;
    String diceOps[] = {"Sides " + String(cfg.dice[state.detailDiceIndex].sides), "Del", "Add"};
    int posX2 = 5;
    for (int i = 0; i < 3; i++)
    {
        bool highlight = (state.focus == FocusArea::DiceOps && state.detailDiceOpIndex == i);
        DisplayManager::drawHighlightedText(posX2, opsStartY, FONT_SIZE_NORMAL, diceOps[i], highlight);
        posX2 += DisplayManager::getTextWidth(diceOps[i], FONT_SIZE_NORMAL) + 10;
    }
    int secondRowY = opsStartY + DisplayManager::getTextHeight(FONT_SIZE_NORMAL) + 4;

    // ===== 分割线（两个操作区之间） =====
    drawDivider(secondRowY - 2);

    // ===== 配置操作区（1号字体） =====
    String configOps[] = {"Save", "Use", "Remove", "Quit"};
    posX2 = 5;
    for (int i = 0; i < 4; i++)
    {
        bool highlight = (state.focus == FocusArea::ConfigOps && state.detailConfigOpIndex == i);
        DisplayManager::drawHighlightedText(posX2, secondRowY, FONT_SIZE_NORMAL, configOps[i], highlight);
        posX2 += DisplayManager::getTextWidth(configOps[i], FONT_SIZE_NORMAL) + 10;
    }
}
