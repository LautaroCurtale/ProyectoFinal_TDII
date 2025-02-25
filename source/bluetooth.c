#include "bluetooth.h"

volatile char rxBuffer[UART_BUFFER_SIZE];
volatile uint16_t rxIndex = 0;
char txBuffer[UART_BUFFER_SIZE];
uint8_t bt_ready = 0;

void init_SWM_USART_DEFAULT(void) {
	CLOCK_EnableClock (kCLOCK_Swm);

	SWM_SetMovablePinSelect(SWM0, kSWM_USART1_TXD, kSWM_PortPin_P0_9);
	SWM_SetMovablePinSelect(SWM0, kSWM_USART1_RXD, kSWM_PortPin_P0_8);

	CLOCK_DisableClock(kCLOCK_Swm);
}

void Bluetooth_Init(void) {
    usart_config_t config;

    CLOCK_Select(kUART1_Clk_From_MainClk);

    USART_GetDefaultConfig(&config);
    config.baudRate_Bps = BLUETOOTH_BAUDRATE;
    config.enableTx = true;
    config.enableRx = true;

    // Inicializa el UART para el módulo Bluetooth
    USART_Init(BLUETOOTH_UART, &config, CLOCK_GetFreq(kCLOCK_MainClk));

    USART_EnableInterrupts(USART1, kUSART_RxReadyInterruptEnable);
    NVIC_EnableIRQ(USART1_IRQn);
}

void Bluetooth_SendChar(uint8_t c) {
   USART_WriteBlocking(BLUETOOTH_UART, (uint8_t *)&c, 1);
}

void Bluetooth_SendFlag(uint8_t flag) {
    Bluetooth_SendChar(flag);  // Enviar la bandera como un byte
}

void Bluetooth_SendHorario(HorarioConfig *config) {
    char buffer[32];
    for (int i = 0; i < MAX_HORARIOS; i++) {
        // C:%u/%u
        sprintf(buffer, "%02u,%02u,",
            config->hora[i],
            config->minuto[i]
        );
        USART_WriteBlocking(BLUETOOTH_UART, (uint8_t *) buffer, strlen(buffer));

    }
    sprintf(buffer, "\n");
    USART_WriteBlocking(BLUETOOTH_UART, (uint8_t *) buffer, strlen(buffer));
}

uint8_t extract_2_digits(char dec, char unit) {
	return (dec - '0') * 10 + (unit - '0');
}

void Bluetooth_ReceiveHorario(HorarioConfig *config) {
    uint8_t index = 1;
    uint8_t buffer_index = 0;
    uint8_t horario_index = 0;
    char buffer[5] = {0};
    uint8_t aux_hour = 0, aux_minute = 0;
    config->cantComidasTotal = 0;
    while(rxBuffer[index] != '\0') {
        if (rxBuffer[index] == 'T' || rxBuffer[index] == '$') {
        	buffer[buffer_index] = '\0';
            buffer_index = 0;

            aux_hour = extract_2_digits(buffer[0], buffer[1]);
            aux_minute = extract_2_digits(buffer[3], buffer[4]);
            if (aux_hour > 24 || aux_minute > 59) {
                aux_hour = 0;
                aux_minute = 0;
            } else {
                config->cantComidasTotal++;
            }
            config->hora[horario_index] = aux_hour;
            config->minuto[horario_index] = aux_minute;
            horario_index++;
        } else {
            buffer[buffer_index] = rxBuffer[index];
            buffer_index++;
        }
        index++;
    }
    for (int i = horario_index; i < 4; i++) {
    	config->hora[i] = 99;
    }
}

void USART1_IRQHandler(void) {
	if (flag_tick_usart > 2000)
        reset_usart();
	if (rxIndex < (UART_BUFFER_SIZE - 1)) {
		uint8_t c = USART_ReadByte(BLUETOOTH_UART);
		if (c != '\r' && c != '\n' && c != '$') {
			rxBuffer[rxIndex] = c;
			rxIndex++;
		} else {
			if (c == '$') {
				rxBuffer[rxIndex] = c;
				rxIndex++;
			}
			rxBuffer[rxIndex] = '\0';
            bt_ready = 1;
		}
	}
}

void reset_usart(void) {
	for (int i = 0; i < rxIndex; i++)
		rxBuffer[i] = '\0';
	flag_tick_usart = 0;
	rxIndex = 0;
    bt_ready = 0;
}
