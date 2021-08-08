/*
 * Touch_ctl.h
 *
 *  Created on: May 5, 2021
 *      Author: paul_
 */

#ifndef INC_TOUCH_CTL_H_
#define INC_TOUCH_CTL_H_

#define KEYCODE_PLAY     1
#define KEYCODE_MANU     2
#define KEYCODE_UP       3
#define KEYCODE_DOWN     4
#define KEYCODE_LEFT     5
#define KEYCODE_RIGHT    6
#define KEYCODE_ACT      8
#define KEYCODE_MEM      9
#define KEYCODE_D_MANU   10

void Init_Touch(void);

void Touch_SPI(void);

void LCD_SPI(void);

#endif /* INC_TOUCH_CTL_H_ */
