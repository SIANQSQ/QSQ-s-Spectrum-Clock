#ifndef __SERIAL_H
#define __SERIAL_H

#define RX_BUFFER_SIZE 100

extern char rx_buffer[]; // ?????
extern uint16_t rx_index; // ?????
extern uint8_t string_detected; 
void USART1_Init(void);

#endif
