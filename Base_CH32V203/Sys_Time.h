#ifndef _SYS_TIME_H__
#define _SYS_TIME_H__

/*
本文件属于MCU专属文件，每个MCU不同，这里的内容就会不同 ———— 需要系统提供时钟
这个文件是给Time的，也就是说你得找一个能满足以下条件时钟来作为动力  （选一个幸运儿做苦力）。
    滴答定时器（首选）
                                            2022.02.28
    或者任意16位及以上定时器
                                            2022.07.04
    有中断
    
为了方便上层统一调用，本文件需要提供一个 【宏函数群】上层使用
也就是说，无论是什么MCU【宏函数群】的内容可以不一样，但是【宏】必须保持一致
    底层需要提供
    1、时钟中断函数宏 SYS_Time_Interrupt （10us进入一次）
    2、中断标志位（可以没有）
    3、中断标志位清除（可以没有）
    
    总结：提供 初始化 & 控制【函数群】
*/

#include "Items.h"

#define Base_SysTick        //使用 滴答定时器 作为【系统时钟】

/* 【宏函数群】   */

#define Exist_SYS_Time_Falg         //存在 中断标志位

#ifdef Exist_SYS_TIME
    #ifdef Base_SysTick
        void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
        #define SYS_Time_Interrupt() SysTick_Handler()
    #else
        void TIM4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));      //很关键
        #define SYS_Time_Interrupt() TIM4_IRQHandler()
    #endif

    #ifdef Exist_SYS_Time_Falg
        #ifdef Base_SysTick
            #define SYS_Time_Interrupt_Flag() SysTick->SR
            #define SYS_Time_Interrupt_FlagClear() SysTick->SR = 0
        #else
            #define SYS_Time_Interrupt_Flag() TIM_GetITStatus(TIM4, TIM_IT_Update)
            #define SYS_Time_Interrupt_FlagClear() TIM_ClearFlag(TIM4, TIM_IT_Update)
        #endif
    #endif
#endif
/*  end   */

#define Frequency 100000 //目前是 10us

void Sys_Time_Init (int Set);

#endif
