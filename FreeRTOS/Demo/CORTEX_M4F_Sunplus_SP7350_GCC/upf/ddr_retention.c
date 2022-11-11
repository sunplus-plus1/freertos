#include <FreeRTOS.h>
#include<FreeRTOSConfig.h>
#include "CMSIS/CMSDK_CM4.h"
#include "CMSIS/core_cm4.h"

#include "upf.h"
#include "platform.h"


#define CM4_SRAM_RET_ADDRESS    0xFA280000
#define UMCTL2_REG_Base	   	    0xF8169000
#define DWC_PHY_REG_Base        0xF9000000
#define UMCTL2_REG(OFFSET)		(umctl2_reg_ptr->umctl2_reg[OFFSET >> 2])
#define DWC_PHY_REG(OFFSET)		(dwc_phy_reg_ptr->dwc_phy_reg[OFFSET ])


#define RET_CSRS_CNT		1054
u32 ret_csrs[RET_CSRS_CNT];
u32 ret_csrs_restore[RET_CSRS_CNT];

struct dwc_phy_regs {
	u16 dwc_phy_reg[4096];	/* change the size here, (area >> 2) */
};
static volatile struct dwc_phy_regs *dwc_phy_reg_ptr = (volatile struct dwc_phy_regs *)(DWC_PHY_REG_Base);

struct umctl2_regs {
	unsigned int umctl2_reg[1024];	/* change the size here, (area >> 2) */
};
static volatile struct umctl2_regs *umctl2_reg_ptr = (volatile struct umctl2_regs *)(UMCTL2_REG_Base);


void wait_loop(unsigned int wait_counter)
{
	unsigned int i;

	for (i = 0; i < wait_counter; i++) 
	{
		__asm__("nop");
	}
}
void ctl_apb_wr(u32 adr, u32 dat) 
{
	UMCTL2_REG(adr) = dat;
}

int ctl_apb_rd(u32 adr) 
{
	u32 value;
	value = UMCTL2_REG(adr);
	return value;
}

void dwc_ddrphy_apb_wr(u32 adr, u32 dat) 
{
    DWC_PHY_REG(adr)=dat;
}

int dwc_ddrphy_apb_rd(u32 adr) 
{
	u16 value;
    value= DWC_PHY_REG(adr);
	return value;
}

void apb_ctl_before_retention(void)
{
	volatile u16 rd_data;

	wait_loop(10000);
    ctl_apb_wr(0x490,0x00000000);   //blocks AXI ports from taking anymore transactions

	while (1) 
	{
	    rd_data = ctl_apb_rd(0x3fc);
	    if (rd_data == 0) 
		{
			break;
	    }
	}
	
    ctl_apb_wr(0x30,0x00000020);    //Causes system to move to self refresh state

	rd_data = ctl_apb_rd(0x4);
	while (((rd_data&0x30)!= 0x20) | ((rd_data&0x300) != 0x200)) 
	{
	    rd_data = ctl_apb_rd(0x4);
	}

	ctl_apb_wr(0x320,0x00000000);   //SWCTL
	ctl_apb_wr(0x1b0,0x00001f70);   //Set dfi_init_start to 1
   
	rd_data = ctl_apb_rd(0x1bc);
	while((rd_data & 0x01)!= 0)
	{
		rd_data = ctl_apb_rd(0x1bc);    //Wait PHY de-assert dfi_init_complete
	}
	*(volatile unsigned int*)0xf8801900 = 'u';
	ctl_apb_wr(0x1b0,0x00001f50);   //Set dfi_init_start to 0

	rd_data = ctl_apb_rd(0x1bc);
	while((rd_data & 0x01)!= 1) 
	{
		rd_data = ctl_apb_rd(0x1bc);  //Wait PHY assert dfi_init_complete
	}

	MOON0_REG->m0_sft_cfg[14] = 0x00200000; // 1->0: PwrOKIn MO_DDRPHY_PWROKIN ddrphy pwrokin
	wait_loop(10000); 							// When PWROKIN is 0, PHY input signals need be valid at least 10ns
	MOON0_REG->m0_sft_cfg[14] = 0x00010000;  // 1->0: assert DDRCTL APB RST_N
	MOON0_REG->m0_sft_cfg[14] = 0x00080000;  // 1->0: assert DDRPHY APB RST_N
	MOON0_REG->m0_sft_cfg[14] = 0x00020000;  // 1->0: assert DDRCTL AXI RST_N
	MOON0_REG->m0_sft_cfg[14] = 0x00040000;  // 1->0: assert DDRCTL AHB RST_N
	MOON0_REG->m0_sft_cfg[14] = 0x00100000;  // 1->0: assert DDRDFI RST_N
	
	MOON0_REG->m0_sft_cfg[2]  = 0x00040004;  //RF_MASK_V_CLR(1 << 2);
	MOON0_REG->m0_sft_cfg[2]  = 0x00080008;  //RF_MASK_V_CLR(1 << 3);
	MOON0_REG->m0_sft_cfg[2]  = 0x00100010;  //RF_MASK_V_CLR(1 << 4);
	MOON0_REG->m0_sft_cfg[2]  = 0x00200020;  //RF_MASK_V_CLR(1 << 5);
	*(volatile unsigned int*)0xf8801900 = 'x';
}

