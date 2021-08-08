/*
 * Display.c
 *
 *  Created on: Jul 9, 2021
 *      Author: paul_
 */

#include "Display.h"
#include "Lcd_ctl.h"

//==============================================================================
//                        System function
//==============================================================================

#define BACKGROUND    0x00                 // ������ɫ
#define GRID          0x7BEF               // ���屳�������ߵ���ɫ
#define GRID_COLOR    0x7BEF7BEF           // 32bits �����ߵ���ɫ
#define WINDOW_Y0     19                   // ������ʾ������ʼ�߶�

union Reg {
	unsigned I;
	char *A;
};

unsigned R0, R1, R2, R3, R5, R6, R7, R8, R9, R11, R12;
char *R10;

union Reg R4;

unsigned short AA[205];  //one more than orginal size

// R0:pDat, R1:pTab, R2:Col, R3:Var, R4:pBuf, R5:Cnt, R6:Tmp,
//void Fill_Base(R3 = u32 Color)// �л��������ɫ RET: R4+202 Used: R3-R5
void Fill_Base(void)
{
	R4.A = (char*) &AA[204]; // 204��/2 ��408 Bytes
	R5 =102;

Fill_Loop0:
	*(unsigned*)R4.A =R3;
	R4.A= R4.A +4;
	R5 =R5 -1;
	if (R5!=0)
	{
		goto Fill_Loop0;
	}
	R4.A = (char*) &AA[203];
	R3= GRID;
}

/*// void Buld_0(R4 = u16* pCol)   // ���������л������ı������� Used: R3-R5*/
void Buld_0(void)
{
	R3 = BACKGROUND;
	Fill_Base();
}

// void Buld_1(R4 = u16* pCol)   // ���������л������ı������� Used: R3-R6
void Buld_1(void)
{
	R3 = GRID_COLOR;
	Fill_Base();
	R3 =0;
	R5=402;
	*(unsigned short*)(R4.A +R5)=R3;
	*(unsigned short*)(R4.A -2)=R3;
}

// void Buld_2(R4 = u16* pCol)   // ���������л������ı������� Used: R3-R6
void Buld_2(void)
{
	R3 = BACKGROUND;
	Fill_Base();
	R5 =400;
	*(unsigned short*)(R4.A +R5)=R3;
	*(unsigned short*)(R4.A)=R3;
}

// void Buld_3(R4 = u16* pCol)   // ��������л������ı������� Used: R3-R6
void Buld_3(void)
{
	R3 = BACKGROUND;
	Fill_Base();
	R5=450;

Loop3:
	R5 = R5 -50;
	*(unsigned short*)(R4.A +R5)=R3;
	if ( R5!=0)
	{
		goto Loop3;
	}
	return;
}

// void Buld_4(R4 = u16* pCol)   // ���������л������ı�������
void Buld_4(void)
{
	R3 = BACKGROUND;
	Fill_Base();
	R5=410;

Loop7:
	R5 = R5 -10;
	*(unsigned short*)(R4.A +R5)=R3;
	if ( R5!=0)
	{
		goto Loop7;
	}
	return;
}

