/*
 * Ext_flash.h
 *
 *  Created on: Jun 4, 2021
 *      Author: paul_
 */

#ifndef INC_EXT_FLASH_H_
#define INC_EXT_FLASH_H_

#define RDSR       0x05     // Read Status Register instruction
#define READ       0x03     // Read from Memory instruction
#define Dummy_Byte 0xA5
#define PP         0x02     // Write to Memory instruction
#define SE         0x20     // Sector 4K Erase instruction
#define WREN       0x06     // Write enable instruction

#define WIP_Flag   0x01     // Write In Progress (WIP) flag

void ExtFlash_WaitForWriteEnd(void);
void ExtFlash_PageRD(char* pBuffer, unsigned ReadAddr, short Length);
void ExtFlash_PageWR(char* pBuffer, unsigned WriteAddr);

#endif /* INC_EXT_FLASH_H_ */
