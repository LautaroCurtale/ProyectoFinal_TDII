#ifndef DISPLAY_RTC_I2C_H_
#define DISPLAY_RTC_I2C_H_

#include "definiciones.h"

#define DS3231_ADDRESS  0x68
#define REG_SECONDS     0x00
#define REG_MINUTES     0x01
#define REG_HOURS       0x02

#define D16x2_ADDRESS   0x27

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYLEFT 0x02
#define LCD_BLINKON 0x01
#define LCD_CURSORON 0x02
#define LCD_DISPLAYON 0x04
#define LCD_MOVERIGHT 0x04
#define LCD_DISPLAYMOVE 0x08
#define LCD_5x10DOTS 0x04
#define LCD_2LINE 0x08
#define LCD_8BITMODE 0x10
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE_BIT 0x04
#define LCD_CHARACTER  1
#define LCD_COMMAND    0
#define MAX_LINES      2
#define MAX_CHARS      16

extern uint8_t edit_menu;                  // Menú de edición (0: inactivo, 1: activo)
extern uint8_t edit_mode;                  // Modo edición (0: inactivo, 1: activo)
extern uint8_t current_edit;               // Índice de la hora en edición
extern uint8_t flagh2, flagh3, flagh4;
extern uint8_t flag_hora;

void MostrarPantallaNormal(uint8_t hours, uint8_t minutes, uint8_t seconds, HorarioConfig *config);
void EditarHorarios(HorarioConfig *config);
void RTC_Inicializacion(void);
void RTC_WriteRegister(uint8_t reg, uint8_t value);
uint8_t RTC_ReadRegister(uint8_t reg);
void RTC_ObtenerHora(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
void LCD_Init(void);
void display_i2c_write_byte(uint8_t value);
void LCD_ToggleEnable(uint8_t val);
void LCD_SendByte(uint8_t val, int mode);
void LCD_Clear(void);
void LCD_SetCursor(int line, int position);
void LCD_Char(char val);
void LCD_Print(const char *s);

#endif /* DISPLAY_RTC_I2C_H_ */