// Draw_Digit(R0:pDat, R2:Col, R3:Color, R4:Posi, R8:Mask)// �����ֲ��� Used: R5-R7
// Draw_Analog(R2:Col, R3:Color, R4:pDat)// ��ģ�Ⲩ������  Used: R3-R7
void Draw_Digit_Analog(unsigned short DorA)
{
	if(DorA == 1)  //draw digit
	{
	R7 =R0 +900;
	R7 = R7 + R2;
	R5 = *(char*) R7;
	R6 = *(char*) (R7 -1);
	R4.I = R4.I >> 1;
	R5 = R5 & R8;
	if (R5 == 0 )
		R5 = R4.I;
	else
		R5 = R4.I + 30;
	R6 = R6 & R8;
	if( R6 != 0)
		R4.I = R4.I + 30;
	goto Analog0;
	}

	if(DorA == 0)  //draw analog
	{
		R4.A = R4.A +2;
		R5 = *(char*)R4.A; //n1
		R4.I = *(char*)(R4.A-1); //n0
		goto Analog0;
	}
	return;

	R4.A = R4.A +2;
	R5 = *(char*)R4.A; //n1
	R4.I = *(char*)(R4.A-1); //n0

	R5 = R5 +50;
	R4.I =R4.I +50;

	Analog0:
	if (R5 == 255)
		return;
	if (R5 == 0)
		return;

	if(R5 >= R4.I)  // R5 = | n1 - n0 |
	{
		R6 =R4.I;
		R5 = R5 -R4.I;
	}
	else
	{
		R6 = R5;  // n1, n0 ����С���� R6
		R5 = R4.I -R5;
	}

	if(R6 > 199)
		return;

	R4.I = R5 +R6;
	if(R4.I >199)
	{
		R5 = 199 - R6;
		goto Analog2;
	}

	if( R4.I < 3 )
		return;

	if( R6 <3)
	{
		R6 = 3;
		R5 =R4.I -3;
		goto Analog2;
	}

	if(R5 ==0 )
	{
		R6 = R6 -1;
		R5 = R5 +2;
	}

Analog2:
	if( R5>= 20)
		R3= R3 +20;

	R3 =  *(unsigned short*) (R1 +R3);
	R6 = R6 << 1;
//    ADD     R6,  SP, R6            ;// ȷ����ʾλ��
    R6 = (unsigned) &AA[204- R6/2];

Analog3:
	*(unsigned short*)R6 = R3;
	R6 =R6 +2;
	R5 =R5 -1;
	if(R5>=0)
		goto Analog3;

return;
}

// Cursor_0(R1:pTab, R2:Col)// ���������α�˵�  Used: R3-R6
void Cursor_0(short *pTab)
{
	R3 =6;
	R4.A = (char*) &AA[204];

Cursor01:
	R5 = pTab[R3];
	if( (R5&1) != 0)
	{
		goto Cursor02;
	}
	R5 = pTab[R3 +10];
	R4.A = (char*)&AA[204 - R5/2];
	R6 = pTab[R3 +20];
	R4.I = R4.I -4;
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;

Cursor02:
	R3 = R3 -1;
	if( R3 >= 0)
	{
		goto Cursor01;
	}
	return;
}

void Cursor_1(short *pTab)
{
	R3 =6;
	R4.A = (char*) &AA[204];

Cursor11:
	R5 = pTab[R3];
	if( (R5&1) != 0)
	{
		goto Cursor12;
	}
	R5 = pTab[R3 +10];
	R4.A = (char*)&AA[204 - R5/2];
	R6 = pTab[R3 +20];
	R4.I = R4.I -2;
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;

Cursor12:
	R3 = R3 -1;
	if( R3 >= 0)
	{
		goto Cursor11;
	}
	return;
}

void Cursor_2(short *pTab)
{
	R3 =6;
	R4.A = (char*) &AA[204];

Cursor21:
	R5 = pTab[R3];
	if( (R5&1) != 0)
	{
		goto Cursor22;
	}
	R5 = pTab[R3 +10];
	R4.A = (char*)&AA[204 - R5/2];
	R6 = pTab[R3 +20];
	*(unsigned short*)R4.A =R6;

Cursor22:
	R3 = R3 -1;
	if( R3 >= 0)
	{
		goto Cursor21;
	}
	return;
}

// Cursor_3(R1:pTab, R2:Col)// ���������α���  Used: R3-R6
void Cursor_3(short *pTab)
{
	R3 =6;
	R4.A = (char*) &AA[204];

Cursor31:
	R5 = pTab[R3];
	if( (R5&2) != 0)
	{
		goto Cursor32;
	}

	R5= R2 -1;
	R5 = R5 &3;
	if( R5 != 0)
	{
		goto Cursor32;
	}

	R5 = pTab[R3 +10];
	R4.A = (char*)&AA[204 - R5/2];
	R6 = pTab[R3 +20];
	*(unsigned short*)R4.A =R6;

Cursor32:
	R3 = R3 -1;
	if( R3 >= 0)
	{
		goto Cursor31;
	}

	return;
}

