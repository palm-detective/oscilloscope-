/*
 * Bios.h
 *
 *  Created on: May 10, 2021
 *      Author: paul_
 */

#ifndef INC_BIOS_H_
#define INC_BIOS_H_

#define KEYCODE_PLAY     1
#define KEYCODE_MANU     2
#define KEYCODE_UP       3
#define KEYCODE_DOWN     4
#define KEYCODE_LEFT     5
#define KEYCODE_RIGHT    6
#define KEYCODE_ACT      8
#define KEYCODE_MEM      9
#define KEYCODE_D_MANU   10

// void Ctrl(AiRANGE, AC+LV+ACT) controls switches
//======+======================+=======================+=======================+
// Val: |      AC/DC           |		               |            3 Az       |
//------+----------------------+-----------------------+-----------------------+
enum    { AC = 0x00, DC = 0x02,  LV = 0x00,  HV = 0x01, GND = 0x00, ACT = 0x04 };
//======+======================+=======================+=======================+

// ���� void Ctrl(u8 Item, u32 Val) �� Item �Ķ����Լ� Val ����˵��
//=====+========+========+========+========+========+========+========+========+
//Item:|������ʱ|���ģʽ|ģ�ⶨʱ|ģ�����|ģ������|ģ���ƽ|����״̬|������ʱ
//-----+--------+--------+--------+--------+--------+--------+--------+--------+
// Val:|   mS   |   0~2  |  0~99  |   u16  |  u16*  |   u16  |   0~1  |   u16  |
//-----+--------+--------+--------+--------+--------+--------+--------+--------+
enum   {/*DELAYmS,*/ OUT_MOD, DAC_TIM, OUT_CNT, OUT_BUF, /*OUT_VAN,*/  SMPL_ST, SMPLTIM,
//=====+========+========+========+========+========+========+========+========+
//Item:|��������|��������| CHA����| CHAƫ��|�Զ��ػ�
//-----+--------+--------+--------+--------+--------+--------+--------+--------+
// Val:|  u16*  |   u16  |   0~1  | 0~4095 |    0
//-----+--------+--------+--------+--------+--------+--------+--------+--------+
       /*  SMPLBUF,*/ SMPLNUM, AiRANGE, AOFFSET/*, PWROFF */
//=====+========+========+========+========+========+========+========+========+
};
enum    {/*LOW = 0, HIGH = 1,*/ ANALOG = 2, PULSED = 4, /*SIMULTANEO = 5, INTERLEAVE = 6,*/};

// BIOS_Info(u8 Item)
enum    { CHA_CNT/*, SIN_TAB, TRG_TAB, SAW_TAB*/ };

extern volatile char  Key_Buffer, Twink, Blink/*, Key_Status_Last*/;
extern volatile short PopupCnt;
extern short  KEYTIME;
extern const short FARR[];
void BIOS_Ctrl(char Item, unsigned Val);
unsigned  BIOS_Info(char Item);
void Set_Base(short Base);
void Set_Fout(short Range);
void Set_Duty(void);
void ADC_Start(void);
void Tim3_ISP(void);

#endif /* INC_BIOS_H_ */
