/*
 * Flash.h
 *
 *  Created on: May 8, 2021
 *      Author: paul_
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

extern char  Cal_Flag;
extern short TIM_BASE_Val;

void Read_CalFlag(void);
void Read_Parameter(void);
char Save_Parameter(void);
void Read_Kpg(void);
void Restore(void);
char Save_Kpg(void);
void Restore_CalPar(void);

#endif /* INC_FLASH_H_ */