// Cursor_4(R1:pTab, R2:Col)// �����л��α�˵�  Used: R3-R8
void Cursor_4(short *pTab)
{
	R3 = 7;

Cursor40:
	R4.A = (char*) &AA[204];
	R5 = pTab[R3];
	if((R5 & 1) == 0)
	{
		goto Cursor41;
	}
	goto Cursor49;

Cursor41:
	R5 = pTab[R3+10];
	R6 = pTab[R3+20];

	R8 =R5 -2;
	if (R5 != R8)
	{
		goto Cursor42;
	}

	*(unsigned short*)R4.A =R6;
	R4.A = R4.A +404;
	*(unsigned short*)R4.A =R6;
	goto Cursor49;

Cursor42:
	R8 = R8 +1;
	if (R2 != R8)
	{
		goto Cursor43;
	}

	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	R4.A = R4.A +400;
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	goto Cursor49;

Cursor43:
	R8 = R8 +1;
	if (R2 != R8)
	{
		goto Cursor45;
	}

	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	R4.A = (char*) &AA[4];
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	R5 = pTab[R3+10];
	if((R5 & 2) != 0)
	{
		goto Cursor45;
	}
	R4.A = (char*) &AA[204];
	R7 = R4.I +400;

Cursor44:
	R4.A= R4.A +8;
	*(unsigned short*)R4.A =R6;
	if(R7 >= R4.I)
	{
		goto Cursor44;
	}
	goto Cursor49;

Cursor45:
	R8 = R8 +1;
	if (R8 != R2)
	{
		goto Cursor46;
	}
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	R4.I = R4.I +400;
	*(unsigned short*)R4.A =R6;
	R4.A= R4.A +2;
	*(unsigned short*)R4.A =R6;
	goto Cursor49;

Cursor46:
	R8 = R8 +1;
	if (R8 != R2)
	{
		goto Cursor49;
	}
	*(unsigned short*)R4.A =R6;
	R4.I = R4.I +404;
	*(unsigned short*)R4.A =R6;

Cursor49:
	R3 = R3 +1;
	if (R3 != 10)
		goto Cursor40;

return;
}

// R0:pDat, R1:pTab, R2:Col, R3:Tmp, R4:pBuf, R5:WR , R6:LCD, R7:Ctrl, R8:RS
// void Send_LCD(u16* pBuf, u16 Row) // �ӻ���������һ�����ݵ�LCD Used: R3-R8
void Send_LCD(short* pTab)
{
	R5 = (unsigned)&AA[204];

	LCD_Set_Posi(R2, WINDOW_Y0);
	R4.I =203;

Send_LCD_Loop:
	LCD_Set_Pixel( *(unsigned short*)R5);
	R5 = R5+2;
	R4.I = R4.I - 1;
	if(R4.I !=0 )
		goto Send_LCD_Loop;

	return;
}

// Draw_Pop(R2:Col, R10:pPop, R11:By0, R12:By1)// ���������� Used: R5-R7
void Draw_Pop(short* pTab)
{
	R7 = 0x10001;
	R5 = pTab[36];
	R6 = pTab[37];

	R3 = R11;
	if( R2 == R3 )
	{
		R5 = R5 +6;
		R6 = R6 - 12;
		R7 = 0X80008;
	}

	R3 = R11 + 1;
	if( R2 == R3 )
	{
		R5 = R5 +4;
		R6 = R6 - 8;
		R7 = 0X40004;
	}

	R3 = R11 + 2;
	if( R2 == R3 )
	{
		R5 = R5 +2;
		R6 = R6 - 4;
		R7 = 0X20002;
	}

	R3 = R12 -3;
	if( R2 == R3 )
	{
		R5 = R5 +2;
		R6 = R6 - 4;
		R7 = 0X20002;
	}

	R3 = R12 -2;
	if( R2 == R3 )
	{
		R5 = R5 +4;
		R6 = R6 - 8;
		R7 = 0X40004;
	}

	R3 = R12 -1;
	if( R2 == R3 )
	{
		R5 = R5 +6;
		R6 = R6 - 12;
		R7 = 0X80008;
	}

//    ADD     R4,  SP, R5
	R4.A = (char*) &AA[204-R5/2];
	R8 = pTab[39];
	R9 = pTab[40];
	R3 =  *(unsigned short*) R10;
	R10= R10+2;

	if(R8 == 0)
	{
		goto Pop_Loop_Penetrate;
	}

Pop_Loop:
	if((R3 & R7) != 0)
		R9 = *R4.A;
	else
		R8 = *R4.A;
	R4.A = R4.A + 1;
	R6 = R6 - 2;
	if(R6 == 0 )
		return;

	if((R7 & 0x80000000) != 0 )
	{
		R3 = *(unsigned short*) R10;
		R10= R10+2;
		R7 = 0x10001;
	}
	else
		R7 = R7 << 1;
	goto Pop_Loop;

Pop_Loop_Penetrate:
	if((R2 & R7) != 0)
		R9 = *R4.A;
	R4.A = R4.A + 1;
	R6 = R6 - 2;
	if(R6 == 0 )
		return;

	if((R7 & 0x80000000) != 0 )
	{
		R3 = *(unsigned short*) R10;
		R10= R10+2;
		R7 = 0x10001;
		}
	else
		R7 = R7 << 1;
	goto Pop_Loop_Penetrate;

}

