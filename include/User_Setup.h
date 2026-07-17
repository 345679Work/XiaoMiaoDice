// 定义屏幕驱动芯片
#define ST7735_DRIVER

// 定义屏幕分辨率
#define TFT_WIDTH 128
#define TFT_HEIGHT 160

// 定义颜色顺序（ST7735 通常需要这样设置）
// #define ST7735_REDTAB
#define ST7735_BLACKTAB

#define TFT_BGR  1  // 强制开启 BGR 颜色顺序，修复红蓝反色问题

// 定义 SPI 引脚，与表格一一对应
#define TFT_CS 5
#define TFT_DC 4
#define TFT_RST 19
#define TFT_SCLK 18
#define TFT_MOSI 23

// 字体
#define LOAD_GLCD 1
#define LOAD_FONT2 1

// 使用硬件 SPI
#define SPI_FREQUENCY 40000000 // 40 MHz
#define SPI_READ_FREQUENCY 20000000

#define LINE_HEIGHT 8