/*
 * Lcd_ctl.h
 *
 *  Created on: May 5, 2021
 *      Author: paul_
 */

#ifndef INC_LCD_CTL_H_
#define INC_LCD_CTL_H_

void Init_LCD(void);

void LCD_ClrScrn(unsigned short Color);

void LCD_Set_Posi( short Sx, short Sy);

void LCD_Set_Pixel(unsigned short Color);

unsigned short LCD_Get_Pixel(void);

void lcd_write_cmd_data(unsigned short Cmd, short Len, unsigned short *Data);

#endif /* INC_LCD_CTL_H_ */
