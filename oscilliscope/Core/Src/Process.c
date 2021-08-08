/*
 * Process.c
 *
 *  Created on: May 10, 2021
 *      Author: paul_
 */

#include "stm32f4xx_hal.h"
#include "Bios.h"
#include "Process.h"
#include "Menu.h"
#include "Draw.h"
#include "Func.h"
#include "main.h"
#include <string.h>
#include <stdlib.h>

short Vmax, Vmin, Vmid, Vavg/*, Start*/;
int   Vrms, Vpp, LowT, HighT, Edge, Vssq;
short StateA, CouplA, GainA, KindA;
short Ch1_Posi, Ch2_Posi, Tri_Posi;
short Trigger_k,Posi_2F1, Posi_2F2, Vo_Temp;
char  Norm_Clr = 0, NORM_Kflag, SNGL_Kflag = 1;

short Sampl[0x2000+2];              // adc sample buffer

//=======+======+======+======+======+======+======+======+======+======+======+
//       | LV ���̵�λ |      MV ���̵�λ          |       HV ���̵�λ         |
//-------+------+------+------+------+------+------+------+------+------+------+
// ����  |   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |
//-------+------+------+------+------+------+------+------+------+------+------+
// ����  |  x1  |  x2  |  x1  |  x2  |  x4  |  x10 |  x1  |  x2  |  x5  |  x10 |
//-------+------+------+------+------+------+------+------+------+------+------+
char GK[]={   2,     4,     2,     4,     8,     20,    2,     4,    10,    20};

// �  LV = ,  HV =
//=============+=======+=======+=======+=======+=======+=======+=======+
//                     |   LV  |   MV  |   HV  |   LV  |   MV  |   HV  |
//---------------------+-------+-------+-------+-------+-------+-------+
//                     | CH_A          |       | CH_A  |
//---------------------+-------+-------+-------+-------+-------+-------+
short Kpg[] = { 715,   700,    700,   1200,    1100,   1080  };

short *KpA = &Kpg[0], *KgA = &Kpg[3];


/*******************************************************************************
 Align_Set:
*******************************************************************************/
void Align_Set(void)
{
  unsigned short i, TmpA = 0, StA = 0;

  Analys();
  for(i=0; i<100; i++)
  {
    AiPosi(100);
    HAL_Delay(10);                             // Wait 10mS
    Analys();
    TmpA = 2048-Vavg;

    if(TmpA != 0)
    {
      KpA[KindA+(StateA?1:0)] += 700*TmpA/2048;
      StA = 0;
    }
    else
      StA++;

    if(StA > 4)
    	return;
  }

}

/*******************************************************************************
 Channel's zero alignment:
*******************************************************************************/
void Zero_Align(void)
{
  GainA  = 0;//0x20;
  KindA  = HV;
  StateA = ACT;
  BIOS_Ctrl(AiRANGE, HV+AC+ACT);
  AiPosi(100);
  HAL_Delay(1000);
  Align_Set();

  GainA  = 0;//0x20;
  KindA  = HV;
  StateA = GND;
  BIOS_Ctrl(AiRANGE, HV+AC+GND);
  AiPosi(100);
  HAL_Delay(1000);
  Align_Set();

  GainA  = 0;//0x20;
  KindA  = LV;
  StateA = GND;
  BIOS_Ctrl(AiRANGE, LV+AC+GND);
  AiPosi(100);
  HAL_Delay(1000);
  Align_Set();

}

/*******************************************************************************
 Channel's error analys
*******************************************************************************/
void Analys(void)
{
  unsigned i, SumA = 0;

  BIOS_Ctrl(SMPL_ST, DISABLE);
  BIOS_Ctrl(SMPLTIM, 720-1);      // 72MHz/360 = 400kHz = 2.5uS
  BIOS_Ctrl(SMPLNUM, 4096);       // 8192
  BIOS_Ctrl(SMPL_ST, ENABLE);
  while((BIOS_Info(CHA_CNT) != 0)) {};
  for(i=2; i<4000+2; i++){
    SumA += Sampl[i];
  }
  Vavg = SumA/4000; // 4000
}

/*******************************************************************************
 DevCtrl:  offset
*******************************************************************************/
void AiPosi(char Val)
{
   BIOS_Ctrl(AOFFSET,((char)Val-100)*(KpA[KindA+(StateA?1:0)]/2)*GK[GainA]/KgA[KindA+(StateA?1:0)]+KpA[KindA+(StateA?1:0)]);
}

/*******************************************************************************
 WaveProcess: ���㴦���������������
*******************************************************************************/

