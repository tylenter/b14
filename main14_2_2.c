/* CHUONG TRÌNH 2: DÙNG SLEEP  */

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h" 

/* Khai báo hàm */
void GPIO_Config(void);
void vBlinkTask_Sleep(void *pvParameters); // Ham cua taski

int main(void)
{
    // Cau hinh GPIO cho LED PC13
    GPIO_Config();
    
    // Tao task nhap nhay led
    xTaskCreate(
        vBlinkTask_Sleep,       // Con tro toi ham cua task
        "Blink_Sleep",          // Ten task
        128,                    // Kich thuoc stack (word)
        NULL,                   // Tham so truyen vào task
        1,                      // Do uu tien
        NULL                    // Handle cu task
    );

    // Bat dau chay bo lap lich
    vTaskStartScheduler();

    while (1)
    {
    }
}

/**
 * @brief Task nhap nhay led su dung sleep cua freertos
 */
void vBlinkTask_Sleep(void *pvParameters)
{
    // Bien luu thoi gian delay (500ms)
    // pdMS_TO_TICKS là macro cua FreeRTOS de chuyen ms sang "tick"
    const TickType_t xDelay = pdMS_TO_TICKS(500);

    while (1)
    {
        // Bat led
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        
        // Doi 500ms - CPU se ngu (chuyen sang task)
        vTaskDelay(xDelay); 

        // Tat led
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        
        // Doi 500- CPU lai ngu
        vTaskDelay(xDelay);
    }
}


/*Cac ham phu tro */

void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // Cap clock cho GPIOC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // Output Push-Pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // Tat LED ban dau
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}
