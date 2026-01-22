#include "myAPP_display.h"
#include "ltx.h"
#include "ltx_app.h"
#include "ltx_log.h"
#include "ltx_script.h"
#include "ST7305.h"
#include "myAPP_device_init.h"
#include "myAPP_system.h"

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

// 时间显示脚本
struct ltx_Script_stu script_display_time;
void script_cb_display_time(struct ltx_Script_stu *script);

// APP 相关
int myAPP_display_init(struct ltx_App_stu *app){
    // 创建时间跳动定时器
    ltx_Task_init(&task_tick_dance, task_cb_tick_dance, 300, 0);
    ltx_Task_add_to_app(&task_cb_tick_dance, app, "tick_dance");


    // 创建显示管理脚本
    ltx_Script_init(&script_display_manager, script_cb_display_manager, 0);
    // 创建时间显示脚本
    ltx_Script_init(&script_display_time, script_cb_display_time, 0);
    
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
    // 每 300ms 自增一次偏移
    dance_offset = (dance_offset+1)%3;
    // 时间需要跳动，发布话题要求更新显示
    // ltx_Topic_publish(&topic_time_need_dance);
    // 不额外创建话题发布，直接用 task 自己的 topic
}

// 时间显示脚本
void script_cb_display_time(struct ltx_Script_stu *script){
    if(ltx_Script_get_triger_type(script) == SC_TRIGER_RESET){ // 外部要求此脚本复位，可在此处做释放资源等操作
        HAL_SPI_DMAStop(&hspi1_handler);
        return ;
    }

    switch(script->step_now){
        case 0:
            ltx_Script_next_step_topic(script, script->step_now + 1, 0, topic_time_need_dance); // 以 TickType_t 最大值等待时间需要跳动定时器触发，触发后再更新显示

            break;

        case 1: // 显示小时十位
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 2: // 显示小时个位
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 3: // 显示左冒号
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 4: // 显示分钟十位
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 5: // 显示分钟个位
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 6: // 显示右冒号
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 7: // 显示秒十位
            
            ltx_Script_next_step_topic(script, script->step_now + 1, 10, &topic_spi_tx_over); // 等待刷新完成
            break;

        case 8: // 显示秒个位
            
            ltx_Script_next_step_topic(script, 0, 10, &topic_spi_tx_over); // 等待刷新完成，刷新完成则进入等待下次定时器触发
            break;

        default:

            break;
    }
}

// 显示管理脚本
void script_cb_display_manager(struct ltx_Script_stu *script){
    switch(script->step_now){
        case 0: // 刚开机，进时间显示
            ltx_Script_reset(&script_cb_display_time, 0);
            ltx_Script_resume(&script_cb_display_time);

            // ltx_Script_next_step_topic(script, 1, 0, &); // 等待按键切换状态
            break;

        default:

            break;
    }
}
