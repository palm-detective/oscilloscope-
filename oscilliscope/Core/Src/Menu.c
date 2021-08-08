/*
 * Menu.c
 *
 *  Created on: May 30, 2021
 *      Author: paul_
 */

#include "stm32f4xx_hal.h"

#include "Menu.h"
#include "Draw.h"
#include "Func.h"
#include "Bios.h"
#include "Display.h"
#include "Process.h"
#include "Lcd_ctl.h"
#include "Touch_ctl.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>

short  StdBy_Time = 0;
short  Current = 0, Status = 0;
char   Freq_Flag = 1, StdBy_Flag = 0;
short  Freq_Cnt  = 0;
char   Windows_Pop = 0;

const char  A[][6] = {"_"   , "@"};
const char  C[][6] = {".BMP", ".DAT" , ".BUF" , ".CSV" , ".SVG" , ".DAT" , ".BUF"};
const char  D[][6] = {"1K"  , "2K"   , "4K"   , "8K"};
const char  E[][6] = {"H"   , "L"};
const char  F[][6] = {"Hz"  , "Hz"   , "K "   , "M "};
const char  I[][6] = {"Data ", "-Data", "Inp+D", "D-Inp", "Inp-D", "-Inp ", "Hide "};
const char  M[][6] = {"STOP", "SCAN" , "AUTO" , "NORM" , "SNGL"};
const char  N[][6] = {"1"   , "10"};
const char  P[][6] = {"%"};
const char  T[][6] = {"uS"  , "uS"   , "mS"   , "S "};
const char  V[][6] = {"uV"  , "mV"   , "V "   , "kV"};
const char  Z[][6] = {"Freq", "Duty" , "Vrms" , "Vavg" , "Vp_p" , "Vmax" , "Vmin"};
const char  CP[][6] ={" DC" , " AC"};
const char  OT[][6] ={"SQU  ", "SIN  " ,  "TRI  ", "SAW  "};

char  TimeBase_Str[][8]  ={"1.0uS","2.0uS","5.0uS","10uS ","20uS ","50uS ",
                          "0.1mS","0.2mS","0.5mS","1.0mS",
                          "2.0mS","5.0mS","10mS ","20mS ","50mS ","0.1S ",
                          "0.2S ","0.5S ","1.0S ","2.0S "};

const int VScale[]= { 10,20, 50, 100, 200, 500, 1000, 2000, 5000,  10000};
long  VScale_10X[]= { 100, 200, 500, 1000, 2000, 5000,  10000,20000,50000, 100000,};
const short Dac_Tim[12] = {10000,10000,4000,2000,1000, 400, 200, 100,  40,  20,  10};

//----------+------+------+------+------+------+------+------+------+------+------+
// Attnx1   |   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |
// Attnx10  |  10  |  11  |  12  |  13  |  14  |  15  |  16  |  17  |  18  |  19  |
//----------+------+------+------+------+------+------+------+------+------+------+
const char  R[][6]={"10mV","20mV","50mV","0.1V","0.2V","0.5V","1.0V","2.0V","5.0V"," 10V",
             "0.1V","0.2V","0.5V","1.0V","2.0V","5.0V"," 10V"," 20V"," 50V","100V" };

//----------+------+------+------+------+------+------+------+------+------+------+
// Timebase |   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |
//          |  10  |  11  |  12  |  13  |  14  |  15  |  16  |  17  |  18  |  19  |
//----------+------+------+------+------+------+------+------+------+------+------+
const char  B[][6]={"1uS " ,"2uS " ,"5uS " ,"10uS","20uS","50uS",".1mS",".2mS",".5mS","1mS " ,
             "2mS " ,"5mS " ,"10mS","20mS","50mS","0.1S","0.2S","0.5S"," 1S "," 2S ",};
//-----------+-------+-------+-------+-------+-------+-------+-------+-------+
// Frequency |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |
//           |   8   |   9   |  10   |  11   |  12   |  13   |  14   |  15   |
//-----------+-------+-------+-------+-------+-------+-------+-------+-------+
const char  Q[][6]= { "10Hz", "20Hz", "50Hz","100Hz","200Hz","500Hz","1KHz ","2KHz",
               "5KHz","10KHz","20KHz","50KHz",".1MHz",".2MHz",".5MHz","1MHz"};
