#include "myAPP_device_init.h"
#include "ltx.h"
#include "ltx_app.h"
#include "ltx_param.h"
#include "ltx_log.h"
#include "ltx_script.h"
#include "ltx_event_group.h"
#include "ST7305.h"
#include "myAPP_system.h"
#include "myAPP_display.h"
#include "myAPP_button.h"

// 所需初始化外部硬件完成事件
#define EVENT_INIT_LCD_OVER         0x0001

// 函数声明
// 显示屏回调
void myLCD_write_cs(uint8_t pin_level);
void myLCD_write_dc(uint8_t pin_level);
void myLCD_write_rst(uint8_t pin_level);
void myLCD_transmit_data(const uint8_t *buf, uint16_t len);
void myLCD_transmit_data_dma(const uint8_t *buf, uint16_t len);
void myLCD_pin_init(void);
// lcd 初始化脚本回调
void script_cb_lcd_init(struct ltx_Script_stu *script);
// lcd 清屏脚本回调
void script_cb_lcd_clear(struct ltx_Script_stu *script);
// 绘制 logo 脚本回调
void script_cb_draw_logo(struct ltx_Script_stu *script);

// 外部硬件初始化完成事件组回调
void eventg_cb_device_init_over(struct ltx_Event_group_stu *event);

uint8_t lcd_buffer0[512];
uint8_t lcd_buffer1[512];

// dma 发送完成话题
struct ltx_Topic_stu topic_spi_tx_over = _LTX_TOPIC_DEAFULT_CONFIG(topic_spi_tx_over);
// lcd 清屏完成话题
struct ltx_Topic_stu topic_lcd_clear_over = _LTX_TOPIC_DEAFULT_CONFIG(topic_lcd_clear_over);

// 所有外部硬件初始化完成事件组
struct ltx_Event_group_stu eventg_device_init_over;


// 显示屏对象结构体
struct st7305_stu myLCD = {
    .is_initialized = 0,

    .write_cs = myLCD_write_cs,
    .write_dc = myLCD_write_dc,
    .write_rst = myLCD_write_rst,

    .transmit_data = myLCD_transmit_data,
    .transmit_data_dma = myLCD_transmit_data_dma,

    .delay_ms = HAL_Delay,
    .pin_init = myLCD_pin_init,
};

// 显示屏初始化脚本对象结构体
struct ltx_Script_stu script_lcd_init;
// lcd 清屏脚本
struct ltx_Script_stu script_lcd_clear;
// 绘制开机 logo 脚本
struct ltx_Script_stu script_draw_logo;

// app 相关
int myAPP_device_init_init(struct ltx_App_stu *app){
    // 创建显示屏初始脚本
    ltx_Script_init(&script_lcd_init, script_cb_lcd_init, 0);

    // 创建所有外部硬件初始化完成事件组
    ltx_Event_group_init(&eventg_device_init_over, eventg_cb_device_init_over, EVENT_INIT_LCD_OVER, 10000); // 10s 超时时间

    return 0;
}

int myAPP_device_init_pause(struct ltx_App_stu *app){

    ltx_Script_pause(&script_lcd_init);

    return 0;
}

int myAPP_device_init_resume(struct ltx_App_stu *app){

    ltx_Script_resume(&script_lcd_init);

    return 0;
}

int myAPP_device_init_destroy(struct ltx_App_stu *app){

    ltx_Script_pause(&script_lcd_init);

    ltx_Event_group_cancel(&eventg_device_init_over);

    return 0;
}

struct ltx_App_stu app_device_init = {
    .is_initialized = 0,
    .status = ltx_App_status_pause,
    .name = "device_init",

    .init = myAPP_device_init_init,
    .pause = myAPP_device_init_pause,
    .resume = myAPP_device_init_resume,
    .destroy = myAPP_device_init_destroy,

    .task_list = NULL,
    
    .next = NULL,
};


