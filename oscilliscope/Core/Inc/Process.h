/*
 * Process.h
 *
 *  Created on: May 10, 2021
 *      Author: paul_
 */

#ifndef INC_PROCESS_H_
#define INC_PROCESS_H_

#define SCAN              1
#define AUTO              2
#define NORM              3
#define SINGL             4
#define NONE              5

#define RISE              0
//#define FALL              1
#define PRE_SMPL          150
//#define CH_A              0

extern short *KpA, *KpB, *KgA, *KgB, Kpg[];
extern int   Vrms, Vpp,  LowT, HighT, Edge, Vssq;
extern short Vmax, Vmin, Vmid, Vavg /*, Start*/;
extern short StateA, CouplA, GainA, KindA /*,Str[]*/;
extern char  Norm_Clr, NORM_Kflag, SNGL_Kflag ;
extern char  GK[];
extern short Sampl[];
extern short Ch1_Posi, Ch2_Posi, Tri_Posi;

void Zero_Align(void);
void Align_Set(void);
void AiPosi(char Val);
void Analys(void);
void WaveProcess(void);
void Build_Ext(short x, char Ext, char Yp);
void Auto_Fit(void);

#endif /* INC_PROCESS_H_ */
