#include "upf_private.h"


SemaphoreHandle_t xPowerUp_Semaphore;

void wait_loop();
void _power_up_main_domain(void)
{
    Send_Cmd_To_PMIC(0x81);
	 MOON0_REG->m0_sft_cfg[1] = 0x007E007E ;  ///maindomain powerup will reset CA55
	/*  power up finish */
	//wait_loop(10000); 
	*(volatile unsigned int*)0xf8801900 = 'Q';
	*(volatile unsigned int*)0xf8801900 = 'Q';
    PMC_REGS->pmc_main_pwr_ctrl = 0x1;
}

void _power_up_CA55_domain(void)
{

	Send_Cmd_To_PMIC(0xb2);
/// core0	
	PMC_REGS->pmc_corepsw_en   &= 0xFFFFFFFE ;
    PMC_REGS->pmc_coremem_sden &= 0xFFFFFFFE ;
    PMC_REGS->pmc_iso_en       &= 0xFFFFFFFE ;
/// core1                       
    PMC_REGS->pmc_corepsw_en   &= 0xFFFFFFFD ;
    PMC_REGS->pmc_coremem_sden &= 0xFFFFFFFD ;
    PMC_REGS->pmc_iso_en       &= 0xFFFFFFFD ;
/// core2                       
    PMC_REGS->pmc_corepsw_en   &= 0xFFFFFFFB ;
    PMC_REGS->pmc_coremem_sden &= 0xFFFFFFFB ;
    PMC_REGS->pmc_iso_en       &= 0xFFFFFFFB ;
/// core3                       
    PMC_REGS->pmc_corepsw_en   &= 0xFFFFFFF7 ;
    PMC_REGS->pmc_coremem_sden &= 0xFFFFFFF7 ;
    PMC_REGS->pmc_iso_en       &= 0xFFFFFFF7 ;
	
	PMC_REGS->pmc_pctl_reg     |= 0x000003FF ; 

    PMC_REGS->pmc_lvs_pwd = 0x00AA55FF ;  /// LVS password
    PMC_REGS->pmc_lvs_disable = 0x0 ;     /// LVS disable
    MOON0_REG->m0_sft_cfg[1] = 0x007E0000 ;  ///de-assert CA55 reset
}

#if 0
void wait_loop1(unsigned int wait_counter)
{
	unsigned int i;

	for (i = 0; i < wait_counter; i++) 
	{
		__asm__("nop");
	}
}
int _power_up_CA55_cluster()
{
	const TickType_t xTimeout = pdMS_TO_TICKS( TIMEOUT_MS );
	TickType_t xLastWakeTime;
	
	/* power up CA55, default is 0.8V. no need to set */
	Send_Cmd_To_PMIC(0xb2);
	
	wait_loop1(10000);
	/* p-channel cores request powerdown*/
	PMC_REGS->pmc_pctl_reg &= 0x1ef;
	PMC_REGS->pmc_pctl_reg |= 0x210;

	/* polling statue = accept*/
	xLastWakeTime = xTaskGetTickCount();
	do{
		/* cluster accept [19]*/
		if((PMC_REGS->pmc_pctl_reg >> 19) & 0x1 == 0x1)
			break;
		/* cluster deny [24]*/
		if((PMC_REGS->pmc_pctl_reg >> 24) & 0x1 == 0x1)
		{
			printf(" [up] cluster is in deny state PMC_REGS->pmc_pctl_reg=%x !!\n",PMC_REGS->pmc_pctl_reg);
			return -1;
		}
	}while((xTaskGetTickCount() - xLastWakeTime) < xTimeout);
	
	if((xTaskGetTickCount() - xLastWakeTime) >= xTimeout)
	{	
		printf("power down cores timeout G36.29 = %x  !!\n",PMC_REGS->pmc_pctl_reg);
		return -1;
	}
	/* disable request */
	PMC_REGS->pmc_pctl_reg &= (0x1<<9);
	
	PMC_REGS->pmc_lvs_pwd = 0x00AA55FF ;  /// LVS password
    PMC_REGS->pmc_lvs_disable = 0x0 ;     /// LVS disable

	/* assert CA55 reset */
//	MOON0_REG->m0_sft_cfg[1] 	= 0x00600000 ;

	return 0;
}

