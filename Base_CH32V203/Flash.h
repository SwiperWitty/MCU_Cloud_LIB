/*
 * Flash.h
 *
 *  Created on: 2022年8月1日
 *      Author: gxwl
 */

#ifndef __FLASH_H_
#define __FLASH_H_

#include "Items.h"

#ifdef Exist_FLASH
    #define FLASH_PAGE_SIZE ((uint32_t)256)         //一页的大小
    #define FLASH_AREA_SIZE (FLASH_PAGE_SIZE*16)    //区大小
    #define FLASH_SIZE  ((uint32_t)(64*1024))       //Flash大小64K

    #define FLASH_DATA_END  ((uint32_t)0x08009000)              //掉电保存【数据区】结束地址
    #define FLASH_DATA      (FLASH_DATA_END - FLASH_PAGE_SIZE)  //掉电保存【数据区】只有一页(请确保这个位置不在程序段)

    #define FLASH_CODE      FLASH_DATA_END              //Flash存放【APP的代码区】
    #define FLASH_CODE_END  (0x08000000 + FLASH_SIZE)   //Flash存放【APP的代码区】一直到最大Flash

    #define FLASH_END       FLASH_CODE_END              //Flash最终地址
#endif

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

struct _Flash_DATA
{
    int addr;
    char information;
    char version;
    char baud;
    char IP;
    char GPIO;
};

extern const struct _Flash_DATA Flash_DATA;

int Read_Flash(int Address);
char Clear_Flash_Area(int Area);

#endif