void WaveProcess(void)
{
  //------------------ ����Ԥ����,���ɽ���ָ��������ݵ���Ч�� -----------------//

  short* Ain  = (short*)&Sampl[2];

  int  Ak = (KgA[KindA+(StateA?1:0)]*4)/GK[GainA];
  short  i, n, k, p = 0;

  short  VtlA, ViA, VthA, VeA, VpA, VoA;            // VtA,

  short  DtA   = 0, TslA  =  0, TshA = 0, StA  = 2;
  short  FallA = 0, RiseA =  0, UpA  = 0, DnA  = 0;
  int  MaxA  = 0, MinA  = ~0, SumA = 0, SsqA = 0; // ���/��Сֵ,�ۼ�/ƽ���� = 0

  short  Dpth  = DEPTH[List[SMPL_DPTH].Val] - 1;
  short  Tp    = PRE_SMPL+1, Tm = List[TR_MODE].Val & 1;
  short  Ks    = KS[List[TIM_BASE].Val],  Sm = List[SYNCMODE].Val;
  char   Ext = List[EXT_REFN].Val;

//---------------------- ��������Ԥ����,����ʵ�ʴ�����ֵ ---------------------//
  if(Status == STOP)
  {
    VtlA  = (((Tri_Posi + Ch1_Posi - List[SENSITIVE].Val-100)<<12)/Ak)+2048;
    VthA  = (((Tri_Posi + Ch1_Posi + List[SENSITIVE].Val-100)<<12)/Ak)+2048;
    VpA   = ((((List[V0_POSI].Val)       -100)<<12)/Ak)+2048; //??
  }
  else
  {
    VtlA  = (((List[THRESHOLD].Val + List[V0_POSI].Val - List[SENSITIVE].Val-100)<<12)/Ak)+2048;
    VthA  = (((List[THRESHOLD].Val + List[V0_POSI].Val + List[SENSITIVE].Val-100)<<12)/Ak)+2048;
    VpA   = ((((List[V0_POSI].Val)       -100)<<12)/Ak)+2048;
  }
//-------------------------- ��������ͳ������� ------------------------------//


  if(((List[SYNCMODE].Val == NONE)||(List[SYNCMODE].Val == SCAN))
     &&(List[TIM_BASE].Val>11))
  {
    Dpth = 302;
    List[VIEWPOSI].Val = 0;
  }

  for(i=0; i<Dpth; i++)
  {
	n = __HAL_DMA_GET_COUNTER(&hdma_adc1);//    DMA1_Channel1->CNDTR;              //DMA_CH_A->CNDTR; //DMA1_CNDTR1
    if(i >= (Dpth-n)) break;      // ����һ���ڵȴ�A/Dת�����
    ViA = Ain[i];

    if(MaxA < ViA) MaxA = ViA;    // ͳ�����ֵ
    if(MinA > ViA) MinA = ViA;    // ͳ����Сֵ
    SumA += ViA;                  // ͳ���ۼӺ�
    SsqA +=(ViA-VpA)*(ViA-VpA);   // ͳ��ƽ����


    if(StA == 2) DtA = 0;                  // ����CH_A�׸�������ǰ��ʱ���ۼ�
    else         DtA++;

    if(ViA > VthA){                        // ViA ������ֵ����
      if(StA == 0){
        TslA += DtA; DtA = 0; RiseA++;     // CH_A��λʱ���ۼƣ��������ۼ�
        if(UpA < Tp) UpA = i;              // ��¼Ԥ������CH_A��һ��������λ��
      }
      StA = 1;                             // ��ǰCH_A״̬��Ϊ��λ
    } else if(ViA < VtlA){                 // ViA ������ֵ����
      if(StA == 1){
        TshA += DtA; DtA = 0; FallA++;     // CH_A��λʱ���ۼƣ��½����ۼ�
        if(DnA < Tp) DnA = i;              // ��¼Ԥ������CH_A��һ���½���λ��
      }
      StA = 0;                             // ��ǰCH_A״̬��Ϊ��λ
    }
  }
  if(n == 0)
  {                              // ����ȫ����ɺ��������ֵ i == Dpth
    Vmax = MaxA; Vmin = MinA; Vavg = SumA/i; Vssq = SsqA/(i+1);
    Vrms = (Sqrt32(Vssq)); Vmid = (Vmax + Vmin)/2; Vpp = Vmax - Vmin;
    HighT = TshA/FallA; LowT = TslA/RiseA; Edge = FallA+ RiseA;

  }
//--------------------------- ���ɲ�����ʾ���� -------------------------------//

  if(((Sm == NONE)||(Sm == SCAN))){
    if(List[TIM_BASE].Val > 11)
      k = 1;                         // NONE SCAN ģʽ��ǿ�ƴ���
    else
    {
      k =((Tm == RISE)? UpA : DnA)-Tp; // k:Ԥ�����󴥷���λ��

      if((Sm == SCAN)&&(k < 0)){ // AUTO ģʽ���޴���ʱ
        if(i > Dpth/4) k = 1;    // ������1/4����ǿ�ƴ���
        else           k = 0;    // ������1/4��ǰ����ʾ
      }

    }
  } else
    k =((Tm == RISE)? UpA : DnA)-Tp; // k:Ԥ�����󴥷���λ��

  if((!n)&&(Sm == SINGL)&&(k > 0)){ // SNGL ģʽ���д���ʱ�����������Զ�ֹͣ
    Status = STOP;
    DispStatus();
  }

  if((Sm == AUTO)&&(k < 0)){ // AUTO ģʽ���޴���ʱ
    if(i > Dpth/4) k = 1;    // ������1/4����ǿ�ƴ���
    else           k = 0;    // ������1/4��ǰ����ʾ
  }

  Trigger_k = k;

  if(Trigger_k>0)NORM_Kflag = 1;

  if((SNGL_Kflag)&&(Status == STOP))
  {
    Ch1_Posi = List[V0_POSI].Val;
    Ch2_Posi = List[EXT_POSI].Val;
    Tri_Posi = List[THRESHOLD].Val;
    SNGL_Kflag = 0;
  }

  if(k > 0)
  {                                               // �д���ʱ����ʾ����
    k += List[VIEWPOSI].Val+((1024-Ks)*Tp+512)/1024; // ���β�ֵ���
    VeA =(((Ain[k]-2048)*Ak)>>12)+100;

    for(n=0; n<300;)
    {
      ViA =(((Ain[k]-2048)*Ak)>>12)+100;
      k++;
      while(p > 0){
        VoA = VeA +((ViA-VeA)*(1024-p))/1024; // ���� A ͨ����ǰ���ε�Ĳ�ֵ
        if(Status == STOP){

          Vo_Temp = VoA+(List[V0_POSI].Val-Ch1_Posi);

          if(Vo_Temp > 199) Vo_Temp = 202;
          if(Vo_Temp <   3) Vo_Temp = 1;                     // �����޷�

          WaveBuf[n]= Vo_Temp;

          Build_Ext(n, Ext, List[V0_POSI].Val);
        }
        else{
          if(VoA > 199) VoA = 202;                   // �����޷�  ��������
          if(VoA <   3) VoA = 2;                     // �����޷�
          WaveBuf[n]= VoA;

          Build_Ext(n, Ext, List[V0_POSI].Val);
        }

        if(n++ >= 300) break;
        p -= Ks;
      }
      p += 1024;
      VeA = ViA;                              // �ݴ浱ǰ���ε��ֵ
    }
    Norm_Clr = 0;                                          //NORM����
  }
  else if((k < 0) && ((Sm != NORM)||(Norm_Clr))){          //NORM��������Ļ
    for(n=0; n<300;){
      while(p > 0){
        WaveBuf[n]= 255;

        if(n++ >= 300) break;
        p -= Ks;
      }
      p += 1024;
    }
  }
  if((i == Dpth)&&(Status != STOP))
  {
	  ADC_Start();       // ���¿�ʼ ADC ɨ�����
  }
}

