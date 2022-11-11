#ifndef __UPF_PRIVATE_H__
#define __UPF_PRIVATE_H__

#include <FreeRTOS.h>
#include<FreeRTOSConfig.h>
#include <task.h>
#include <semphr.h>
#include "CMSIS/CMSDK_CM4.h"
#include "CMSIS/core_cm4.h"

#include "platform.h"



#define MAIN_DOMAIN_PU_ACK_IRQ			(0)
#define MAIN_DOMAIN_PD_ACK_IRQ			(1)

#define PMC_RESET_VECTOR_ADDRESS		(0x100000)

#define TIMEOUT_MS						(10UL)

extern SemaphoreHandle_t xPowerDown_Semaphore;

void ddr_retention_save(void);
void ddr_retention_load(void);

void upf_set_power_down_bit(void);
void upf_set_retention_done_bit(void);

void Send_Cmd_To_PMIC(char cmd);

void vDoPowerdownTask( void *pvParameters );
void vDoPowerupTask( void *pvParameters );

#endif