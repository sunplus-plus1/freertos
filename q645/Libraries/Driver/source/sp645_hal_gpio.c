#include "sp645_hal_gpio.h"

int gpio_input_invert_val_set(uint32_t bit,int invert_set)
{
	uint32_t idx;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	if(invert_set)
		*((volatile unsigned int *) (GPIO_O_INV(idx))) = RF_MASK_V_SET(1 << (bit & 0x0f));
	else
		*((volatile unsigned int *) (GPIO_O_INV(idx))) = RF_MASK_V_CLR(1 << (bit & 0x0f));

	return 0;
}

uint32_t gpio_input_invert_val_get(uint32_t bit)
{
	uint32_t idx, value, reg_val;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	value = 1 << (bit & 0x0f);

	reg_val = *((volatile unsigned int *)(GPIO_I_INV(idx)));

	return ((reg_val & value) ? 1 : 0);
}


int gpio_output_invert_val_set(uint32_t bit,int invert_set)
{
	uint32_t idx;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	if(invert_set)
		*((volatile unsigned int *) (GPIO_O_INV(idx))) = RF_MASK_V_SET(1 << (bit & 0x0f));
	else
		*((volatile unsigned int *) (GPIO_O_INV(idx))) = RF_MASK_V_CLR(1 << (bit & 0x0f));

	return 0;
}

uint32_t gpio_output_invert_val_get(uint32_t bit)
{
	uint32_t idx, value, reg_val;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	value = 1 << (bit & 0x0f);

	reg_val = *((volatile unsigned int *)(GPIO_O_INV(idx)));
	return ((reg_val & value) ? 1 : 0);
}

int gpio_open_drain_val_set(uint32_t bit,int od_set)
{
	uint32_t idx;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	if(od_set)
		*((volatile unsigned int *) (GPIO_OD(idx))) = RF_MASK_V_SET(1 << (bit & 0x0f));
	else
		*((volatile unsigned int *) (GPIO_OD(idx))) = RF_MASK_V_CLR(1 << (bit & 0x0f));

	return 0;
}

uint32_t gpio_open_drain_val_get(uint32_t bit)
{
	uint32_t idx, value, reg_val;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	value = 1 << (bit & 0x0f);

	reg_val = *((volatile unsigned int *)(GPIO_OD(idx)));
	return ((reg_val & value) ? 1 : 0);
}

/* set first register value , enable
   1: pin set in GPIO mode
   0: pin set in function mode
*/
int gpio_first_val_set(uint32_t bit,int firset_set)
{
	uint32_t idx, value, reg_val;

	idx = bit >> 5;
	if (idx > 4) {
		return -1;
	}

	value = 1 << (bit & 0x1f);

	reg_val = *((volatile unsigned int *)(GPIO_FIRST(idx)));
	if(firset_set)
	{
		reg_val |= value;
	}
	else
	{
		reg_val &= (~value);
	}

	*((volatile unsigned int *) (GPIO_FIRST(idx))) = reg_val;

	return 0;
}

uint32_t gpio_first_val_get(uint32_t bit)
{
	uint32_t idx, value, reg_val;

	idx = bit >> 5;
	if (idx > 5) {
		return -1;
	}

	value = 1 << (bit & 0x1f);

	reg_val = *((volatile unsigned int *)(GPIO_FIRST(idx)));

	return((reg_val & value) ? 1 : 0);
}

/* set master value
   1: gpio control by gpio register
   0: gpio control by IOP
*/
int gpio_master_val_set(uint32_t bit,int master_set)
{
	uint32_t idx;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	if(master_set)
		*((volatile unsigned int *) (GPIO_MASTER(idx))) = RF_MASK_V_SET(1 << (bit & 0x0f));
	else
		*((volatile unsigned int *) (GPIO_MASTER(idx))) = RF_MASK_V_CLR(1 << (bit & 0x0f));

	return 0;
}