int dram_test(void)
{
	u8   *wr_src_8b  ;
	u16  *wr_src_16b ;
	u32  *wr_src_32b ;
	u8   value_8b    ;
	u16  value_16b   ;
	u32  value_32b   ;
	u32  *rd_src_32b ;
	u8   *rd_src_8b  ;
	u8   count, flip;

	unsigned char *des_adr = (unsigned char *)0x50000000;
	*(volatile unsigned int*)0xf8801900 = 'd';
	wr_src_16b   = (u16 *)(des_adr);
	value_16b    = 0;
	do {
		*wr_src_16b = value_16b;
		wr_src_16b  = wr_src_16b + 1;
		value_16b   = value_16b  + 1;
	} while ((ulong)wr_src_16b < (ulong)(des_adr + 0x1000));

	rd_src_32b   = (u32 *)(des_adr);
	value_16b    = 0;
	do {
		value_32b = ((value_16b + 1) << 16) | ((value_16b + 0) <<  0) ;
		if (value_32b != *rd_src_32b) 
		{
			return -1;
		}
		rd_src_32b = rd_src_32b + 1;
		value_16b  = value_16b  + 2;
	} while ((ulong)rd_src_32b < (ulong)(des_adr + 0x1000));
	*(volatile unsigned int*)0xf8801900 = 'e';

	return 0;
}

void ddr_retention_save(void)
{
	#include <upf/ddr_ret_save.h>

	u32 cm4_sram_base_addr = CM4_SRAM_RET_ADDRESS;  //S-RAM(CM4) CPU view	
	volatile u32 *ddr_ret = (u32 *)cm4_sram_base_addr;
	for (int i=0; i<RET_CSRS_CNT; i++) 
	{
	    *(ddr_ret + i) = ret_csrs[i];
	}
	
	if (0 == dram_test()) {
		    printf("\n ddr_retention_save dram_test ok \n");
		} else {
		    printf("\n ddr_retention_save dram_test fail \n");
		}
	apb_ctl_before_retention();
}


