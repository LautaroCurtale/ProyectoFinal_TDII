#include "display_rtc_i2c.h"

i2c_master_handle_t i2cHandle;
i2c_master_config_t i2cConfig;

// Conversión BCD a Decimal
static uint8_t BCD_to_Dec(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void display_i2c_write_byte(uint8_t value) {
    i2c_master_transfer_t xfer;
    xfer.slaveAddress = D16x2_ADDRESS;
    xfer.direction = kI2C_Write;
    xfer.subaddress = 0;
    xfer.subaddressSize = 0;
    xfer.data = &value;
    xfer.dataSize = 1;

    I2C_MasterTransferBlocking(I2C1, &xfer);
}

// Función para escribir un byte al RTC
void RTC_WriteRegister(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    i2c_master_transfer_t xfer;
    xfer.slaveAddress = DS3231_ADDRESS;
    xfer.direction = kI2C_Write;
    xfer.subaddress = 0;
    xfer.subaddressSize = 0;
    xfer.data = data;
    xfer.dataSize = 2;
    xfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(I2C1, &xfer);
}

// Función para leer un byte del DS3231
uint8_t RTC_ReadRegister(uint8_t reg) {
    uint8_t value;
    i2c_master_transfer_t xfer;

    // Configurar lectura del registro
    xfer.slaveAddress = DS3231_ADDRESS;
    xfer.direction = kI2C_Write;
    xfer.subaddress = reg;
    xfer.subaddressSize = 1;
    xfer.data = NULL;
    xfer.dataSize = 0;
    xfer.flags = kI2C_TransferDefaultFlag;
    I2C_MasterTransferBlocking(I2C1, &xfer);

    // Leer valor del registro
    xfer.direction = kI2C_Read;
    xfer.data = &value;
    xfer.dataSize = 1;
    I2C_MasterTransferBlocking(I2C1, &xfer);

    return value;
}

// Inicializar el módulo RTC
void RTC_Inicializacion(void) {
    // Leer el registro de control para verificar si el reloj está funcionando
    uint8_t status = RTC_ReadRegister(0x0F); // Registro de estado
    if (status & 0x80) { // Comprobar el bit OSF (Oscillator Stop Flag)
        // El reloj se ha detenido, reiniciarlo
        RTC_WriteRegister(0x0E, 0x00); // Registro de control: habilitar el oscilador
    }
}

// Leer la hora actual del RTC (formato 24 horas)
void RTC_ObtenerHora(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    uint8_t bcdTime[3]; // Array para almacenar los valores de segundos, minutos y horas

    // Leer los registros de tiempo (0x00 = segundos, 0x01 = minutos, 0x02 = horas)
    for (uint8_t i = 0; i < 3; i++) {
        bcdTime[i] = RTC_ReadRegister(i);
    }

    // Convertir los valores de BCD a decimal
    *seconds = BCD_to_Dec(bcdTime[0]); // ((bcdTime[0] >> 4) * 10) + (bcdTime[0] & 0x0F);
    *minutes = BCD_to_Dec(bcdTime[1]); // ((bcdTime[1] >> 4) * 10) + (bcdTime[1] & 0x0F);


    // Si está en formato de 12 horas (bit 6 encendido), conviértelo a 24 horas
    if (bcdTime[2] & 0x40) { // Comprobar si el bit 6 (12/24 hr) está encendido
        uint8_t isPM = bcdTime[2] & 0x20; // Bit 5 indica AM/PM
        *hours = (*hours % 12) + (isPM ? 12 : 0);
    } else {
        // Extraer el formato de 24 horas del registro de horas
        *hours = BCD_to_Dec(bcdTime[2]); // ((bcdTime[2] >> 4) * 10) + (bcdTime[2] & 0x0F);
    }
}

// Mostrar la información en el LCD
void MostrarPantallaNormal(uint8_t hours, uint8_t minutes, uint8_t seconds, HorarioConfig *config) {
    char buffer[32];
    char buffer2[16];

    // Calcular comidas realizadas
    config->cantComidas = 0;
    for (uint8_t p = 0; p < config->cantComidasTotal; p++) {
        if (hours > config->hora[p] ||
           (hours == config->hora[p] && minutes >= config->minuto[p])) {
            config->cantComidas++;
        } else {
        	break;
        }
    }

    // Actualizar pantalla
    sprintf(buffer, "T=%02u:%02u:%02u C:%u/%u", hours, minutes, seconds, config->cantComidas, config->cantComidasTotal);
    LCD_SetCursor(0, 0);
    LCD_Print(buffer);


	if (config->cantComidas == 0){
   		sprintf(buffer2, "Sig. Com.=%02u:%02u", config->hora[config->cantComidas], config->minuto[config->cantComidas]);
		LCD_SetCursor(1, 0);
   		LCD_Print(buffer2);
   	} else if (config->cantComidas == 1 && flagh2 == 1){
   		sprintf(buffer2, "Sig. Com.=%02u:%02u", config->hora[config->cantComidas], config->minuto[config->cantComidas]);
   		LCD_SetCursor(1, 0);
   		LCD_Print(buffer2);
   	} else if (config->cantComidas == 2 && flagh3 == 1){
   		sprintf(buffer2, "Sig. Com.=%02u:%02u", config->hora[config->cantComidas], config->minuto[config->cantComidas]);
   		LCD_SetCursor(1, 0);
   		LCD_Print(buffer2);
   	} else if (config->cantComidas == 3 && flagh4 == 1){
    		sprintf(buffer2, "Sig. Com.=%02u:%02u", config->hora[config->cantComidas], config->minuto[config->cantComidas]);
    		LCD_SetCursor(1, 0);
   		LCD_Print(buffer2);
   	} else if ((config->cantComidas >= 4) || ((config->cantComidas >= 1) && flagh2 == 0) || ((config->cantComidas >= 2) && flagh3 == 0) || ((config->cantComidas >= 3) && flagh4 == 0)){
   		sprintf(buffer2, "Sig. Com.=%02u:%02u", config->hora[0], config->minuto[0]);
    		LCD_SetCursor(1, 0);
   		LCD_Print(buffer2);
	}
}


// Editar horarios usando el teclado matricial
void EditarHorarios(HorarioConfig *config) {
	char buffer3[16];
	char buffer4[16];
    char bufferedit[32];
    char key;
    uint8_t nuevaHora, nuevoMinuto;
    uint8_t index_edit = 0;
    LCD_Clear();
    LCD_SetCursor(0, 0);
    sprintf(buffer3,"Editar Horarios:");
    LCD_Print(buffer3);
    LCD_SetCursor(1, 0);
    sprintf(buffer4,"1-2-3-4 #:No");
    LCD_Print(buffer4);

    key = keypad_esperar_tecla();
    int key_value = key - '1';
    index_edit = key_value;
    switch (key){
        case '1':
        	LCD_Clear();
        	LCD_SetCursor(0, 0);
        	LCD_Print("Ingrese Hora 1");
        	// Leer la hora
        	LCD_SetCursor(1, 0);
        	LCD_Print("H:__");
        	LCD_SetCursor(1, 2);  // Posicionar el cursor para ingresar la hora
        	nuevaHora = keypad_esperar_numero(2);
        	// Leer los minutos
        	LCD_SetCursor(1, 5);
        	LCD_Print(":__");
        	LCD_SetCursor(1, 6);  // Posicionar el cursor para ingresar los minutos
        	nuevoMinuto = keypad_esperar_numero(2);
        	if ((nuevaHora >= 0 && nuevaHora < 24 && nuevoMinuto >= 0 && nuevoMinuto < 60)
        			&&((nuevaHora < config->hora[index_edit+1]) || (nuevaHora == config->hora[index_edit+1] && nuevoMinuto < config->minuto[index_edit+1]))) {
        		LCD_Clear();
        		LCD_SetCursor(0, 0);
        	    LCD_Print("Horario Guardado");
        	    delay_ms(2000);
        	    LCD_Clear();
				LCD_SetCursor(0, 0);
    	    	sprintf(bufferedit, "Hora 1=%02u:%02u", nuevaHora, nuevoMinuto);
    	    	LCD_Print(bufferedit);
				LCD_SetCursor(1, 0);
    	    	LCD_Print("Conf:C Canc:D");
        	    key = keypad_esperar_tecla();
        	    	if (key == 'C'){
            	        config->hora[index_edit] = nuevaHora;
            	        config->minuto[index_edit] = nuevoMinuto;
            	        LCD_Clear();
            	        break;
        	       } else if ((key == 'D')){
        	    	   LCD_Clear();
        	    	   break;
        	       }
        	    } else {
        	    	LCD_Clear();
        	        LCD_SetCursor(0, 0);
        	        LCD_Print("Horario Invalido");
        	        delay_ms(2000);
        	        LCD_Clear();
        	        break;
        	    }
        	    break;
        case '2':
        	if (flagh2 == 1){
        		LCD_Clear();
        	    LCD_SetCursor(0, 0);
        	    LCD_Print("Editar Hora 2");
        	    LCD_SetCursor(1, 0);
        	    LCD_Print("C:Si #:No B:Nule");
        	    key = keypad_esperar_tecla();
        	    	if (key == '#'){
        	    		LCD_Clear();
        	    		break;
        	    	} else if (key == 'B'){
        	    		LCD_Clear();
        	    		LCD_SetCursor(0, 0);
        	    		LCD_Print("Horario Anulado");
        	    		flagh2 = 0;
        	    		flagh3 = 0;
        	    		flagh4 = 0;
        	    		config->cantComidasTotal = 1;
        	    		delay_ms(3000);
        	    		LCD_Clear();
        	    		break;
        	    	} else if (key == 'C'){
                		LCD_Clear();
                	    LCD_SetCursor(0, 0);
                	    LCD_Print("Ingrese Hora 2");
                	    // Leer la hora
                	    LCD_SetCursor(1, 0);
                	    LCD_Print("H:__");
                	    LCD_SetCursor(1, 2);  // Posicionar el cursor para ingresar la hora
                	    nuevaHora = keypad_esperar_numero(2);
                	    // Leer los minutos
                	    LCD_SetCursor(1, 5);
                	    LCD_Print(":__");
                	    LCD_SetCursor(1, 6);  // Posicionar el cursor para ingresar los minutos
                	    nuevoMinuto = keypad_esperar_numero(2);
		        	    	if ((nuevaHora >= 0 && nuevaHora < 24 && nuevoMinuto >= 0 && nuevoMinuto < 60)
		        	    		&& ((flagh3 == 1) && ((nuevaHora < config->hora[index_edit+1]) || (nuevaHora == config->hora[index_edit+1] && nuevoMinuto < config->minuto[index_edit+1])))
		        	    		&& ((nuevaHora > config->hora[index_edit-1]) || (nuevaHora == config->hora[index_edit-1] && nuevoMinuto > config->minuto[index_edit-1]))) {
		        	    			LCD_Clear();
		        	    			LCD_SetCursor(0, 0);
		        	    			LCD_Print("Horario Guardado");
		        	    			delay_ms(3000);
		        	    			LCD_Clear();
									LCD_SetCursor(0, 0);
									sprintf(bufferedit, "Hora 2=%02u:%02u", nuevaHora, nuevoMinuto);
									LCD_Print(bufferedit);
									LCD_SetCursor(1, 0);
									LCD_Print("Conf:C Canc:D");
									key = keypad_esperar_tecla();
										if (key == 'C'){
											config->hora[index_edit] = nuevaHora;
											config->minuto[index_edit] = nuevoMinuto;
											LCD_Clear();
											break;
										} else if ((key == 'D')){
											LCD_Clear();
											break;
										}
		        	    } else {
		        	    	LCD_Clear();
		        	        LCD_SetCursor(0, 0);
		        	        LCD_Print("Horario Invalido");
		        	        delay_ms(3000);
		        	        LCD_Clear();
		        	        break;
		        	    }
        	    	} break;
        		} else if (flagh2 == 0){
        				LCD_Clear();
						LCD_SetCursor(0, 0);
						LCD_Print("Activar Hora 2");
						LCD_SetCursor(1, 0);
						LCD_Print("*:Si - #:No");
						key = keypad_esperar_tecla();
							if (key == '*'){
								LCD_Clear();
								flagh2 = 1;
								config->cantComidasTotal = 2;
								LCD_SetCursor(0, 0);
								LCD_Print("Ingrese Horario");
								LCD_SetCursor(1, 0);
								LCD_Print("En El Menu");
								delay_ms(3000);
								LCD_Clear();
								break;
						} else if (key == '#'){
								LCD_Clear();
								break;
						}
				} else
					break;
        case '3':
        	if (flagh3 == 1 && flagh2 == 1){
        		LCD_Clear();
        	    LCD_SetCursor(0, 0);
        	    LCD_Print("Editar Hora 3");
        	    LCD_SetCursor(1, 0);
        	    LCD_Print("C:Si #:No B:Nule");
        	    key = keypad_esperar_tecla();
        	    	if (key == '#'){
        	    		LCD_Clear();
        	    		break;
        	    	} else if (key == 'B'){
        	    		LCD_Clear();
        	    		LCD_SetCursor(0, 0);
        	    		LCD_Print("Horario Anulado");
        	    		flagh3 = 0;
        	    		flagh4 = 0;
        	    		config->cantComidasTotal = 2;
        	    		delay_ms(3000);
        	    		LCD_Clear();
        	    		break;
        	    	} else if (key == 'C'){
                		LCD_Clear();
                	    LCD_SetCursor(0, 0);
                	    LCD_Print("Ingrese Hora 3");
                	    // Leer la hora
                	    LCD_SetCursor(1, 0);
                	    LCD_Print("H:__");
                	    LCD_SetCursor(1, 2);  // Posicionar el cursor para ingresar la hora
                	    nuevaHora = keypad_esperar_numero(2);
                	    // Leer los minutos
                	    LCD_SetCursor(1, 5);
                	    LCD_Print(":__");
                	    LCD_SetCursor(1, 6);  // Posicionar el cursor para ingresar los minutos
                	    nuevoMinuto = keypad_esperar_numero(2);
		        	    	if ((nuevaHora >= 0 && nuevaHora < 24 && nuevoMinuto >= 0 && nuevoMinuto < 60)
		        	    		&& ((flagh4 == 1) && (((nuevaHora < config->hora[index_edit+1]) || (nuevaHora == config->hora[index_edit+1] && nuevoMinuto < config->minuto[index_edit+1]))))
		        	    		&& ((nuevaHora > config->hora[index_edit-1]) || (nuevaHora == config->hora[index_edit-1] && nuevoMinuto > config->minuto[index_edit-1]))) {
		        	    			LCD_Clear();
		        	    			LCD_SetCursor(0, 0);
		        	    			LCD_Print("Horario Guardado");
		        	    			delay_ms(3000);
		        	    			LCD_Clear();
									LCD_SetCursor(0, 0);
									sprintf(bufferedit, "Hora 3=%02u:%02u", nuevaHora, nuevoMinuto);
									LCD_Print(bufferedit);
									LCD_SetCursor(1, 0);
									LCD_Print("Conf:C Canc:D");
									key = keypad_esperar_tecla();
										if (key == 'C'){
											config->hora[index_edit] = nuevaHora;
											config->minuto[index_edit] = nuevoMinuto;
											LCD_Clear();
											break;
										} else if ((key == 'D')){
											LCD_Clear();
											break;
										}
		        	    } else {
		        	    	LCD_Clear();
		        	        LCD_SetCursor(0, 0);
		        	        LCD_Print("Horario Invalido");
		        	        delay_ms(3000);
		        	        LCD_Clear();
		        	        break;
		        	    }
        	    	} break;
        		} else if (flagh3 == 0 && flagh2 == 1){
        				LCD_Clear();
						LCD_SetCursor(0, 0);
						LCD_Print("Activar Hora 3");
						LCD_SetCursor(1, 0);
						LCD_Print("*:Si - #:No");
						key = keypad_esperar_tecla();
							if (key == '*'){
								LCD_Clear();
								flagh3 = 1;
								config->cantComidasTotal = 3;
								LCD_SetCursor(0, 0);
								LCD_Print("Ingrese Horario");
								LCD_SetCursor(1, 0);
								LCD_Print("En El Menu");
								delay_ms(3000);
								LCD_Clear();
								break;
						} else if (key == '#'){
								LCD_Clear();
								break;
						}
				} else
					break;
        	case '4':
            	if (flagh4 == 1 && flagh3 == 1 && flagh2 == 1){
            		LCD_Clear();
            	    LCD_SetCursor(0, 0);
            	    LCD_Print("Editar Hora 4");
            	    LCD_SetCursor(1, 0);
            	    LCD_Print("C:Si #:No B:Nule");
            	    key = keypad_esperar_tecla();
            	    	if (key == '#'){
            	    		LCD_Clear();
            	    		break;
            	    	} else if (key == 'B'){
            	    		LCD_Clear();
            	    		LCD_SetCursor(0, 0);
            	    		LCD_Print("Horario Anulado");
            	    		flagh4 = 0;
            	    		config->cantComidasTotal = 3;
            	    		delay_ms(3000);
            	    		LCD_Clear();
            	    		break;
            	    	} else if (key == 'C'){
                    		LCD_Clear();
                    	    LCD_SetCursor(0, 0);
                    	    LCD_Print("Ingrese Hora 4");
                    	    // Leer la hora
                    	    LCD_SetCursor(1, 0);
                    	    LCD_Print("H:__");
                    	    LCD_SetCursor(1, 2);  // Posicionar el cursor para ingresar la hora
                    	    nuevaHora = keypad_esperar_numero(2);
                    	    // Leer los minutos
                    	    LCD_SetCursor(1, 5);
                    	    LCD_Print(":__");
                    	    LCD_SetCursor(1, 6);  // Posicionar el cursor para ingresar los minutos
                    	    nuevoMinuto = keypad_esperar_numero(2);
    		        	    	if ((nuevaHora >= 0 && nuevaHora < 24 && nuevoMinuto >= 0 && nuevoMinuto < 60)
    		        	    		&& ((nuevaHora > config->hora[index_edit-1]) || (nuevaHora == config->hora[index_edit-1] && nuevoMinuto > config->minuto[index_edit-1]))) {
    		        	    			LCD_Clear();
    		        	    			LCD_SetCursor(0, 0);
    		        	    			LCD_Print("Horario Guardado");
    		        	    			delay_ms(3000);
    		        	    			LCD_Clear();
    									LCD_SetCursor(0, 0);
    									sprintf(bufferedit, "Hora 4=%02u:%02u", nuevaHora, nuevoMinuto);
    									LCD_Print(bufferedit);
    									LCD_SetCursor(1, 0);
    									LCD_Print("Conf:C Canc:D");
    									key = keypad_esperar_tecla();
    										if (key == 'C'){
    											config->hora[index_edit] = nuevaHora;
    											config->minuto[index_edit] = nuevoMinuto;
    											LCD_Clear();
    											break;
    										} else if ((key == 'D')){
    											LCD_Clear();
    											break;
    										}
    		        	    } else {
    		        	    	LCD_Clear();
    		        	        LCD_SetCursor(0, 0);
    		        	        LCD_Print("Horario Invalido");
    		        	        delay_ms(3000);
    		        	        LCD_Clear();
    		        	        break;
    		        	    }
            	    	} break;
            		} else if (flagh4 == 0 && flagh3 == 1){
            				LCD_Clear();
    						LCD_SetCursor(0, 0);
    						LCD_Print("Activar Hora 4");
    						LCD_SetCursor(1, 0);
    						LCD_Print("*:Si - #:No");
    						key = keypad_esperar_tecla();
    							if (key == '*'){
    								LCD_Clear();
    								flagh4 = 1;
    								edit_mode = 1;
    								config->cantComidasTotal = 4;
    								LCD_SetCursor(0, 0);
    								LCD_Print("Ingrese Horario");
    								LCD_SetCursor(1, 0);
    								LCD_Print("En El Menu");
    								delay_ms(3000);
    								LCD_Clear();
    								break;
    						} else if (key == '#'){
    								LCD_Clear();
    								break;
    						}
    				} else
    					break;
        	case '#':
        	edit_menu = 0;
        	flag_hora = 1;
        	LCD_Clear();
        	break;
        }
}

//   Funciones display

void LCD_SendByte(uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    display_i2c_write_byte(high);
    LCD_ToggleEnable(high);
    display_i2c_write_byte(low);
    LCD_ToggleEnable(low);
}

void LCD_SetCursor(int line, int position) {
    int line_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    int val = 0x80 + line_offsets[line] + position;
    LCD_SendByte(val, LCD_COMMAND);
}

void LCD_Char(char val) {
    LCD_SendByte(val, LCD_CHARACTER);
}

void LCD_Print(const char *s) {
    while (*s) {
        LCD_Char(*s++);
    }
}

void LCD_Clear(void) {
    LCD_SendByte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

void LCD_ToggleEnable(uint8_t val) {
#define DELAY_US 1
    delay_ms(DELAY_US);
    display_i2c_write_byte(val | LCD_ENABLE_BIT);
    delay_ms(DELAY_US);
    display_i2c_write_byte(val & ~LCD_ENABLE_BIT);
    delay_ms(DELAY_US);
}

void LCD_Init(void) {
    
    LCD_SendByte(0x03, LCD_COMMAND);
    LCD_SendByte(0x03, LCD_COMMAND);
    LCD_SendByte(0x03, LCD_COMMAND);
    LCD_SendByte(0x02, LCD_COMMAND);

    LCD_SendByte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    LCD_SendByte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    LCD_SendByte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    LCD_Clear();
}