// lcd 初始化脚本回调
void script_cb_lcd_init(struct ltx_Script_stu *script){
    // if(ltx_Script_get_triger_type(script) == SC_TRIGER_RESET){ // 外部要求此脚本重置，可在这里做释放资源等操作
    //     return ;
    // }
    switch(script->step_now){
        case 0:
            // 准备复位
            LTX_LOG_INFO("Start init LCD...\n");
            myLCD.is_initialized = 0;
            myLCD.write_cs(1);
            myLCD.write_rst(1);

            // 10ms 后复位
            ltx_Script_next_step_delay(script, script->step_now + 1, 10);
            break;

        case 1:
            // 复位
            myLCD.write_rst(0);

            // 10ms 后释放
            ltx_Script_next_step_delay(script, script->step_now + 1, 10);

            break;

        case 2:
            // 释放复位
            myLCD.write_rst(1);

            // 10ms 后初始化
            ltx_Script_next_step_delay(script, script->step_now + 1, 10);

            break;

        case 3:
            // 拉低片选
            myLCD.write_cs(0);

            // 进入初始化
            ltx_Script_next_step_delay(script, 9, 0);

            break;

        default:
            // 初始化显示屏
            uint32_t i = script->step_now - 9;
            switch(st7305_init_table[i].ctrl){
                case LCD_CTRL_WRITE_CMD:
                    st7305_write_cmd(&myLCD, st7305_init_table[i].data_buf[0]);
                    ltx_Script_next_step_delay(script, script->step_now + 1, 0);
                    break;
                    
                case LCD_CTRL_WRITE_DATA:
                    st7305_write_data(&myLCD, st7305_init_table[i].data_buf, st7305_init_table[i].data_len);
                    ltx_Script_next_step_delay(script, script->step_now + 1, 0);
                    break;
                    
                case LCD_CTRL_DELAY:
                    // lcd->delay_ms(st7305_init_table[i].delay_ms);
                    ltx_Script_next_step_delay(script, script->step_now + 1, st7305_init_table[i].delay_ms);
                    break;
                    
                case LCD_CTRL_OVER:
                    // 初始化完成
                    myLCD.is_initialized = 1;
                    LTX_LOG_INFO("LCD init Okay at step: %d, at %dms\n", script->step_now, ltx_Sys_get_tick());
                    // 结束脚本
                    ltx_Script_next_step_over(script);
                    // 发布 lcd 初始化完成事件
                    ltx_Event_group_publish(&eventg_device_init_over, EVENT_INIT_LCD_OVER);

                    break;

                default: // 未知步骤类型
                    LTX_LOG_ERRO("LCD init Error at step: %d, case: %d\n", script->step_now, st7305_init_table[i].ctrl);
                    // 结束脚本
                    ltx_Script_next_step_over(script);
                    _SYS_ERROR(SYS_ERROR_LCD | SYS_ERROR_LCD_INIT, "LCD init Failed!");

                    break;
            }

            break;
    }

}


// 显示屏回调
void myLCD_write_cs(uint8_t pin_level){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, pin_level);
}

void myLCD_write_dc(uint8_t pin_level){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, pin_level);
}

void myLCD_write_rst(uint8_t pin_level){
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, pin_level);
}

void myLCD_transmit_data(const uint8_t *buf, uint16_t len){
    HAL_StatusTypeDef spi_tx_status = HAL_SPI_Transmit(&hspi1_handler, buf, len, 1000);
    if(spi_tx_status != HAL_OK){
        LTX_LOG_ERRO("Spi tx Error: %d, %d\n", spi_tx_status, hspi1_handler.ErrorCode);
        // ltx_Script_pause(&script_lcd_init);
    }
}

void myLCD_transmit_data_dma(const uint8_t *buf, uint16_t len){
    HAL_StatusTypeDef spi_tx_status = HAL_SPI_Transmit_DMA(&hspi1_handler, buf, len);
    if(spi_tx_status != HAL_OK){
        LTX_LOG_ERRO("Spi dma tx Error: %d, %d\n", spi_tx_status, hspi1_handler.ErrorCode);
        LTX_LOG_ERRO("Dma: %d\n", hdmaCh1_handler.State);
    }
}

