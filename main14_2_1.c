/* CHUONG TRÌNH 1: DÙNG DELAY   */

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Khai báo hàm */
void GPIO_Config(void);
void Delay_ms_blocking(volatile uint32_t ms);
void vBlinkTask_Delay(void *pvParameters); // 

int main(void)
{
    // Cau hình GPIO cho LED PC13
    GPIO_Config();
    
    // Tao Task nhap nháy LED
    xTaskCreate(
        vBlinkTask_Delay,       // Con tro toi ham cua task
        "Blink_Delay",          // Tên task 
        128,                    // Kích thuoc stack (word)
        NULL,                   // Tham so truyen vao task
        1,                      // Do uu tien
        NULL                    // Handle cua task
    );

    // Bat dau chay bo lap lich
    vTaskStartScheduler();

    while (1)
    {
    }
}

/**
 * @brief Task nhap nhay led su dung delay
 */
void vBlinkTask_Delay(void *pvParameters)
{
    while (1)
    {
        // Bat LED (PC13 active-low)
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        
        // Doi 500ms - CPU se ban 100%
        // Ham nay chiem CPU de khong task nao chay duoc
        Delay_ms_blocking(500); 

        // Tat led
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        
        // Doi 500ms - CPU ban 100%
        Delay_ms_blocking(500);
    }
}


/* --- Các hàm phu tro --- */

void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // C?p clock cho GPIOC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // Output Push-Pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // Tat led ban dau
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

/**
  * @brief  Hàm delay kieu "busy-wait"
  * (Gia su clock là 72MHz)
  */
void Delay_ms_blocking(volatile uint32_t ms)
{
    volatile uint32_t i, j;
    for (i = 0; i < ms; i++)
    {
        // Vòng lap nay de chay 1ms o 72MHz
        for (j = 0; j < 7200; j++); 
    }
}
