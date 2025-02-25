#include "definiciones.h"
#include "display_rtc_i2c.h"
#include "bluetooth.h"

volatile uint16_t flag_tick = 0;
volatile uint16_t flag_tick_usart = 0;
uint8_t edit_menu = 0;                  // Menú de edición (0: inactivo, 1: activo)
uint8_t edit_mode = 0;                  // Modo edición (0: inactivo, 1: activo)
uint8_t current_edit = 0;               // Índice de la hora en edición
uint8_t flagh2 = 1, flagh3 = 1, flagh4 = 1; // Banderas horarios
uint8_t flagbluetooth = 0;				// Bandera App
uint8_t flag_hora = 1;
uint32_t duracion = 5;


int main(void) {
	BOARD_InitBootPins();
    BOARD_BootClockFRO18M();
    BOARD_InitDebugConsole();
    init_SWM_USART_DEFAULT();
    GPIO_Inicializacion();
    SysTick_Inicializacion();
    I2C_Inicializacion();
    RTC_Inicializacion();
    LCD_Init();
    Bluetooth_Init();


    uint8_t hours, minutes, seconds;

    HorarioConfig config = {
        {8, 12, 18,22},
        {0, 30, 0, 30},
        0,
        4
    };

    index_row_col index = {0, 3, 0, 3};
    char flagapp;
    char key;

//    Calibración RTC
//    RTC_WriteRegister(0x00, 0x00);
//    RTC_WriteRegister(0x01, 0x50);
//    RTC_WriteRegister(0x02, 0x17);

    while (1) {
//      Obtener la hora actual del RTC
        RTC_ObtenerHora(&hours, &minutes, &seconds);

        flagapp = 0;
        if (bt_ready) {
            flagapp = rxBuffer[0];
            // Imprimir en la consola (debug)
            printf("Dato recibido: %s\n", rxBuffer);
            bt_ready = 0;
        }
        switch (flagapp) {
            case 'a':
                // Enviar horarios a la app
                Bluetooth_SendHorario(&config);
                break;
    	    case 'e':
//                 Inicio la edicion en la app
                 flagbluetooth = 1;
                break;
            case 'f':
                // Recibir horarios de la app
                Bluetooth_ReceiveHorario(&config);
                if(config.hora[1] == 99){
                	flagh2 = 0;
                	flagh3 = 0;
    				flagh4 = 0;
                }
                if(config.hora[2] == 99){
                	flagh2 = 1;
                	flagh3 = 0;
    				flagh4 = 0;
                }
                if(config.hora[3] == 99){
                	flagh2 = 1;
                	flagh3 = 1;
    				flagh4 = 0;
                }
                flagbluetooth = 0;
                break;
           	 case 't':
//                 Se cancelo la edicion en la app
                 flagbluetooth = 0;
             break;
            default:
            break;
        }


        //Mostrar estado normal
        if (flag_hora != 0){
		MostrarPantallaNormal(hours, minutes, seconds, &config);
        }

        // Comprobar entrada del teclado
        key = keypad_obtener_tecla(index);
        if (key == 'A') {
        	edit_menu = 1;
        	flag_hora = 0;
        	LCD_Clear();
    		if (flagbluetooth == 1){
    			edit_menu = 0;
    			flag_hora = 1;
    		}
        	while (edit_menu == 1){
                EditarHorarios(&config);
        	}
        }
    }
    return 0 ;
}
