#include "myAPP_display.h"
#include "ltx.h"
#include "ltx_app.h"
#include "ltx_log.h"
#include "ltx_script.h"
#include "ltx_lock.h"
#include "ST7305.h"
#include "myAPP_device_init.h"
#include "myAPP_system.h"
#include "myAPP_button.h"
#include "num_trans.h"
#include "colon_trans.h"

// 时间跳动偏移定时任务
struct ltx_Task_stu task_tick_dance;
void task_cb_tick_dance(void *param);
// 时间跳动需要更新显示话题
// struct ltx_Topic_stu topic_time_need_dance = _LTX_TOPIC_DEAFULT_CONFIG(topic_time_need_dance);
struct ltx_Topic_stu *topic_time_need_dance = &(task_tick_dance.timer.topic); // 不额外创建话题，直接用 task 自己的 topic

// 单帧发送完成话题
// struct ltx_Topic_stu topic_draw_frame_over = _LTX_TOPIC_DEAFULT_CONFIG(topic_draw_frame_over);

// 显示管理脚本
struct ltx_Script_stu script_display_manager;
void script_cb_display_manager(struct ltx_Script_stu *script);

// 设置时间无操作超时时间闹钟，用锁超时回调代替，因为 v2 设置闹钟很麻烦
struct ltx_Lock_stu lock_set_time_timeout;
void lock_cb_set_time_timeout(struct ltx_Lock_stu *lock);

// 时间显示脚本
struct ltx_Script_stu script_display_time;
void script_cb_display_time(struct ltx_Script_stu *script);
// 时间设置脚本
struct ltx_Script_stu script_display_config_time;
void script_cb_display_config_time(struct ltx_Script_stu *script);
// 时间设置子脚本，用于显示设置值
struct ltx_Script_stu script_display_config_time_sub;
void script_cb_display_config_time_sub(struct ltx_Script_stu *script);

// APP 相关
int myAPP_display_init(struct ltx_App_stu *app){
    // 创建时间跳动定时器
    ltx_Task_init(&task_tick_dance, task_cb_tick_dance, 200, 0);
    ltx_Task_add_to_app(&task_tick_dance, app, "tick_dance");


    // 创建显示管理脚本
    ltx_Script_init(&script_display_manager, script_cb_display_manager, 0);
    // 创建时间显示脚本
    ltx_Script_init(&script_display_time, script_cb_display_time, 0);
    // 创建时间设置脚本
    ltx_Script_init(&script_display_config_time, script_cb_display_config_time, 0);
    ltx_Script_init(&script_display_config_time_sub, script_cb_display_config_time_sub, 0);

    // 设置时间无操作超时时间闹钟
    ltx_Lock_init(&lock_set_time_timeout, lock_cb_set_time_timeout);
    
    return 0;
}

int myAPP_display_pause(struct ltx_App_stu *app){

    ltx_Script_pause(&script_display_manager);

    return 0;
}

int myAPP_display_resume(struct ltx_App_stu *app){

    ltx_Script_resume(&script_display_manager);

    return 0;
}

int myAPP_display_destroy(struct ltx_App_stu *app){

    ltx_Script_pause(&script_display_manager);

    // free...

    return 0;
}

struct ltx_App_stu app_display = {
    .is_initialized = 0,
    .status = ltx_App_status_pause,
    .name = "display",

    .init = myAPP_display_init,
    .pause = myAPP_display_pause,
    .resume = myAPP_display_resume,
    .destroy = myAPP_display_destroy,

    .task_list = NULL,
    
    .next = NULL,
};

uint8_t dance_offset = 0;
// 时间跳动定时任务
void task_cb_tick_dance(void *param){
    // 每 200ms 自增一次偏移
    dance_offset = (dance_offset+1)%3;
    // 时间需要跳动，发布话题要求更新显示
    // ltx_Topic_publish(&topic_time_need_dance);
    // 不额外创建话题发布，直接用 task 自己的 topic
}

