#include "ST7305.h"

#if 0 // 这里是官方的默认参数
lcd_init_seq_stu st7305_init_table[] = {
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xD6}}, // NVM Load Control
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X17}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X02}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xD1}}, // Booster Enable
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X01}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC0}}, // Gate Voltage Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X0E}}, // VGH=15V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X05}}, // VGL=-7.5V

// 电压设置 起始
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC1}}, // VSHP Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X41}}, // VSHP1=5V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X41}}, // VSHP2=5V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X41}}, // VSHP3=5V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X41}}, // VSHP4=5V

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC2}}, // VSLP Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X32}}, // VSLP1=1V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X32}}, // VSLP2=1V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X32}}, // VSLP3=1V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X32}}, // VSLP4=1V

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC4}}, // VSHN Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X4B}}, // VSHN1=-4V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X4B}}, // VSHN2=-4V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X4B}}, // VSHN3=-4V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X4B}}, // VSHN4=-4V

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC5}}, // VSLN Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X00}}, // VSLN1=1V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X00}}, // VSLN2=1V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X00}}, // VSLN3=1V
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X00}}, // VSLN4=1V
// 电压设置 结束

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xD8}}, // HPM=32Hz
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0XA6}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0XE9}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB2}}, // Frame Rate Control
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X11}}, // HPM=32hz ; LPM=0.5hz

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB3}}, // Update Period Gate EQ Control in HPM
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0XE5}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0XF6}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X05}}, // HPM EQ Control
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X46}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X77}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X77}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X77}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X77}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X76}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X45}}, //

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB4}}, // Update Period Gate EQ Control in LPM
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X05}}, // LPM EQ Control
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X46}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X77}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X77}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X77}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X77}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X76}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X45}}, //

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB7}}, // Source EQ Enable
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X13}}, //

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB0}}, // Gate Line Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X3F}}, // 252 line

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x11}}, // Sleep out
    {LCD_CTRL_DELAY, 120}, // 延时 120 ms

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC9}}, // Source Voltage Select
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x00}}, // VSHP1; VSLP1 ; VSHN1 ; VSLN1

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC7}}, // ultra low power code
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0xC1}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x41}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x26}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x36}}, // Memory Data Access Control
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X00}}, // MX=0 ; DO=0

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x3A}}, // Data Format Select
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X11}}, // 10:4write for 24bit ; 11: 3write for 24bit

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB9}}, // Gamma Mode Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X20}}, // 20: Mono 00:4GS

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB8}}, // Panel Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X25}}, // dot inversion; one line interval; dot inversion

    // {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x21}}, // Inverse

    // WRITE RAM 122x250
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x2A}}, // Column Address Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X19}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X23}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x2B}}, // Row Address Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X00}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X7C}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x35}}, // TE
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X00}}, //

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xD0}}, // Auto power down
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0XFF}}, //

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x39}}, // 0x39 low power 0x38 high power
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x29}}, // DISPLAY ON

    // 初始化结束
    {LCD_CTRL_OVER, },
};
#endif

lcd_init_seq_stu st7305_init_table[] = {
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xD6}}, // NVM Load Control
    {LCD_CTRL_WRITE_DATA, 2, (const uint8_t []){0X17, 0X02}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xD1}}, // Booster Enable
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X01}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC0}}, // Gate Voltage Setting
    {LCD_CTRL_WRITE_DATA, 2, (const uint8_t []){0X12, 0X0A}}, // VGH=15V VGL=-7.5V

// 电压设置 起始
    // VSHPCTRL (Source High Positive Voltage Control )
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC1}},
    {LCD_CTRL_WRITE_DATA, 4, (const uint8_t []){0x3C, 0x3E, 0x3C, 0x3C}},
   
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC2}},
    {LCD_CTRL_WRITE_DATA, 4, (const uint8_t []){0x23, 0x21, 0x23, 0x23}},
   
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC4}},
    {LCD_CTRL_WRITE_DATA, 4, (const uint8_t []){0x5A, 0x5C, 0x5A, 0x5A}},
   
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC5}},
    {LCD_CTRL_WRITE_DATA, 4, (const uint8_t []){0x37, 0x35, 0x37, 0x37}},
