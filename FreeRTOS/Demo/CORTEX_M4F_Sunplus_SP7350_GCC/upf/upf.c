#include "upf_private.h"

#define UPF_TASK_STACK_SIZE				(configMINIMAL_STACK_SIZE)
#define CA55_2_CM4_IRQ_INT0				(9)

#define MAILBOX0						 REG_1(258, 24)

SemaphoreHandle_t xPowerDown_Semaphore;


void Send_Cmd_To_PMIC(char cmd)
{

	UART_putc(cmd);

	*(volatile unsigned int*)0xf8801980 = '@';
	UART_putc(0x99);
	//need to respond
}

void vCA55_TO_CM4_Mailbox_ISR()
{
	static BaseType_t xHigherPriorityTaskWoken;
	int value = MAILBOX0;
	value = value ;
	
	if(xSemaphoreGiveFromISR( xPowerDown_Semaphore, &xHigherPriorityTaskWoken ) != pdTRUE)
	{
		printf("Semaphore Send ISR Fail!\r\n");
	}

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void upf_main()
{
	printf("\nFreeRTOS (build @upf_main "__TIME__")\n\n");
	
	
	xPowerDown_Semaphore = xSemaphoreCreateBinary( );

	if( xPowerDown_Semaphore != NULL )
	{
		NVIC_SetVector(CA55_2_CM4_IRQ_INT0, (u32)vCA55_TO_CM4_Mailbox_ISR); 
		NVIC_EnableIRQ(CA55_2_CM4_IRQ_INT0);
		NVIC_SetPriority(CA55_2_CM4_IRQ_INT0, 0xE0); 

		xTaskCreate( vDoPowerdownTask, "powerdown", UPF_TASK_STACK_SIZE, NULL, 1, NULL );
		xTaskCreate( vDoPowerupTask, "powerup", UPF_TASK_STACK_SIZE, NULL, 1, NULL );
	}
}

void upf_set_power_down_bit(void)
{
	/* used for romcode */				
	PMC_REGS->pmc_ca55_reset_state = 0xF000; // [15]=1,[14:12]=1 ca55 reset and wait ddr retention
	PMC_REGS->pmc_reset_vector = PMC_RESET_VECTOR_ADDRESS;
}

void upf_set_retention_done_bit(void)
{
	/* set ddr retention done bit [15]*/	
	PMC_REGS->pmc_ca55_reset_state = 0x1; // [15]=0
}