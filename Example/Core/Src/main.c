/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "stm_coslib.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



/* define task*/
task_t ValueInc,GlobalReset;
/* define button*/
button_t Button_1,Button_2,Button_3;
/* define menuitems */
menu_t Root,Menu1,Menu2; 

uint32_t TimerInc;

/*

  B1 button is defined in c file
		button_typedef Button_Def[__COS_BUTTON_ButonTypeSize]=
		{
			{B1_GPIO_Port,B1_Pin}
		};

*/


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
 SysTick_Config(SystemCoreClock/1000); //1ms update
  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */




/* Create buttons, high time - first definition order*/
COS_BUTTON_Create(&Button_3,__COS_BUTTON_GET_TYPEDEF(0),2000,Falling_Edge,IRQ_Break); //2sec, BUTTON_IRQ_FLAG.Bits.BIT_0
COS_BUTTON_Create(&Button_2,__COS_BUTTON_GET_TYPEDEF(0),1000,Falling_Edge,IRQ_Break); //1sec push time, BUTTON_IRQ_FLAG.Bits.BIT_1
COS_BUTTON_Create(&Button_1,__COS_BUTTON_GET_TYPEDEF(0),50,Falling_Edge,IRQ_Break); //50ms push time, BUTTON_IRQ_FLAG.Bits.BIT_2

/* Create tasks*/
COS_TASK_Create(&ValueInc,1000,TASK_RUN); //1 sec
COS_TASK_Create(&GlobalReset,20000,TASK_RUN); //20 sec

/*Create menus*/
COS_MENU_SetRoot(&Root);
COS_MENU_AddTo(&Root,&Menu1); //First child
COS_MENU_AddTo(&Root,&Menu2); //Sibling

COS_TASK_Scheduler();


}




/* Task control*/
void COS_TASK_TimeEllapsedCallBack(task_t *Task)
{
	if(Task==&ValueInc)
	{
	  TimerInc++;
	}
	else if(Task==&GlobalReset)
	{
		TimerInc=0;
	  COS_MENU_BackToRoot();
	}

}

/* Menu and button control*/
void COS_MENU_EventHandle(menu_t *Menu)
{
	
	if(Menu==&Root)
	{
		
		if(BUTTON_IRQ_FLAG.Bits.BIT_2)
		{
		   HAL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
		}
		else if(BUTTON_IRQ_FLAG.Bits.BIT_1)
		{
      COS_MENU_MoveToChild(1);
		}
		else if(BUTTON_IRQ_FLAG.Bits.BIT_0)
		{

		}
		
	}
	else if(Menu==&Menu1)
	{
		
		if(BUTTON_IRQ_FLAG.Bits.BIT_2)
		{
       COS_MENU_Move(MENU_GOTO_FORWARD);
		}
		else if(BUTTON_IRQ_FLAG.Bits.BIT_1)
		{
       COS_MENU_Move(MENU_GOTO_BACKWARD);
		}
		else if(BUTTON_IRQ_FLAG.Bits.BIT_0)
		{
      COS_MENU_BackToRoot();
		}
		
	}
	else if(Menu==&Menu2)
	{
		
		if(BUTTON_IRQ_FLAG.Bits.BIT_2)
		{
       COS_MENU_Move(MENU_GOTO_FORWARD);
		}
		else if(BUTTON_IRQ_FLAG.Bits.BIT_1)
		{
       COS_MENU_Move(MENU_GOTO_BACKWARD);
		}
		else if(BUTTON_IRQ_FLAG.Bits.BIT_0)
		{
      COS_MENU_BackToRoot();
		}
		
	}
	
	
	
	/* if any button pushed, Reset the reset task timer*/
	if(BUTTON_IRQ_FLAG.Bulk)
	{ 
	  COS_TASK_RefreshTimer(&GlobalReset);
	}
	
}








/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
