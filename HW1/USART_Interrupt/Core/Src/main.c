/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// 接收缓存数组（定�?6字节�?
uint8_t rcv_buf[6] = {0};
// 发�?�缓存数�?
uint8_t send_buf[64] = {0};
// 接收计数
uint8_t rcv_cnt = 0;
// 包头错误标志
uint8_t header_err = 0;
// 数据接收完成标志
uint8_t data_received = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void USART1_SendReply(void);
void USART1_SendDebugInfo(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  // �?启USART1接收中断，接�?1个字�?
  HAL_UART_Receive_IT(&huart1, &rcv_buf[rcv_cnt], 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    
    // 检查是否接收到完整数据
    if(data_received == 1)
    {
      // 发送调试信息
      USART1_SendDebugInfo();
      
      // 发送回复
      USART1_SendReply();
      
      // 清除标志
      data_received = 0;
      header_err = 0;
      rcv_cnt = 0;
      
      // 清空接收缓存
      memset(rcv_buf, 0, sizeof(rcv_buf));
      
      // 重新开启接收中断
      HAL_UART_Receive_IT(&huart1, &rcv_buf[rcv_cnt], 1);
    }

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
  * @brief  发�?�回复函�?
  * @retval None
  */
void USART1_SendReply(void)
{
    uint16_t hour, minute;
    
    // 延时1�?
    HAL_Delay(1000);
    
    if(header_err == 0)
    {
        // 包头正确，转换时间并发�?�正常回�?
        hour = (uint16_t)rcv_buf[2];    // �?3字节：小时（16进制�?
        minute = (uint16_t)rcv_buf[3];  // �?4字节：分钟（16进制�?
        
        // 使用sprintf组字符串，格式：AB 时间 随便�?条信�? 换行
        sprintf((char*)send_buf, "AB %02d:%02d 要和我组乐队吗\r\n", hour, minute);
    }
    else
    {
        // 包头错误，发送错误信�?
        sprintf((char*)send_buf, "AB 包头错误\r\n");
    }
    
    // 发�?�字符串
    HAL_UART_Transmit(&huart1, send_buf, strlen((char*)send_buf), 1000);
}

/**
  * @brief  发�?�调试信息函�?
  * @retval None
  */
void USART1_SendDebugInfo(void)
{
    // �?化的调试信息
    sprintf((char*)send_buf, "DEBUG: ");
    HAL_UART_Transmit(&huart1, send_buf, strlen((char*)send_buf), 1000);
    
    // 发�?�每个字节的十六进制�?
    for(int i = 0; i < 6; i++)
    {
        sprintf((char*)send_buf, "%02X ", rcv_buf[i]);
        HAL_UART_Transmit(&huart1, send_buf, strlen((char*)send_buf), 1000);
    }
    
    // 发�?�包头检查结�?
    if(rcv_buf[0] == 0xAA && rcv_buf[1] == 0xBB)
    {
        sprintf((char*)send_buf, "OK\r\n");
    }
    else
    {
        sprintf((char*)send_buf, "ERROR\r\n");
    }
    HAL_UART_Transmit(&huart1, send_buf, strlen((char*)send_buf), 1000);
}

/**
  * @brief  UART接收完成回调函数
  * @param  huart: UART句柄
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        // 接收计数自增
        rcv_cnt++;
        
        // �?查是否接收完6字节数据
        if(rcv_cnt >= 6)
        {
            // �?查包头是否正�? (AA BB)
            if(rcv_buf[0] == 0xAA && rcv_buf[1] == 0xBB)
            {
                header_err = 0;  // 包头正确
            }
            else
            {
                header_err = 1;  // 包头错误
            }
            
            // 设置数据接收完成标志
            data_received = 1;
        }
        else
        {
            // 继续接收下一个字�?
            HAL_UART_Receive_IT(&huart1, &rcv_buf[rcv_cnt], 1);
        }
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
