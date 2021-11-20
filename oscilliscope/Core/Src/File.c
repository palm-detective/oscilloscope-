/*
 * File.c
 *
 *  Created on: May 30, 2021
 *      Author: paul_
 */

#include "File.h"
#include "ff.h"
#include "fatfs.h"
#include "Menu.h"
#include "Func.h"
#include "Process.h"
#include "Draw.h"
#include "Display.h"
#include "Lcd_ctl.h"
#include <string.h>

#define OK           0             //

const char Vol_Str[][8] ={" 10mV  "," 20mV  "," 50mV  "," 0.1V  "," 0.2V  ",
                          " 0.5V  "," 1.0V  "," 2.0V  "," 5.0V  "," 10V   "};
const char Vol_10X[][8] ={" 0.1V  "," 0.2V  "," 0.5V  "," 1.0V  "," 2.0V  ",
                          " 5.0V  "," 10V   "," 20V  "," 50V  "," 100V  ",};
const char VScale_Str[][6] ={"10",   "20",   "50",   "100",  "200",
                          "500",  "1000", "2000", "5000", "10000"};
const char VScale_Str_10X[][6] ={"100",   "200",   "500",   "1000",  "2000",
                          "5000",  "10000", "20000", "50000", "100000"};
const char TScale_Str[][8] ={ "1",    "2",     "5",     "10",    "20",   "50",   "100",
                           "200",  "500",   "1000",  "2000",  "5000", "10000","20000",
                           "50000","100000","200000","500000","1000000", "2000000"};

const unsigned short BMP_Color[16] = {WHT,  CYN, CYN_,  YEL,  YEL_, PUR, PUR_, GRN,
                      GRN_, GRY, ORN, BLU, RED, BLK, LGN,  DAR};

const char BmpHead[54]   = {0X42, 0X4D, 0X76, 0X96, 0X00, 0X00, 0X00, 0X00,
                      0X00, 0X00, 0X76, 0X00, 0X00, 0X00, 0X28, 0X00,
                      0X00, 0X00, 0X40, 0X01, 0X00, 0X00, 0XF0, 0X00,
                      0X00, 0X00, 0X01, 0X00, 0X04, 0X00, 0X00, 0X00,
                      0X00, 0X00, 0X82, 0X0B, 0X00, 0X00, 0X12, 0X0b,
                      0X00, 0X00, 0X12, 0X0b, 0X00, 0X00, 0X00, 0X00,
                      0X00, 0X00, 0X00, 0X00, 0X00, 0X00};

const short CLK_TAB[44] =
  {0x000,0x000,0x000,0x000,0x070,0x3FF,0x070,0x000,0x000,0x000,0x000,
   0x000,0x002,0x004,0x008,0x070,0x070,0x070,0x080,0x100,0x200,0x000,
   0x020,0x020,0x020,0x020,0x070,0x070,0x070,0x020,0x020,0x020,0x020,
   0x000,0x200,0x100,0x080,0x070,0x070,0x070,0x008,0x004,0x002,0x000};


char  F_Buff[4096+60],F_Sector[512];
short Addr = 0, Length = 0;
long  Svg_Cnt = 0;
char  Num[4];
int   Clk_Cnt = 0;

/*******************************************************************************
Color_Num: �����ǰ��ɫ�Ķ�Ӧ��ɫ����
*******************************************************************************/
char Color_Num(short Color)
{
  if(Color == WHT)                  return 0;
  else if((Color & CYN  )== CYN  )  return 1;
  else if((Color & CYN_ )== CYN_ )  return 2;
  else if((Color & YEL  )== YEL  )  return 3;
  else if((Color & YEL_ )== YEL_ )  return 4;
  else if((Color & PUR  )== PUR  )  return 5;
  else if((Color & PUR_ )== PUR_ )  return 6;
  else if((Color & GRN  )== GRN  )  return 7;
  else if((Color & GRN_ )== GRN_ )  return 8;
  else if((Color & GRY  )== GRY  )  return 9;
  else if((Color & ORN  )== ORN  )  return 10;
  else if((Color & BLU  )== BLU  )  return 11;
  else if((Color & RED  )== RED  )  return 12;
  else if((Color & BLK  )== BLK  )  return 13;
  else if((Color & LGN  )== LGN  )  return 14;
  else                              return 15;
}

