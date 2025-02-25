#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "definiciones.h"

#define BLUETOOTH_UART USART1
#define BLUETOOTH_BAUDRATE 9600
#define UART_BUFFER_SIZE 32
#define BLUETOOTH_END_CHAR '$'

extern volatile char rxBuffer[UART_BUFFER_SIZE];
extern volatile uint16_t rxIndex;
extern char txBuffer[UART_BUFFER_SIZE];
extern uint8_t bt_ready;


void init_SWM_USART_DEFAULT(void);
void Bluetooth_Init(void);
void Bluetooth_SendChar(uint8_t c);
void Bluetooth_SendFlag(uint8_t flag);
void Bluetooth_SendHorario(HorarioConfig *config);
void Bluetooth_ReceiveHorario(HorarioConfig *config);

void reset_usart(void);

#endif // BLUETOOTH_H