void myLCD_pin_init(void){

}


// 所有外部硬件初始化完成或者初始化超时回调
void eventg_cb_device_init_over(struct ltx_Event_group_stu *eventg){
    if(ltx_Event_group_is_timeout(eventg)){
        LTX_LOG_ERRO("Device init Timeout!\n");
        LTX_LOG_ERRO("events: 0x%08x\n", eventg->events);

        return ;
    }

    // 所有外部硬件均初始化完成
    LTX_LOG_INFO("All devices init over.\n");
    // 关闭 device_init app
    ltx_App_destroy(&app_device_init);

    // 创建 lcd 清屏脚本
    ltx_Script_init(&script_lcd_clear, script_cb_lcd_clear, 0);
    // 创建屏幕显示 logo 脚本并运行
    ltx_Script_init(&script_draw_logo, script_cb_draw_logo, 0);
    ltx_Script_resume(&script_draw_logo);

    // 显示完 logo 再正式启动业务 app
    // 启动业务 app
    // 启动显示 app
    // ltx_App_init(&app_display);
    // ltx_App_resume(&app_display);
}

uint8_t T_heng_x = 8; // (0~10)
uint8_t T_heng_y = 20; // (0~124)
uint8_t T_heng_w = 1; // (1~11)
uint8_t T_heng_h = 47-20; // (1~125)

uint8_t T_shu_x = 2; // (0~10)
uint8_t T_shu_y = 30; // (0~124)
uint8_t T_shu_w = 6; // (1~11)
uint8_t T_shu_h = 6; // (1~125)

uint8_t i_dian_x = 8; // (0~10)
uint8_t i_dian_y = 59; // (0~124)
uint8_t i_dian_w = 1; // (1~11)
uint8_t i_dian_h = 6; // (1~125)

uint8_t i_shu_x = 2; // (0~10)
uint8_t i_shu_y = 59; // (0~124)
uint8_t i_shu_w = 5; // (1~11)
uint8_t i_shu_h = 6; // (1~125)

uint8_t x_x = 2; // (0~10)
uint8_t x_y = 80; // (0~124)
uint8_t x_w = 7; // (1~11)
uint8_t x_h = 24; // (1~125)