int _power_up_CA55_core()
{
	const TickType_t xTimeout = pdMS_TO_TICKS( TIMEOUT_MS );
	TickType_t xLastWakeTime;

	/* p-channel cores request powerdown*/
	PMC_REGS->pmc_pctl_reg &= 0x3F0;
	PMC_REGS->pmc_pctl_reg |= 0x1EF;

	/*config SD/PSW */	
	PMC_REGS->pmc_coremem_sdpwd = 0x5500FFAA;
	PMC_REGS->pmc_coremem_sden	&= ~0xF;
	
	PMC_REGS->pmc_corepsw_pwd   = 0x55FF00AA;
	PMC_REGS->pmc_corepsw_en    &= ~0xF;
	
	/* polling statue = accept*/
	xLastWakeTime = xTaskGetTickCount();
	do{
		/* core 0/1/2/3 accept [18:15]*/
		if((PMC_REGS->pmc_pctl_reg >> 15) & 0xF == 0xF)
			break;
		/* core 0/1/2/3 deny [23:20]*/
		if((PMC_REGS->pmc_pctl_reg >> 20) & 0xF != 0)
		{
			printf("[up] one of the cores is in deny state  %x  !!\n",PMC_REGS->pmc_pctl_reg);
			return -1;
		}
	}while((xTaskGetTickCount() - xLastWakeTime) < xTimeout);
	*(volatile unsigned int*)0xf8801900 = 'W';
	if((xTaskGetTickCount() - xLastWakeTime) >= xTimeout)
	{	
		printf("power down cores timeout G36.29 = %x  !!\n",PMC_REGS->pmc_pctl_reg);
		return -1;
	}
	/* disable request */
	PMC_REGS->pmc_pctl_reg &= (0xF<<5);
	
	/*config ISO */	
	PMC_REGS->pmc_iso_pwd       = 0xFFAA5500;
	PMC_REGS->pmc_iso_en	    &= ~0xF;
	
	MOON0_REG->m0_sft_cfg[1] = 0x007E0000 ;  ///de-assert CA55 reset
	return 0;
}
#endif

void Main_Domain_PowerUP(void)
{
    _power_up_main_domain();

#if 0
	if(_power_up_CA55_cluster() != 0)
	{
		return;
	}
	
	if(_power_up_CA55_core() != 0)
	{
		return;
	}
#endif

	_power_up_CA55_domain();
 
 	ddr_retention_load();

	upf_set_retention_done_bit();
}

/* tirgger by send 0x82 cmd to PMIC */
void Main_Domain_PowerUP_REQ_Handler()
{
	static BaseType_t xHigherPriorityTaskWoken;
	printf("UP ISR ! \n");
	Main_Domain_PowerUP();
	
	/*semap will hang until ca55 restart.*/
	if(xSemaphoreGiveFromISR( xPowerUp_Semaphore, &xHigherPriorityTaskWoken ) != pdTRUE)
	{
		printf("Semaphore Send ISR Fail! \n");
	}
	printf("Semaphore Send ISR OK! \n");
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void power_up_maindomain(void)
{
	/* register interrupt */
	xPowerUp_Semaphore = xSemaphoreCreateBinary( );
	if(xPowerUp_Semaphore != NULL)
	{
		NVIC_SetVector(MAIN_DOMAIN_PU_ACK_IRQ, (u32)Main_Domain_PowerUP_REQ_Handler);
		NVIC_EnableIRQ(MAIN_DOMAIN_PU_ACK_IRQ);
		NVIC_SetPriority(MAIN_DOMAIN_PU_ACK_IRQ, 0xE0); 
	}
}

void vDoPowerupTask( void *pvParameters )
{
    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

	power_up_maindomain();
	
    for( ;; )
    {
		if( xPowerUp_Semaphore != NULL )
		{  
			printf("power up wait!\r\n");
			if(xSemaphoreTake( xPowerUp_Semaphore, portMAX_DELAY) == pdTRUE)
			{
				printf("power up start!\r\n");
				//Main_Domain_PowerUP();
			}
		}
	}
}
