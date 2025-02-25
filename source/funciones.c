#include "definiciones.h"


void GPIO_Inicializacion (void) {
	gpio_pin_config_t out_config = {kGPIO_DigitalOutput, 1};
	gpio_pin_config_t in_config = {kGPIO_DigitalInput};
	GPIO_PortInit (GPIO, PUERTO_0);
	GPIO_PinInit (GPIO, PUERTO_0, FILA_1, &in_config);
	GPIO_PinInit (GPIO, PUERTO_0, FILA_2, &in_config);
	GPIO_PinInit (GPIO, PUERTO_0, FILA_3, &in_config);
	GPIO_PinInit (GPIO, PUERTO_0, FILA_4, &in_config);
	GPIO_PinInit (GPIO, PUERTO_0, COLUMNA_1, &out_config);
	GPIO_PinInit (GPIO, PUERTO_0, COLUMNA_2, &out_config);
	GPIO_PinInit (GPIO, PUERTO_0, COLUMNA_3, &out_config);
	GPIO_PinInit (GPIO, PUERTO_0, COLUMNA_4, &out_config);
	GPIO_PinInit (GPIO, 0, MOTOR, &out_config);
	GPIO_PinWrite(GPIO, 0, MOTOR, 0);
}
void SysTick_Inicializacion (void) {
	(void) SysTick_Config (SystemCoreClock / 1000);
}

void SysTick_Handler(void) {
	if (flag_tick > 0xFFF0)
		flag_tick = 0;
	if (flag_tick_usart > 0xFFF0)
		flag_tick_usart = 0;
	flag_tick_usart++;
	flag_tick++;
}

void delay_ms (uint16_t mseg) {
	flag_tick = 0;
	while (flag_tick <= mseg);
}


void I2C_Inicializacion (void) {
	uint32_t baudRate = 400000;
	uint32_t frecuencia = 12000000;
	i2c_master_config_t i2config;
	CLOCK_Select (kI2C1_Clk_From_MainClk);
	CLOCK_EnableClock (kCLOCK_Swm);
	SWM_SetMovablePinSelect (SWM0, kSWM_I2C1_SDA, kSWM_PortPin_P0_27);
	SWM_SetMovablePinSelect (SWM0, kSWM_I2C1_SCL, kSWM_PortPin_P0_28);
	CLOCK_DisableClock (kCLOCK_Swm);
	I2C_MasterGetDefaultConfig (&i2config);
	i2config.baudRate_Bps = baudRate;
	I2C_MasterInit (I2C1, &i2config, frecuencia);
}

char keypad_obtener_tecla(index_row_col index) {
    for (uint8_t columna = index.col_start; columna <= index.col_end; columna ++) {
        GPIO_PinWrite (GPIO, PUERTO_0, COLUMNA_1 + columna, 0);
        for (uint8_t fila = index.row_start; fila <= index.row_end; fila ++) {
            if (GPIO_PinRead (GPIO, PUERTO_0, FILA_1 + fila) == 0) {
                delay_ms(50);
                if (GPIO_PinRead (GPIO, PUERTO_0, FILA_1 + fila) == 0) {
                    GPIO_PinWrite (GPIO, PUERTO_0, COLUMNA_1 + columna, 1);
                    return KEYPAD_MAP[fila][columna];
                }
            }
        }
        GPIO_PinWrite (GPIO, PUERTO_0, COLUMNA_1 + columna, 1);;
    }
    return '\0';
}

char keypad_esperar_tecla(void) {
    char key;
    do {
        key = keypad_obtener_tecla((index_row_col) {0, 3, 0, 3});
    } while (key == '\0'); // Esperar hasta que se detecte una tecla
	return key;
}

int keypad_esperar_numero(int digitos) {
    int numero = 0;
    char nkey;

    for (int i = 0; i < digitos; i++) {
        do {
            nkey = keypad_esperar_tecla();  // Esperar entrada válida
        } while (nkey < '0' || nkey > '9');  // Validar que sea un dígito
        while(keypad_obtener_tecla((index_row_col) {0, 3, 0, 3}) != '\0')
        	delay_ms(25);
        numero = numero * 10 + (nkey - '0');  // Construir el número
    }

    return numero;
}

void iniciar_motor(uint8_t hours, uint8_t minutes, uint8_t seconds, HorarioConfig *config, uint32_t duracion)
{
    for (int i = 0; i < config->cantComidasTotal; i++)
    {
        if (hours == config->hora[i] && minutes == config->minuto[i] && seconds == 0) // Verifica coincidencia de hora y minuto
        {
        	   GPIO_PinWrite(GPIO, 0, MOTOR, 1);

        	   // Retardo para la duración especificada (bloqueante)
        	   delay_ms(duracion * 1000);

        	   // Detener el PWM
        	   GPIO_PinWrite(GPIO, 0, MOTOR, 0);
            break;
        }
    }
}
