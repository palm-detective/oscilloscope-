/*
 * Fat12.c
 *
 *  Created on: May 31, 2021
 *      Author: paul_
 */

#include "Fat12.h"
#include "Ext_flash.h"

#define OK           0             //
#define SEC_ERR      1             //
#define FAT_ERR      2             //
#define OVER         3             //
#define NEW          4             //

typedef struct
{
  unsigned FAT1_BASE;          // FAT1ַ
  unsigned FAT2_BASE;          // FAT2ַ
  unsigned ROOT_BASE;          //
  unsigned FILE_BASE;          //
  unsigned FAT_LEN;
  unsigned SEC_LEN;            //
  unsigned FAT_END;            //
  char     FAT1_SEC;           // FAT1
  char     FAT2_SEC;
}FAT_InitTypeDef;

//-------FLASH-----W25Q64BV----8 Mbyte---------------------------------------------//
FAT_InitTypeDef FAT_V ={0x1000, 0x2000, 0x3000, 0x7000, 0x1000, 0x1000, 0x7FF, 1, 1 };

char Clash = 0;

/*******************************************************************************
(256 Bytes)
*******************************************************************************/
char ReadDiskData(char* pBuffer, unsigned ReadAddr, short Length)
{
  char n = 0;

  while(1)
  {
    Clash = 0;
    ExtFlash_PageRD(pBuffer, ReadAddr, Length);
    if(n++ > 6)
    	return SEC_ERR;     //
    if(Clash == 0)
    	return OK;       //
  }
}

/*******************************************************************************

*******************************************************************************/
char OpenFileRd(char* pBuffer, char* pFileName, short* pCluster, unsigned* pDirAddr)
{
  short i, n;

  *pCluster = 0;
  for(*pDirAddr=FAT_V.ROOT_BASE; *pDirAddr<FAT_V.FILE_BASE; )
  {
    if(ReadDiskData(pBuffer, *pDirAddr,FAT_V.SEC_LEN)!= OK)
    	return SEC_ERR;
    for(n=0; n<FAT_V.SEC_LEN; n+=32)
    {   //?????????????????????256
      for(i=0; i<11; i++)
      {
        if(pBuffer[n + i]!= 0)
        {
          if(pBuffer[n + i]!= pFileName[i])
        	  break;
          if(i == 10)
          {                             //
            *pCluster = *(short*)(pBuffer + n + 0x1A); //
            return OK;
          }
        }
        else
        	return NEW;               //
      }
      *pDirAddr += 32;
    }
  }
  return OVER;
}

/*******************************************************************************
 д����ҳ��(256 Bytes)  ����USB��д��ͻ����д
*******************************************************************************/
char ProgDiskPage(char* pBuffer, int ProgAddr)
{
  char n = 0;

  while(1){
    Clash = 0;
    ExtFlash_PageWR(pBuffer, ProgAddr);
    if(n++ > 6) return SEC_ERR;     // ��ʱ������
    if(Clash == 0) return OK;       // �޳�ͻ�����򷵻�
  }
}

/*******************************************************************************
 ���ҿ��дغţ�����ʱָ��ָ����һ�����дغţ���ǰ�غű�����ָ��+1��λ��
*******************************************************************************/
char SeekBlank(char* pBuffer, short* pCluster)
{
  short  Tmp;
  char   Buffer[2];
  char   Tmp_Flag = 1;

  *(pCluster+1)= *pCluster;                                    // ���浱ǰ�غ�

  for(*pCluster=0; (*pCluster)<4095; (*pCluster)++){
    if(ReadDiskData(Buffer, FAT_V.FAT1_BASE +(*pCluster)+(*pCluster)/2, 2)!= 0) return SEC_ERR;
    Tmp = ((*pCluster)& 1)?((*(short*)Buffer)>>4):((*(short*)Buffer)& 0xFFF);

    if((Tmp == 0)&&(Tmp_Flag == 0)&&(((*pCluster))!= *(pCluster+1))) {
      Tmp_Flag = 1;
      return OK;
    }
    if((Tmp == 0)&&(Tmp_Flag == 1))  {
      *(pCluster+2) = *pCluster;
      Tmp_Flag = 0;
    }
  }
  return OK;
}

/*******************************************************************************
 ����һ�غ�д��FAT��ǰ������λ������ʱָ��ָ����һ�غţ�ָ��+1Ϊ��ǰ�غ�
*******************************************************************************/
char SetCluster(char* pBuffer, short* pCluster)
{
  short  Offset, i, k;
  int  SecAddr;

  i = *(pCluster+1);                    // ��ȡԭ��ǰ�غ�
  k = *pCluster;                        // ��ȡ��һ�غ�
  Offset = i+ i/2;
  SecAddr = FAT_V.FAT1_BASE +(Offset & 0xF000 );
  Offset &= 0x0FFF;
  if(ReadDiskData(pBuffer, SecAddr, FAT_V.SEC_LEN)!= 0) return SEC_ERR; //???????????? old=258
  if(i & 1){
    pBuffer[Offset  ]=(pBuffer[Offset]& 0x0F)+((k <<4)& 0xF0);
    pBuffer[Offset+1]= k >>4;
  } else {
    pBuffer[Offset  ]= k & 0xFF;
    pBuffer[Offset+1]=(pBuffer[Offset+1]& 0xF0)+((k>>8)& 0x0F);
  }

  if(ProgDiskPage(pBuffer, SecAddr)!= 0) return SEC_ERR;
  return OK;
}

