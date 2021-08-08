/*
 * Fat12.h
 *
 *  Created on: May 31, 2021
 *      Author: paul_
 */

#ifndef INC_FAT12_H_
#define INC_FAT12_H_

char   OpenFileRd(char* Buffer, char* FileName, short* Cluster, unsigned* pDirAddr);
char   ReadFileSec(char* Buffer, short* Cluster);
char CloseFile(char* pBuffer, int Lenght, short* pCluster, int* pDirAddr);
char OpenFileWr(char* pBuffer, char* pFileName, short* pCluster, int* pDirAddr);
char ProgFileSec(char* pBuffer, short* pCluster);

#endif /* INC_FAT12_H_ */
