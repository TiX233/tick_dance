/**
 * @file ST7305.h
 * @author realTiX
 * @brief ST7305 2.19 inch 全反射显示屏驱动库，支持设置窗口局刷
 *        spi 上限 33.3Mbit/s
 *        局刷最小刷屏单元为 3 个字节，24 个像素
 *        pix 分布（竖屏，122*250，因为 122 不能整除以 12，所以 x 需要偏移 (122%12==0?0:(12-122%12)) = 10 pix）：
 *                                                                                                          第1列                               ...  第122列
 *                          第一个单元起始列                                                                   |         右侧单元起始列                  |
 *                                |                                                                           |               |                        |
 *                                V                                                                           V               V                        V
 *        第1行，1单元起行   -> [pix1] [pix3] [pix5] [pix7] [tix9]  [pix11] [pix13] [pix15] [pix17] [pix19] [pix21] [pix23] [pix1] [pix3] [pix5] ... [pix23]
 *        第2行，1单元终行      [pix2] [pix4] [pix6] [pix8] [pix10] [pix12] [pix14] [pix16] [pix18] [pix20] [pix22] [pix24] [pix2] [pix4] [tix6] ... [pix24]
 *        第3行，下侧单元起行-> [pix1] [pix3] ...
 *        第4行，下侧单元终行   [pix2] [pix4] ...
 *        ...                  ...
 *        第250行              [pix2] ...
 * 
 *        搞不懂这个屏幕为什么不做成 120*250 的，为了多刷两列得浪费十列，也就是（全刷）刷屏效率只有 92%
 * 
 * @version 0.2
 * @date 2026-01-16 (0.1，初步完成功能)
 *       2026-01-21 (0.2，修改初始化序列，修改设置窗口)
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef __ST7305_H__
#define __ST7305_H__

#include "main.h"

// 如果需要全屏显存的话，开启此宏
// #define ST7305_NEED_FULL_BUFFER

#define LCD_WIDTH                   122
#define LCD_HEIGHT                  250
#define LCD_X_OFFSET                10      // x 需要偏移 10 pix(122%12==0?0:(12-122%12))
#define LCD_UNIT_WIDTH              11      // 122/12 + (122%12==0?0:1) = 11
#define LCD_UNIT_HEIGHT             125     // 250/2 = 125
#ifdef ST7305_NEED_FULL_BUFFER
    #define FULL_BUFFER_LENGTH      4125    // 11*3*125 = 4125，全屏缓存大小
#endif

#define ST7305_PIN_LEVEL_DC_DATA    1
#define ST7305_PIN_LEVEL_DC_CMD     0

typedef enum {
    LCD_CTRL_WRITE_CMD = 0,
    LCD_CTRL_WRITE_DATA,
    LCD_CTRL_DELAY = 0x25,
    LCD_CTRL_OVER = 0x68,
} LCD_CTRL_e;

typedef struct{
    uint8_t ctrl; // LCD_CTRL_e
    union {
        uint16_t data_len;
        uint16_t delay_ms;
    };
    const uint8_t *data_buf;
} lcd_init_seq_stu;

typedef enum {
    color_mono_white = 0,
    color_mono_black = 1,
} color_mono_e;

struct st7305_stu {
    uint8_t is_initialized;

    void (*write_cs)(uint8_t pin_level);
    void (*write_dc)(uint8_t pin_level);
    void (*write_rst)(uint8_t pin_level);

    void (*transmit_data)(const uint8_t *buf, uint16_t len);
    void (*transmit_data_dma)(const uint8_t *buf, uint16_t len);

    void (*delay_ms)(uint32_t ms);
    void (*pin_init)(void);

#ifdef ST7305_NEED_FULL_BUFFER
    uint8_t *full_buffer;
#endif
};

extern lcd_init_seq_stu st7305_init_table[];

#ifdef ST7305_NEED_FULL_BUFFER
int st7305_init(struct st7305_stu *lcd, uint8_t *full_buffer);
#else
int st7305_init(struct st7305_stu *lcd);
#endif
void st7305_clear(struct st7305_stu *lcd, uint8_t color);
void st7305_fill_unit(struct st7305_stu *lcd, uint16_t unit_x, uint16_t unit_y, uint16_t unit_w, uint16_t unit_h, uint8_t color);
void st7305_draw_unit(struct st7305_stu *lcd, uint16_t unit_x, uint16_t unit_y, uint8_t *buffer);

void st7305_write_cmd(struct st7305_stu *lcd, uint8_t cmd);
void st7305_write_data(struct st7305_stu *lcd, const uint8_t *data, uint16_t len);
uint8_t st7305_write_data_dma(struct st7305_stu *lcd, const uint8_t *data, uint16_t len);
void st7305_set_unit_window(struct st7305_stu *lcd, uint16_t unit_x0, uint16_t unit_y0, uint16_t unit_x1, uint16_t unit_y1);

void st7305_power_high(struct st7305_stu *lcd);
void st7305_power_low(struct st7305_stu *lcd);

#ifdef ST7305_NEED_FULL_BUFFER
void st7305_buf_draw_pix(struct st7305_stu *lcd, uint16_t x, uint16_t y, uint8_t color);
void st7305_buf_refresh(struct st7305_stu *lcd);
#endif

#endif // __ST7305_H__