/*******************************************************************************
 Build_Ext: ��չͨ��������ʾ
*******************************************************************************/
void  Build_Ext(short x, char Ext, char Yp)
{
  char* Buf = WaveBuf + TRACK1_SIZE;
  short  Vo;

  Posi_2F1 = List[EXT_POSI].Val -  Data[1] - List[V0_POSI].Val;
  Posi_2F2 = List[EXT_POSI].Val;// - Data[2];

  switch(Ext)
  {
  case 0: // D
    Vo = Data[x+10]-Data[1]+List[EXT_POSI].Val; break;
  case 1: // -D
    Vo = List[EXT_POSI].Val+Data[1]-Data[x+10]; break;
  case 2: // Inp+D
    Vo = Data[x+10]+ WaveBuf[x]+Posi_2F1; break;
  case 3: // D-Inp
    Vo = (Data[x+10]-Data[1])- (WaveBuf[x]-List[V0_POSI].Val)+ List[EXT_POSI].Val; break;
  case 4: // Inp-D
    Vo = (WaveBuf[x]-List[V0_POSI].Val)-(Data[x+10]-Data[1]) + List[EXT_POSI].Val; break;
  case 5: // -Inp
    Vo = Yp+List[EXT_POSI].Val - WaveBuf[x]; break;
  }
  if(Vo > 199) Vo = 202;
  if(Vo <   3) Vo = 1;                     // �����޷�
  Buf[x]= Vo;
}

