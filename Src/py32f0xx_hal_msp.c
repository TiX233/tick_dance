/**
 ******************************************************************************
 * @file    py32f0xx_hal_msp.c
 * @author  MCU Application Team
 * @brief   This file provides code for the MSP Initialization
 *          and de-Initialization codes.
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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* External functions --------------------------------------------------------*/

/**
 * @brief Initialize global MSP
 */
void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
}

/**
 * @brief Initialize MSP for SPI
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* Initialize SPI1 */
    if (hspi->Instance == SPI1)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();  /* Enable GPIOB clock */
        __HAL_RCC_GPIOA_CLK_ENABLE();  /* Enable GPIOA clock */
        __HAL_RCC_SYSCFG_CLK_ENABLE(); /* Enable SYSCFG clock */
        __HAL_RCC_SPI1_CLK_ENABLE();   /* Enable SPI1 clock */
        __HAL_RCC_DMA_CLK_ENABLE();    /* Enable DMA clock */
        HAL_SYSCFG_DMA_Req(1);         /* SPI1_TX DMA_CH1 */
        // HAL_SYSCFG_DMA_Req(0x200);                      /* SPI1_RX DMA_CH2 */

        // TE
        GPIO_InitStruct.Pin = GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Enable EXTI interrupt */
        // HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
        // HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);

        // rst
        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);

        // d/c
        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* SPI NSS*/
        GPIO_InitStruct.Pin = GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 1);

        /* SPI SCK GPIO pin configuration  */
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        if (hspi->Init.CLKPolarity == SPI_POLARITY_LOW)
        {
            GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        }
        else
        {
            GPIO_InitStruct.Pull = GPIO_PULLUP;
        }
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF10_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Configure GPIO as SPI: MOSI*/
        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF10_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Interrupt configuration */
        HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(SPI1_IRQn);
        /* DMA_CH1 configuration */
        hdmaCh1_handler.Instance = DMA1_Channel1;
        hdmaCh1_handler.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdmaCh1_handler.Init.PeriphInc = DMA_PINC_DISABLE;
        hdmaCh1_handler.Init.MemInc = DMA_MINC_ENABLE;
        if (hspi->Init.DataSize <= SPI_DATASIZE_8BIT)
        {
            hdmaCh1_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            hdmaCh1_handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        }
        else
        {
            hdmaCh1_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            hdmaCh1_handler.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        }
        hdmaCh1_handler.Init.Mode = DMA_NORMAL;
        hdmaCh1_handler.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        /* DMA initialization */
        HAL_DMA_Init(&hdmaCh1_handler);
        /* Link DMA handle with SPI handle */
        __HAL_LINKDMA(hspi, hdmatx, hdmaCh1_handler);

        /* DMA interrupt configuration */
        HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    }
}

/**
 * @brief Deinitializes the SPI's MSP
 */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1)
    {
        /* Reset SPI clock */
        __HAL_RCC_SPI1_FORCE_RESET();
        __HAL_RCC_SPI1_RELEASE_RESET();

        /* Disable peripheral and GPIO clocks */
        /* Deconfigure SPI SCK */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);

        HAL_NVIC_DisableIRQ(SPI1_IRQn);

        HAL_DMA_DeInit(&hdmaCh1_handler);
        HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
    }
}

/**
 * @brief Initialize RTC MSP
 * @param hrtc：RTC handle
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /* Configure LSE/LSI as RTC clock source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_MEDIUM;
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while(1){
            LTX_LOG_ERRO("Config osc to LSE Failed!\n");
            HAL_Delay(1000);
        }
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        while(1){
            LTX_LOG_ERRO("Config rtc Failed!\n");
            HAL_Delay(1000);
        }
    }

    /* Enable RTC peripheral clock */
    __HAL_RCC_RTCAPB_CLK_ENABLE();
    /* Enable RTC clock */
    __HAL_RCC_RTC_ENABLE();

    /* Configure NVIC for RTC interrupts */
    // HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);
    // HAL_NVIC_EnableIRQ(RTC_IRQn);

    // __HAL_RTC_OVERFLOW_ENABLE_IT(hrtc, RTC_IT_OW);
    // __HAL_RTC_SECOND_ENABLE_IT(hrtc, RTC_IT_SEC);
}

/**
 * @brief Deinitialize RTC MSP
 * @param hrtc：RTC handle
 */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
    /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_RTC_DISABLE();
}

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
