#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"

/* Ðinh nghia cac bit dieu khien cho Event Group */
#define BIT_TASK_A 0x01  
#define BIT_TASK_B 0x02  
#define BIT_TASK_C 0x04  

/* Khai bao Event Group và Mutex */
EventGroupHandle_t xEventGroup;
SemaphoreHandle_t xUARTMutex;

/* ==================== UART1 Init theo thu vien STM32 ==================== */
void UART1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // PA9: TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA10: RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

/* Gui chuoi qua UART1 có bao ve bang mutex */
void UART1_SendString(const char* str)
{
    if (xUARTMutex != NULL)
    {
        if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE)
        {
            while (*str)
            {
                while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
                USART_SendData(USART1, *str++);
            }
            xSemaphoreGive(xUARTMutex);
        }
    }
}

/* ==================== Cac Task ==================== */
void TaskA(void* pvParameters)
{
    while (1)
    {
        xEventGroupWaitBits(xEventGroup, BIT_TASK_A, pdTRUE, pdFALSE, portMAX_DELAY);
        UART1_SendString("Task A dang chay\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void TaskB(void* pvParameters)
{
    while (1)
    {
        xEventGroupWaitBits(xEventGroup, BIT_TASK_B, pdTRUE, pdFALSE, portMAX_DELAY);
        UART1_SendString("Task B dang chay\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void TaskC(void* pvParameters)
{
    while (1)
    {
        xEventGroupWaitBits(xEventGroup, BIT_TASK_C, pdTRUE, pdFALSE, portMAX_DELAY);
        UART1_SendString("Task C dang chay\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* Task dieu khien theo dung thu tu mong muon */
void TaskControl(void* pvParameters)
{
    while (1)
    {
        UART1_SendString("Kich hoat Task A\r\n");
        xEventGroupSetBits(xEventGroup, BIT_TASK_A);
        vTaskDelay(pdMS_TO_TICKS(1000));
			
        UART1_SendString("Kich hoat Task B\r\n");
        xEventGroupSetBits(xEventGroup, BIT_TASK_B);
        vTaskDelay(pdMS_TO_TICKS(1000));

        UART1_SendString("Kich hoat Task C\r\n");
        xEventGroupSetBits(xEventGroup, BIT_TASK_C);
        vTaskDelay(pdMS_TO_TICKS(1000));

        UART1_SendString("Kich hoat dong thoi A + B + C\r\n");
        xEventGroupSetBits(xEventGroup, BIT_TASK_A | BIT_TASK_B | BIT_TASK_C);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

/* ==================== Ham main ==================== */
int main(void)
{
    SystemInit();         // Khoi tao he thong
    UART1_Init();         // Khoi tao UART1

    xUARTMutex = xSemaphoreCreateMutex();     // Tao mutex bao ve UART
    xEventGroup = xEventGroupCreate();        // Tao Event Group

    // T?o các task
    xTaskCreate(TaskA, "TaskA", 128, NULL, 2, NULL);
    xTaskCreate(TaskB, "TaskB", 128, NULL, 2, NULL);
    xTaskCreate(TaskC, "TaskC", 128, NULL, 2, NULL);
    xTaskCreate(TaskControl, "Control", 128, NULL, 3, NULL);

    vTaskStartScheduler(); // Bat dau FreeRTOS

    while (1); 
}
