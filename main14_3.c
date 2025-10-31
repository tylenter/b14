#include "stm32f10x.h"
#include "stdio.h"  
#include "string.h" 

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_exti.h"
/* ------------------------------------------ */

/* Khai báo hàm */
void UART1_Init(void);
void RTC_Config(void);
void EXTI_Wakeup_Config(void);
void UART_SendString(const char* str);

// Bien buffer de gui UART
char uart_buf[100];

/**
  * @brief  Hàm main
  */
int main(void)
{
    uint32_t current_time;
    
    /* 1. Khoi tao UART */
    UART1_Init();

    /* 2. Kich hoat clock cho PWR và BKP (Backup domain) */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* 3. Cho phep truy cap Backup domain (chua RTC) */
    PWR_BackupAccessCmd(ENABLE);

    /* 4. Kiem tra */
    if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
    {
        uint32_t rtc_val;
        
			/* TH1: Thuc day tu Standby			*/
        
        // Xoa co Standby
        PWR_ClearFlag(PWR_FLAG_SB);

        // Doi cac thanh ghi dong bo
        RTC_WaitForSynchro();
        
        // Lay gia tri RTC va gui gi
        rtc_val = RTC_GetCounter(); // Bay gio cho phep gan
        sprintf(uart_buf, "Da thuc day! Gia tri RTC: %u\r\n", (unsigned int)rtc_val);
        UART_SendString(uart_buf);
    }
    else
    {
			/* TH2: Khoi dong lan dau */
        UART_SendString("Khoi dong lan dau.\r\n");
        
        // Cau hinh RTC lan dau
        RTC_Config();
        
        // Dat RTC ve 0
        RTC_SetCounter(0);
        RTC_WaitForLastTask();
    }
    
    /* 5. Cau hinh EXTI de danh thuc */
    EXTI_Wakeup_Config();

    /* 6. Ðat bao thuc(sau 5s) */
    current_time = RTC_GetCounter(); // Bay gio cho phep gan
    RTC_SetAlarm(current_time + 5);
    RTC_WaitForLastTask(); // Doi ghi xong Alarm

    /* 7. Gui thong bao va vao standby */
    UART_SendString("Chuan bi vao Standby (sau 5s se thuc day)...\r\n");

    // Doi USART gui xong
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    
    /* 8. Vao che do standby */
    // Xoa co wakeup truoc
    PWR_ClearFlag(PWR_FLAG_WU);
    
    // Vao che do
    PWR_EnterSTANDBYMode();

    /* Chuong trinh se khong bao gio chay den day */
    while (1)
    {
    }
}

/**
  * @brief  Cau hinh RTC lan dau 
  */
void RTC_Config(void)
{
    // 1. Kich hoat LSE
    RCC_LSEConfig(RCC_LSE_ON);
    // Ðoi LSE san sang
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

    // 2. Chon LSE lam nguon clock cho RTC
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    
    // 3. Kich hoat RTC Clock
    RCC_RTCCLKCmd(ENABLE);

    // 4. Doi dong bo thanh ghi
    RTC_WaitForSynchro();
    // Doi thao tac ghi cuoi cung hoan tat
    RTC_WaitForLastTask();

    // 5. Kích hoat ngatt RTC Alarm (de co the kich hoat EXTI)
    RTC_ITConfig(RTC_IT_ALR, ENABLE);
    RTC_WaitForLastTask();

    // 6. Ðat Prescaler
    // (LSE = 32768 Hz) -> (32768 / (32767 + 1)) = 1 Hz
    RTC_SetPrescaler(32767); // 0x7FFF
    RTC_WaitForLastTask();
}

/**
  * @brief  Cau hình EXTI Line 17 (RTC Alarm) de danh thuc
  */
void EXTI_Wakeup_Config(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    // Cau hinh EXTI Line 17 (RTC Alarm)
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event; // Che do Event (khong phai Interrupt)
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}


/**
  * @brief  Khoi tao UART1 (PA9=TX, PA10=RX)
  */
void UART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // Kich hoat clock cho GPIOA và USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // Cau hình PA9 (TX)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Cau hình PA10 (RX)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Cau hình USART1
    USART_InitStructure.USART_BaudRate = 9600; 
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx; // Chi can che do TX
    
    USART_Init(USART1, &USART_InitStructure);
    
    // Kich hoat USART1
    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  Gi 1 chuoi qua UART
  */
void UART_SendString(const char* str)
{
    while(*str)
    {
        // Doi thanh ghi TX trong
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        // Gui ký tu
        USART_SendData(USART1, *str);
        str++;
    }
}
