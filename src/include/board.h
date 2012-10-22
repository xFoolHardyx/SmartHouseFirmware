//----------------------------------------------------------------------------------------------------
//         ATMEL Microcontroller Software Support  -  ROUSSET  -
//----------------------------------------------------------------------------------------------------
// The software is delivered "AS IS" without warranty or condition of any
// kind, either express, implied or statutory. This includes without
// limitation any warranty or condition with respect to merchantability or
// fitness for any particular purpose, or against the infringements of
// intellectual property rights of others.
//----------------------------------------------------------------------------------------------------
// File Name:  Board.h
// Object:  AT91SAM7S Evaluation Board Features Definition File.
//
// Creation: JPP   16/June/2004
//----------------------------------------------------------------------------------------------------
#ifndef Board_h
#define Board_h
#include "AT91SAM7S128.h"
#define __inline inline
#define true 1
#define false 0
//-----------------------------------------------
// SAM7Board Memories Definition
//-----------------------------------------------
// The AT91SAM7S2564 embeds a 64-Kbyte SRAM bank, and 256 K-Byte Flash
#define  INT_SRAM 0x00200000
#define  INT_SRAM_REMAP 0x00000000
#define  INT_FLASH 0x00000000
#define  INT_FLASH_REMAP 0x01000000
#define  FLASH_PAGE_NB 1024
#define  FLASH_PAGE_SIZE 256
//------------------------
// Leds Definition
//------------------------
#define LED4 (1<<3) // PA3 (pin 1 on EXT connector)
#define LED_MASK (LED4)
//--------------
// Master Clock
//--------------
#define EXT_OC 18432000 // Exetrnal ocilator MAINCK
#define MCK 47923200 // MCK (PLLRC div by 2)
#define MCKKHz (MCK/1000)      //
#endif   // Board_h