static RTC_TimeTypeDef rtc_time = {
    .Hours = 0,
    .Minutes = 0,
    .Seconds = 0,
};
// 时间显示脚本
void script_cb_display_time(struct ltx_Script_stu *script){

    if(ltx_Script_get_triger_type(script) == SC_TRIGER_RESET){ // 外部要求此脚本复位，可在此处做释放资源等操作
        HAL_SPI_DMAStop(&hspi1_handler);
        // st7305_power_high(&myLCD); // 显示屏进高功耗，提高刷新率
        return ;
    }

    switch(script->step_now){
        case 0:
            st7305_power_low(&myLCD); // 显示屏进低功耗，降低刷新率
            ltx_Script_next_step_topic(script, 20, 0, topic_time_need_dance); // 以 TickType_t 最大值等待时间需要跳动定时器触发，触发后再更新显示

            break;

        case 20:
            st7305_power_high(&myLCD); // 显示屏进高功耗，提高刷新率
            ltx_Script_next_step_delay(script, 1, 50); // 进高功耗后等一段时间再开始刷屏

            break;

        case 1: // 显示小时十位
            st7305_power_high(&myLCD); // 显示屏进高功耗，提高刷新率

            HAL_RTC_GetTime(&hrtc_handler, &rtc_time, RTC_FORMAT_BIN);

            st7305_set_unit_window(&myLCD, 2, 4, 9, 20);
            st7305_write_data_dma(&myLCD, nums_trans[rtc_time.Hours/10][dance_offset], 408);
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 2: // 显示小时个位

            st7305_set_unit_window(&myLCD, 2, 21, 9, 37);
            st7305_write_data_dma(&myLCD, nums_trans[rtc_time.Hours%10][(dance_offset+1)%3], 408);
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 3: // 显示左冒号

            st7305_set_unit_window(&myLCD, 3, 38, 8, 45);
            st7305_write_data_dma(&myLCD, colon_trans[dance_offset], 144);
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 4: // 显示分钟十位

            st7305_set_unit_window(&myLCD, 2, 46, 9, 62);
            st7305_write_data_dma(&myLCD, nums_trans[rtc_time.Minutes/10][(dance_offset+2)%3], 408);
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 5: // 显示分钟个位

            st7305_set_unit_window(&myLCD, 2, 63, 9, 79);
            st7305_write_data_dma(&myLCD, nums_trans[rtc_time.Minutes%10][dance_offset], 408);
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 6: // 显示右冒号

            st7305_set_unit_window(&myLCD, 3, 80, 8, 87);
            st7305_write_data_dma(&myLCD, colon_trans[(dance_offset+1)%3], 144);
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 7: // 显示秒十位

            st7305_set_unit_window(&myLCD, 2, 88, 9, 104);
            st7305_write_data_dma(&myLCD, nums_trans[rtc_time.Seconds/10][(dance_offset+1)%3], 408);
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 8: // 显示秒个位

            st7305_set_unit_window(&myLCD, 2, 105, 9, 121);
            st7305_write_data_dma(&myLCD, nums_trans[rtc_time.Seconds%10][(dance_offset+2)%3], 408);
            
            ltx_Script_next_step_topic(script, 0, 10, &topic_spi_tx_over); // 等待刷新完成，刷新完成则进入等待下次定时器触发
            break;

        default:

            break;
    }
}

uint8_t time_setting_place = 0;

void rtc_time_increase(void){
    switch(time_setting_place){
        case 0: // 设置小时
            rtc_time.Hours = (rtc_time.Hours + 1)%24;
            break;

        case 1: // 设置分钟十位
            if(rtc_time.Minutes/10 == 5){
                rtc_time.Minutes -= 50;
            }else {
                rtc_time.Minutes += 10;
            }

            break;

        case 2: // 设置分钟个位
            // rtc_time.Minutes = (rtc_time.Minutes + 1)%60;

            if(rtc_time.Minutes % 10 == 9){
                rtc_time.Minutes -= 9;
            }else {
                rtc_time.Minutes = rtc_time.Minutes + 1;
            }

            break;

        case 3: // 设置秒十位
            if(rtc_time.Seconds/10 == 5){
                rtc_time.Seconds -= 50;
            }else {
                rtc_time.Seconds += 10;
            }

            break;

        case 4: // 设置秒个位
            // rtc_time.Seconds = (rtc_time.Seconds + 1)%60;

            if(rtc_time.Seconds % 10 == 9){
                rtc_time.Seconds -= 9;
            }else {
                rtc_time.Seconds = rtc_time.Seconds + 1;
            }

            break;

        default:
            time_setting_place = 0;

            break;
    }
}

