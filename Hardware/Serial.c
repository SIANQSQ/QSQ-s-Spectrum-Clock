#include "stm32f10x.h"
#include "string.h"
#include "main.h"
#define RX_BUFFER_SIZE 100

char rx_buffer[RX_BUFFER_SIZE]; // 接收缓冲区
volatile uint16_t rx_index = 0; // 缓冲区索引
volatile uint8_t string_detected = 0; // 字符串检测标志

void USART1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能GPIOA和USART1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // 配置USART1 Tx (PA9) 为推挽复用输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART1 Rx (PA10) 为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART1
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // 使能USART1接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // 配置USART1中断
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 使能USART1
    USART_Cmd(USART1, ENABLE);
}

extern int mode;
extern int changemode;
void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        // 读取接收到的字符
        char received_char = USART_ReceiveData(USART1);

        // 将字符存入缓冲区
        if (rx_index < RX_BUFFER_SIZE - 1) {
            rx_buffer[rx_index++] = received_char;
        }
		// 检查是否接收到目标字符串 "OFF"
        if (rx_index >= 3) { 
            if (strncmp(&rx_buffer[rx_index - 3], "OFF", 3) == 0) {
                rx_index = 0;       // 重置缓冲区索引
				changemode=1;
				mode=0;
            }
        }
        // 检查是否接收到目标字符串 "Light"
        if (rx_index >= 5) { 
            if (strncmp(&rx_buffer[rx_index - 5], "Light", 5) == 0) {
                rx_index = 0;       // 重置缓冲区索引
				changemode=1;
				if(mode>20 || mode<=10)mode = 11;
				else
				{
					mode++;
					if(mode==NumOfLightMode+11) mode = 11;
				}
            }
        }
		if (rx_index >= 5) { 
            if (strncmp(&rx_buffer[rx_index - 5], "Clock", 5) == 0) {
                rx_index = 0;       // 重置缓冲区索引
				changemode=1;
				if(mode>30 || mode<=20)mode = 21;
				else
				{
					mode++;
					if(mode==NumOfClockMode+21) mode = 21;
				}
            }
        }
        // 检查是否接收到目标字符串 "spectrum1"
        if (rx_index >= 9) { // "spectrum" 有 8 个字符（包括终止符）
            if (strncmp(&rx_buffer[rx_index - 8], "Spectrum", 8) == 0) {
                rx_index = 0;       // 重置缓冲区索引
				changemode=1;
				if(mode>10)mode = 1;
				else
				{
					mode++;
					if(mode==NumOfSpectrumMode+1) mode = 1;
				}
			}
        }
        // 如果缓冲区满了，重置索引
        if (rx_index >= RX_BUFFER_SIZE - 1) {
            rx_index = 0;
        }

        // 清除中断标志
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
