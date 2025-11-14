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
#include "dma.h"
#include "spi.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ANALOG_TABLE_SIZE 256
#define UART3_BUFFER_SIZE 256
#define SPI3_BUFFER_SIZE 2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t table[8] = {0x00, 0x01, 0x02, 0x03,
					0x04, 0x05, 0x06, 0x07};

volatile uint8_t spi3_tx_buffer[SPI3_BUFFER_SIZE] = {0};
volatile uint8_t spi3_rx_buffer[SPI3_BUFFER_SIZE] = {0};

uint8_t uart3_tx_buffer[UART3_BUFFER_SIZE];
uint8_t uart3_rx_buffer[UART3_BUFFER_SIZE];

typedef enum
{
	OK = 0,
	ERROR_T
} RESULTS;

typedef enum
{
	LOW = 0,
	HIGH
} NWR_STATUS;

NWR_STATUS NWR_N = HIGH;
NWR_STATUS NWR_R = HIGH;

uint8_t channel_n = 0U;
uint8_t channel_r = 0U;

/* signal, MUX_n, MUX_n_analogInputChannel, MUX_r, MUX_r_analogInputChannel */
//uint8_t analogTable[ANALOG_TABLE_SIZE][5] = {};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void SPI3_Start_Comm(void);
void UART3_Start_Comm(void);

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
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */

  SPI3_Start_Comm();
  UART3_Start_Comm();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 * @brief This function redirects printf stdout to uart3
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart3, (uint8_t*)&ch, 1, 0xFFFF);
	return ch;
}

/**
 * @brief This function starts SPI Communication for Slave device
 * @retval None
 */
void SPI3_Start_Comm(void)
{
	HAL_SPI_TransmitReceive_DMA(&hspi3,(uint8_t*)spi3_tx_buffer,
			(uint8_t*)spi3_rx_buffer,SPI3_BUFFER_SIZE);
}

/**
 * @brief This function starts SPI Communication for Slave device
 * @retval None
 */
void UART3_Start_Comm(void)
{
	HAL_UART_Receive_DMA(&huart3,uart3_rx_buffer,UART3_BUFFER_SIZE);
}

/**
  * @brief  Function to handle SPI TxRx
  * @retval None
*/
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{

	if(hspi->Instance == SPI3)
	{
		volatile uint8_t index = spi3_rx_buffer[0];
		index >>= 3U;

		spi3_tx_buffer[0] = table[index];
		spi3_tx_buffer[1] = 0U;

	}

}

/**
  * @brief  Function to handle UART Rx
  * @retval None
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  HAL_UART_Receive_DMA(&huart3, uart3_rx_buffer, UART3_BUFFER_SIZE);

  for(int i = 0; i < UART3_BUFFER_SIZE; i++)
  {
          printf("HAL_UART_Receive_DMA:: %02X\n",uart3_rx_buffer[i]);
  }
}


/**
  * @brief  Function to manage GPIO External Interrupts
  * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
        if(GPIO_Pin == USER_Btn_Pin)
        {
                /* write your code here to manage USER_Btn [B1] Interrupt */
        }

        if( (GPIO_Pin >= MUX_N_ADDR_0_Pin) && (GPIO_Pin <= MUX_N_ADDR_4_Pin) )
        {
                channel_n |= (uint8_t)GPIO_Pin;
        }

        if(GPIO_Pin == MUX_N_NWR_Pin)
        {

                if(HAL_GPIO_ReadPin(GPIOF, GPIO_Pin) == GPIO_PIN_RESET)
                {
                        NWR_N = LOW;
                }

                if( (HAL_GPIO_ReadPin(GPIOF, GPIO_Pin) == GPIO_PIN_SET) && (NWR_N == LOW) )
                {
                        NWR_N = HIGH;
                        spi3_tx_buffer[1] = channel_n;
                        // spi3_tx_buffer[1] = analog_n_Parameters[channel_n];
                        channel_n = 0U;
                }
        }

        if( (GPIO_Pin >= MUX_R_ADDR_0_Pin) && (GPIO_Pin <= MUX_R_ADDR_4_Pin) )
        {
                channel_r |= (uint8_t)GPIO_Pin;
        }

        if(GPIO_Pin == MUX_R_NWR_Pin)
        {
                if(HAL_GPIO_ReadPin(GPIOF, GPIO_Pin) == GPIO_PIN_RESET)
                {
                        NWR_R = LOW;
                }

                if( (HAL_GPIO_ReadPin(GPIOF, GPIO_Pin) == GPIO_PIN_SET) && (NWR_R == LOW) )
                {
                        NWR_R = HIGH;
                        spi3_tx_buffer[1] = channel_r;
                        // spi3_tx_buffer[1] = analog_r_Parameters[channel_r];
                        channel_r = 0U;
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
