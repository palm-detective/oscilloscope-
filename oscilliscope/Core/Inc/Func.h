/*
 * Func.h
 *
 *  Created on: Jun 4, 2021
 *      Author: paul_
 */

#ifndef INC_FUNC_H_
#define INC_FUNC_H_

#define SIGN         0
#define UNSIGN       1

void u8ToDec3(char *p, char n);
void memset16(unsigned short* ptr, unsigned short value, short len);
void Value2Str(char *p, long n, const char *pUnit, char e, char Mode);
long Exp(char x);
short Sqrt32(int n);
void u16ToDec4Str(char *p, short n);

#endif /* INC_FUNC_H_ */
