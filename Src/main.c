/**
 ******************************************************************************
 * @file    main.c
 * @author  MCU Application Team
 * @brief   Main program body
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
#include "ltx_log.h"
#include "ltx_app.h"
#include "myAPP_system.h"
#include "myAPP_device_init.h"

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1_handler;
DMA_HandleTypeDef hdmaCh1_handler;

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void sys_clock_init(void);
static void sys_spi1_init(void);
static void sys_button_init(void);

/**
 * @brief  Main program.
 * @retval int
 */
int main(void)
{
    ltx_Log_init();
    LTX_LOG_INFO("\n\nSYSTEM START\n\n");

    /* Reset of all peripherals, Initializes the Systick */
    HAL_Init();

    // 外设初始化
    sys_clock_init();
    sys_spi1_init();
    sys_button_init();

    // 创建系统 app 并运行
    ltx_App_init(&app_system);
    ltx_App_resume(&app_system);
    
    // 创建外部硬件初始化 app 并运行
    ltx_App_init(&app_device_init);
    ltx_App_resume(&app_device_init);


    // 启动调度器
    LTX_LOG_INFO("Start scheduler...\n");
    ltx_Sys_scheduler();

    while (1);
}

/**
 * @brief  System clock configuration function
 * @param  None
 * @retval None
 */
static void sys_clock_init(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Oscillator configuration */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE; /* Select oscillators HSE, HSI, LSI, LSE */
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;                                                                                              /* Enable HSI */
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                                                                                              /* HSI not divided */
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_24MHz;                                                                     /* Configure HSI clock as 24MHz */
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;                                                                                             /* Disable HSE */
    /*RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;*/
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF; /* Disable LSI */
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_MEDIUM;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF; /* Disable PLL */
    /*RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;*/
    /* Configure oscillators */
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        APP_ErrorHandler();
    }

    /* Clock source configuration */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1; /* Select clock types HCLK, SYSCLK, PCLK1 */
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;                                         /* Select HSI as system clock */
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;                                             /* AHB  clock not divided */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;                                              /* APB  clock not divided */
    /* Configure clock source */
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        APP_ErrorHandler();
    }
}


static void sys_spi1_init(void){
    /*Deinitialize SPI configuration*/
    hspi1_handler.Instance = SPI1;                                  /* SPI1 */
    hspi1_handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; /* Prescaler: 4 */
    hspi1_handler.Init.Direction = SPI_DIRECTION_1LINE;             /* Full-duplex */
    hspi1_handler.Init.CLKPolarity = SPI_POLARITY_LOW;              /* Clock polarity: Low */
    hspi1_handler.Init.CLKPhase = SPI_PHASE_1EDGE;                  /* Data sampling on the first clock edge */
    hspi1_handler.Init.DataSize = SPI_DATASIZE_8BIT;                /* SPI data size: 8-bit */
    hspi1_handler.Init.FirstBit = SPI_FIRSTBIT_MSB;                 /* MSB transmitted first */
    hspi1_handler.Init.NSS = SPI_NSS_SOFT;                          /* NSS hardware output mode */
    hspi1_handler.Init.Mode = SPI_MODE_MASTER;                      /* Configured as master */
    if (HAL_SPI_DeInit(&hspi1_handler) != HAL_OK)
    {
        APP_ErrorHandler();
    }

    /* SPI initialization */
    if (HAL_SPI_Init(&hspi1_handler) != HAL_OK)
    {
        APP_ErrorHandler();
    }
}


static void sys_button_init(void){

    __HAL_RCC_GPIOB_CLK_ENABLE();  /* Enable GPIOB clock */
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_EVT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void APP_ErrorHandler(void)
{
    /* Infinite loop */
    while (1){
        LTX_LOG_ERRO("\n\nSYSTEM ERROR\n\n");

        HAL_Delay(1000);
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       for example: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* Infinite loop */
    while (1)
    {
    }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
