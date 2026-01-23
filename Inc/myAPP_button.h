#ifndef __MYAPP_BUTTON_H__
#define __MYAPP_BUTTON_H__

#include "ltx_app.h"

// #define btn_read_a()                HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)
// #define btn_read_b()                HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)
#define btn_read_a()                ((GPIOB->IDR & GPIO_PIN_2)?1:0)
#define btn_read_b()                ((GPIOB->IDR & GPIO_PIN_6)?1:0)

// #define btn_read_debounce_b()       btn_val_debounce_b

extern struct ltx_App_stu app_button;
extern struct ltx_Topic_stu topic_btn_b_click_1;    // 单击
extern struct ltx_Topic_stu topic_btn_b_longpress;  // 长按

void lock_cb_debounce_over(struct ltx_Lock_stu *lock);

#endif // __MYAPP_BUTTON_H__