const unsigned char x_char_buffer[];
// 显示 logo 脚本回调
void script_cb_draw_logo(struct ltx_Script_stu *script){
    // if(ltx_Script_get_triger_type(script) == SC_TRIGER_RESET){ // 外部要求此脚本重置，可在这里做释放资源等操作
    //     return ;
    // }
    switch(script->step_now){
        case 0: // 将 dma buffer 全填为 1
            for(uint16_t i = 0; i < sizeof(lcd_buffer0); i ++){
                lcd_buffer0[i] = 0xFF;
            }
            // 运行清屏脚本并等待完成
            ltx_Script_reset(&script_lcd_clear, 0);
            ltx_Script_resume(&script_lcd_clear);
            ltx_Script_next_step_topic(script, script->step_now + 1, 100, &topic_lcd_clear_over);
            break;

        case 1:
            // T 横
            // st7305_fill_unit(&myLCD, T_heng_x, T_heng_y, T_heng_w, T_heng_h, 1);

            st7305_set_unit_window(&myLCD, T_heng_x, T_heng_y, T_heng_x + T_heng_w - 1, T_heng_y + T_heng_h - 1);
            st7305_write_data_dma(&myLCD, lcd_buffer0, T_heng_w * T_heng_h * 3); // 1 个 unit 3 个字节

            ltx_Script_next_step_topic(script, script->step_now + 1, 20, &topic_spi_tx_over);
            break;

        case 2:
            // T 竖
            st7305_set_unit_window(&myLCD, T_shu_x, T_shu_y, T_shu_x + T_shu_w - 1, T_shu_y + T_shu_h - 1);
            st7305_write_data_dma(&myLCD, lcd_buffer0, T_shu_w * T_shu_h * 3); // 1 个 unit 3 个字节

            ltx_Script_next_step_topic(script, script->step_now + 1, 20, &topic_spi_tx_over);

            break;

        case 3:
            // i 点
            st7305_set_unit_window(&myLCD, i_dian_x, i_dian_y, i_dian_x + i_dian_w - 1, i_dian_y + i_dian_h - 1);
            st7305_write_data_dma(&myLCD, lcd_buffer0, i_dian_w * i_dian_h * 3); // 1 个 unit 3 个字节

            ltx_Script_next_step_topic(script, script->step_now + 1, 20, &topic_spi_tx_over);

            break;

        case 4:
            // i 竖
            st7305_set_unit_window(&myLCD, i_shu_x, i_shu_y, i_shu_x + i_shu_w - 1, i_shu_y + i_shu_h - 1);
            st7305_write_data_dma(&myLCD, lcd_buffer0, i_shu_w * i_shu_h * 3); // 1 个 unit 3 个字节

            ltx_Script_next_step_topic(script, script->step_now + 1, 20, &topic_spi_tx_over);

            break;

        case 5:
            // x
            st7305_set_unit_window(&myLCD, x_x, x_y, x_x + x_w - 1, x_y + x_h - 1);
            st7305_write_data_dma(&myLCD, x_char_buffer, 504); // 1 个 unit 3 个字节

            ltx_Script_next_step_delay(script, script->step_now + 1, 500); // logo 停留一段时间
            break;

        default:
            // logo 绘制完成
            // 结束脚本
            ltx_Script_next_step_over(script);

            // 启动业务 app
            // 启动显示 app
            ltx_App_init(&app_display);
            ltx_App_resume(&app_display);

            // 启动按键 app
            ltx_App_init(&app_button);
            ltx_App_resume(&app_button);

            break;
    }
}

// lcd 清屏脚本回调
void script_cb_lcd_clear(struct ltx_Script_stu *script){
    if(ltx_Script_get_triger_type(script) == SC_TRIGER_RESET){ // 外部要求此脚本重置，可在这里做释放资源等操作
        // HAL_SPI_DMAStop(&hspi1_handler);
        return ;
    }
    static uint8_t clear_trans_count = 0;
    switch(script->step_now){
        case 0: // 将 dma buffer 全填为 0
            for(uint16_t i = 0; i < sizeof(lcd_buffer0); i ++){
                lcd_buffer1[i] = 0;
            }
            ltx_Script_next_step_delay(script, script->step_now + 1, 0);
            clear_trans_count = 0;

            break;

        case 1:
            st7305_set_unit_window(&myLCD, 0, 0, LCD_UNIT_WIDTH-1, LCD_UNIT_HEIGHT-1);
            st7305_write_data_dma(&myLCD, lcd_buffer1, 512);
            ltx_Script_next_step_topic(script, script->step_now + 1, 20, &topic_spi_tx_over);

            break;

        case 2:
            clear_trans_count ++;
            if(clear_trans_count == 8){
                
                // st7305_write_data_dma(&myLCD, lcd_buffer1, 5);
                // ltx_Script_next_step_topic(script, script->step_now + 1, 20, &topic_spi_tx_over);
                st7305_write_data(&myLCD, lcd_buffer1, 29);
                ltx_Script_next_step_delay(script, script->step_now + 1, 0);

                return;
            }
            st7305_write_data_dma(&myLCD, lcd_buffer1, 512);
            ltx_Script_next_step_topic(script, script->step_now, 20, &topic_spi_tx_over);

            break;

        default:
            // 结束脚本
            ltx_Script_next_step_over(script);
            // 发布清屏完成话题
            ltx_Topic_publish(&topic_lcd_clear_over);

            break;
    }

}

// spi dma 发送完成回调
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
    // ltx_Lock_off(&lock_spi);
    ltx_Topic_publish(&topic_spi_tx_over);
}

const unsigned char x_char_buffer[504] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
};