int dwc_umctl2_init_before_ctl_rst(void)
{

	ctl_apb_wr(0x304, 0x00000001);
	ctl_apb_wr(0x030, 0x00000001);
	//ctl_apb_rd(0x004, 0x00000000);
	ctl_apb_rd(0x004);

	ctl_apb_wr(0x000, 0x83080020);
	ctl_apb_wr(0x010, 0x40004030);
	ctl_apb_wr(0x014, 0x0000526d);
	ctl_apb_wr(0x01c, 0xd2c48bae);
	ctl_apb_wr(0x020, 0x00001203);
	ctl_apb_wr(0x024, 0xff20dab0);
	ctl_apb_wr(0x02c, 0x00000001);
	ctl_apb_wr(0x030, 0x0000012a);
	ctl_apb_wr(0x034, 0x0008ac00);
	ctl_apb_wr(0x038, 0x02630001);
	ctl_apb_wr(0x050, 0xe0518022);
	ctl_apb_wr(0x054, 0x0001001f);
	ctl_apb_wr(0x060, 0x00000000);
	ctl_apb_wr(0x064, 0x00618130);
	ctl_apb_wr(0x068, 0x00480000);
	ctl_apb_wr(0x0c0, 0x00000000);
	ctl_apb_wr(0x0c4, 0x00000000);
	ctl_apb_wr(0x0d0, 0x00030002);
	ctl_apb_wr(0x0d4, 0x00010002);
	ctl_apb_wr(0x0d8, 0x00001a00);
	ctl_apb_wr(0x0dc, 0x0054002d);
	ctl_apb_wr(0x0e0, 0x00310020);
	ctl_apb_wr(0x0e4, 0x00040009);
	ctl_apb_wr(0x0e8, 0x0000004d);
	ctl_apb_wr(0x0ec, 0x0000004d);
	ctl_apb_wr(0x0f0, 0x00000000);
	ctl_apb_wr(0x0f4, 0x0000e8c1);
	ctl_apb_wr(0x100, 0x1b201a22);
	ctl_apb_wr(0x104, 0x00060630);
	ctl_apb_wr(0x108, 0x070e1517);
	ctl_apb_wr(0x10c, 0x00c0c006);
	ctl_apb_wr(0x110, 0x0f04080f);
	ctl_apb_wr(0x114, 0x043d0c0c);
	ctl_apb_wr(0x118, 0x03080009);
	ctl_apb_wr(0x11c, 0x00000e0a);
	ctl_apb_wr(0x120, 0x01011301);
	ctl_apb_wr(0x124, 0x00000030);
	ctl_apb_wr(0x128, 0x0006090d);
	ctl_apb_wr(0x12c, 0x4b010007);
	ctl_apb_wr(0x130, 0x00020000);
	ctl_apb_wr(0x134, 0x0c100002);
	ctl_apb_wr(0x138, 0x00000136);
	ctl_apb_wr(0x13c, 0x00000000);
	ctl_apb_wr(0x180, 0x13200018);
	ctl_apb_wr(0x184, 0x02800070);
	ctl_apb_wr(0x188, 0x00000000);
	ctl_apb_wr(0x190, 0x0397820a);
	ctl_apb_wr(0x194, 0x00090202);
	ctl_apb_wr(0x198, 0x07017110);
	ctl_apb_wr(0x19c, 0x00000021);
	ctl_apb_wr(0x1a0, 0x00400018);
	ctl_apb_wr(0x1a4, 0x00ee00f0);
	ctl_apb_wr(0x1a8, 0x80000000);
	ctl_apb_wr(0x1b0, 0x00000055);
	ctl_apb_wr(0x1b4, 0x0000170a);
	ctl_apb_wr(0x1b8, 0x00000008);
	ctl_apb_wr(0x1c0, 0x00000000);
	ctl_apb_wr(0x1c4, 0xc8000001);
	
	ctl_apb_wr(0x200, 0x00000016);
	ctl_apb_wr(0x204, 0x00080202);
	ctl_apb_wr(0x208, 0x02020000);
	ctl_apb_wr(0x20c, 0x02020202);
	ctl_apb_wr(0x210, 0x00001f1f);
	ctl_apb_wr(0x214, 0x070f0707);
	ctl_apb_wr(0x218, 0x08070707);
	ctl_apb_wr(0x21c, 0x00000f08);
	ctl_apb_wr(0x220, 0x00003f3f);
	ctl_apb_wr(0x224, 0x07070707);
	ctl_apb_wr(0x228, 0x07070707);
	ctl_apb_wr(0x22c, 0x001f1f07);
	
	ctl_apb_wr(0x240, 0x051c0320);
	ctl_apb_wr(0x244, 0x00000000);
	ctl_apb_wr(0x250, 0x04cd9804);
	ctl_apb_wr(0x254, 0x00000021);
	ctl_apb_wr(0x25c, 0x7a00253a);
	ctl_apb_wr(0x264, 0xf70009d5);
	ctl_apb_wr(0x26c, 0x4300fd5f);
	ctl_apb_wr(0x300, 0x00000001);
	ctl_apb_wr(0x304, 0x00000000);
	ctl_apb_wr(0x30c, 0x00000000);
	ctl_apb_wr(0x320, 0x00000001);
	ctl_apb_wr(0x328, 0x00000000);
	ctl_apb_wr(0x36c, 0x00110011);
	ctl_apb_wr(0x400, 0x00000010);
	ctl_apb_wr(0x404, 0x000051fc);
	ctl_apb_wr(0x408, 0x000013c8);
	ctl_apb_wr(0x490, 0x00000001);
	ctl_apb_wr(0x494, 0x00210005);
	ctl_apb_wr(0x498, 0x02af0351);
	ctl_apb_wr(0x49c, 0x01110d03);
	ctl_apb_wr(0x4a0, 0x0028012b);
	ctl_apb_wr(0x4b4, 0x00001058);
	ctl_apb_wr(0x4b8, 0x00004076);
	ctl_apb_wr(0x540, 0x00000001);
	ctl_apb_wr(0x544, 0x00110009);
	ctl_apb_wr(0x548, 0x004c0028);
	ctl_apb_wr(0x54c, 0x01010a05);
	ctl_apb_wr(0x550, 0x04d606ec);
	ctl_apb_wr(0x564, 0x000052be);
	ctl_apb_wr(0x568, 0x00000287);
	ctl_apb_wr(0x5f0, 0x00000001);
	ctl_apb_wr(0x5f4, 0x0021000e);
	ctl_apb_wr(0x5f8, 0x041f0445);
	ctl_apb_wr(0x5fc, 0x01110301);
	ctl_apb_wr(0x600, 0x032f03b6);
	ctl_apb_wr(0x614, 0x000060dd);
	ctl_apb_wr(0x618, 0x00004028);
	//ctl_apb_wr(0x61c, 0x00000000);
	ctl_apb_wr(0x6a0, 0x00000001);
	ctl_apb_wr(0x060, 0x00000000);
	ctl_apb_wr(0x030, 0x0000012a);
	ctl_apb_wr(0x030, 0x00000122);

	return 0;
}

