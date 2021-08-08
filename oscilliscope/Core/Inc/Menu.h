/*
 * Menu.h
 *
 *  Created on: May 30, 2021
 *      Author: paul_
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

typedef struct { char*  PTR;           // 0 or string
                 short  Val;           //  valueֵ
                 char   Flg;} item;    // update

typedef struct { const char*  PTR;
                 const short  Val;
                 const char   Flg;} item_c;

typedef enum _ITEM_NUMBER
{
                   Y_RANGES, COUPLING, PROBEATT,  V0_POSI,   V1_POSI,   V2_POSI,  YN_TYPE, //7
                   TIM_BASE, VIEWPOSI, SMPL_DPTH, T1_POSI,   T2_POSI,   XN_TYPE,           //13
                   SYNCMODE, TR_MODE,  FITTRIGG,  THRESHOLD, SENSITIVE, TR_TYPE,           //19
                   FREQUENC, DUTY,     VRMS,      VAVG,      VP_P,      VMAX,     VMIN,    //26
                   EXT_REFN, EXT_POSI, EXT_HIDE,                                           //29
				   SAVE_BMP, SAVE_DAT, SAVE_BUF,  SAVE_CSV,  SAVE_SVG,  LOAD_DAT,  LOAD_BUF, //36
                   TPYE_OUT, FREQ_OUT, FREQ_DUT,                                           //39
                   AUTO_CAL, RESTORE,  STANDBY                                             //42
} ITEM_NUMBER;

#define BLINK      1
#define MIN_UNIT   60     /*60s 1min*/
#define UPD        0x20    // update flag
#define YN         0
#define XN         1
#define TRG        2
#define ME         3
#define EX         4
#define FN         5
#define SN         6
#define ST         7
#define T_X        305     //
#define N_X         4     // ��Ŀ����ע�Ϳ��ڵ���ʼx����
#define LABLE_Y     3
#define Font8x14_H  14
#define Font6x8_H   10
#define MAX_FREQ    9      /*���ģ�Ⲩ�����Ƶ��Ϊ10KHz*/
#define TITLE_Y_POS     225
#define STOP       0x01    // ������ͣ
#define LOADM      0x10

extern item List[];
extern item_c List_c[];
extern short  MenuNow[];
extern const short MenuNow_C[], KS[], DEPTH[], MAX[], MIN[];
extern const char M_ITEMS[], MU_BEGN[];
extern char  TimeBase_Str[][8];
extern const int VScale[];
extern long  VScale_10X[];
extern short StdBy_Time;
extern short Current, Status;
extern char  StdBy_Flag;
extern char  Windows_Pop;
extern const char  N[][6];
extern const char  R[][6];
extern const short S_X[];
extern const unsigned short TCOLOR[];

void Update_Title(void);
void Update_Blink(void);
void Update_Detail(void);
void DispMeter(short Item, char St, char Mode, int Val);
void DispStatus(void);
void Update_Windows(void);
void DispFreq(char St, char Mode, int Val);
void Clr_WavePosi(short Col);
void Show_List(void);
void Update_Proc_All(void);
void DispBMPInfo(void);
void DispFileInfo(char Info);

#endif /* INC_MENU_H_ */