/*******************************************************************************
 Print_Clk: ����ָʾ
*******************************************************************************/
void PrintClk(short x0, short y0, char Phase)
{
  short i, j, b;
  SetColor(GRY, TCOLOR[FN]);
  x0=S_X[SAVE_BMP]+30;
  DispStr(S_X[SAVE_BMP]-3, LABLE_Y, PRN, "             ");
  for(i=1; i<11; ++i){
    b = CLK_TAB[Phase *11 + i];
    for(j=0; j<11; ++j){
      LCD_Set_Posi((x0+j), (y0+i));
      if((b >> j)& 0x001)
        LCD_Set_Pixel(BLU);
      else
        LCD_Set_Pixel(Background);
    }
  }
}

/*******************************************************************************
 WriteVar
*******************************************************************************/
void WriteVar(short Var, short* pClu)
  {
    short temp_addr,temp_len;
    unsigned j;

    memset(Num, 0, 4);
    u16ToDec4Str(Num,Var);
    Length = strlen((char const*)Num);
    memcpy(&F_Buff[Addr], Num, Length);
    Addr = Addr + Length;
    Svg_Cnt = Svg_Cnt+ Length;
    if(Addr >=4096)
    {
      f_write(&USERFile, F_Buff, 4096, &j);
      PrintClk(264,2,(Clk_Cnt++ >>1)& 3);  // ����ָʾ
      temp_len = Addr -4096;
      temp_addr = Length - temp_len;
      Addr = 0;
      memset(F_Buff,0,4096);
      memset(Data,0,310);
      memcpy(&F_Buff[Addr],Num + temp_addr, temp_len);
      Addr = Addr + temp_len;
      Length = 0;
    }
  }

/*******************************************************************************
 WriteStr
*******************************************************************************/
void WriteStr(char* Str, short* pClu)
  {
    short temp_addr,temp_len;
    unsigned j;

    Length = strlen((char const*)Str);
    memcpy(&F_Buff[Addr], Str, Length);
    Addr = Addr + Length;
    Svg_Cnt = Svg_Cnt+ Length;
    if(Addr >=4096)
    {
        f_write(&USERFile, F_Buff, 4096, &j);
      PrintClk(264,2,(Clk_Cnt++ >>1)& 3);     // ����ָʾ
      temp_len = Addr -4096;
      temp_addr = Length - temp_len;
      Addr = 0;
      memset(F_Buff,0,4096);
      memset(Data,0,310);
      memcpy(&F_Buff[Addr],Str + temp_addr, temp_len);
      Addr = Addr + temp_len;
      Length = 0;
    }
  }

/*******************************************************************************

*******************************************************************************/
void Make_Filename(short FileNo, char* FileName)
{
  char Num[4];
  u8ToDec3(Num, FileNo);
  FileName[4] = Num[0];
  FileName[5] = Num[1];
  FileName[6] = Num[2];
}

/*******************************************************************************
Load_File_Num(char Tpye)
*******************************************************************************/
short Load_File_Num(char Type)
{
  short FileNo=0;
  char  pFileName[13]="FILE       ";

  switch(Type)
  {
  case SAVE_BMP:
    memcpy(pFileName,"IMG_   .BMP",12);
    break;
  case SAVE_DAT:
    memcpy(pFileName,"FILE   .DAT",12);
    break;
  case SAVE_BUF:
    memcpy(pFileName,"DATA   .BUF",12);
    break;
  case SAVE_CSV:
    memcpy(pFileName,"DATA   .CSV",12);
    break;
  case SAVE_SVG:
    memcpy(pFileName,"DATA   .SVG",12);
    break;
  }
    Make_Filename(FileNo, pFileName);
    while( f_open( &USERFile, pFileName, FA_OPEN_EXISTING)==FR_OK)
    {
      f_close(&USERFile);
      FileNo++;
      Make_Filename(FileNo, pFileName);
    }

  return FileNo;
}

/*******************************************************************************
void File_Num(void)
*******************************************************************************/
void File_Num(void)
{
  List[SAVE_BMP].Val =  Load_File_Num(SAVE_BMP);
  List[SAVE_DAT].Val =  Load_File_Num(SAVE_DAT);
  List[SAVE_BUF].Val =  Load_File_Num(SAVE_BUF);
  List[SAVE_CSV].Val =  Load_File_Num(SAVE_CSV);
  List[SAVE_SVG].Val =  Load_File_Num(SAVE_SVG);
}

