#include "upf_private.h"

void Main_Domain_PowerDown_ACK_Handler(void)
{
	printf("#### Main_Domain_PowerDown_ACK_Handler \n");
	/* de-assert DDRPHY PWROKIN*/
	MOON0_REG->m0_sft_cfg[14]  = 0x00200000;

	/* power off (talk with PMIC)*/
	Send_Cmd_To_PMIC(0x80);
	
	/* power up  for test (talk with PMIC)*/
//	Send_Cmd_To_PMIC(0x82);

	*(volatile unsigned int *)0xF88011D4 = 0x1;//G35.21=0x1;
}

int power_down_maindomain()
{	
	STAMP = 0xabcd1234;
	STAMP_1 = 0xabcd1234;
	/* register interrupt */
	NVIC_SetVector(MAIN_DOMAIN_PD_ACK_IRQ, (u32)Main_Domain_PowerDown_ACK_Handler);
    NVIC_EnableIRQ(MAIN_DOMAIN_PD_ACK_IRQ);

	/* register power down interrupt*/
	PMC_REGS->pmc_main_pwr_ctrl	|= 0x2 ;
	
	/*Q-channel CTRL  G30*/
	
	/*config ISO */
	PMC_REGS->pmc_iso_pwd       = 0xFFAA5500;
	printf("power_down_maindomain,wait power down ack \n");

	return 0;
}

int power_down_cluster()
{
	const TickType_t xTimeout = pdMS_TO_TICKS( TIMEOUT_MS );
	TickType_t xLastWakeTime;
	
	printf("power down cluster in \n");

	/* p-channel cluster request powerdown*/
	PMC_REGS->pmc_pctl_reg &= 0x1ef;
	PMC_REGS->pmc_pctl_reg |= 0x200;
	
	/* polling statue = accept*/
	
	xLastWakeTime = xTaskGetTickCount();
	do{
		/* cluster accept [19]*/
		if((PMC_REGS->pmc_pctl_reg >> 19) & 0x1 == 0x1)
			break;
		/* cluster deny [24]*/
		if((PMC_REGS->pmc_pctl_reg >> 24) & 0x1 == 0x1)
		{
			printf(" [down] cluster is in deny state !!\n");
			return -1;
		}	
	}while((xTaskGetTickCount() - xLastWakeTime) < xTimeout);
		
	if((xTaskGetTickCount() - xLastWakeTime) >= xTimeout)
	{
		if((PMC_REGS->pmc_pctl_reg >> 19) & 0x1 != 0x1)
		{
			printf("\n (PMC_REGS->pmc_pctl_reg >> 19)=%x \n",(PMC_REGS->pmc_pctl_reg >> 19));
			return -1;
		}
	}
	/* disable request */
	PMC_REGS->pmc_pctl_reg &= (0x1<<9);
	
	/* assert CA55 reset */
	MOON0_REG->m0_sft_cfg[1] 	= 0x00600060 ;
	
	/*config LVS*/	
	PMC_REGS->pmc_lvs_pwd       = 0xFFAA5500;
	PMC_REGS->pmc_lvs_disable	= 0x1;

	/* power off (talk with PMIC)*/
	Send_Cmd_To_PMIC(0xb0);
	
	printf("power down cluster ok \n");
	return 0;
}

int power_down_cores()
{
	const TickType_t xTimeout = pdMS_TO_TICKS( TIMEOUT_MS );
	TickType_t xLastWakeTime;
	
	printf("power_down_cores in \n");
	/* p-channel cores request powerdown*/
	PMC_REGS->pmc_pctl_reg &= 0x3F0;
	PMC_REGS->pmc_pctl_reg |= 0x1E0;

	/* polling statue = accept*/
	xLastWakeTime = xTaskGetTickCount();
	do{
		/* core 0/1/2/3 accept [18:15]*/
		if((PMC_REGS->pmc_pctl_reg >> 15) & 0xF == 0xF)
			break;
		/* core 0/1/2/3 deny [23:20]*/
		if((PMC_REGS->pmc_pctl_reg >> 20) & 0xF != 0)
		{
			printf("[down] one of the cores is in deny state  !!\n");
			return -1;
		}	
	}while((xTaskGetTickCount() - xLastWakeTime) < xTimeout);
	
	if((xTaskGetTickCount() - xLastWakeTime) >= xTimeout)
	{	
		printf("power down cores timeout G36.29 = %x  !!\n",PMC_REGS->pmc_pctl_reg);
		return -1;
	}
	/* disable request */
	PMC_REGS->pmc_pctl_reg &= (0xF<<5);
	
	/* assert CA55 reset */
	MOON0_REG->m0_sft_cfg[1] = 0x001E001E ;
	
	/*config ISO/SD/PSW */	
	PMC_REGS->pmc_iso_pwd       = 0xFFAA5500;
	PMC_REGS->pmc_iso_en		|= 0xF;
	
	PMC_REGS->pmc_coremem_sdpwd = 0x5500FFAA;
	PMC_REGS->pmc_coremem_sden	|= 0xF;
	
	PMC_REGS->pmc_corepsw_pwd   = 0x55FF00AA;
	PMC_REGS->pmc_corepsw_en	|= 0xF;

	printf("power_down_cores ok \n");
	return 0;
}
int power_down_ca55()
{
	if(power_down_cores() != 0)
		return -1;
	
	if(power_down_cluster() != 0)
		return -1;

	return 0;
}
void power_down_ddr_retention()
{
	ddr_retention_save();
}

void power_down_init()
{
	PMC_REGS->pmc_timer         = 0x00030003; //Quick
    PMC_REGS->pmc_timer2        = 0x00030003; //Quick
	
	PMC_REGS->pmc_ctrl |=(1 << 4) |   // enable powerdown IOP DM
                         (1 << 5) |   // disable system reset PMC
                         (1 << 8) |   // change wakeup source0 polarity
                         (1 <<10) ;   // change wakeup source1 polarity

    PMC_REGS->pmc_main_dowmain_pwd_1      = 0x55aa00ff;        
    PMC_REGS->pmc_main_dowmain_pwd_11     = 0x00ff55aa; 
	
	PMC_REGS->pmc_corepsw_pwd   = 0x55FF00AA;            
    PMC_REGS->pmc_coremem_sdpwd = 0x5500FFAA;        
    PMC_REGS->pmc_lvs_pwd       = 0x00AA55FF;         
    PMC_REGS->pmc_iso_pwd       = 0xFFAA5500;
	
	upf_set_power_down_bit();
	printf(" %s %d \n",__FUNCTION__,__LINE__);
}

void vDoPowerdownTask( void *pvParameters )
{
    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    for( ;; )
    {
		if( xPowerDown_Semaphore != NULL )
		{
			printf(" power down wait Semaphore !\n"); 
		    if(xSemaphoreTake( xPowerDown_Semaphore, portMAX_DELAY) == pdTRUE)
			{
				printf(" power down start !\n"); 

				power_down_init();
		
				power_down_ddr_retention();
				
				if(power_down_ca55() != 0)
				{
					printf(" power_down_ca55 fail \n");
					continue;
				}
				
				if(power_down_maindomain() != 0)
				{
					printf(" power_down_maindomain fail \n");
					continue;
				}
			}
		}
	}
}