// R0:pDat, R1:pTab, R2:Col, R3:Var, R4:pCol, R5:Cnt, R6:Tmp,
// R0:pBuf, R1:pTab, R2:Col, R3:LCD, R4:Ctrl, R5:WR,  R6:Tmp, R7:Cnt, R8:RS
void DrawWindow(char* pBuf, short* pTab)
{
	R0 = (unsigned)&pBuf[0];
	R2 = 0;
	R10 = &pBuf[900];
	R11 = pTab[34];
	R12 = pTab[35];
	R12 = R11 + R12;

Draw_Loop:
	if (R2 == 0)
	{
		Buld_0();
		goto Draw_Wave;
	}

	if (R2 == 302)
	{
		Buld_0();
		goto Draw_Wave;
	}

	if (R2 == 1)
	{
		Buld_1();
		goto Draw_Wave;
	}
	R3 = 301;

	if (R2 == R3)
	{
		Buld_1();
		goto Draw_Wave;
	}

	R3 = R2 - 1;
	R6 = 25;
	R5 = R3/R6;
	R5 = R5*R6;
	R5 = R3 - R5;

	if (R5 == 0)
	{
		Buld_4();
		goto Draw_Wave;
	}

	R6 = 5;
	R5 = R3/R6;
	R5 = R5*R6;
	R5 = R3 - R5;

	if (R5 == 0)
	{
		Buld_3();
		goto Draw_Wave;
	}
	Buld_2();

Draw_Wave:
	if( (R2<3) || (R2 >= 300) ) //3 to 299
	{
		goto Horozontal;
	}
	R3 = pTab[3]; //Ch_3
	if((R3 & 4) == 0)
	{
		R3 = 46;
		R4.I = pTab[13];
		R8 = 8;
		Draw_Digit_Analog(1);
	}

	R3 =pTab[2]; //Ch_2
	if((R3 & 4) == 0)
	{
		R3 = 44;
		R4.I = pTab[12];
		R8 = 4;
		Draw_Digit_Analog(1);
	}

	R3 =pTab[1]; //Ch_1
	if((R3 & 4) == 0)
	{
		R3 = 42;
		R4.A = &pBuf[300];
		Draw_Digit_Analog(0);
	}

	R3 =pTab[0]; //Ch_0
	if((R3 & 4) == 0)
	{
		R3 = 40;
		R4.A = pBuf;
		Draw_Digit_Analog(0);
	}

Horozontal:
	if(R2 == 0)
	{
		Cursor_0(pTab);
		goto Vertical;
	}

	if(R2 == 302)
	{
		Cursor_0(pTab);
		goto Vertical;
	}

	if(R2 == 1)
	{
		Cursor_1(pTab);
		goto Vertical;
	}
	R5 = 301;
	if(R2 == R5)
	{
		Cursor_1(pTab);
		goto Vertical;
	}

	if(R2 == 2)
	{
		Cursor_2(pTab);
		goto Vertical;
	}
	if(R2 == 300)
	{
		Cursor_2(pTab);
		goto Vertical;
	}
	Cursor_3(pTab);

Vertical:
	Cursor_4(pTab);

	R3 = pTab[38];
	if( (R3&1) !=1 )
	{
		goto Send;
	}

	if(R2 < R11)
	{
		goto Send;
	}

	if(R2 < R12)
	{
		Draw_Pop(pTab);
	}

Send:
	Send_LCD(pTab);

	if( R2 != 302 )
	{
		R2 =R2 +1;
		goto Draw_Loop;
	}

	return;
}