/*******************************************************************************
Restore_Scene: ���زɼ����ݻ�����ʱ�ƻ����ֳ����ָ�����ʱ���ָ��ֳ�
*******************************************************************************/
void Restore_Scene(void)
{
  short *ptr;

  ptr=(short*)F_Sector;
  *ptr++=0x0;
  Sampl[0]=*ptr++;
  Sampl[1]=*ptr++;
  List[Y_RANGES].Val=*ptr++;
  List[PROBEATT].Val=*ptr++;
  List[SMPL_DPTH].Val=*ptr++;
}

/*******************************************************************************
Load_Buf: ���ر�����Ĳɼ����ݻ�����    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
char Load_Buf(short FileNo)
{
  char pFileName[13]="DATA   .BUF";
  short *ptr;
  int i,Rvalue,n;
  unsigned j;

  Make_Filename(FileNo, pFileName);

  Rvalue =f_open( &USERFile, pFileName, FA_OPEN_EXISTING |FA_READ );
  if(Rvalue !=FR_OK)
	  return Rvalue;
  n=2;
  if(List[SMPL_DPTH].Val==3)
	  n =4 ;//8K
  for(i=0;i<n;i++)
  {
    if(f_read(&USERFile, F_Buff, 4096, &j)!= FR_OK)
    	return FILE_RW_ERR;
    if(j != 4096)
    	return FILE_RW_ERR;
    memcpy(&(Sampl[i*4096/2+2]),F_Buff,4096);
  }
  if(f_read(&USERFile, F_Buff, 4096, &j)!= FR_OK)
  	return FILE_RW_ERR;
  f_close(&USERFile);

  ptr=(short*)F_Sector;                               //�����ֳ�
  *ptr++=0xaa55;
  *ptr++=Sampl[0];
  *ptr++=Sampl[1];
  *ptr++=List[Y_RANGES].Val;
  *ptr++=List[PROBEATT].Val;
  *ptr++=List[SMPL_DPTH].Val;
  ptr =(short*)F_Buff;                            //��ԭ���Ĳ���
  Sampl[0]=*ptr++;
  Sampl[1]=*ptr++;
  List[Y_RANGES].Val=*ptr++;
  List[PROBEATT].Val=*ptr++;
  List[SMPL_DPTH].Val=*ptr++;
  Status |=  STOP;
  DispStatus();
  if(List[LOAD_BUF].Val<99)
	  List[LOAD_BUF].Val++;
  return 0;
}

/*******************************************************************************
Save_Buf: ����ɼ����ݻ�����ΪBUF��ʽ    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
char Save_Buf(short FileNo)
{
  char pFileName[13]="DATA   .BUF";
  int i;
  short *p;
  char  l=0;
  short n;
  unsigned j;

  n=2;

  if(List[SMPL_DPTH].Val==3) n =4 ;//8K

  Make_Filename(FileNo, pFileName);
  if(f_open(&USERFile, pFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ)!=FR_OK)
	  return DISK_RW_ERR;

  for(i=0; i<n; i++)
  {
    memcpy(F_Buff,&(Sampl[i*4096/2+2]),4096);
    if(f_write( &USERFile, F_Buff, 4096, &j)!= FR_OK) return FILE_RW_ERR; // д������
    PrintClk(264,2,(l++ >>1) & 3);                    // ����ָʾ
  }
  memset(F_Buff,0,4096);
  p =(short*)&F_Buff;
  *p++=Sampl[0];
  *p++=Sampl[1];
  *p++=List[Y_RANGES].Val;
  *p++=List[PROBEATT].Val;
  *p++=List[SMPL_DPTH].Val;
  if(f_write( &USERFile, F_Buff, 12, &j)!= FR_OK) return FILE_RW_ERR; // д������
  f_close(&USERFile);
  if(List[SAVE_BUF].Val<99)
	  List[SAVE_BUF].Val++;
  return OK;
}

/*******************************************************************************
Save_Dat: ���浱ǰ��Ļ��ʾͼ��ԭʼ����    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
char Save_Dat(short FileNo)
{
  char pFileName[13]="FILE   .DAT";
  unsigned j;

  Make_Filename(FileNo, pFileName);
  memset(F_Buff,0,4096);
  if(f_open(&USERFile, pFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ)!=FR_OK)
	  return DISK_RW_ERR;
  F_Buff[0]  = 1;              //Ver0001
  F_Buff[1]  = List[V0_POSI].Val;
  F_Buff[2]  = List[EXT_POSI].Val;
  F_Buff[3]  = List[TIM_BASE].Val;
  F_Buff[4]  = List[Y_RANGES].Val;
  F_Buff[5]  = List[COUPLING].Val;
  F_Buff[6]  = List[PROBEATT].Val;
  F_Buff[7]  = List[TR_MODE].Val;

  memcpy(&F_Buff[10], WaveBuf, 300);
  if(f_write( &USERFile, F_Buff, 310, &j)!= FR_OK) return FILE_RW_ERR; // д������
  f_close(&USERFile);
  if(List[SAVE_DAT].Val<99)List[SAVE_DAT].Val++;
  return OK;
}

/*******************************************************************************
Load_Dat: ���ر��������Ļͼ��ԭʼ����    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
char Load_Dat(short FileNo)
{
  char pFileName[13]="FILE   .DAT";
  short  Rvalue;
  int i;
  unsigned j;

  Make_Filename(FileNo, pFileName);

  Rvalue =f_open( &USERFile, pFileName, FA_OPEN_EXISTING |FA_READ );
  if(Rvalue !=FR_OK)
	  return Rvalue;

  if(f_read(&USERFile, F_Buff, 310, &j)!= FR_OK)
   	return FILE_RW_ERR;
   if(j != 310)
   	return FILE_RW_ERR;

  memcpy(Data, F_Buff, 310);

  List[TIM_BASE].Val = F_Buff[3];
  List[Y_RANGES].Val = F_Buff[4];
  List[COUPLING].Val = F_Buff[5];
  List[PROBEATT].Val = F_Buff[6];
  List[TR_MODE].Val  = F_Buff[7];

  for(i=0;i<4;i++)List[i].Flg |= UPD;
  List[EXT_REFN].Flg |= UPD;
  List[EXT_POSI].Flg |= UPD;
  List[TIM_BASE].Flg |= UPD;
  List[TR_MODE].Flg |= UPD;
  Update_Detail();
  if(List[LOAD_DAT].Val<99)List[LOAD_DAT].Val++;
  return OK;
}

/*******************************************************************************
Save_Bmp: ���浱ǰ��Ļ��ʾͼ��ΪBMP��ʽ    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
char Save_Bmp(short FileNo)
{
  char  pFileName[13]="IMG_   .BMP";
  short x, y, i=54, j,ColorH,ColorL ;
  short k=0, l=0;
  unsigned m;

  if(Current == FN)
  {
    Close_Pop();
    DrawWindow(WaveBuf, ParamTab);
  }
  Make_Filename(FileNo, pFileName);

  if(f_open(&USERFile, pFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ)!=FR_OK)
	  return DISK_RW_ERR;
  memcpy(F_Buff, BmpHead, 54);
  i = 0x0036;                                     // ��ɫ���ſ�ʼ��ַ
  for(j=0; j<16; ++j)
  {
    F_Buff[j*4 +i+0]=(BMP_Color[j] & 0xF800)>>8;  // Bule
    F_Buff[j*4 +i+1]=(BMP_Color[j] & 0x07E0)>>3;  // Green&
    F_Buff[j*4 +i+2]=(BMP_Color[j] & 0x001F)<<3;  // Red
    F_Buff[j*4 +i+3]= 0;                          // Alpha
  }
  i = 0x0076;                                     // ͼ�����ݿ�ʼ��ŵ�ַ
  for(y=0; y<240; y++)
  {
    for(x=0; x<320 ; x+=2)
    {
      LCD_Set_Posi(x, y);
      ColorH = LCD_Get_Pixel();
      LCD_Set_Posi(x+1, y);
      ColorL = LCD_Get_Pixel();
      F_Buff[i] =(Color_Num(ColorH)<<4)+ Color_Num(ColorL);
      i++;
      if(i>=4096)
      {
        i=0;
        if(f_write( &USERFile, F_Buff, 4096, &m)!= FR_OK) return FILE_RW_ERR; // д������
        if(l==0)
        	PrintClk(168,2,(k++ >>1)& 3);         // ����ָʾ
        l++;
        if(l>=2)l=0;
      }
    }
  }
  if(i!=0)
  {
      if(f_write( &USERFile, F_Buff, i, &m)!= FR_OK)
      {
    	  List_Pop();
    	  return FILE_RW_ERR;
      } // д������
  }
  if(f_close(&USERFile)!= FR_OK )
	  return FILE_RW_ERR;
  if(Current == FN)
	  List_Pop();
  if(List[SAVE_BMP].Val<99)
	  List[SAVE_BMP].Val++;

  return OK;
}

/*******************************************************************************
Save_Csv: ����ɼ����ݻ�����ΪCSV��ʽ    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
char Save_Csv(short FileNo)
{

  int Ak = (KgA[KindA+(StateA?1:0)]*4)/GK[GainA];

  char pFileName[13]="DATA   .CSV";
  short i,k=0,l=0;
  unsigned j;
  char Num[4],track[4];
  short temp;
  short count;
  char* ptr;
  char* buf;

  Make_Filename(FileNo, pFileName);
  if(f_open(&USERFile, pFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ)!=FR_OK)
	  return DISK_RW_ERR;
  memcpy(&F_Buff[0],TimeBase_Str[List[TIM_BASE].Val],5);
  memcpy(&F_Buff[5],"   ",3);
  buf=&F_Buff[8];
  //------------------------------------------------------------------
    i=0;
    ptr=(char*)&R[List[Y_RANGES].Val];
    while(*ptr != 0)
    	buf[i++] = *ptr++;
    buf[i++]   = 0x2a;
    ptr=(char*)&N[List[PROBEATT].Val];
    while(*ptr != 0)
    	buf[i++] = *ptr++;
    buf[i]   = 0x2c;
    buf[i+1] = 0;
    count = i+1;

  k=8+count;
  memcpy(&F_Buff[k],"\r\n",2);
  k+=2;
  for(i=0; i<DEPTH[List[SMPL_DPTH].Val]; i++)
  {
    temp = ((((Sampl[2+i])-2048)*Ak)>>12)+100;

    if(temp > 0)
    {
      if(temp > 200)  track[0] = 199;
      else            track[0] = temp;
    } else            track[0] = 0;
    u8ToDec3(Num,track[0]);
    for(count=0; count<3; count++)
    {
      if(Num[count] == 0) break;
      F_Buff[k++] = Num[count];
      if(k >= 4096)
      {
        if(f_write( &USERFile, F_Buff, 4096, &j)!= FR_OK) return FILE_RW_ERR; // д������
        PrintClk(264,2,(l++ >>1) & 3);                    // ����ָʾ
        k = 0;
      }
    }
    F_Buff[k++] = 0x2c;
    F_Buff[k++] = 0x0d;
    F_Buff[k++] = 0x0a;
    if(k >= 4096)
    {
      if(f_write( &USERFile, F_Buff, 4096, &j)!= FR_OK) return FILE_RW_ERR; // д������
      PrintClk(264,2,(l++ >>1)& 3);                    // ����ָʾ
      k = k - 4096;
      memcpy(&F_Buff[k], &F_Buff[k+4096], k);
    }
  }
  if(k != 0)
  {
    F_Buff[k++]=0x0d;
    F_Buff[k++]=0x0a;
    memset(&F_Buff[k],32,(4096-k)); //32->�ո�
    if(f_write( &USERFile, F_Buff, 4096, &j)!= FR_OK) return FILE_RW_ERR; // д������
    PrintClk(264,2,(l++ >>1)& 3);                     // ����ָʾ
  }
  if(f_close(&USERFile)!= FR_OK )
	  return FILE_RW_ERR;
  if(List[SAVE_CSV].Val<99)List[SAVE_CSV].Val++;
  return OK;
}

/*******************************************************************************
Save_Svg: ����ɼ����ݻ�����ΪBUF��ʽ    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
char Save_Svg(short FileNo)
{
  char pFileName[13]="DATA   .SVG";
  short pCluster[3];
  short track[4];
  int l = 0;
  short i,x, y;
  short temp;
  char  *str;
  short Posi1 = 0;
  int Ak = (KgA[KindA+(StateA?1:0)]*4)/GK[GainA];
  unsigned j;

  Svg_Cnt = 0;
  Addr = 0, Length = 0;
  str = (char*)&Data;
  memset(Data,0,310);
  memset(F_Buff,0,4096);
  Make_Filename(FileNo, pFileName);
  if(f_open(&USERFile, pFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ)!=FR_OK)
	  return DISK_RW_ERR;
  //SVG �ĵ���ĳЩ��������SVG ,ָ���ⲿ��DTD
  str = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
  WriteStr(str, pCluster);
  str = "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n";
  WriteStr(str, pCluster);
  str = "<svg width=\"";
  WriteStr(str, pCluster);
  WriteVar(DEPTH[List[SMPL_DPTH].Val], pCluster);
  str = "\" height=\"200\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" onload=\"init(evt)\" onmousemove=\"msMove(evt)\">\n";
  WriteStr(str, pCluster);

  //�������С��ɫ
  str = "<path stroke=\"none\" fill=\"black\" d=\"M0,0 H";
  WriteStr(str, pCluster);
  WriteVar(DEPTH[List[SMPL_DPTH].Val], pCluster);
  str = " V200 H0 V0\"/>\n";
  WriteStr(str, pCluster);
  //��ǰ���ڱ���
  str = "<path stroke=\"none\" fill=\"#400040\" d=\"M";  //��ɫ
  WriteStr(str, pCluster);
  WriteVar(List[VIEWPOSI].Val, pCluster);
  str = ",0 H";
  WriteStr(str, pCluster);
  WriteVar(List[VIEWPOSI].Val, pCluster);
  str = " V200 H";
  WriteStr(str, pCluster);
  WriteVar(List[VIEWPOSI].Val+300, pCluster);
  str = "  V0\"/>\n";
  WriteStr(str, pCluster);
  //��������
 for (y=0; y<=200; y+=25){
    //<path stroke="#404040" d="M0,0 H4096"/>
    str = "<path stroke=\"#404040\" d=\"M0,";
    WriteStr(str, pCluster);
    WriteVar(y, pCluster);
    str = " H";
    WriteStr(str, pCluster);
    WriteVar(DEPTH[List[SMPL_DPTH].Val], pCluster);
    str = "\"/>\n";
    WriteStr(str, pCluster);
  }
  for (x=0; x<=DEPTH[List[SMPL_DPTH].Val]; x+=25)
  {
    //<path stroke="#404040" d="M0,0 V256"/>
    str = "<path stroke=\"#404040\" d=\"M";
    WriteStr(str, pCluster);
    WriteVar(x, pCluster);
    str = ",0 V200\"/>\n";
    WriteStr(str, pCluster);
  }
  //ͨ��A����
  str = "<path stroke=\"cyan\" fill=\"none\" stroke-width=\"1\" d=\"";
  WriteStr(str, pCluster);
  for (i=0; i<=DEPTH[List[SMPL_DPTH].Val]; i++)
  {
      temp = ((((Sampl[2+i])-2048)*Ak)>>12)+100;
    if(temp > 0)
    {
      if(temp > 200)  track[0] = 199;
      else            track[0] = temp;
    } else            track[0] = 0;
    track[0] =  200- track[0];
    if(i==0)str = "M";
    else    str = " L";
    WriteStr(str, pCluster);
    WriteVar(i, pCluster);
    str = ",";
    WriteStr(str, pCluster);
    WriteVar(track[0], pCluster);
  }
  str = "\"/>\n";
  WriteStr(str, pCluster);

  //�Ҷ˵�λ��ʱ������
  str = "<text font-family=\"Arial\" x=\"638\" y=\"170\" fill=\"cyan\" font-size=\"18\" text-anchor=\"end\" id=\"CH1s\">CH1: ";
  WriteStr(str, pCluster);
  if(List[PROBEATT].Val)str = (char*)Vol_10X[List[Y_RANGES].Val];
  else                         str = (char*)Vol_Str[List[Y_RANGES].Val];
  WriteStr(str, pCluster);
  str = "/div ";
  WriteStr(str, pCluster);
  str = "(";
  WriteStr(str, pCluster);
  if(List[PROBEATT].Val)str = "X10";        //X10
  else                         str = "X1";  //X1
  WriteStr(str, pCluster);
  str = ") ";
  WriteStr(str, pCluster);
  str = (char*)TimeBase_Str[List[TIM_BASE].Val]; //Timebase
  WriteStr(str, pCluster);
  str = "/div</text>\n";
  WriteStr(str, pCluster);
  str = "<text font-family=\"Arial\" x=\"2\" y=\"170\" fill=\"cyan\" font-size=\"18\" text-anchor=\"start\" id=\"CH1\"> </text>\n";
  WriteStr(str, pCluster);
 //===============================
  //����
  Posi1 = 199-List[V0_POSI].Val;
  //===============================
  str = "<script type=\"text/ecmascript\"><![CDATA[\n";
  WriteStr(str, pCluster);
  str = "function init(evt) { if ( window.svgDocument == null ) svgDocument = evt.target.ownerDocument; }\n";
  WriteStr(str, pCluster);
  str = "function fmtT(val, div) {\n";
  WriteStr(str, pCluster);
  str = "if (div > 10000) return Math.floor(val*div/25/1000) + \" ms\";\n";
  WriteStr(str, pCluster);
  str = "if (div > 250) return Math.floor(val*div/25)/1000 + \" ms\";\n";
  WriteStr(str, pCluster);
  str = "return Math.floor(val*div*1000/25)/1000 + \" us\";\n";
  WriteStr(str, pCluster);
  str = "}\n";
  WriteStr(str, pCluster);
  str = "function fmtU(val, div, shift) { return Math.floor((shift-val)*div/25) + \" mV\"; }\n";
  WriteStr(str, pCluster);
  str = "function msMove (evt)\n";
  WriteStr(str, pCluster);
  str = "{\n";
  WriteStr(str, pCluster);
  str = "var x = evt.pageX;\n";
  WriteStr(str, pCluster);
  str = "var y = evt.clientY;\n";
  WriteStr(str, pCluster);
  str = "if (x < 0 || y < 0 || x >= ";
  WriteStr(str, pCluster);
  WriteVar(DEPTH[List[SMPL_DPTH].Val], pCluster);
  str = "|| y >= 200)\n";
  WriteStr(str, pCluster);
  str = "return;\n";
  WriteStr(str, pCluster);
  str = "var hscroll = (document.all ? document.scrollLeft : window.pageXOffset);\n";
  WriteStr(str, pCluster);
  str = "var width = window.innerWidth;\n";
  WriteStr(str, pCluster);
  str = "\n";
  WriteStr(str, pCluster);
  str = "svgDocument.getElementById(\"CH1\").setAttribute(\"x\", hscroll+20);\n";
  WriteStr(str, pCluster);
  str = "svgDocument.getElementById(\"CH1s\").setAttribute(\"x\", hscroll+width-20);\n";
  WriteStr(str, pCluster);
  str = "\n";
  WriteStr(str, pCluster);
  //==============================
  str = "svgDocument.getElementById(\"CH1\").firstChild.data = \"CH1: \"+fmtU(y, ";
  WriteStr(str, pCluster);
  if(List[PROBEATT].Val)str = (char*)VScale_Str_10X[List[Y_RANGES].Val];
  else str = (char*)VScale_Str[List[Y_RANGES].Val];
  WriteStr(str, pCluster);
  str = ", ";
  WriteStr(str, pCluster);
  WriteVar((short)Posi1, pCluster);
  str = ") + \" \" + fmtT(x, ";
  WriteStr(str, pCluster);
  str = (char*)TScale_Str[List[TIM_BASE].Val]; //Timebase
  WriteStr(str, pCluster);
  str = ");\n";
  WriteStr(str, pCluster);

  //==============================
  str = "}\n";
  WriteStr(str, pCluster);
  str = "]]></script>\n";
  WriteStr(str, pCluster);
  str = "</svg>\n";
  WriteStr(str, pCluster);
  if(f_write( &USERFile, F_Buff, Length, &j)!= FR_OK) return FILE_RW_ERR; // д������
  PrintClk(264,2,(l++ >>1)& 3);                               // ����ָʾ

  if(f_close(&USERFile)!= FR_OK )
	  return FILE_RW_ERR;
  if(List[SAVE_SVG].Val<99)List[SAVE_SVG].Val++;
  memset(Data,0,310);
  return OK;
}