//-
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
// ��Ŀ����  |    ESTR     |    CSTR        | S_X    |  PTR  | Val | MAX   | MIN   | Flg |
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
item List[]={{/* "Y Ranges",*//*"����ѡ��"*/ /* 56 ,*/(char *)R ,   7 , /*  9 ,   0 ,*/ UPD },
             {/* "Coupling",*//*"��Ϸ�ʽ"*/ /* 90 ,*/(char *)CP ,  1 , /*  1 ,   0 ,*/ UPD },
             {/* "ProbeAtt",*//*"̽ͷģʽ"*/ /* 44 ,*/(char *)N ,   0 , /*  1 ,   0 ,*/ UPD },
             {/* "Y Offset",*//*"��ֱƫ��"*/ /*120 ,*/     0 , 100 , /*198 ,   2 ,*/ UPD },
             {/* "CursorV1",*//*"��λ�α�"*/ /*  0 ,*/(char *)V , 150 , /*198 ,   2 ,*/ UPD },
             {/* "CursorV2",*//*"��λ�α�"*/ /*  0 ,*/(char *)V ,  25 , /*198 ,   2 ,*/ UPD },
             {/* "CursorTp",*//*"�α���ʽ"*/ /*  0 ,*/     0 ,   0 , /*  1 ,   0 ,*/ UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "TimeBase",*//*"ʱ��ѡ��"*/ /*132 ,*/(char *)B ,   7 ,/*  19 ,   0 ,*/ UPD },//
             {/* "ViewPosi",*//*"ˮƽλ��"*/ /*100 ,*/     0 ,   1 ,/*8192 ,   1 ,*/ UPD },
             {/* "SmplDpth",*//*"�������"*/ /*232 ,*/(char *)D ,   2 ,/*   3 ,   0 ,*/   0 },
             {/* "CursorT1",*//*"����α�"*/ /*  0 ,*/(char *)T ,  50 ,/* 298 ,   2 ,*/ UPD },
             {/* "CursorT2",*//*"�Ҷ��α�"*/ /*  0 ,*/(char *)T , 250 ,/* 298 ,   2 ,*/ UPD },
             {/* "CursorTp",*//*"�α���ʽ"*/ /*  0 ,*/     0 ,   0 ,/*   1 ,   0 ,*/ UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*"SyncMode"*//*"ͬ��ģʽ"*//*   0 ,*/(char *)M ,   2 , /*  4 ,   1 ,*/ UPD },
             { /*"TrigMode"*//*"��������"*//* 228 ,*/(char *)E ,   0 , /*  1 ,   0 ,*/ UPD },
             { /*"Auto Fit"*//*"�Զ���ֵ"*//* 218 ,*/(char *)A ,   0 , /*  1 ,   0 ,*/ UPD },
             { /*"Threshol"*//*"������ֵ"*//*   0 ,*/     0 ,  15 , /* 98 ,  -98,*/ UPD },      //198
             { /*"Sensitiv"*//*"�����ز�"*//* 238 ,*/     0 ,   5 , /* 98 ,   1 ,*/ UPD },
             { /*"CursorTp"*//*"�α���ʽ"*//*   0 ,*/     0 ,   0 , /*  1 ,   0 ,*/ UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*  " Freq ",*//*  "Ƶ��ֵ"*//* 232 ,*/(char *)F ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Duty ",*//*  "ռ�ձ�"*//* 232 ,*/(char *)P ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vrms ",*//*  "��Чֵ"*//* 232 ,*/(char *)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vavg ",*//*  "ƽ��ֵ"*//* 232 ,*/(char *)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vp-p ",*//*  "���ֵ"*//* 232 ,*/(char *)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vmax ",*//*  "���ֵ"*//* 232 ,*/(char *)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vmin ",*//*  "��Сֵ"*//* 232 ,*/(char *)V ,   0 ,/* 100 ,   0 , */  0 },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*"Ext Refn",*//*"�ο�ģʽ"*/ /*172 ,*/(char *)I,  0 , /*  5 ,   0 , */UPD },
             { /*"Ext Posi",*//*"��ֱƫ��"*/ /*  0 ,*/     0 , 80 , /*199 ,   1 , */UPD },
             { /*"Ext Hide",*//*"�ο�����"*/ /*  0 ,*/(char *)I,   0 , /*  1 ,   0,  */UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*"Save Bmp",*//*"����ͼ��"*//* 232 ,*/(char *)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Dat",*//*"���沨��"*//* 232 ,*/(char *)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Buf",*//*"��������"*//* 232 ,*/(char *)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Csv",*//*"��������"*//* 232 ,*/(char *)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Svg",*//*"��������"*//* 232 ,*/(char *)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Load Dat",*//*"��ȡ����"*//* 232 ,*/(char *)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Load Buf",*//*"��ȡ����"*//* 232 ,*/(char *)C ,   0 , /*255 ,   0 , */  0 },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "Out Tpye",*//*"�������"*/ /*  0 ,*/      0,   0 , /*  1,    0 , */0 },
             {/* "Out Freq",*//*"���Ƶ��"*/ /*  0 ,*/(char *)Q ,   6 , /* 15 ,   0 , */UPD },
             {/* "Out Duty",*//*"���ռ��"*/ /*  0 ,*/(char *)P ,   5 , /*  9 ,   1 , */UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "AutoCal ",*//*"����У��"*/ /* 80 , */    0 ,   0 ,/* 99 ,  0 , */  0   },
             {/* "Restore ",*//*"��������"*/ /* 80 , */    0 ,   0 ,/* 99 ,  0 , */  0   },
             {/* "Standby ",*//*"����ʱ��"*/ /* 0 , */     0 ,  10 ,/* 99 ,  0 , */  UPD }};
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+

//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
// ��Ŀ����  |    ESTR     |    CSTR        | S_X    |  PTR  | Val | MAX   | MIN   | Flg |
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
item_c List_c[]={
             {/* "Y Ranges",*//*"����ѡ��"*/ /* 56 ,*/(const char*)R ,   7 , /*  9 ,   0 ,*/ UPD },
             {/* "Coupling",*//*"��Ϸ�ʽ"*/ /* 90 ,*/(const char*)CP ,   1 , /*  1 ,   0 ,*/ UPD },
             {/* "ProbeAtt",*//*"̽ͷģʽ"*/ /* 44 ,*/(const char*)N ,   0 , /*  1 ,   0 ,*/ UPD },
             {/* "Y Offset",*//*"��ֱƫ��"*/ /*120 ,*/     0 , 100 , /*198 ,   2 ,*/ UPD },
             {/* "CursorV1",*//*"��λ�α�"*/ /*  0 ,*/(const char*)V , 150 , /*198 ,   2 ,*/ UPD },
             {/* "CursorV2",*//*"��λ�α�"*/ /*  0 ,*/(const char*)V ,  25 , /*198 ,   2 ,*/ UPD },
             {/* "CursorTp",*//*"�α���ʽ"*/ /*  0 ,*/     0 ,   0 , /*  1 ,   0 ,*/ UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "TimeBase",*//*"ʱ��ѡ��"*/ /*132 ,*/(const char*)B ,   7 ,/*  19 ,   0 ,*/ UPD },//
             {/* "ViewPosi",*//*"ˮƽλ��"*/ /*100 ,*/     0 ,   1 ,/*8192 ,   1 ,*/ UPD },
             {/* "SmplDpth",*//*"�������"*/ /*232 ,*/(const char*)D ,   2 ,/*   3 ,   0 ,*/   0 },
             {/* "CursorT1",*//*"����α�"*/ /*  0 ,*/(const char*)T ,  50 ,/* 298 ,   2 ,*/ UPD },
             {/* "CursorT2",*//*"�Ҷ��α�"*/ /*  0 ,*/(const char*)T , 250 ,/* 298 ,   2 ,*/ UPD },
             {/* "CursorTp",*//*"�α���ʽ"*/ /*  0 ,*/     0 ,   0 ,/*   1 ,   0 ,*/ UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*"SyncMode"*//*"ͬ��ģʽ"*//*   0 ,*/(const char*)M ,   2 , /*  4 ,   1 ,*/ UPD },
             { /*"TrigMode"*//*"��������"*//* 228 ,*/(const char*)E ,   0 , /*  1 ,   0 ,*/ UPD },
             { /*"Auto Fit"*//*"�Զ���ֵ"*//* 218 ,*/(const char*)A ,   0 , /*  1 ,   0 ,*/ UPD },
             { /*"Threshol"*//*"������ֵ"*//*   0 ,*/     0 ,  15 , /* 98 ,  -98,*/ UPD },      //198
             { /*"Sensitiv"*//*"�����ز�"*//* 238 ,*/     0 ,   5 , /* 98 ,   1 ,*/ UPD },
             { /*"CursorTp"*//*"�α���ʽ"*//*   0 ,*/     0 ,   0 , /*  1 ,   0 ,*/ UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*  " Freq ",*//*  "Ƶ��ֵ"*//* 232 ,*/(const char*)F ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Duty ",*//*  "ռ�ձ�"*//* 232 ,*/(const char*)P ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vrms ",*//*  "��Чֵ"*//* 232 ,*/(const char*)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vavg ",*//*  "ƽ��ֵ"*//* 232 ,*/(const char*)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vp-p ",*//*  "���ֵ"*//* 232 ,*/(const char*)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vmax ",*//*  "���ֵ"*//* 232 ,*/(const char*)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vmin ",*//*  "��Сֵ"*//* 232 ,*/(const char*)V ,   0 ,/* 100 ,   0 , */  0 },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*"Ext Refn",*//*"�ο�ģʽ"*/ /*172 ,*/(const char*)I,   0 , /*  5 ,   0 , */UPD },
             { /*"Ext Posi",*//*"��ֱƫ��"*/ /*  0 ,*/     0 , 80 , /*199 ,   1 , */UPD },
             { /*"Ext Hide",*//*"�ο�����"*/ /*  0 ,*/(const char*)I,   0 , /*  1 ,   0,  */UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*"Save Bmp",*//*"����ͼ��"*//* 232 ,*/(const char*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Dat",*//*"���沨��"*//* 232 ,*/(const char*)C ,   0 , /*/(const char*)C5 ,   0 , */  0 },
             { /*"Save Buf",*//*"��������"*//* 232 ,*/(const char*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Csv",*//*"��������"*//* 232 ,*/(const char*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Svg",*//*"��������"*//* 232 ,*/(const char*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Load Dat",*//*"��ȡ����"*//* 232 ,*/(const char*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Load Buf",*//*"��ȡ����"*//* 232 ,*/(const char*)C ,   0 , /*255 ,   0 , */  0 },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "Out Tpye",*//*"�������"*/ /*  0 ,*/      0,   0 , /*  1,    0 , */0 },
             {/* "Out Freq",*//*"���Ƶ��"*/ /*  0 ,*/(const char*)Q ,   6 , /* 15 ,   0 , */UPD },
             {/* "Out Duty",*//*"���ռ��"*/ /*  0 ,*/(const char*)P ,   5 , /*  9 ,   1 , */UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "AutoCal ",*//*"����У��"*/ /* 80 , */    0 ,   0 ,/* 99 ,  0 , */  0   },
             {/* "Restore ",*//*"��������"*/ /* 80 , */    0 ,   0 ,/* 99 ,  0 , */  0   },
             {/* "Standby ",*//*"����ʱ��"*/ /* 0 , */     0 ,  10 ,/* 99 ,  0 , */  UPD }};
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+

//--------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
short               MenuNow[]={   0 ,   7 ,  13 ,  19+1 ,  26 ,  29 ,  36 ,  39 }; //          |
const short       MenuNow_C[]={ 0 ,   7 ,  13 ,  19+1 ,  26 ,  29 ,  36 ,  39 };//     |
const unsigned short TCOLOR[]={ CYN , YEL , GRN , WHT , PUR , BLU , ORN , CYN };//ֵ   |
const char          TSTR[][4]={"Yn" ,"Xn" ,"Tr" ,"Me" ,"Ex" ,"Fn" ,"Sn" ,"St" };//   |
const char          TITLE_Y[]={ 200 , 175 , 150 , 125 , 100 ,  75 ,  50 ,  25 };//  |
const char          MU_BEGN[]={   0 ,   7 ,  13 ,  19 ,  26 ,  29 ,  36 ,  39 };//������ʼ��� |
const char          M_ITEMS[]={   7 ,   6 ,   6 ,   7 ,   3 ,   7 ,   3 ,   5 };//�˵���Ŀ���� |

//----��Ŀ��Ӣ���ַ���----------------------------------------------------------------------------------//
const char ESTR[][10]={" Y Ranges ",  " Coupling ",  " ProbeAtt ",  " Y Offset ",  " CursorV1 ",  " CursorV2 ",  " Hide_Vn_ ",
                " TimeBase ",  " ViewPosi ",  " SmplDpth ",  " CursorT1 ",  " CursorT2 ",  " Hide_Tn_ ",
                " SyncMode ",  " TrigMode ",  " Auto Fit ",  " Threshol ",  " Sensitiv ",  " Hide-Tri ",
                " Freq "  ,    " Duty "  ,    " Vrms "  ,    " Vavg "  ,    " Vp-p "  ,    " Vmax "  ,    " Vmin "  ,
                " Ext Refn ",  " Ext Posi ",  " Ext Hide ",
                " Save Bmp ",  " Save Dat ",  " Save Buf ",  " Save Csv ",  " Save Svg ",  " Load Dat ",  " Load Buf ",
                " Out Tpye ",  " Out Freq ",  " Out Duty ",
                " Auto Cal ",  " Restore  ",  " Standby  "};

//-------------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
const char T_SCALE[]={ 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7};
//-------------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
const char V_SCALE[]={ 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6};
//-------------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
const char B_SCALE[]={ 4, 8, 2, 4, 8, 2, 4, 8, 2, 4, 8, 2, 4, 8, 2, 4, 8, 2, 4, 8};
const char X_SCALE[]={ 4, 8, 2, 4, 8, 2, 4, 8, 2, 4, 4, 8, 2, 4, 8, 2, 4, 8, 2, 4};
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
const char BLOCK_X[]={ 220 , 220 , 220 , 230 , 220 , 220 , 220 , 220 };//�˵���X����  |
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
const char BLOCK_Y[]={  70 ,  70 ,  67 ,  30 ,  62 ,  19 ,  11 ,   3 };//�˵���Y����  |
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
const char BLOCK_W[]={  70 ,  70 ,  70 ,  44 ,  70 ,  70 ,  70 ,  70 };//�˵������   |

//--------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
const short S_X[]={	47  , 47+24 ,32  ,120 , 7  , 7  ,  0 ,
                112 ,100 ,248 ,  7 , 7  , 0  ,
                289   ,221 ,207 ,  0 ,231 , 0  ,
                162 ,162 ,162 ,162 ,162 ,162 ,162 ,
                168 , 0  ,168 ,
                248 ,248 ,248 ,248 ,248 ,248 ,248 ,
                7   , 7  ,  7 ,
                7   , 7  ,  7 ,   7,   7,
                8};
//----��Ŀ������ֵ����--------------------------------------------------------------------------------//
const short MAX[]=	{ 9   ,1  ,1    ,198, 198, 198, 1 ,
                  19  ,8000 ,3  , 298, 298, 1 ,
                  4   ,1  ,1    ,98 , 98 , 1,
                  100 ,100,100  ,100, 100, 100,100,
                  5   ,199, 1,
                  235 ,235,235  ,235,235,235, 235,
                  3,  15  ,9  ,
                  10 ,99,99, 60,60,
                  4};
//----��Ŀ������ֵ����--------------------------------------------------------------------------------//
const short MIN[]=	{ 1,0,0,2,2,2,0,
                  0, 1, 0, 2,2,0,
                  1,0,0,-98,1,0,
                  0,0,0,0,0,0,0,
                  0,1,0,
                  0,0,0,0,0,0,0,
                  0,0,1,
                  1,0,0, 0, 0,
                  0};

char NumStr[11];
char NSTR[][6] = {"  ", "  ", "  "};

const short DEPTH[]={ 1024 , 2048 , 4096 , 8192};//8192
//----------+------+------+------+------+------+------+------+------+------+------+
const short KS[]  ={  51  ,  102 ,  256 ,  512 , 1024 , 1024 , 1024 , 1024 , 1024 , 1024 ,
            /*��ֵϵ�� */ 1024 , 1024 , 1024 , 1024 , 1024 , 1024 , 1024 , 1024 , 1024 , 1024 };
//----------+------+------+------+------+------+------+------+------+------+------+

//                0-us                       9--ms                     18--1s
const int Ts[] = {25,25,25,25,25,50,100,200,500,1,2,5,10,20,50,100,200,500,1,2};
short Presampling = X_SIZE/2;
//----------------+------------------------+-----------------+
// ��Ŀ��ʾ�ַ��� |           ESTR         |       CSTR      |
//----------------+------------------------+-----------------+
const char FnNote[][14]= {"File R/W OK",          /*"�ļ��������"   */
                   "File Ver Er",          /*"�ļ��汾����"   */
                   "Not Found! ",          /*"�ļ���������"   */
                   "File R/W Er",          /*"�ļ���������"   */
                   "Disk Busy! ",          /*"���̶�д����"   */};
//----------------+------------------------+-----------------+

const short Dac_Psc[12] = {4,    2,    2,   2,   2,    2,   2,   2,    2,   2,   2};

//------------------------------------------------------------------------------------

/*******************************************************************************
Draw_4Point:
*******************************************************************************/
void Draw_4Point(short x, short y, char Str_Cnt, char High)
{
  LCD_Set_Posi(x,y);
  LCD_Set_Pixel(DAR);
  LCD_Set_Posi(x,y+High-1);
  LCD_Set_Pixel(DAR);
  LCD_Set_Posi(x+Str_Cnt*((High>10)? 8:6),y);
  LCD_Set_Pixel(DAR);
  LCD_Set_Posi(x+Str_Cnt*((High>10)? 8:6),y+High-1);
  LCD_Set_Pixel(DAR);
}

/*******************************************************************************
 Update_Item:
*******************************************************************************/
void Update_Title(void)
{
  short i;

  for(i=0; i<8; i++)
  {
    SetColor(GRY, TCOLOR[i]);
    DispStr8x14(T_X-2, TITLE_Y[i], (Current == i)? INV : PRN, (char*)TSTR[i]);
    if(Current == i)
    {
        Draw_4Point(T_X-2, TITLE_Y[i], 2, Font8x14_H);
    }
  }
}

/*******************************************************************************
 Update_Blink:
*******************************************************************************/
void Update_Blink(void)
{
  char* ptr;
  long Tmp;
  unsigned short i, Range = List[Y_RANGES].Val + 10*List[PROBEATT].Val;

  i = MenuNow[Current];
  ptr = (char*)(List[i].PTR +(List[i].Val)*6);
  if(Blink == BLINK)
  {
    Blink = 0;
    List[MenuNow[ME]].Flg |= UPD; //
    switch(i)
    {
    case SAVE_BMP:
    case SAVE_DAT:
    case SAVE_BUF:
    case SAVE_CSV:
    case SAVE_SVG:
    case LOAD_DAT:
    case LOAD_BUF:
      SetColor(GRY, TCOLOR[FN]);
      u8ToDec3(NumStr, List[i].Val);
      Draw_Circle_D(Twink, S_X[i]-3, LABLE_Y, 11, 0, 6*11);
      DispStr(S_X[i],    LABLE_Y, Twink, ((i > SAVE_SVG)? "Load" : "Save"));//0513
      DispStr(S_X[i]+24, LABLE_Y, Twink, (char *)NumStr);
      DispStr(S_X[i]+42, LABLE_Y, Twink, (List[i].PTR +(i - SAVE_BMP)*6));
      break;

    case Y_RANGES:
    case COUPLING:
    case PROBEATT:
      SetColor(GRY, TCOLOR[YN]);
      Draw_Circle_D(Twink, S_X[Y_RANGES]-3, TITLE_Y_POS, 11, 0, 6*7);
      DispStr(S_X[Y_RANGES], TITLE_Y_POS, Twink, (List[Y_RANGES].PTR +(Range)*6));
      DispStr(S_X[COUPLING], TITLE_Y_POS, Twink, (List[COUPLING].PTR +(List[COUPLING].Val)*6));
      DispStr10x14(S_X[PROBEATT], TITLE_Y_POS, PRN+SYMB,(List[PROBEATT].Val?("A"):("B")));
      break;

    case V0_POSI:
      if(Twink) ParamTab[P1F] |=  D_HID;
      else      ParamTab[P1F] &= ~D_HID;
      break;

    case V1_POSI:
    case V2_POSI:
      Tmp = (i == V1_POSI)? V1F : V2F;
      if(Twink) ParamTab[Tmp] |=  D_HID;
      else      ParamTab[Tmp] &= ~D_HID;
      Tmp  =(ParamTab[V1x2]- ParamTab[V2x2])/2;         // V2 - V1 ��ֵ
      Tmp *= X_SCALE[Range]* Exp(V_SCALE[Range]);
      Value2Str(NumStr, Tmp, V[0], 3, UNSIGN);
      SetColor(GRY, TCOLOR[YN]);
      Draw_Circle_D(Twink, S_X[V1_POSI]-3, LABLE_Y, 11, 0, 6*9);
      DispStr(S_X[V1_POSI]+0*6, LABLE_Y, Twink+SYMB, "]");
      DispStr(S_X[V1_POSI]+1*6, LABLE_Y, Twink,      "V:      ");
      DispStr(S_X[V1_POSI]+3*6, LABLE_Y, Twink, (char *)NumStr);
      break;

    case YN_TYPE:
      if(Twink) ParamTab[V1F] |=  D_HID;
      else      ParamTab[V1F] &= ~D_HID;
      if(Twink) ParamTab[V2F] |=  D_HID;
      else      ParamTab[V2F] &= ~D_HID;
      break;

    case TIM_BASE:
      SetColor(GRY, TCOLOR[XN]);
      Draw_Circle_D(Twink, S_X[i]-3, TITLE_Y_POS, 11, 2, 6*5);
      DispStr(S_X[i], TITLE_Y_POS, Twink, TimeBase_Str[List[TIM_BASE].Val]);
      break;

    case VIEWPOSI:
      if(Twink) ParamTab[T0F] |=  D_HID;
      else      ParamTab[T0F] &= ~D_HID;
      break;

    case SMPL_DPTH:
      SetColor(GRY, TCOLOR[XN]);
      Draw_Circle_D(Twink, S_X[SMPL_DPTH]-3, LABLE_Y, 11, 0, 6*11);
      DispStr(S_X[SMPL_DPTH],    LABLE_Y,  Twink, "SmplDpth:  ");
      DispStr(S_X[SMPL_DPTH]+6*9, LABLE_Y,  Twink, (char *)&D[List[SMPL_DPTH].Val]);
      break;

    case T1_POSI:
    case T2_POSI:
      Tmp = (i == T1_POSI)? T1F : T2F;
      if(Twink) ParamTab[Tmp] |=  D_HID;
      else      ParamTab[Tmp] &= ~D_HID;
      Tmp  = ParamTab[T2x1] - ParamTab[T1x1];          // T2 - T1 ��ֵ
      Tmp *= B_SCALE[List[TIM_BASE].Val]* Exp(T_SCALE[List[TIM_BASE].Val]);
      if(Tmp>=0)
        Value2Str(NumStr, Tmp, T[0], 3, UNSIGN);
      else
        Value2Str(NumStr, Tmp, T[0], 3, SIGN);
      SetColor(GRY, TCOLOR[XN]);
      Draw_Circle_D(Twink, S_X[T1_POSI]-3, LABLE_Y, 11, 0, 6*9);
      DispStr(S_X[T1_POSI]+0*6, LABLE_Y, Twink+SYMB, "]");
      DispStr(S_X[T1_POSI]+1*6, LABLE_Y, Twink,      "T:      ");
      DispStr(S_X[T1_POSI]+3*6, LABLE_Y, Twink,      (char *)NumStr);
      break;

    case XN_TYPE:
      if(Twink) ParamTab[T1F] |=  D_HID;
      else      ParamTab[T1F] &= ~D_HID;
      if(Twink) ParamTab[T2F] |=  D_HID;
      else      ParamTab[T2F] &= ~D_HID;
      break;

    case SYNCMODE:
      if(!(Status &  STOP))
      {
      SetColor(GRY, TCOLOR[TRG]);
      Draw_Circle_D(Twink, S_X[i]-3, TITLE_Y_POS, 11, 1, 6*4);
      DispStr(S_X[i], TITLE_Y_POS, Twink, (char *)ptr);
      }
      break;

    case TR_MODE:
    case SENSITIVE:
      SetColor(GRY, TCOLOR[TRG]);
      Draw_Circle_D(Twink, S_X[TR_MODE]-3, TITLE_Y_POS, 11, 0, 6*8);
      DispStr10x14(S_X[TR_MODE], TITLE_Y_POS, Twink+SYMB,(List[TR_MODE].Val?("D"):("C")));

      Tmp  = List[SENSITIVE].Val;                        // �����ز�ֵ
      Tmp *= X_SCALE[Range]* Exp(V_SCALE[Range]);
      Value2Str(NumStr, Tmp, V[0], 2, UNSIGN);
      DispStr(S_X[SENSITIVE]+9, TITLE_Y_POS, Twink,  "     ");
      DispStr10x14(S_X[SENSITIVE], TITLE_Y_POS, Twink+SYMB,"E");
      DispStr(S_X[SENSITIVE]+9, TITLE_Y_POS, Twink     ,  (char *)NumStr);
      break;

    case FITTRIGG:
      SetColor(GRY, TCOLOR[TRG]);
      DispStr10x14(S_X[FITTRIGG], TITLE_Y_POS, PRN+SYMB,(List[FITTRIGG].Val?("F"):("B")));
      break;

    case THRESHOLD:
      if(Twink) ParamTab[VTF] |=  D_HID;
      else      ParamTab[VTF] &= ~D_HID;
      break;

    case TR_TYPE:
      if(Twink) ParamTab[VTF] |=  D_HID;
      else      ParamTab[VTF] &= ~D_HID;
      break;

    case EXT_REFN:
      SetColor(GRY, TCOLOR[EX]);
      Draw_Circle_D(Twink, S_X[i]-3, TITLE_Y_POS, 11, 1, 6*5);
      DispStr(S_X[i], TITLE_Y_POS, Twink, (char*)(List[EXT_HIDE].Val? I[List[EXT_REFN].Val] : I[6]));
      ParamTab[P2F] &= ~D_HID;
      break;

    case EXT_POSI:
      if(Twink) ParamTab[P2F] |=  D_HID;
      else      ParamTab[P2F] &= ~D_HID;
      break;

    case EXT_HIDE:
      SetColor(GRY, TCOLOR[EX]);
      Draw_Circle_D(Twink, S_X[i]-3, TITLE_Y_POS, 11, 1, 6*5);
      DispStr(S_X[i], TITLE_Y_POS, Twink, (char*)(List[EXT_HIDE].Val? I[List[EXT_REFN].Val] : I[6]));
      ParamTab[P2F] &= ~D_HID;
      break;

    case TPYE_OUT:
      SetColor(GRY, TCOLOR[SN]);
      Draw_Circle_D(Twink, S_X[TPYE_OUT]-3, LABLE_Y, 11, 0, 6*9);
      DispStr(S_X[i],    LABLE_Y, Twink, "FO: ");
      DispStr(S_X[i]+24, LABLE_Y, Twink, "     ");
      DispStr(S_X[i]+24, LABLE_Y, Twink, (char*)OT[List[i].Val]);
      break;

    case FREQ_OUT:
      SetColor(GRY, TCOLOR[SN]);
      Draw_Circle_D(Twink, S_X[FREQ_OUT]-3, LABLE_Y, 11, 0, 6*9);
      DispStr(S_X[i],    LABLE_Y, Twink, "FO: ");
      DispStr(S_X[i]+24, LABLE_Y, Twink, "     ");
      DispStr(S_X[i]+24, LABLE_Y, Twink, (char*)Q[List[i].Val]);
      break;

    case FREQ_DUT:
      SetColor(GRY, TCOLOR[SN]);
      Value2Str(NumStr, List[i].Val*10, List[i].PTR, 2, UNSIGN);
      Draw_Circle_D(Twink, S_X[FREQ_DUT]-3, LABLE_Y, 11, 0, 6*9);
      DispStr(S_X[i],    LABLE_Y, Twink, "Duty: ");
      DispStr(S_X[i]+36, LABLE_Y, Twink, "   ");
      DispStr(S_X[i]+36, LABLE_Y, Twink, (char *)NumStr);
      break;

    case AUTO_CAL:
      if(Twink) ParamTab[P1F] |=  W_HID;
      else      ParamTab[P1F] &= ~W_HID;

      SetColor(GRY, TCOLOR[ST]);
      Draw_Circle_D(Twink, S_X[i]-3, LABLE_Y, 11, 0, 6*9);
      DispStr( S_X[i]+0, LABLE_Y, Twink+PRN,  "Press:   ");
      DispStr(S_X[i]+40, LABLE_Y, Twink+SYMB, "{|");//������ͣͼ��
      break;

    case RESTORE:
      SetColor(GRY, TCOLOR[ST]);
      Draw_Circle_D(Twink, S_X[i]-3, LABLE_Y, 11, 0, 6*9);
      DispStr( S_X[i]+0, LABLE_Y, Twink+PRN,  "Press:   ");
      DispStr( S_X[i]+40, LABLE_Y, Twink+SYMB, "{|");//������ͣͼ��
      break;

    case STANDBY:
      SetColor(GRY, TCOLOR[ST]);
      Value2Str(NumStr, List[STANDBY].Val, NSTR[0], 2, UNSIGN);
      Draw_Circle_D(Twink, S_X[i]-3, LABLE_Y, 11, 0, 6*9);
      DispStr(S_X[i],    LABLE_Y, Twink+PRN,  " ");
      DispStr(S_X[i]+6, LABLE_Y, Twink+PRN,  (char *)NumStr);
      DispStr(S_X[i]+30, LABLE_Y, Twink+PRN,  "min ");
      break;

    }
    if(i != V0_POSI)  ParamTab[P1F]   = L_HID ;//0513
  }
}
/*******************************************************************************
 Update_Detail:
*******************************************************************************/
void Update_Detail(void)
{
  long scale = 0;
  int  Ak = (KgA[KindA+(StateA?1:0)]*4)/GK[GainA];
  unsigned short i, k = HighT+LowT;
  long Tmp = 0;
  short Range = List[Y_RANGES].Val + 10*List[PROBEATT].Val;
  unsigned long FTmp;

  for(i=Y_RANGES; i<=STANDBY; i++){
    if(i==FREQUENC)
    {
      if(Freq_Cnt > 10){
        Freq_Cnt = 0;
        Freq_Flag |= UPD;
      }
      else Freq_Cnt++;
    }

    if(List[i].Flg & UPD){
      List[i].Flg &= ~UPD;
      switch (i){
      case AUTO_CAL:   // ˢ����ʾ����У��
        break;
      case Y_RANGES:   // ˢ����ʾ Y ���̵�λ
      case PROBEATT:   // ˢ����ʾ̽ͷ˥������
      case COUPLING:   // ˢ����ʾ���ģʽ״̬
      case V0_POSI:    // ˢ����ʾ��ֱƫ��λ��
      case V1_POSI:    // ˢ����ʾ��λ�α�λ��
      case V2_POSI:    // ˢ����ʾ��λ�α�λ��
      case YN_TYPE:    // ˢ����ʾ�α���ʽ
        SetColor(GRY, TCOLOR[YN]);
        if(i == Y_RANGES || i == COUPLING||i == PROBEATT)
        {
      Draw_Circle_D(INV, S_X[Y_RANGES]-3, TITLE_Y_POS, 11, 0, 6*7);
      DispStr(S_X[Y_RANGES], TITLE_Y_POS, INV,      (List[Y_RANGES].PTR +(Range)*6));
      DispStr(S_X[COUPLING], TITLE_Y_POS, INV,      (List[COUPLING].PTR +(List[COUPLING].Val)*6));
      DispStr10x14(S_X[PROBEATT], TITLE_Y_POS, PRN+SYMB,(List[PROBEATT].Val?("A"):("B")));
        }
        ParamTab[P1x2] = 2*((List[V0_POSI].Val+1));
        ParamTab[VTx2] = ParamTab[P1x2]+ 2*(List[THRESHOLD].Val+1);
        ParamTab[V1x2] = 2*(List[V1_POSI].Val+1);
        ParamTab[V2x2] = 2*(List[V2_POSI].Val+1);
        ParamTab[V1F]  = (List[YN_TYPE].Val)? L_HID : SHOW;
        ParamTab[V2F]  = (List[YN_TYPE].Val)? L_HID : SHOW;

        Tmp  =(ParamTab[V1x2]- ParamTab[V2x2])/2;         // ˢ����ʾV2-V1��ֵ
        Tmp *= X_SCALE[Range]* Exp(V_SCALE[Range]);
        Value2Str(NumStr, Tmp, V[0], 3, UNSIGN);
        Draw_Circle_D(INV, S_X[V1_POSI]-3, LABLE_Y, 11, 0, 6*9);
        DispStr(S_X[V1_POSI]+0*6, LABLE_Y, INV+SYMB, "]");
        DispStr(S_X[V1_POSI]+1*6, LABLE_Y, INV,      "V:      ");
        DispStr(S_X[V1_POSI]+7*6, LABLE_Y, INV,  "  ");
        DispStr(S_X[V1_POSI]+3*6, LABLE_Y, INV,  (char *)NumStr);

        SetColor(GRY, TCOLOR[TRG]);
        Tmp  = List[SENSITIVE].Val;                       // ˢ����ʾ�����ز�ֵ
        Tmp *= X_SCALE[Range]* Exp(V_SCALE[Range]);
        Value2Str(NumStr, Tmp,V[0], 2, UNSIGN);
        DispStr(S_X[SENSITIVE]+9, TITLE_Y_POS, INV,  "     ");
        DispStr(S_X[SENSITIVE]+9, TITLE_Y_POS, INV,  (char *)NumStr);
        DispStr10x14(S_X[SENSITIVE], TITLE_Y_POS, INV+SYMB,"E");
        GainA  = (char)List[Y_RANGES].Val;

        if((char)List[Y_RANGES].Val>1)
          KindA  = HV;
        else
          KindA  = LV;                //Ch1ѡ���ѹ���ѹ����
        StateA =  ((char)List[Y_RANGES].Val>5)?ACT: GND;

        CouplA = (List[COUPLING].Val)? AC : DC;                      // AC);  //AC/DC��Ϸ�ʽ
        BIOS_Ctrl(AiRANGE, KindA+CouplA+StateA);             //Ch1״̬ˢ��
        AiPosi(List[V0_POSI].Val);                        //Ch1Ӳ����Ӧλ��
        break;

      case SAVE_BMP: //��ʾ��ѡ��Ҫ������ļ����
      case SAVE_DAT:
      case SAVE_BUF:
      case SAVE_CSV:
      case SAVE_SVG:
      case LOAD_DAT: //��ʾ��ѡ��Ҫ��ȡ���ļ����
      case LOAD_BUF:
        SetColor(GRY, TCOLOR[FN]);
        u8ToDec3(NumStr, List[i].Val);
        Draw_Circle_D(INV, S_X[i]-3, LABLE_Y, 11, 0, 6*11);
        DispStr(S_X[i],    LABLE_Y, INV, ((i > SAVE_SVG)? "Load" : "Save"));//0513
        DispStr(S_X[i]+24, LABLE_Y, INV, (char *)NumStr);
        DispStr(S_X[i]+42, LABLE_Y, INV, (List[i].PTR +(i - SAVE_BMP)*6));
        break;

      case TPYE_OUT:
        if(List[TPYE_OUT].Val ==0) //suqar
        {
          BIOS_Ctrl(OUT_MOD, DISABLE);            // �ر����ģʽ
          Set_Fout(List[FREQ_OUT].Val);
          Set_Duty();
          BIOS_Ctrl(OUT_MOD, PULSED);             // �趨�������ģʽ
        }
        else
        {
          if(List[FREQ_OUT].Val > MAX_FREQ)
          {                                    //ģ�Ⲩ�����������20KHz
             List[FREQ_OUT].Val = MAX_FREQ ;   //10KHz
          }
           BIOS_Ctrl(OUT_MOD, DISABLE);            // �ر����ģʽ
           htim7.Init.Prescaler = Dac_Psc[List[FREQ_OUT].Val] - 1;
           BIOS_Ctrl(DAC_TIM, Dac_Tim[List[FREQ_OUT].Val]-1); // 72MHz/2/180/100 = 2KHz ģ���������(ÿ����36��)
           BIOS_Ctrl(OUT_CNT, 180);
           BIOS_Ctrl(OUT_BUF, BIOS_Info(List[TPYE_OUT].Val));    // ����ģ��������Ҳ������ݱ�
           BIOS_Ctrl(OUT_MOD, ANALOG);             // �趨ģ�����ģʽ
        }

      break;

      case FREQ_OUT: // ��ʾ�������Ƶ��
        if(List[TPYE_OUT].Val ==0) //suqar
        {
          BIOS_Ctrl(OUT_MOD, DISABLE);            // �ر����ģʽ
          Set_Fout(List[FREQ_OUT].Val);
          Set_Duty();
          BIOS_Ctrl(OUT_MOD, PULSED);             // �趨�������ģʽ
        }
        else
        {
          if(List[FREQ_OUT].Val > MAX_FREQ)
          {                                    //ģ�Ⲩ�����������20KHz
            List[FREQ_OUT].Val = MAX_FREQ ;   //10KHz
          }
          BIOS_Ctrl(OUT_MOD, DISABLE);            // �ر����ģʽ
          htim7.Init.Prescaler = Dac_Psc[List[FREQ_OUT].Val] - 1;
          BIOS_Ctrl(DAC_TIM, Dac_Tim[List[FREQ_OUT].Val]-1); // 72MHz/2/180/100 = 2KHz ģ���������(ÿ����36��)
          BIOS_Ctrl(OUT_CNT, 180);
          BIOS_Ctrl(OUT_BUF, BIOS_Info(List[TPYE_OUT].Val));    // ����ģ��������Ҳ������ݱ�
          BIOS_Ctrl(OUT_MOD, ANALOG);             // �趨ģ�����ģʽ
        }
        break;

      case FREQ_DUT: // ��ʾ�������ռ�ձ�
        SetColor(GRY, TCOLOR[SN]);
        Draw_Circle_D(INV, S_X[i]-3, LABLE_Y, 11, 0, 6*9);
        if(List[TPYE_OUT].Val == 0) //suqar
        {
          Set_Duty();
        }
        break;

      case EXT_REFN: // ��ʾ�ο���������
      case EXT_POSI: // ��ʾ�ο�����ƫ��
        SetColor(GRY, TCOLOR[EX]);
        if((i == EXT_REFN) || (i == EXT_POSI))
        {
          Draw_Circle_D(INV, S_X[EXT_REFN]-3, TITLE_Y_POS, 11, 1, 6*5);
          DispStr(S_X[EXT_REFN], TITLE_Y_POS, INV, (char*)(List[EXT_HIDE].Val? I[List[EXT_REFN].Val] : I[6]));
        }
        if(List[EXT_REFN].Val < 6) ParamTab[P2x2]=(List[EXT_POSI].Val + 1)*2;
        else                       ParamTab[P2x2]=(List[ V0_POSI].Val + 1)*2;
        break;

      case EXT_HIDE:
        if(List[EXT_HIDE].Val == 0) ParamTab[P2F] |=  W_HID;  // W_HID�����ο�����
        else                        ParamTab[P2F] &= ~W_HID;  // W_HID��ʾ�ο�����
        SetColor(GRY, TCOLOR[EX]);
        Draw_Circle_D(INV, S_X[EXT_REFN]-3, TITLE_Y_POS, 11, 1, 6*5);
        DispStr(S_X[EXT_REFN], TITLE_Y_POS, INV, (char*)(List[EXT_HIDE].Val? I[List[EXT_REFN].Val] : I[6]));
      break;

      case SYNCMODE:  // ˢ����ʾͬ��ģʽ
      case TR_MODE:   // ˢ����ʾ������ʽ
      case FITTRIGG:  // ˢ����ʾ�Զ�������ֵ
      case THRESHOLD: // ˢ����ʾ������ֵ
      case SENSITIVE: // ˢ����ʾ����������
      case TR_TYPE:   // ˢ����ʾ�α���ʽ
        DispStatus();
        SetColor(GRY, TCOLOR[TRG]);
        if(i == TR_MODE || i==SENSITIVE)
        {
          Draw_Circle_D(INV, S_X[TR_MODE]-3, TITLE_Y_POS, 11, 0, 6*8);
          DispStr10x14(S_X[TR_MODE], TITLE_Y_POS, INV+SYMB,(List[TR_MODE].Val?("D"):("C")));
        }
        if(i == FITTRIGG)
        {
          DispStr10x14(S_X[FITTRIGG], TITLE_Y_POS, PRN+SYMB,(List[FITTRIGG].Val?("F"):("B")));
          if(List[FITTRIGG].Val)KEYTIME = 300;
          else KEYTIME = 0;
        }
        ParamTab[VTx2]= ParamTab[P1x2]+2*List[THRESHOLD].Val;
        Tmp  = List[SENSITIVE].Val;
        Tmp *= X_SCALE[Range]* Exp(V_SCALE[Range]);
        Value2Str(NumStr, Tmp, V[0], 2, UNSIGN);
        DispStr10x14(S_X[SENSITIVE], TITLE_Y_POS, INV+SYMB,"E");
        DispStr(S_X[SENSITIVE]+9, TITLE_Y_POS, INV,  "     ");
        DispStr(S_X[SENSITIVE]+9, TITLE_Y_POS, INV,  (char *)NumStr);
        ParamTab[VTF]=(List[TR_TYPE].Val)? L_HID : SHOW;
        break;

      case TIM_BASE:  // ˢ����ʾʱ����λ
      case VIEWPOSI:  // ˢ����ʾˮƽ�Ӵ�λ��
      case SMPL_DPTH: // ˢ����ʾ�������
      case T1_POSI:   // ˢ����ʾ����α�λ��
      case T2_POSI:   // ˢ����ʾ�Ҷ��α�λ��
      case XN_TYPE:   // ˢ����ʾ�α���ʽ
        SetColor(GRY, TCOLOR[XN]);
        if(i == TIM_BASE)
        {
          Draw_Circle_D(INV, S_X[i]-3, TITLE_Y_POS, 11, 2, 6*5);
          DispStr(S_X[i], TITLE_Y_POS, INV, TimeBase_Str[List[TIM_BASE].Val]);
        }
        if(i == SMPL_DPTH){
          Draw_Circle_D(INV, S_X[SMPL_DPTH]-3, LABLE_Y, 11, 0, 6*11);
          DispStr(S_X[SMPL_DPTH],    LABLE_Y,  INV, "SmplDpth:  ");
          DispStr(S_X[SMPL_DPTH]+6*9, LABLE_Y,  INV, (char *)&D[List[SMPL_DPTH].Val]);
        }
        Update_Windows();

        ParamTab[T0x1] = 2 + Presampling - List[VIEWPOSI].Val;
        ParamTab[T0F] &= ~D_HID;
        ParamTab[T1F] &= ~D_HID;
        ParamTab[T1F] &= ~D_HID;
        ParamTab[T1x1] = List[T1_POSI].Val+1;
        ParamTab[T2x1] = List[T2_POSI].Val+1;
        ParamTab[T1F]  = (List[XN_TYPE].Val)? L_HID : SHOW;
        ParamTab[T2F]  = (List[XN_TYPE].Val)? L_HID : SHOW;
        Tmp  = ParamTab[T2x1] - ParamTab[T1x1];
        Tmp *= B_SCALE[List[TIM_BASE].Val]* Exp(T_SCALE[List[TIM_BASE].Val]);
        Value2Str(NumStr, Tmp, T[0], 3, UNSIGN);
        Draw_Circle_D(INV, S_X[V1_POSI]-3, LABLE_Y, 11, 0, 6*9);
        DispStr(S_X[T1_POSI]+0*6, LABLE_Y, INV+SYMB, "]");
        DispStr(S_X[T1_POSI]+1*6, LABLE_Y, INV,      "T:      ");
        DispStr(S_X[T1_POSI]+7*6, LABLE_Y, INV,  "  ");
        DispStr(S_X[T1_POSI]+3*6, LABLE_Y, INV,  (char *)NumStr);
        Set_Base(List[TIM_BASE].Val);          // Ӳ������ɨ��ʱ����λ
      break;

      case FREQUENC://��ʾƵ��
        FTmp=Edge*25*1000/2;
        if(List[TIM_BASE].Val<9)FTmp= FTmp*1000000;
        else if(List[TIM_BASE].Val<18)FTmp= FTmp*1000;
        FTmp=FTmp/Ts[List[TIM_BASE].Val]/DEPTH[List[SMPL_DPTH].Val];
          DispMeter(i, 1, UNSIGN,FTmp);
      break;

      case DUTY:    //��ʾռ�ձ�
         DispMeter(i, k, UNSIGN,(HighT*100)/k);
        break;

      case VRMS://��ʾ��ѹ��Чֵ
        if(abs(Vrms)<4)
        {
          Tmp = 0.00;
        }else{
		  scale = (List[PROBEATT].Val?
                  VScale_10X[List[Y_RANGES].Val]:VScale[List[Y_RANGES].Val])*40;
	          Tmp  = ((((Vrms)*Ak)>>12))*scale;
        }
        DispMeter(i, 1, UNSIGN, Tmp);
        break;

      case VP_P://��ʾ��ѹ���ֵ
        if(abs(Vpp)<7){
          Tmp = 0.00;
        }
        else
        {
           scale = (List[PROBEATT].Val?
                  VScale_10X[List[Y_RANGES].Val]:VScale[List[Y_RANGES].Val])*40;
          Tmp  = ((((Vmax-2048)*(Ak))>>12)-(((Vmin-2048)*(Ak))>>12))*scale;
        }
        DispMeter(i, 1, UNSIGN, Tmp);
        break;

      case VAVG://��ʾ��ѹƽ��ֵ
        if(abs(Vavg)<4)
        {
          Tmp = 0.00;
        }else{
          scale = (List[PROBEATT].Val?
                  VScale_10X[List[Y_RANGES].Val]:VScale[List[Y_RANGES].Val])*40;
          Tmp = ((((Vavg-2048)*(Ak))>>12)+100-List[V0_POSI].Val)*scale;
        }
        DispMeter(i, 1, UNSIGN, Tmp);
        break;

      case VMAX://��ʾ��ѹ���ֵ
        if(abs(Vmax)<4){
          Tmp = 0.00;
        }else{
		  scale = (List[PROBEATT].Val?
                  VScale_10X[List[Y_RANGES].Val]:VScale[List[Y_RANGES].Val])*40;
          Tmp  = ((((Vmax-2048)*(Ak))>>12)+100-List[V0_POSI].Val)*scale;
        }
        DispMeter(i, 1, UNSIGN, Tmp);
        break;

      case VMIN://��ʾ��ѹ��Сֵ
        if(abs(Vmin)<4){
          Tmp = 0.00;
        }else{
		  scale = (List[PROBEATT].Val?
                           VScale_10X[List[Y_RANGES].Val]:VScale[List[Y_RANGES].Val])*40;
                  Tmp  = ((((Vmin-2048)*(Ak))>>12)+100-List[V0_POSI].Val)*scale;
        }
        DispMeter(i, 1, SIGN, Tmp);
        break;
      case STANDBY:

        break;

      }
      /**/
      if(Freq_Flag){
        Freq_Flag = 0;
        FTmp=Edge*25*1000/2;
        if(List[TIM_BASE].Val<9)FTmp= FTmp*1000000;
        else if(List[TIM_BASE].Val<18)FTmp= FTmp*1000;
        FTmp=FTmp/Ts[List[TIM_BASE].Val]/DEPTH[List[SMPL_DPTH].Val];
        DispFreq(    1, UNSIGN,FTmp);
      }
    }
  }

}
/*******************************************************************************

*******************************************************************************/
void DispMeter(short Item, char St, char Mode, int Val)
{
  SetColor(GRY, TCOLOR[ME]);

  Draw_Circle_D(INV, S_X[Item]-3, LABLE_Y, 11, 0, 6*12);
  DispStr(S_X[Item], LABLE_Y, INV, (char *)&Z[Item-FREQUENC]);// ��ʾ������Ŀ����
  Value2Str(NumStr, Val, List[Item].PTR, 3, Mode);

  if(St)
  {
    DispStr(S_X[Item]+24, LABLE_Y, INV, ":       ");
    DispStr(S_X[Item]+30, LABLE_Y, INV, (char *)NumStr);   // ��ʾ������ֵ+����
  }
  else
	  DispStr(S_X[Item]+24, LABLE_Y, INV, ": ----- ");
}

/*******************************************************************************

*******************************************************************************/
void DispStatus(void)
{
  char* p;
  SetColor(GRY, (Status & STOP) ? RED : TCOLOR[TRG]);
  p =(Status & STOP)?(char*)M :(List[SYNCMODE].PTR +(List[SYNCMODE].Val)*6);
  Draw_Circle_D(INV, S_X[SYNCMODE]-3, TITLE_Y_POS, 11, 1, 6*4);
  DispStr(S_X[SYNCMODE], TITLE_Y_POS, INV, (char *)p);
}

/*******************************************************************************
���»���������λ��
*******************************************************************************/
void Update_Windows(void)
{
  short  x0,x,y,i,j,Width;
  short  Depth =  DEPTH[List[SMPL_DPTH].Val];
  Width =(300*300)/Depth;
  if(Current == XN &&(PopType & LIST_POP) )
  {
      Windows_Pop = 1;
      x0=2+(List[VIEWPOSI].Val)*300/Depth;
      x=2;
      y=15;
      for(i=0;i<300;i++){    //��������
        LCD_Set_Posi(x++, y);
        LCD_Set_Pixel(DAR);
        for(j=0; j<2; j++)LCD_Set_Pixel(RED);
        LCD_Set_Pixel(DAR);
      }
      if(Current == XN && (MenuNow[XN]==SMPL_DPTH ))
      {  //Depthѡ��
        if(Key_Buffer == KEYCODE_RIGHT || Key_Buffer == KEYCODE_LEFT)
        {  //ѡ���б䶯ʱ�ı�Ԥ����λ��
          List[VIEWPOSI].Val= 0;
          x0=2+(List[VIEWPOSI].Val)*300/Depth;
        }
      }
      x = x0;               //��ǰ�����ڻ�����λ��
      y=15;
      for(i=0;i<Width;i++){
        LCD_Set_Posi(x++, y);
        for(j=0; j<4; j++)LCD_Set_Pixel(GRN);
      }
  }
  else if(Current == XN &&!(PopType & LIST_POP))
  {
    Windows_Pop = 0;
    Clr_WavePosi(DAR);
  }
}

/*******************************************************************************
 void Clr_WavePosi(u16 Col)  �������λ����ʾ
*******************************************************************************/
void Clr_WavePosi(short Col)
{
  short x,y,i,j;
  x=2;
  y=14;
  for(i=0;i<300;i++)
  {
    LCD_Set_Posi(x++, y);
    for(j=0; j<6; j++)
    	LCD_Set_Pixel(Col);
  }
}

/*******************************************************************************

*******************************************************************************/
void DispFreq(char St, char Mode, int Val)
{
  SetColor(GRY, TCOLOR[ME]);

  Draw_Circle_D(INV, S_X[FREQUENC]-6*15-3+4, LABLE_Y, 11, 0, 6*12);
  DispStr(S_X[FREQUENC]-6*15+4, LABLE_Y, INV, (char*)&Z[0]);// ��ʾ������Ŀ����
  Value2Str(NumStr, Val, List[FREQUENC].PTR, 3, Mode);

  if(St) {
    DispStr(24+S_X[FREQUENC]-6*15+4, LABLE_Y, INV, ":       ");
    DispStr(30+S_X[FREQUENC]-6*15+4, LABLE_Y, INV, (char *)NumStr);   // ��ʾ������ֵ+����
  }
  else   DispStr(24+S_X[FREQUENC]-6*15+4, LABLE_Y, INV, ": ---- ");
}

/*******************************************************************************
 Show_List: ���ò˵���λ�á��ߴ硢��ɫ����ʾ����
*******************************************************************************/
void Show_List(void)
{
  short i, Ny0, Nx0, CurrentMax = MU_BEGN[Current]+ M_ITEMS[Current];
  char  Flag = 0;
  char  str[12];
  ParamTab[BXx1]= BLOCK_X[Current];
  ParamTab[BYx2]= BLOCK_Y[Current]*2;
  ParamTab[WXx1]= BLOCK_W[Current];
  ParamTab[HYx2]=(M_ITEMS[Current]+1)*32;
  PopColor(WHT, DAR);
  Buil_Pop();
  for(i=MU_BEGN[Current]; i<CurrentMax; i++){
    if(i == MenuNow[Current]) {
      Flag = INV;
    }
    else                      {
      PopColor(WHT, DAR);
      Flag = PRN;
    }
    Ny0 = ((CurrentMax - i)*17 - 10);
    Nx0 = N_X ;
    memset(str,0,12);
    memcpy(str,&ESTR[i],10);
    NoteSTR(Nx0, Ny0, Flag, (char *)str);
    Note_4Point(PRN, Nx0, Ny0, strlen((char*)str), Font6x8_H);
  }
  List_Pop();
}

/*******************************************************************************

*******************************************************************************/
void DispFileInfo(char Info)
{
  SetColor(DAR, BLU);
  Draw_Circle_D(INV, S_X[SAVE_BMP]-3, LABLE_Y, 11, 0, 6*11);
  DispStr(S_X[SAVE_BMP], LABLE_Y, INV, (char *)(&FnNote+14*Info));
  HAL_Delay(2000);

}

/*******************************************************************************

*******************************************************************************/
void DispBMPInfo(void)
{
  SetColor(GRY, TCOLOR[FN]);
  u8ToDec3(NumStr, List[SAVE_BMP].Val);
  Draw_Circle_D(INV, S_X[SAVE_BMP]-3, LABLE_Y, 11, 0, 6*11);
  DispStr(S_X[SAVE_BMP],    LABLE_Y, INV, ("Save"));//0513
  DispStr(S_X[SAVE_BMP]+24, LABLE_Y, INV, (char *)NumStr);
  DispStr(S_X[SAVE_BMP]+42, LABLE_Y, INV, (List[SAVE_BMP].PTR ));
}

/*******************************************************************************
Update_Proc_All  ȫ��ˢ��
*******************************************************************************/
void Update_Proc_All(void)
{
  short i;
  long Tmp = 0;
  short Range = List[Y_RANGES].Val + 10*List[PROBEATT].Val;

  for(i=Y_RANGES; i<=STANDBY; i++){
    List[i].Flg |= UPD;
  }

  for(i=Y_RANGES; i<=STANDBY; i++){
    if(List[i].Flg & UPD){
      List[i].Flg &= ~UPD;
      switch (i){
      case Y_RANGES:   // ˢ����ʾ Y ���̵�λ
      case PROBEATT:   // ˢ����ʾ̽ͷ˥������
      case COUPLING:   // ˢ����ʾ���ģʽ״̬
      case V0_POSI:    // ˢ����ʾ��ֱƫ��λ��
        SetColor(GRY, TCOLOR[YN]);
        if(i == Y_RANGES || i == COUPLING||i == PROBEATT){
          Draw_Circle_D(INV, S_X[Y_RANGES]-3, TITLE_Y_POS, 11, 0, 6*7);
          DispStr(S_X[Y_RANGES], TITLE_Y_POS, INV,      (List[Y_RANGES].PTR +(Range)*6));
          DispStr(S_X[COUPLING], TITLE_Y_POS, INV,      (List[COUPLING].PTR +(List[COUPLING].Val)*6));
          DispStr10x14(S_X[PROBEATT], TITLE_Y_POS, PRN+SYMB,(List[PROBEATT].Val?("A"):("B")));
        }
        ParamTab[P1x2] = 2*((List[V0_POSI].Val+1));
        ParamTab[VTx2] = ParamTab[P1x2]+ 2*(List[THRESHOLD].Val+1);

        StateA =  (List[Y_RANGES].Val>5)?ACT: GND;                                    //ACT GND �ӵػ򼤻�״̬
        GainA  = List[Y_RANGES].Val;
        if(List[Y_RANGES].Val>1)
          KindA  = HV;
        else
          KindA  = LV;                //Ch1ѡ���ѹ���ѹ����

        CouplA = (List[COUPLING].Val)? AC : DC;                      // AC);  //AC/DC��Ϸ�ʽ
        BIOS_Ctrl(AiRANGE, KindA+CouplA+StateA);             //Ch1״̬ˢ��
        AiPosi(List[V0_POSI].Val);                        //Ch1Ӳ����Ӧλ��

        break;

      case SYNCMODE:  // ˢ����ʾͬ��ģʽ
      case TR_MODE:   // ˢ����ʾ������ʽ
      case FITTRIGG:  // ˢ����ʾ�Զ�������ֵ
      case THRESHOLD: // ˢ����ʾ������ֵ
      case SENSITIVE: // ˢ����ʾ����������
      case TR_TYPE:   // ˢ����ʾ�α���ʽ
        DispStatus();
        SetColor(GRY, TCOLOR[TRG]);
        if(i == TR_MODE || i==SENSITIVE)
        {
          Draw_Circle_D(INV, S_X[TR_MODE]-3, TITLE_Y_POS, 11, 0, 6*8);
          DispStr10x14(S_X[TR_MODE], TITLE_Y_POS, INV+SYMB,(List[TR_MODE].Val?("D"):("C")));
        }
        if(i == FITTRIGG)
        {
          DispStr10x14(S_X[FITTRIGG], TITLE_Y_POS, PRN+SYMB,(List[FITTRIGG].Val?("F"):("B")));
          if(List[FITTRIGG].Val)KEYTIME = 300;
          else KEYTIME = 0;
        };
        ParamTab[VTx2]= ParamTab[P1x2]+2*List[THRESHOLD].Val;
        Tmp  = List[SENSITIVE].Val;
        Tmp *= X_SCALE[Range]* Exp(V_SCALE[Range]);
        Value2Str(NumStr, Tmp, V[2], 2, UNSIGN);
        DispStr10x14(S_X[SENSITIVE], TITLE_Y_POS, INV+SYMB,"E");
        DispStr(S_X[SENSITIVE]+9, TITLE_Y_POS, INV,  "     ");
        DispStr(S_X[SENSITIVE]+9, TITLE_Y_POS, INV,  (char *)NumStr);
        ParamTab[VTF]=(List[TR_TYPE].Val)? L_HID : SHOW;
        break;

      case TIM_BASE:  // ˢ����ʾʱ����λ
        SetColor(GRY, TCOLOR[XN]);
        if(i == TIM_BASE)
        {
          Draw_Circle_D(INV, S_X[i]-3, TITLE_Y_POS, 11, 2, 6*5);
          DispStr(S_X[i], TITLE_Y_POS, INV, (char *)TimeBase_Str[List[TIM_BASE].Val]);
        }
        Set_Base(List[TIM_BASE].Val);          // Ӳ������ɨ��ʱ����λ
      break;

      }
    }
  }
}
