/**
 ******************************************************************************
 * @file    py32f0xx_it.c
 * @author  MCU Application Team
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 Puya Semiconductor Co.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by Puya under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "py32f0xx_it.h"

/* Private includes ----------------------------------------------------------*/
#include "ltx.h"
#include "ltx_log.h"
#include "ltx_lock.h"
#include "ltx_cmd.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/

/******************************************************************************/
/*          Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
}

// 来自 deepseek
// Cortex-M0+ 异常栈帧结构
typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;    // 异常发生时的LR
    uint32_t pc;    // 程序计数器(关键!)
    uint32_t psr;   // 程序状态寄存器
} HardFault_StackFrame;

void myHF_Handler(uint32_t *stack_pointer) {
    // 1. 获取栈帧
    HardFault_StackFrame *frame = (HardFault_StackFrame *)stack_pointer;
    
    uint32_t pre_exception_sp = (uint32_t)stack_pointer + sizeof(HardFault_StackFrame);
    
    // 3. 打印基本信息
    LTX_LOG_STR("\n! HF !\n");
HF_tag:
    LTX_LOG_STR("\nStack Frame:\n");
    for(uint8_t i = 0; i < 5; i ++){
        LTX_LOG_FMT("0x%08lx\n", (uint32_t)(*((uint32_t *)frame + i)));
    }
    LTX_LOG_FMT("0x%08lX (LR)\n", frame->lr);
    LTX_LOG_FMT("0x%08lX (PC)\n", frame->pc & ~1UL);
    LTX_LOG_FMT("0x%08lX (PSR)\n", frame->psr);

    LTX_LOG_FMT("0x%08lx (SP)\n", pre_exception_sp);

    LTX_LOG_FMT("\nStack type: %s\n", (frame->lr & 0x4) ? "PSP" : "MSP");
Call_tag:
    LTX_LOG_STR("\nCall stack:\n");
    uint32_t *stack_base = (uint32_t *)pre_exception_sp;
    uint32_t prev_addr = frame->pc & 0xFFFFFFFE;
    
    for (uint8_t depth = 0; depth < 32; depth ++) {
        uint32_t addr = stack_base[depth];
        uint32_t code_addr = addr & 0xFFFFFFFE;  // 清除 Thumb 位
        
        // 检查是否有效代码地址
        if ((code_addr & 0xFF000000) == 0x08000000) {
            LTX_LOG_FMT("#%d: 0x%08lX", depth, code_addr);
            
            // 检查连续性
            if (code_addr < prev_addr) {
                LTX_LOG_STR("  (ret)\n");
            } else {
                LTX_LOG_STR("  (data?)\n");
            }
            
            prev_addr = code_addr;
        } else if (addr == 0) {
            // LTX_LOG_FMT("%d: 0\n", depth);
        } else {
            LTX_LOG_FMT("%d: 0x%08lX (?)\n", depth, addr);
            // break;
        }
    }

    LTX_LOG_STR("\nSYSTEM SHUTDOWN\n");
    
    while (1) { // 如果有输入则重新打印一次信息
        if(SEGGER_RTT_HasData(0)){
            extern uint8_t cmd_buffer[CMD_BUF_SIZE];
            SEGGER_RTT_Read(0, cmd_buffer, CMD_BUF_SIZE - 1);
            if(cmd_buffer[0] == 'c'){
                goto Call_tag;
            }
            
            goto HF_tag;
        }
    }
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
    __asm volatile(
        "MOVS R0, #4             \n"  // 准备掩码 (4 = 0b100)
        "MOV R1, LR              \n"  // 复制LR到R1 (LR是只读的)
        "TST R1, R0              \n"  // 测试EXC_RETURN的bit2
        "BEQ use_msp             \n"  // 如果结果为0，跳转到use_msp
        "MRS R0, PSP             \n"  // 使用PSP
        "B call_handler          \n"  // 跳转到调用处理程序
        "use_msp:                \n"
        "MRS R0, MSP             \n"  // 使用MSP
        "call_handler:           \n"
        "LDR R1, =myHF_Handler   \n"  // 加载C函数地址
        "BX R1                   \n"  // 跳转到C函数
    );

    LTX_LOG_STR("\n\n!HF!\n\n");
    while (1)
    {
    }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
    // 运行调度器
    ltx_Sys_scheduler();
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    HAL_IncTick();

    ltx_Sys_tick_tack();
}

/******************************************************************************/
/* PY32F0xx Peripheral Interrupt Handlers                                     */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file.                                          */
/******************************************************************************/
/**
 * @brief This function handles DMA1 channel1 Interrupt .
 */
void DMA1_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(hspi1_handler.hdmatx);
}

/**
 * @brief This function handles SPI1 Interrupt .
 */
void SPI1_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi1_handler);
}

#if 0
void EXTI2_3_IRQHandler(void){
    // 清除中断标志位
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
}
#endif

void EXTI4_15_IRQHandler(void){
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_11)){ // TE
        // 发布 TE 事件
        ltx_Topic_publish(&topic_te);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_11);
    }

    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_6)){ // 按键 B
        // 消抖闹钟
        // 用锁来代替，因为锁有超时回调，而且 v2 版本配置闹钟太麻烦了
        ltx_Lock_locked(&lock_debounce, 15);

        // 清除中断标志位，感觉对于按键放在最后会不会更好一点，上面内点代码耗时也能消点抖
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
    }

}


/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