// 电压设置 结束

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xD8}},
    // {LCD_CTRL_WRITE_DATA, 2, (const uint8_t []){0X80, 0XE9}}, // HPM=25.5/51Hz
    // {LCD_CTRL_WRITE_DATA, 2, (const uint8_t []){0XA6, 0XE9}}, // HPM=16/32Hz
    {LCD_CTRL_WRITE_DATA, 2, (const uint8_t []){0X26, 0XE9}}, // 关闭 osc

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB2}}, // Frame Rate Control
    // {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X15}}, // 高功耗下 32/51 Hz，低功耗下 8Hz
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X05}}, // 高功耗下 16/25.5 Hz，低功耗下 8Hz，不知道为什么改啥都是 8Hz
    // {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X00}}, // 高功耗下 16/25.5 Hz，低功耗下 0.25Hz
    // {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X10}}, // 高功耗下 32/51 Hz，低功耗下 0.25Hz

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB3}}, // Update Period Gate EQ Control in HPM
    {LCD_CTRL_WRITE_DATA, 10, (const uint8_t []){0XE5, 0XF6, 0X17, 0X77, 0X77, 0X77, 0X77, 0X77, 0X77, 0X71}}, //
   
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB4}}, // Update Period Gate EQ Control in LPM
    {LCD_CTRL_WRITE_DATA, 8, (const uint8_t []){0X05, 0X46, 0X77, 0X77, 0X77, 0X77, 0X76, 0X45}}, //

    // 
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x62}},
    {LCD_CTRL_WRITE_DATA, 3, (const uint8_t []){0x32, 0x03, 0x1F}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB7}}, // Source EQ Enable
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X13}}, //

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB0}}, // Gate Line Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X3F}}, // 252 line

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x11}}, // Sleep out
    {LCD_CTRL_DELAY, 120}, // 延时 120 ms

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC9}}, // Source Voltage Select
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x00}}, // VSHP1; VSLP1 ; VSHN1 ; VSLN1

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x36}}, // Memory Data Access Control
    // {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x00}}, //
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X48}}, //

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x3A}}, // Data Format Select
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X11}}, // 10:4write for 24bit ; 11: 3write for 24bit

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB9}}, // Gamma Mode Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X20}}, // 20: Mono 00:4GS

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xB8}}, // Panel Setting
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X29}}, //

    // WRITE RAM 122x250
    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x2A}}, // Column Address Setting
    {LCD_CTRL_WRITE_DATA, 2, (const uint8_t []){0X19, 0X23}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x2B}}, // Row Address Setting
    {LCD_CTRL_WRITE_DATA, 2, (const uint8_t []){0X00, 0X7C}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x35}}, // TE
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0X00}}, //
    // {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x34}}, // TE off

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xD0}}, // Auto power down
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0XFF}}, //


    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x39}}, // 0x39 low power 0x38 high power
    {LCD_CTRL_DELAY, 20}, // 延时 20 ms

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xBB}},
    {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x4F}},

    {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x29}}, // DISPLAY ON
    // {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x20}}, // 关闭反转

    // {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0xC7}}, // ultra low power code
    // {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0xC1}},
    // {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x41}},
    // {LCD_CTRL_WRITE_DATA, 1, (const uint8_t []){0x26}},

    // {LCD_CTRL_DELAY, 2}, // 延时 2 ms
    // {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x38}}, // 0x39 low power 0x38 high power
    // {LCD_CTRL_WRITE_CMD, 1, (const uint8_t []){0x29}}, // DISPLAY ON

    // 初始化结束
    {LCD_CTRL_OVER, },
};

uint8_t st_cmd_buffer[2] = {0, 0};
void st7305_write_cmd(struct st7305_stu *lcd, uint8_t cmd){
    lcd->write_dc(ST7305_PIN_LEVEL_DC_CMD);
    lcd->transmit_data(&cmd, 1);
}

void st7305_write_data(struct st7305_stu *lcd, const uint8_t *data, uint16_t len){
    lcd->write_dc(ST7305_PIN_LEVEL_DC_DATA);
    lcd->transmit_data(data, len);
}

uint8_t st7305_write_data_dma(struct st7305_stu *lcd, const uint8_t *data, uint16_t len){

    lcd->write_dc(ST7305_PIN_LEVEL_DC_DATA);
    lcd->transmit_data_dma(data, len);

    return 0;
}