// 时间设置脚本
// 脚本只能等待一个事件还是太羸弱了...
void script_cb_display_config_time(struct ltx_Script_stu *script){

    static TickType_t long_press_start_tick = 0;
    static TickType_t long_press_counter = 0;
    static uint8_t btn_last_val = 1;
    uint8_t btn_now_val;

    if(ltx_Script_get_triger_type(script) == SC_TRIGER_RESET){ // 外部要求此脚本复位，可在此处做释放资源等操作
        time_setting_place = 0;
        HAL_SPI_DMAStop(&hspi1_handler);
        ltx_Script_reset(&script_display_config_time_sub, 0);
        return ;
    }
    
    switch(script->step_now){
        case 0: // 初始化
            ltx_Script_resume(&script_display_config_time_sub);
            ltx_Script_next_step_delay(script, 1, 0);

            break;

        case 1:
            if(ltx_Script_get_triger_type(script) == SC_TRIGER_TOPIC){ // 按键 b 单击，切换设置时间位
                // 切换设置时间位
                time_setting_place = (time_setting_place+1)%5;

                ltx_Lock_locked(&lock_set_time_timeout, 10000); // 重置无操作闹钟
            }else { // 等待按键 b 超时，扫描按键 a
                btn_now_val = btn_read_a();
                if(btn_last_val){
                    if(!btn_now_val){ // 由松开变按下
                        // 增加数值
                        rtc_time_increase();

                        // 测量是否长按
                        long_press_start_tick = ltx_Sys_get_tick();
                        long_press_counter = 0;
                        
                        ltx_Lock_locked(&lock_set_time_timeout, 10000); // 重置无操作闹钟
                    }
                }else {
                    if(btn_now_val){ // 由按下变松开

                        ltx_Lock_locked(&lock_set_time_timeout, 10000); // 重置无操作闹钟
                    }else { // 继续保持按下
                        if((ltx_Sys_get_tick() - long_press_start_tick) > 1200){ // 按下时间超过 1.2s，认定为长按
                            if((ltx_Sys_get_tick() - long_press_start_tick - 1200)/100 != long_press_counter){ // 长按期间每 Nms 增加 1
                                long_press_counter = (ltx_Sys_get_tick() - long_press_start_tick - 1200)/100;
                                // 增加数值
                                rtc_time_increase();
                            }
                        }
                    }
                }
                btn_last_val = btn_now_val;
            }

            ltx_Script_next_step_topic(script, 1, 30, &topic_btn_b_click_1); // 按键 b 单击，切换设置时间位
            break;

        default:

            break;
    }
}
// 时间设置脚本子脚本
void script_cb_display_config_time_sub(struct ltx_Script_stu *script){

    if(ltx_Script_get_triger_type(script) == SC_TRIGER_RESET){ // 外部要求此脚本复位，可在此处做释放资源等操作
        HAL_SPI_DMAStop(&hspi1_handler);
        return ;
    }

    switch(script->step_now){
        case 0:

            ltx_Script_next_step_topic(script, 1, 0, topic_time_need_dance); // 以 TickType_t 最大值等待时间需要跳动定时器触发，触发后再更新显示
            break;

        case 1:

            switch(time_setting_place){
                case 0: // 设置小时
                    // 显示小时十位
                    st7305_set_unit_window(&myLCD, 2, 4, 9, 20);
                    st7305_write_data_dma(&myLCD, nums_trans[rtc_time.Hours/10][dance_offset], 408);
                    
                    ltx_Script_next_step_topic(script, 2, 10, &topic_spi_tx_over); // 等待刷新完成

                    return ;

                    break;

                case 1: // 设置分钟十位
                    st7305_set_unit_window(&myLCD, 2, 46, 9, 62);
                    st7305_write_data_dma(&myLCD, nums_trans[rtc_time.Minutes/10][(dance_offset+2)%3], 408);

                    break;

                case 2: // 设置分钟个位
                    st7305_set_unit_window(&myLCD, 2, 63, 9, 79);
                    st7305_write_data_dma(&myLCD, nums_trans[rtc_time.Minutes%10][dance_offset], 408);

                    break;
                    
                case 3: // 设置秒十位
                    st7305_set_unit_window(&myLCD, 2, 88, 9, 104);
                    st7305_write_data_dma(&myLCD, nums_trans[rtc_time.Seconds/10][(dance_offset+1)%3], 408);

                    break;
                    
                case 4: // 设置秒个位
                    st7305_set_unit_window(&myLCD, 2, 105, 9, 121);
                    st7305_write_data_dma(&myLCD, nums_trans[rtc_time.Seconds%10][(dance_offset+2)%3], 408);

                    break;

                default:
                    time_setting_place = 0;
                    break;
            }

            ltx_Script_next_step_topic(script, 0, 10, &topic_spi_tx_over); // 等待刷新完成，刷新完成则进入等待下次定时器触发
            break;

        case 2: // 显示小时个位
            st7305_set_unit_window(&myLCD, 2, 21, 9, 37);
            st7305_write_data_dma(&myLCD, nums_trans[rtc_time.Hours%10][(dance_offset+1)%3], 408);
            
            ltx_Script_next_step_topic(script, 0, 10, &topic_spi_tx_over); // 等待刷新完成

            break;

        default:
            break;
    }
}

