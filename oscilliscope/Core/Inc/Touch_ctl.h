/*
 * Touch_ctl.h
 *
 *  Created on: May 5, 2021
 *      Author: paul_
 */

#ifndef INC_TOUCH_CTL_H_
#define INC_TOUCH_CTL_H_

void Init_Touch(void);

void Touch_SPI(void);

void LCD_SPI(void);

char Touch_Read(short *x, short*y);

#endif /* INC_TOUCH_CTL_H_ */