#ifdef ST7305_NEED_FULL_BUFFER
int st7305_init(struct st7305_stu *lcd, uint8_t *full_buffer){
#else
int st7305_init(struct st7305_stu *lcd){
#endif
    if( !lcd->write_cs || 
        !lcd->write_dc  || 
        !lcd->write_rst || 
        !lcd->transmit_data || 
        !lcd->transmit_data_dma || 
        !lcd->delay_ms || 
        !lcd->pin_init){

        return -1;
    }
#ifdef ST7305_NEED_FULL_BUFFER
    if(full_buffer == NULL){
        return -1;
    }
    lcd->full_buffer = full_buffer;
#endif

    lcd->is_initialized = 0;
    lcd->pin_init();

lcd->write_cs(1);

    // 复位
    lcd->write_rst(1);
    lcd->delay_ms(10);
    lcd->write_rst(0);
    lcd->delay_ms(10);
    lcd->write_rst(1);
    lcd->delay_ms(10);

lcd->write_cs(0);

    for(uint16_t i = 0; ; i++){
        switch(st7305_init_table[i].ctrl){
            case LCD_CTRL_WRITE_CMD:
                st7305_write_cmd(lcd, st7305_init_table[i].data_buf[0]);
                break;
                
            case LCD_CTRL_WRITE_DATA:
                st7305_write_data(lcd, st7305_init_table[i].data_buf, st7305_init_table[i].data_len);
                break;
                
            case LCD_CTRL_DELAY:
                lcd->delay_ms(st7305_init_table[i].delay_ms);
                break;
                
            case LCD_CTRL_OVER:
                lcd->is_initialized = 1;
                return 0;
                break;

            default:
                return -2;
                break;
        }
    }

    return -3;
}

/**
 * @brief 设置刷屏的窗口
 */
void st7305_set_unit_window(struct st7305_stu *lcd, uint16_t unit_x0, uint16_t unit_y0, uint16_t unit_x1, uint16_t unit_y1){
    if(unit_x0 > LCD_UNIT_WIDTH || unit_x1 > LCD_UNIT_WIDTH || unit_y0 > LCD_UNIT_HEIGHT || unit_y1 > LCD_UNIT_HEIGHT){
        return ;
    }
    // if(unit_x1 < unit_x0 || unit_y1 < unit_y0){
    //     return ;
    // }

    uint8_t temp[2];

    st7305_write_cmd(lcd, 0x2A); // 指定列范围
    // temp[0] = unit_x0 + 0x19; // x 起始
    // temp[1] = unit_x1 + 0x19; // x 结尾
    temp[0] = LCD_UNIT_WIDTH-1-unit_x1 + 0x19; // x 起始
    temp[1] = LCD_UNIT_WIDTH-1-unit_x0 + 0x19; // x 结尾
    st7305_write_data(lcd, temp, 2);

    st7305_write_cmd(lcd, 0x2B); // 指定行范围
    temp[0] = unit_y0; // y 起始
    temp[1] = unit_y1; // y 结尾
    st7305_write_data(lcd, temp, 2);

    st7305_write_cmd(lcd, 0x2C); // 开始写内存
}

/**
 * @brief 以单元为单位填充矩形范围的单色块，阻塞
 */
void st7305_fill_unit(struct st7305_stu *lcd, uint16_t unit_x, uint16_t unit_y, uint16_t unit_w, uint16_t unit_h, uint8_t color){
    st7305_set_unit_window(lcd, unit_x, unit_y, unit_x + unit_w - 1, unit_y + unit_h - 1);

    uint8_t unit_data_buf[3];
    if(color){
        unit_data_buf[0] = 0xff;
        unit_data_buf[1] = 0xff;
        unit_data_buf[2] = 0xff;
    }else {
        unit_data_buf[0] = 0x00;
        unit_data_buf[1] = 0x00;
        unit_data_buf[2] = 0x00;
    }

    for(uint8_t i = 0; i < unit_w; i ++){
        for(uint8_t j = 0; j < unit_h; j ++){
            st7305_write_data(lcd, unit_data_buf, 3);
        }
    }
}

/**
 * @brief 绘制屏幕某个单元为 buffer 内容（3 个字节），阻塞
 */
void st7305_draw_unit(struct st7305_stu *lcd, uint16_t unit_x, uint16_t unit_y, uint8_t *buffer){
    st7305_set_unit_window(lcd, unit_x, unit_y, unit_x, unit_y);

    st7305_write_data(lcd, buffer, 3);
}

/**
 * @brief 清屏函数，将全屏幕设置为一种颜色，阻塞
 */
void st7305_clear(struct st7305_stu *lcd, uint8_t color){
    st7305_fill_unit(lcd, 0, 0, LCD_UNIT_WIDTH, LCD_UNIT_HEIGHT, color);
}


void st7305_power_high(struct st7305_stu *lcd){
    st7305_write_cmd(lcd, 0x38);
    // st7305_write_cmd(lcd, 0x11);
}

void st7305_power_low(struct st7305_stu *lcd){
    st7305_write_cmd(lcd, 0x39);
    // st7305_write_cmd(lcd, 0x10);
}

// 以下是开启了全屏缓存所支持的函数
#ifdef ST7305_NEED_FULL_BUFFER

/**
 * @brief 给全局数组绘制一个点
 */
void st7305_buf_draw_pix(struct st7305_stu *lcd, uint16_t x, uint16_t y, uint8_t color){
    
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) {
        return;
    }
   
    x += LCD_X_OFFSET;
   
    uint16_t unit_x = x >> 2;
    uint16_t unit_y = y >> 1;
    uint16_t byte_index = unit_y * LCD_UNIT_WIDTH * 3 + unit_x;
   
    uint8_t line_bit_4 = x % 4;
    uint8_t bit_position = 7 - (line_bit_4 * 2 + (y % 2));
   
    if (color != 0) {
        lcd->full_buffer[byte_index] |= (1 << bit_position);
    } else {
        lcd->full_buffer[byte_index] &= ~(1 << bit_position);
    }
}

/**
 * @brief 将全局数组刷新到显示屏上，阻塞
 */
void st7305_buf_refresh(struct st7305_stu *lcd){
    st7305_set_unit_window(lcd, 0, 0, LCD_UNIT_WIDTH - 1, LCD_UNIT_HEIGHT - 1);
    st7305_write_data(lcd, lcd->full_buffer, FULL_BUFFER_LENGTH);
}

#endif