/* get GPIO control mode register value
   1: GPIO Group control mode
   0: IOP control mode
*/
uint32_t gpio_master_val_get(uint32_t bit)
{
	uint32_t idx, value, reg_val;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	value = 1 << (bit & 0x0f);
	reg_val = *((volatile unsigned int *)(GPIO_MASTER(idx)));

	return ((reg_val & value) ? 1 : 0);
}

/* set output enable value
   1: output mode
   0: input mode
*/
int gpio_oe_val_set(uint32_t bit,int oe_set)
{
	uint32_t idx;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	if(oe_set)
		*((volatile unsigned int *) (GPIO_OE(idx))) = RF_MASK_V_SET(1 << (bit & 0x0f));
	else
		*((volatile unsigned int *) (GPIO_OE(idx))) = RF_MASK_V_CLR(1 << (bit & 0x0f));

	return 0;
}

/* get output enable value
   1: output mode
   0: input mode
*/
uint32_t gpio_oe_val_get(uint32_t bit)
{
	uint32_t idx, value, reg_val;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	value = 1 << (bit & 0x0f);

	reg_val = *((volatile unsigned int *)(GPIO_OE(idx)));

	return ((reg_val & value) ? 1 : 0);
}

/****** gpio output mode, set output value ****/
int gpio_out_val_set(uint32_t bit, uint32_t gpio_out_value)
{
	uint32_t idx;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	if(gpio_out_value)
		*((volatile unsigned int *) (GPIO_OUT(idx))) = RF_MASK_V_SET(1 << (bit & 0x0f));
	else
		*((volatile unsigned int *) (GPIO_OUT(idx))) = RF_MASK_V_CLR(1 << (bit & 0x0f));

	return 0;
}

/****** gpio output mode, get output value ****/
uint32_t gpio_out_val_get(uint32_t bit)
{
	uint32_t idx, value, reg_val;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	value = 1 << (bit & 0x0f);

	reg_val = *((volatile unsigned int *)(GPIO_OUT(idx)));

	return ((reg_val & value) ? 1 : 0);
}

/****** gpio input mode, get input value ****/
uint32_t gpio_in_val_get(uint32_t bit)
{
	uint32_t idx, value, reg_val;

	idx = bit >> 5;
	if (idx > 5) {
		return -1;
	}

	value = 1 << (bit & 0x1f);

	reg_val = *((volatile unsigned int *)(GPIO_IN(idx)));

	return ((reg_val & value) ? 1 : 0);
}

#define GPIO_I_PD(X)      (RF_GRP(101, (0+X)))
#define GPIO_I_PU(X)      (RF_GRP(102, (24+X)))
#define GPIO_O_DS(DS,X)    (RF_GRP(101, (4+DS*4+X)))


int gpio_default_input_val_set(uint32_t bit, uint32_t pull)
{
	uint32_t idx;

	if(pull == GPIO_PULL_DISABLE)
		return 0;

	idx = bit >> 5;
	if (idx > 4) {
		return -1;
	}

	if(pull == GPIO_PULL_DOWN)
	{
		*((volatile unsigned int *)(GPIO_I_PD(idx))) |=  1 << (bit & 0x1f);
	}
	else if(pull == GPIO_PULL_UP)
	{
		*((volatile unsigned int *)(GPIO_I_PU(idx))) |=  1 << (bit & 0x1f);
	}
	return 0;
}

int gpio_out_driver_current_set(uint32_t bit, uint32_t ds)
{
	uint32_t idx;

	if(ds == GPIO_DS_DISABLE)
		return 0;

	idx = bit >> 5;
	if (idx > 4) {
		return -1;
	}

	*((volatile unsigned int *)(GPIO_O_DS(ds,idx))) |=  1 << (bit & 0x1f);

	return 0;
}
uint8_t gpio_is_output(uint32_t GPIO_Pin)
{
	if(gpio_open_drain_val_get(GPIO_Pin) || gpio_oe_val_get(GPIO_Pin))
		return 1;

	return 0;
}