int dwc_umctl2_init_after_ctl_rst(void)
{

	ctl_apb_wr(0x304, 0x00000000);
	//ctl_apb_rd(0x030, 0x00000020);
	ctl_apb_rd(0x030);

	ctl_apb_wr(0x030, 0x00000122);
	//ctl_apb_rd(0x030, 0x00000020);
	ctl_apb_rd(0x030);

	ctl_apb_wr(0x030, 0x00000120);
	//ctl_apb_rd(0x1c4, 0xcf000000);
	ctl_apb_rd(0x1c4);

	ctl_apb_wr(0x1c4, 0xc8000000);
	ctl_apb_wr(0x320, 0x00000000);
	ctl_apb_wr(0x1b0, 0x00000054);
	ctl_apb_wr(0x1b0, 0x00000054);
	ctl_apb_wr(0x304, 0x00000002);

	return 0;
}

void apb_ctl_after_retention(void)
{
	volatile u32 rd_data;

	MOON3_REG->m3_sft_cfg[11] = 0x00180008;  //PLLD PSTDIV
	MOON3_REG->m3_sft_cfg[11] = 0x7f801000;  //PLLD FBKDIV 32
	//while(((MOON3_REG->m3_sft_cfg[13]) & (1<<8)) != (1<<8));  //wait pll lock

	MOON0_REG->m0_sft_cfg[2]  = 0x00040000;  //RF_MASK_V_CLR(1 << 2);
	MOON0_REG->m0_sft_cfg[2]  = 0x00080000;  //RF_MASK_V_CLR(1 << 3);
	MOON0_REG->m0_sft_cfg[2]  = 0x00100000;  //RF_MASK_V_CLR(1 << 4);
	MOON0_REG->m0_sft_cfg[2]  = 0x00200000;  //RF_MASK_V_CLR(1 << 5);
	wait_loop(1000);
	MOON0_REG->m0_sft_cfg[14] = 0x00200020; // 0->1: PwrOKIn MO_DDRPHY_PWROKIN ddrphy pwrokin
	wait_loop(1000);
	wait_loop(1000);
	MOON0_REG->m0_sft_cfg[14] = 0x00010001;  // 1->0: assert DDRCTL APB RST_N
	MOON0_REG->m0_sft_cfg[14] = 0x00080008;  // 1->0: assert DDRPHY APB RST_N

	wait_loop(1000);

    // -------------------------------------------------------
	// 2.
	// -------------------------------------------------------
	// DRAM MRS SETTING

	dwc_umctl2_init_before_ctl_rst();

	ctl_apb_wr(0x0d0,0xc0030003);   //skips the DRAM init routine and starts up in self_refresh mode
	ctl_apb_wr(0x30,0x00000020);    //Keep system on self refresh state
	ctl_apb_wr(0x1b0,0x00000050);   //indicates to controller that PHY has completed re-training/initialization
	
	MOON0_REG->m0_sft_cfg[14] = 0x00020002;  // 1->0: assert DDRCTL AXI RST_N
	MOON0_REG->m0_sft_cfg[14] = 0x00040004;  // 1->0: assert DDRCTL AHB RST_N
	MOON0_REG->m0_sft_cfg[14] = 0x00100010;  // 1->0: assert DDRDFI RST_N
	wait_loop(1000);
	wait_loop(1000);
        *(volatile unsigned int*)0xf8801900 = 'g';
	dwc_umctl2_init_after_ctl_rst();
	*(volatile unsigned int*)0xf8801900 = 'h';
     wait_loop(1000);

	// -------------------------------------------------------
	// 3.
	// -------------------------------------------------------
	*(volatile unsigned int*)0xf8801900 = '2';
	
	u32 cm4_sram_base_addr = CM4_SRAM_RET_ADDRESS;  //S-RAM(CM4) CPU view	

	volatile u32 *ddr_ret = (u32 *)cm4_sram_base_addr;
	for (int i=0; i<RET_CSRS_CNT; i++) 
	{
	    ret_csrs_restore[i] = *(ddr_ret + i);
	}

	#include <upf/ddr_ret_load.h>

	ctl_apb_wr(0x1b0,0x00000070);   //trigger dfi_init_start

	while(1)
	{
	   rd_data = ctl_apb_rd(0x1bc);
	   if(rd_data == 1)
	   {
		 break;
	   }
	}

	ctl_apb_wr(0x1b0,0x00000050);
	ctl_apb_wr(0x1b0,0x00000051); //trigger SDRAM initilaztion.
	ctl_apb_wr(0x030,0x00000000); //PWRCTL
	ctl_apb_wr(0x1c4,0x00000001);   //enable the PHY master interface.
	ctl_apb_wr(0x304,0x00000000);   //enable dq and hif.
	rd_data = ctl_apb_rd(0x4);
	while(((rd_data & 0x30)!=0x00) || ((rd_data&0x07) != 0x01))
	{
		rd_data = ctl_apb_rd(0x4);      //Wait exit self_refresh and entry normal mode
	}

	ctl_apb_wr(0x490,0x00000001);   //no longer blocks AXI ports from taking anymore transactions
}

void ddr_retention_load(void)
{
	//while(((MOON3_REG->m3_sft_cfg[13]) & (1<<8)) != (1<<8));  //wait pll lock
	wait_loop(1000);

	apb_ctl_after_retention();
	
	if (0 == dram_test()) 
	{
		printf("\n ddr_retention_load dram_test ok \n");
	} else {
		printf("\n ddr_retention_load dram_test fail \n");
	}
}



