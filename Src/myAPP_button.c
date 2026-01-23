#include "myAPP_button.h"
#include "ltx.h"
#include "ltx_app.h"
#include "ltx_log.h"
#include "ltx_script.h"
#include "ltx_lock.h"

// 按键脚本
struct ltx_Script_stu script_button_a;
void script_cb_button_a(struct ltx_Script_stu *script);
struct ltx_Script_stu script_button_b;
void script_cb_button_b(struct ltx_Script_stu *script);

// 按键 b 消抖完成
struct ltx_Topic_stu *topic_btn_b_debounce_over = &(lock_debounce.alarm_time_out.topic);

// 按键 b 事件话题
struct ltx_Topic_stu topic_btn_b_click_1 = _LTX_TOPIC_DEAFULT_CONFIG(topic_btn_b_click_1);          // 单击
struct ltx_Topic_stu topic_btn_b_longpress = _LTX_TOPIC_DEAFULT_CONFIG(topic_btn_b_longpress);      // 长按

// APP 相关
int myAPP_button_init(struct ltx_App_stu *app){

    // 创建按键 a 管理脚本
    // ltx_Script_init(&script_button_a, script_cb_button_a, 0);
    // 创建按键 b 管理脚本
    ltx_Script_init(&script_button_b, script_cb_button_b, 0);
    
    return 0;
}

int myAPP_button_pause(struct ltx_App_stu *app){

    // ltx_Script_pause(&script_button_a);
    ltx_Script_pause(&script_button_b);

    return 0;
}

int myAPP_button_resume(struct ltx_App_stu *app){

    // ltx_Script_resume(&script_button_a);
    ltx_Script_resume(&script_button_b);

    return 0;
}

int myAPP_button_destroy(struct ltx_App_stu *app){

    // ltx_Script_pause(&script_button_a);
    ltx_Script_pause(&script_button_b);

    // free...

    return 0;
}

struct ltx_App_stu app_button = {
    .is_initialized = 0,
    .status = ltx_App_status_pause,
    .name = "button",

    .init = myAPP_button_init,
    .pause = myAPP_button_pause,
    .resume = myAPP_button_resume,
    .destroy = myAPP_button_destroy,

    .task_list = NULL,
    
    .next = NULL,
};

uint8_t btn_val_debounce_b = 1;

// 按键 a 管理脚本，废弃
void script_cb_button_a(struct ltx_Script_stu *script){
    switch(script->step_now){
        case 0: // 刚开机，等待松开按键
            if(btn_read_a()){
                ltx_Script_next_step_delay(script, script->step_now + 1, 20);
            }else {
                ltx_Script_next_step_delay(script, script->step_now, 20);
            }

            break;

        case 1: // 

            // ltx_Script_next_step_topic(script, 1, 0, &); // 等待按键切换状态
            break;

        case 2: // 

            break;

        default:

            break;
    }
}

// 按键 b 管理脚本
void script_cb_button_b(struct ltx_Script_stu *script){
    static uint8_t btn_last_val = 0;
    static uint8_t flag_pressing = 0;
    // static TickType_t press_tick = 0;

    switch(script->step_now){
        case 0: // 刚开机，等待松开按键
            if(btn_val_debounce_b){ // 按键松开
                btn_last_val = btn_val_debounce_b;
                // 下一步骤等待中断按键按下
                ltx_Script_next_step_topic(script, script->step_now + 1, 0, topic_btn_b_debounce_over); // 以 TickType_t 最大值等待
            }else { // 继续等待
                // ltx_Script_next_step_delay(script, script->step_now, 20);
                ltx_Script_next_step_topic(script, script->step_now, 0, topic_btn_b_debounce_over); // 以 TickType_t 最大值等待
            }

            break;

        case 1: // 按键变化
            if(btn_last_val){
                if(!btn_val_debounce_b){ // 按键由松开变按下
                    // press_tick = ltx_Sys_get_tick();
                    ltx_Script_next_step_topic(script, script->step_now, 1500, topic_btn_b_debounce_over); // 按下超过 1.5s 判定为长按
                    btn_last_val = btn_val_debounce_b;
                    return ;
                }
            }else {
                if(btn_val_debounce_b){ // 按键由按下变松开
                    if(flag_pressing){ // 长按后的松开
                        flag_pressing = 0;
                    }else { // 1.5s 内的松开
                        ltx_Topic_publish(&topic_btn_b_click_1);
                    }
                }else { // 保持按下
                    flag_pressing = 1;
                    ltx_Topic_publish(&topic_btn_b_longpress); // 长按
                }
            }

            btn_last_val = btn_val_debounce_b;
            ltx_Script_next_step_topic(script, script->step_now, 0, topic_btn_b_debounce_over); // 以 TickType_t 最大值等待

            break;

        default:

            break;
    }
}


// 按键 b 中断消抖完成回调
void lock_cb_debounce_over(struct ltx_Lock_stu *lock){
    btn_val_debounce_b = btn_read_b();
}
