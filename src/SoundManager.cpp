// SoundManager.cpp
#include "SoundManager.h"
#include "constants.h"

// 使用 LEDC 产生 PWM 蜂鸣音
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ    2000
#define BUZZER_RES     8

void SoundManager::init() {
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RES);
    ledcAttachPin(PIN_BUZZER, BUZZER_CHANNEL);
    ledcWrite(BUZZER_CHANNEL, 0); // 关闭
}

void SoundManager::beep(int durationMs) {
    ledcWrite(BUZZER_CHANNEL, 128); // 50% duty cycle
    delay(durationMs);
    ledcWrite(BUZZER_CHANNEL, 0);
}