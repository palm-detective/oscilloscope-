/*
 * Draw.h
 *
 *  Created on: May 6, 2021
 *      Author: paul_
 */

#ifndef INC_DRAW_H_
#define INC_DRAW_H_

#define PRN        0x00
#define INV        0x01
#define SYMB       0x10
#define VOID       0x20

//colors in R5G6B5 format
#define CYN        0xFFE0  // 0
#define PUR        0xF81F  // 1
#define YEL        0x07FF  // 2
#define GRN        0x07E0  // 3
#define CYN_       0x8400  // 4
#define PUR_       0x8010  // 5
#define YEL_       0x0410  // 6
#define GRN_       0x0410  // 7

#define ORN        0x051F  // 8
#define BLK        0x0000  // 9
#define WHT        0xFFFF  // 10
#define BLU        0xFC10  // 11

#define RED        0x001F  // 12
#define GRY        0x39E7  //0x7BEF  // 13
#define LGN        0x27E4  // 14
#define DAR        0x39E7  // 15

#define SHOW       0       // All Show
#define D_HID      1       // End Dot & Line Hide
#define L_HID      2       // Only Line Hide
#define W_HID      4       // Wave Hide
#define A_HID      7       // End Dot & Line & Wave Hide
#define P_HID      1       // Pop Hide

//=========== ���ڲ������б�־λ�� ===========

#define P1F        0
#define P2F        1
#define P3F        2
#define P4F        3
#define VTF        4
#define V1F        5
#define V2F        6
#define T0F        7
#define T1F        8
#define T2F        9

//=========== ���ڲ������б���λ�� ============
#define P1x2       10
#define P2x2       11
#define P3x2       12
#define P4x2       13
#define VTx2       14
#define V1x2       15
#define V2x2       16
#define T0x1       17
#define T1x1       18
#define T2x1       19

//=========== ���ڲ���������ɫλ�� ============
#define PC1        20
#define PC2        21
#define PC3        22
#define PC4        23
#define VTC        24
#define V1C        25
#define V2C        26
#define T0C        27
#define T1C        28
#define T2C        29
#define PC1_       30
#define PC2_       31
#define PC3_       32
#define PC4_       33

//========= ���ڲ������е���������λ�� =========

#define BXx1       34           // Pop x0 Color
#define WXx1       35           // Pop Width
#define BYx2       36           // Pop y0 *2
#define HYx2       37           // Pop Hight *2
#define POP_F      38           // Pop Flag
#define POP_B      39           // Pop Board Color
#define POP_T      40           // Pop Text Color
#define LIST_POP   2       //
#define DAILOG_POP 4       //

#define X_SIZE     300
#define POP_SIZE        1600
#define TRACK1_SIZE     300
#define TRACK2_SIZE     300
#define TRACK23_SIZE    300

extern const char DEMO[];
extern char Data[], WaveBuf[], PopType;
extern short ParamTab[];
extern unsigned short Background;

void DispChar8x14(char Mode, char Code);

void DispStr8x14(short x0, short y0, char Mode, char *Str);

void SetColor(unsigned short Board_Color, unsigned short Text_Color);

void DispStr(short x0, short y0, char Mode, char *Str);

void Draw_Circle_D(char Mode, short Posi_x, short Posi_y, short High, short Width, short Distance);

void DispChar10x14(char Mode, char Code);

void DispStr10x14(short x0, short y0, char Mode, char *Str);

void List_Pop(void);

void Close_Pop(void);

void Dialog_Pop(char* Str);

void Calibrate_Pop(char* Str);

void CalSave_Pop(char* Str);

void NotePixel(char Color);

void Note_4Point(char Mode, short x, short y, char Str_Cnt, char High);

void NoteSTR(short Nx0, short Ny0, char Mode, char *Str);

void Buil_Pop(void);

void PopColor(unsigned short Board_Color, unsigned short Text_Color);

#endif /* INC_DRAW_H_ */