void Auto_Fit(void)
{
  char i,j,k=0;
  short Tmp,Tmp1;

  if(Status != STOP){
    //======��ѹ���Զ�ѡ��==============
    for(i=0;i<9;i++){
      //******Aͨ��***********
      Tmp1 = ((((Vmax-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val)
           - ((((Vmin-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val);
      if((Tmp1 < 40)&&(List[Y_RANGES].Val > 2)){       //40
        List[Y_RANGES].Val--;
        List[V0_POSI].Val=100;
        WaveProcess();
        Update_Proc_All();
      }
      else if((Tmp1 > 110)&&(List[Y_RANGES].Val < 9)){ //110
        List[Y_RANGES].Val++;
        List[V0_POSI].Val=100;
        WaveProcess();
        Update_Proc_All();
      }
      HAL_Delay(50);
    }

    //============ʶ�𴥷�Դ======================


    //======�������Զ�ѡ��==============
    //if(!List[TRI_Ch]){//******Aͨ��***********
      Tmp = (((((Vmax-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val)
           - ((((Vmin-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val))/4;
      if((List[THRESHOLD].Val > (((((Vmid-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val)  + Tmp))||
         (List[THRESHOLD].Val < (((((Vmid-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val)  - Tmp))){
           if(abs(((((Vmid-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val)+15)<195)
             List[THRESHOLD].Val = ((((Vmid-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val)+15 ;
           WaveProcess();
           Update_Proc_All();
         }

      ParamTab[VTx2]= ParamTab[P1x2] + 2*List[THRESHOLD].Val;
      List[THRESHOLD].Flg |= UPD;
      Update_Title();

    //===========��ƽʱ��������==================
    HAL_Delay(100);

        if(((((Vmid-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val)>10){
      WaveProcess();
      Update_Proc_All();
      HAL_Delay(100);
      for(i=0;i<9;i++){
        WaveProcess();
        Tmp1 = (((Vmax-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val;
        if((Tmp1 < 40)&&(List[Y_RANGES].Val > 2)){       //40
          List[Y_RANGES].Val--;
          List[V0_POSI].Val=100;
          WaveProcess();
          Update_Proc_All();
        }
        else if((Tmp1 > 100)&&(List[Y_RANGES].Val < 9)){ //110
          List[Y_RANGES].Val++;
          List[V0_POSI].Val=100;
          WaveProcess();
          Update_Proc_All();
        }
        HAL_Delay(50);
      }
    }
    else if((((((Vmax-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val)
         - ((((Vmin-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val)) <10){
      //===========���ź�ʱ��ѹ��2V==================
        List[Y_RANGES].Val=7; //2v
        WaveProcess();
        Update_Proc_All();
        HAL_Delay(50);
      }



    //======ʱ���Զ�ѡ��==============

    //if(!List[TRI_Ch].Val){//******Aͨ��***********
      Tmp1 = ((((Vmax-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val)
           - ((((Vmin-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-List[V0_POSI].Val);
      for(j=0;j<11;j++){
        WaveProcess();
        if((k==0)&&(Edge < 60)&&(List[TIM_BASE].Val >5)){
          Set_Base(5);                           // Ӳ������ɨ��ʱ����λ
          List[TIM_BASE].Val = 5;
          WaveProcess();
          Update_Proc_All();
          k=1;
        }
        if((Edge > 180)&&(List[TIM_BASE].Val > 2)){
          List[TIM_BASE].Val--;
          Set_Base(List[TIM_BASE].Val);   // Ӳ������ɨ��ʱ����λ
          WaveProcess();
                 Update_Proc_All();
          HAL_Delay(50);
        }
        else if((Edge < 80)&&(List[TIM_BASE].Val< 11)){
          List[TIM_BASE].Val++;
          Set_Base(List[TIM_BASE].Val);   // Ӳ������ɨ��ʱ����λ
          WaveProcess();
                 Update_Proc_All();
          HAL_Delay(50);
        }
      }

      if(Tmp1 < 20){
        List[TIM_BASE].Val=5; //50us
        Set_Base(List[TIM_BASE].Val);   // Ӳ������ɨ��ʱ����λ
        HAL_Delay(50);
      }
       Update_Proc_All();
  }
}
