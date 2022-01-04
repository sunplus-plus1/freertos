/* MPS2 CMSIS Library
*
* Copyright (c) 2006-2016 ARM Limited
* SPDX-License-Identifier: BSD-3-Clause
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* @file     CMSDK_CM3.h
* @brief    CMSIS Core Peripheral Access Layer Header File for
*           CMSDK_CM3 Device
*
*******************************************************************************/


#ifndef CMSDK_CM4_H
#define CMSDK_CM4_H

#ifdef __cplusplus
 extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/* -------------------  Cortex-M3 Processor Exceptions Numbers  ------------------- */
  NonMaskableInt_IRQn           = -14,        /*  2 Non Maskable Interrupt          */
  HardFault_IRQn                = -13,        /*  3 HardFault Interrupt             */
  MemoryManagement_IRQn         = -12,        /*  4 Memory Management Interrupt     */
  BusFault_IRQn                 = -11,        /*  5 Bus Fault Interrupt             */
  UsageFault_IRQn               = -10,        /*  6 Usage Fault Interrupt           */
  SVCall_IRQn                   =  -5,        /* 11 SV Call Interrupt               */
  DebugMonitor_IRQn             =  -4,        /* 12 Debug Monitor Interrupt         */
  PendSV_IRQn                   =  -2,        /* 14 Pend SV Interrupt               */
  SysTick_IRQn                  =  -1,        /* 15 System Tick Interrupt           */
/******  CMSDK Specific Interrupt Numbers *********************************************************/
  PL011_UART0_IRQn              = 0,       /*!< PL011_UART0  Interrupt                               */
} IRQn_Type;


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the Cortex-M3 Processor and Core Peripherals  ------- */
#define __CM3_REV                 0x0001U     /* Core revision r0p1                              */
#define __MPU_PRESENT             0U          /* MPU present or not                              */
#define __NVIC_PRIO_BITS          4U          /* Number of Bits used for Priority Levels         */
#define __Vendor_SysTickConfig    0U          /* Set to 1 if different SysTick Config is used    */
#define __FPU_PRESENT             1U          /* FPU present or not                              */

#include <core_cm4.h>                         /* Processor and core peripherals                  */

/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

/* -------------------  Start of section using anonymous unions  ------------------ */
#if defined ( __CC_ARM   )
  #pragma push
#pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler type
#endif



#ifdef __cplusplus
}
#endif

#endif  /* CMSDK_CM3_H */
