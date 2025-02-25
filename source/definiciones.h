#ifndef DEFINICIONES_H_
#define DEFINICIONES_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "peripherals.h"
#include "clock_config.h"
#include "LPC845.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_i2c.h"
#include "fsl_sctimer.h"
#include "fsl_swm.h"
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "fsl_usart.h"
#include "fsl_iocon.h"

#define PUERTO_0 0
#define MOTOR 1
#define FILA_1 16
#define FILA_2 17
#define FILA_3 18
#define FILA_4 19
#define COLUMNA_1 20
#define COLUMNA_2 21
#define COLUMNA_3 22
#define COLUMNA_4 23
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

#define MAX_HORARIOS 4

typedef struct {
    uint8_t row_start;
    uint8_t row_end;
    uint8_t col_start;
    uint8_t col_end;
} index_row_col;

static const char KEYPAD_MAP[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


typedef struct {
    uint8_t hora[MAX_HORARIOS];
    uint8_t minuto[MAX_HORARIOS];
    uint8_t cantComidas;
    uint8_t cantComidasTotal;
} HorarioConfig;

extern volatile uint16_t flag_tick;
extern volatile uint16_t flag_tick_usart;

void GPIO_Inicializacion (void);
void SysTick_Inicializacion (void);
void delay_ms (uint16_t mseg);
void I2C_Inicializacion (void);
// void Keypad_Inicializacion(void);
char keypad_esperar_tecla(void);
char keypad_obtener_tecla(index_row_col index);
int keypad_esperar_numero(int digitos);
void iniciar_motor(uint8_t hours, uint8_t minutes, uint8_t seconds, HorarioConfig *config, uint32_t duracion);

#endif /* DEFINICIONES_H_ */
