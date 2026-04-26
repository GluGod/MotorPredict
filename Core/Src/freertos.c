/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId SensorSampleHandle;
osThreadId ControlHandle;
osThreadId FeatureExtractHandle;
osThreadId DiagnosisHandle;
osThreadId ReportHandle;
osThreadId MonitorHandle;
osMessageQId QueueRawDataHandle;
osMessageQId QueueFeatureHandle;
osMessageQId QueueDiagResultHandle;
osMessageQId QueueControlCmdHandle;
osMutexId MutexUartHandle;
osMutexId MutexI2CHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTaskSensor(void const * argument);
void StartTaskControl(void const * argument);
void StartTaskFeature(void const * argument);
void StartTaskDiagnosis(void const * argument);
void StartTaskReport(void const * argument);
void StartTaskMonitor(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of MutexUart */
  osMutexDef(MutexUart);
  MutexUartHandle = osMutexCreate(osMutex(MutexUart));

  /* definition and creation of MutexI2C */
  osMutexDef(MutexI2C);
  MutexI2CHandle = osMutexCreate(osMutex(MutexI2C));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of QueueRawData */
  osMessageQDef(QueueRawData, 8, uint32_t);
  QueueRawDataHandle = osMessageCreate(osMessageQ(QueueRawData), NULL);

  /* definition and creation of QueueFeature */
  osMessageQDef(QueueFeature, 4, uint32_t);
  QueueFeatureHandle = osMessageCreate(osMessageQ(QueueFeature), NULL);

  /* definition and creation of QueueDiagResult */
  osMessageQDef(QueueDiagResult, 4, uint32_t);
  QueueDiagResultHandle = osMessageCreate(osMessageQ(QueueDiagResult), NULL);

  /* definition and creation of QueueControlCmd */
  osMessageQDef(QueueControlCmd, 2, uint8_t);
  QueueControlCmdHandle = osMessageCreate(osMessageQ(QueueControlCmd), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of SensorSample */
  osThreadDef(SensorSample, StartTaskSensor, osPriorityHigh, 0, 256);
  SensorSampleHandle = osThreadCreate(osThread(SensorSample), NULL);

  /* definition and creation of Control */
  osThreadDef(Control, StartTaskControl, osPriorityAboveNormal, 0, 128);
  ControlHandle = osThreadCreate(osThread(Control), NULL);

  /* definition and creation of FeatureExtract */
  osThreadDef(FeatureExtract, StartTaskFeature, osPriorityNormal, 0, 256);
  FeatureExtractHandle = osThreadCreate(osThread(FeatureExtract), NULL);

  /* definition and creation of Diagnosis */
  osThreadDef(Diagnosis, StartTaskDiagnosis, osPriorityBelowNormal, 0, 128);
  DiagnosisHandle = osThreadCreate(osThread(Diagnosis), NULL);

  /* definition and creation of Report */
  osThreadDef(Report, StartTaskReport, osPriorityLow, 0, 256);
  ReportHandle = osThreadCreate(osThread(Report), NULL);

  /* definition and creation of Monitor */
  osThreadDef(Monitor, StartTaskMonitor, osPriorityLow, 0, 128);
  MonitorHandle = osThreadCreate(osThread(Monitor), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTaskSensor */
/**
* @brief Function implementing the SensorSample thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskSensor */
void StartTaskSensor(void const * argument)
{
  /* USER CODE BEGIN StartTaskSensor */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskSensor */
}

/* USER CODE BEGIN Header_StartTaskControl */
/**
* @brief Function implementing the Control thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskControl */
void StartTaskControl(void const * argument)
{
  /* USER CODE BEGIN StartTaskControl */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskControl */
}

/* USER CODE BEGIN Header_StartTaskFeature */
/**
* @brief Function implementing the FeatureExtract thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskFeature */
void StartTaskFeature(void const * argument)
{
  /* USER CODE BEGIN StartTaskFeature */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskFeature */
}

/* USER CODE BEGIN Header_StartTaskDiagnosis */
/**
* @brief Function implementing the Diagnosis thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskDiagnosis */
void StartTaskDiagnosis(void const * argument)
{
  /* USER CODE BEGIN StartTaskDiagnosis */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskDiagnosis */
}

/* USER CODE BEGIN Header_StartTaskReport */
/**
* @brief Function implementing the Report thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskReport */
void StartTaskReport(void const * argument)
{
  /* USER CODE BEGIN StartTaskReport */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskReport */
}

/* USER CODE BEGIN Header_StartTaskMonitor */
/**
* @brief Function implementing the Monitor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskMonitor */
void StartTaskMonitor(void const * argument)
{
  /* USER CODE BEGIN StartTaskMonitor */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskMonitor */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

