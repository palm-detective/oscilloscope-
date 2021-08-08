/*
 * File.h
 *
 *  Created on: May 30, 2021
 *      Author: paul_
 */

#ifndef INC_FILE_H_
#define INC_FILE_H_

#define FILE_RW_ERR     3
#define DISK_RW_ERR     4

extern char F_Sector[];

void File_Num(void);
char   Save_Dat(short FileNum);
char   Load_Dat(short FileNum);
char   Save_Buf(short FileNum);
char   Load_Buf(short FileNum);
char   Save_Bmp(short FileNum);
char   Save_Csv(short FileNum);
char   Save_Svg(short FileNum);
void Restore_Scene(void);

#endif /* INC_FILE_H_ */