/*******************************************************************************
 ������һ�����Ӵغź󷵻أ���ǰ�غű�����ָ��+1��λ��
*******************************************************************************/
char NextCluster(short* pCluster)
{
  short FatNum;
  int Addr;

  Addr=FAT_V.FAT1_BASE +(*pCluster + *pCluster/2);
  *(pCluster+1)= *pCluster;                                   // ����ǰһ���غ�
  *pCluster = 0;
  if((*(pCluster+1) >=FAT_V.FAT_END)||(*(pCluster+1)< 2)) return SEC_ERR;
  if(ReadDiskData((char*)&FatNum, Addr, 2)!= OK) return SEC_ERR;
  *pCluster= (*(pCluster+1) & 1)?(FatNum >>4):(FatNum & 0xFFF); // ָ����һ���غ�
  return OK;
}

/*******************************************************************************
 д�ļ�����(512/4096 Bytes)����д��ǰFAT�����ز��ҵ�����һ���غ�
*******************************************************************************/
char ProgFileSec(char* pBuffer, short* pCluster)
{
  short Tmp;
  int ProgAddr = FAT_V.FILE_BASE + FAT_V.SEC_LEN*(*pCluster-2);
  if(ProgDiskPage(pBuffer, ProgAddr)!= OK) return SEC_ERR; // д���� 4096 Byte �� дǰ������

  if(NextCluster(pCluster)!=0) return FAT_ERR;                 // ȡ��һ���غ�
  Tmp = *(pCluster+1);
  if(*pCluster == 0){
    *pCluster = Tmp;
    if(SeekBlank (pBuffer, pCluster )!= OK) return OVER;
    if(SetCluster(pBuffer, pCluster )!= OK) return SEC_ERR;
  }
  return OK;
}

/*******************************************************************************
 ���ļ�����(512 Bytes), ����ʱָ��ָ����һ���غţ���ǰ�غű�����ָ��+1��λ��
*******************************************************************************/
char ReadFileSec(char* pBuffer, short* pCluster)
{
  int ReadAddr =FAT_V.FILE_BASE + FAT_V.SEC_LEN*(*pCluster-2);

  if(ReadDiskData(pBuffer, ReadAddr, FAT_V.SEC_LEN)!=OK) return SEC_ERR;  //(SEC_LEN=4096/512)
  if(NextCluster(pCluster)!=0) return FAT_ERR;                 // ȡ��һ���غ�
  return OK;
}

/*******************************************************************************
 �ر��ļ�����������д��FAT�����ļ�����д��Ŀ¼�����FAT1��FAT2
*******************************************************************************/
char CloseFile(char* pBuffer, int Lenght, short* pCluster, int* pDirAddr)
{
  int n;

  n=0xFFF;
  *pCluster = 0xFFF;
  SetCluster(pBuffer, pCluster);

  if(ReadDiskData(pBuffer, (*pDirAddr &(~n)), FAT_V.SEC_LEN)!= OK) return SEC_ERR;
  *(char* )(pBuffer +(*pDirAddr & n)+ 0x0B)= 0x20;
  *(int*)(pBuffer +(*pDirAddr & n)+ 0x1C)= Lenght;
  if(ProgDiskPage(pBuffer, (*pDirAddr &(~n)))!= OK) return SEC_ERR;
    if(ReadDiskData(pBuffer, FAT_V.FAT1_BASE, FAT_V.SEC_LEN)!= OK) return SEC_ERR;
    if(ProgDiskPage(pBuffer, FAT_V.FAT2_BASE     )!= OK) return SEC_ERR;
 return OK;
}

/*******************************************************************************
 дģʽ���ļ��������ļ���һ���غż�Ŀ¼���ַ
*******************************************************************************/
char OpenFileWr(char* pBuffer, char* pFileName, short* pCluster, int* pDirAddr)
{
  int i, n,offset;

  i = OpenFileRd(pBuffer, pFileName, pCluster, (unsigned *)pDirAddr);
  if(i != NEW) return i;
  else{                                                    // ��ǰ��Ϊ�հ�Ŀ¼��
    if(SeekBlank(pBuffer, pCluster)!= OK) return OVER;     // ��FAT��������
      n =*pDirAddr & 0xFFF;
    offset=*pDirAddr-n;
    if(ReadDiskData(pBuffer,offset, FAT_V.SEC_LEN)!= OK) return SEC_ERR;
    for(i=0; i<11; i++) pBuffer[n+i]= pFileName[i];      // ������Ŀ¼��offset +
    *(short*)(pBuffer + n + 0x1A)= *pCluster;
    if(ProgDiskPage(pBuffer,offset)!= OK) return SEC_ERR;
    return OK;
  }
}