/*****************  HAL interface ************************/
void HAL_GPIO_Init(GPIO_InitTypeDef *GPIO_Init)
{

	assert_param(GPIO_Init);
	assert_param(IS_GPIO_PIN(GPIO_Init->Pin));
	assert_param(IS_GPIO_PIN(GPIO_Init->Mode));
	assert_param(IS_GPIO_PIN(GPIO_Init->ds));
	assert_param(IS_GPIO_PIN(GPIO_Init->out_value));
	assert_param(IS_GPIO_PIN(GPIO_Init->pull));

	/* set pin to gpio mode */
	gpio_first_val_set(GPIO_Init->Pin,1);
	/* set pin control by gpio group register */
	gpio_master_val_set(GPIO_Init->Pin,1);

	switch(GPIO_Init->Mode)
	{
		case GPIO_OD_OUTPUT_MODE:
			/*	pin in opendrain mode */
			gpio_open_drain_val_set(GPIO_Init->Pin,1);
			/* need to invert output in opendrain mode */
			gpio_output_invert_val_set(GPIO_Init->Pin,1);
			gpio_out_val_set(GPIO_Init->Pin,GPIO_Init->out_value);
			gpio_out_driver_current_set(GPIO_Init->Pin,GPIO_Init->ds);
		break;
		case GPIO_PP_OUTPUT_MODE:
			gpio_oe_val_set(GPIO_Init->Pin,1);
			gpio_out_val_set(GPIO_Init->Pin,GPIO_Init->out_value);
			gpio_out_driver_current_set(GPIO_Init->Pin,GPIO_Init->ds);
		break;
		case GPIO_INPUT_MODE:
			gpio_oe_val_set(GPIO_Init->Pin,0);
			gpio_default_input_val_set(GPIO_Init->Pin,GPIO_Init->pull);
		break;
		default:
			break;
	}
}

void HAL_GPIO_DeInit(uint16_t GPIO_Pin)
{
	assert_param(IS_GPIO_PIN(GPIO_Pin));

	gpio_first_val_set(GPIO_Pin,0);

	gpio_master_val_set(GPIO_Pin,1);

	gpio_open_drain_val_set(GPIO_Pin,0);

	gpio_output_invert_val_set(GPIO_Pin,1);

	gpio_input_invert_val_set(GPIO_Pin,0);

	gpio_oe_val_set(GPIO_Pin,0);

	gpio_out_val_set(GPIO_Pin,0);

}

GPIO_PinState  HAL_GPIO_ReadPin(uint16_t GPIO_Pin)
{

	GPIO_PinState bitstatus;

	assert_param(IS_GPIO_PIN(GPIO_Pin));

	if(gpio_is_output(GPIO_Pin))
	{
		bitstatus = gpio_out_val_get(GPIO_Pin)? GPIO_OUT_HIGH:GPIO_OUT_LOW;
	}
	else
	{
		bitstatus = gpio_in_val_get(GPIO_Pin)? GPIO_OUT_HIGH:GPIO_OUT_LOW;
	}
	return bitstatus;
}

void HAL_GPIO_WritePin(uint16_t GPIO_Pin, GPIO_PinState PinState)
{

	assert_param(IS_GPIO_PIN(GPIO_Pin));
	assert_param(IS_GPIO_PIN_ACTION(GPIO_Pin));

	/* get input/output mode. write data in output mode only
		1:output mode  0:input mode
	*/
	if(gpio_is_output(GPIO_Pin))
	{
		gpio_out_val_set(GPIO_Pin,PinState);
	}
}

void HAL_GPIO_TogglePin(uint16_t GPIO_Pin)
{
	assert_param(IS_GPIO_PIN(GPIO_Pin));
	/* get input/output mode. write data in output mode only
	   1:output mode  0:input mode
	*/
	if(gpio_is_output(GPIO_Pin))
	{
		gpio_out_val_set(GPIO_Pin,!(gpio_out_val_get(GPIO_Pin)));
	}
}

int HAL_GPIO_Get_Mode(uint16_t GPIO_Pin)
{
	assert_param(IS_GPIO_PIN(GPIO_Pin));
	/* get input/output mode. write data in output mode only
	   1:output mode  0:input mode
	*/
	return gpio_out_val_get(GPIO_Pin);
}