// 显示管理脚本
void script_cb_display_manager(struct ltx_Script_stu *script){
    switch(script->step_now){
        case 0: // 刚开机，清屏 logo

            // 运行清屏脚本并等待完成
            ltx_Script_reset(&script_lcd_clear, 0);
            ltx_Script_resume(&script_lcd_clear);

            ltx_Script_next_step_topic(script, script->step_now + 1, 100, &topic_lcd_clear_over);
            break;

        case 1: // 时间显示
            HAL_RTC_SetTime(&hrtc_handler, &rtc_time, RTC_FORMAT_BIN); // 设置时间
            ltx_Script_reset(&script_display_config_time, 0);
            // ltx_Script_reset(&script_display_time, 0);
            ltx_Script_resume(&script_display_time); // 运行时间显示脚本

            ltx_Script_next_step_topic(script, 2, 0, &topic_btn_b_longpress); // 以 TickType_t 最大值等待按键切换状态：按键 b 长按，进入设置时间
            ltx_Lock_release(&lock_set_time_timeout); // 关闭设置时间无操作超时闹钟

            break;

        case 2: // 时间设置
            if(ltx_Script_get_triger_type(script) == SC_TRIGER_TIMEOUT){
                // 因为 TickType_t 溢出导致的超时调用，重新等待
                ltx_Script_next_step_topic(script, 2, 0, &topic_btn_b_longpress); // 以 TickType_t 最大值等待按键切换状态：按键 b 长按，进入设置时间

                return ;
            }
            ltx_Script_reset(&script_display_time, 0);
            // ltx_Script_reset(&script_display_config_time, 0);
            ltx_Script_resume(&script_display_config_time); // 运行时间设置脚本

            ltx_Script_next_step_topic(script, 1, 0, &topic_btn_b_longpress); // 以 TickType_t 最大值等待按键切换状态：按键 b 长按，退出设置时间
            ltx_Lock_locked(&lock_set_time_timeout, 10000); // 无操作 10s 则退出设置时间

            break;

        default:

            break;
    }
}


// 设置时间的无操作超时回调
void lock_cb_set_time_timeout(struct ltx_Lock_stu *lock){
    // 发送长按事件，让显示管理脚本退出设置时间状态
    ltx_Topic_publish(&topic_btn_b_longpress);
}
