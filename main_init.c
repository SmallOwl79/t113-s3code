#include "sunxi_jtag.h"
#include "sunxi_spi.h"
#include "sunxi_gpio.h"

#include "stdlib.h"
#include "cmsis_os2.h"
#include "control.h"
#include "aw_t113s2.h"
#include "gpio_sun.h"
#include "i2c-sunxi.h"
#ifndef	FSBL
#include "sun8i_emac.h"
#include "lwip/netif.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "Driver_ETH.h"

#include "cedrus.h"
#include "vepoc.h"
#include "ff.h"

#endif

#define LWIP_EN		1

void http_cgi_81_thread(void *arg);

//#pragma location=".ddr_data"
//    FIL file;

//uint16_t jpeg_luma_table[64] = {16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55, 14, 13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62, 18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99};
#ifndef	FSBL
struct cedrus_device cedrus;
#endif

void gpio_set_sun(struct gpio_t_ *gpio, enum gpio_state_t state);

void L1_InvalidateDCache_by_Addr(void * addr, int32_t size);

uint32_t tpd_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, control_struct	*p_ctl);


//const cmd_cell_param_s main_cell_param={
//	    0,//uint16_t    x_offset;
//	    200,//uint16_t	wide;
//	    20,//uint16_t	height;
//	    5,//uint16_t	height_space;
//	    120,//uint16_t	name_wide;
//	    180,//uint16_t	edit_wide;
//	    50,//uint16_t	exec_but_wide;
//	    50,//uint16_t	sys_but_1_wide;
//	    25,//uint16_t	sys_but_2_wide;
//};
//const cmd_cell_param_s table_cell_param={
//	    0,//uint16_t    x_offset;
//	    200,//uint16_t	wide;
//	    20,//uint16_t	height;
//	    5,//uint16_t	height_space;
//	    120,//uint16_t	name_wide;
//	    80,//uint16_t	edit_wide;
//	    50,//uint16_t	exec_but_wide;
//	    50,//uint16_t	sys_but_1_wide;
//	    25,//uint16_t	sys_but_2_wide;
//};

#pragma data_alignment=4096
#pragma location=".ddr_data"
uint8_t video_chroma_buf_grab[2024*1024];

#pragma data_alignment=4096
#pragma location=".ddr_data"
uint8_t video_luma_buf_grab[2048*1024];

#pragma data_alignment=4096
#pragma location=".ddr_data"
uint8_t video_chroma_buf[2048*1024];
#pragma data_alignment=4096
#pragma location=".ddr_data"
uint8_t video_luma_buf[2048*1024];

#pragma data_alignment=4096
#pragma location=".ddr_data"
uint8_t video_chroma_buf_csi[1024*1024];
#pragma data_alignment=4096
#pragma location=".ddr_data"
uint8_t video_luma_buf_csi[1024*1024];


//#pragma data_alignment=4096
//#pragma location=".ddr1_data"
//uint8_t video_chroma_buf1[2048*1024];
////uint8_t video_chroma_buf1[1024];
//#pragma data_alignment=4096
//#pragma location=".ddr1_data"
//uint8_t video_luma_buf1[2048*1024];
//uint8_t video_luma_buf1[1024];

//#pragma data_alignment=1024
//#pragma location=".ddr_data"
//uint8_t jpeg_data_buf[1024*1024];


#pragma data_alignment=64
#pragma location=".ddr_data"
uint8_t				sender_line_buf[720*4 + sizeof(im_header)];

 extern osEventFlagsId_t	httpd_event_flag;

 void httpd_init(osEventFlagsId_t event_f, uint32_t msg_id, uint32_t ack_msg_id);

//#define FSBL	1
//#define FIRST_APP	1
//#pragma location=".ddr_data"
control_struct control;
#ifndef	FSBL
struct netif eth_if_lwip;
#endif
eth_resourse_s eth_res;

//#define FLAGS_MSK1 0x00000001U
//osEventFlagsId_t evt_id;                        // event flags id


#ifdef FSBL
sunxi_spi_t sunxi_spi0 = {
	.base	   = 0x04025000,
	.id		   = 0,
	.clk_rate  = 100 * 1000 * 1000,
	.gpio_cs   = {GPIO_PIN(PORTC, 3), GPIO_PERIPH_MUX2},
	.gpio_sck  = {GPIO_PIN(PORTC, 2), GPIO_PERIPH_MUX2},
	.gpio_mosi = {GPIO_PIN(PORTC, 4), GPIO_PERIPH_MUX2},
	.gpio_miso = {GPIO_PIN(PORTC, 5), GPIO_PERIPH_MUX2},
	.gpio_wp   = {GPIO_PIN(PORTC, 6), GPIO_PERIPH_MUX2},
	.gpio_hold = {GPIO_PIN(PORTC, 7), GPIO_PERIPH_MUX2},
};
#endif

uint_fast32_t allwnr_t113_get_chipid(void){
	return SID->SID_DATA [0] & 0xFFFF;
}
#ifndef	FSBL
#ifdef MURKA_BOARD
#pragma data_alignment=64
#pragma location=".ddr_data"
uint8_t ff_buf[4096];
#pragma location=".ddr_data"
FATFS fs;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ack_msg_on_send_cmd (ack_msg_s *p_ack,cntrl_dev_sys_msg_que_type_s *p_buf, uint8_t send_id){
	return;
	p_ack->collect_ack |= 1<< send_id;
	p_ack->collect_error |= 1<< send_id;

	p_ack->ack[send_id].id = send_id;
	p_ack->ack[send_id].send_cmd = p_buf->cmd;
	p_ack->ack[send_id].send_time = p_buf->time;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ack_msg_on_clear_send_cmd(ack_msg_s *p_ack,cntrl_dev_sys_msg_que_type_s *p_buf, uint8_t send_id){
	p_ack->collect_ack &= ~(1<< send_id);
	memset(&p_ack->ack[send_id],0,sizeof(dev_ack_s));
}
uint32_t dev_config_write(dev_config_struct *p_conf){
uint32_t ret=0;
//	ret |= write_config_param(p_conf, FLASH_PROGRAMM_PARAM_ADRESS);
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef MURKA_BOARD
void cntrl_make_fat_fs_start_init(void){
FRESULT res_fs;
uint8_t disk_err=0;
	memset(&fs,0,sizeof(fs));
	res_fs = f_mount(&fs, "0", 1);                    /* Mount the default drive */
	if(res_fs != FR_OK){
		res_fs = f_mkfs("0",FM_FAT | FM_SFD,512,ff_buf,4096);
		if(res_fs == FR_OK){
			res_fs = f_mount(&fs, "0", 1);                    /* Mount the default drive */
			if(res_fs != FR_OK){
				disk_err=1;
			}
		}
		else{
			disk_err=1;
		}
	}
}
#endif
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef	FSBL
void cntrl_start_lwip(control_struct *p_ctl){
eth_resourse_s *p_eth_res;
	p_eth_res = &eth_res;
	p_ctl->p_eth_res = p_eth_res;
	p_eth_res->p_netif = &eth_if_lwip;

	p_eth_res->cntrl_thread_flag_group = p_ctl->cmd_evt_id;
	p_eth_res->cntrl_thread_init_done_event = main_cntrl_thread_event_tcp_stack_init;

	p_eth_res->p_on_link_param = p_ctl;
//	p_eth_res->on_link_up = &cntrl_on_link_up;
//	p_eth_res->on_link_down = &cntrl_on_link_down;

	p_eth_res->p_on_complete_init_param = p_ctl;
//	p_eth_res->on_complete_init = &cntrl_on_lwip_complete_init;
//

#define DEFAULT_DEV_CONFIG_MASK_TABLE_0					255
#define DEFAULT_DEV_CONFIG_MASK_TABLE_1					255
#define DEFAULT_DEV_CONFIG_MASK_TABLE_2					255
#define DEFAULT_DEV_CONFIG_MASK_TABLE_3					0

#define DEFAULT_DEV_CONFIG_APWIFI_IP_TABLE_0			192
#define DEFAULT_DEV_CONFIG_APWIFI_IP_TABLE_1			168
#define DEFAULT_DEV_CONFIG_APWIFI_IP_TABLE_2			4
#define DEFAULT_DEV_CONFIG_APWIFI_IP_TABLE_3			1

#define DEFAULT_DEV_CONFIG_APWIFI_MASK_TABLE_0			255
#define DEFAULT_DEV_CONFIG_APWIFI_MASK_TABLE_1			255
#define DEFAULT_DEV_CONFIG_APWIFI_MASK_TABLE_2			255
#define DEFAULT_DEV_CONFIG_APWIFI_MASK_TABLE_3			0


#define DEFAULT_DEV_CONFIG_GATE_TABLE_0					192
#define DEFAULT_DEV_CONFIG_GATE_TABLE_1					168
#define DEFAULT_DEV_CONFIG_GATE_TABLE_2					1
#define DEFAULT_DEV_CONFIG_GATE_TABLE_3					1

#define DEFAULT_DEV_CONFIG_MAC_TABLE_0					0xF8
#define DEFAULT_DEV_CONFIG_MAC_TABLE_1					0xD0
#define DEFAULT_DEV_CONFIG_MAC_TABLE_2					0x30
#define DEFAULT_DEV_CONFIG_MAC_TABLE_3					0x29
#define DEFAULT_DEV_CONFIG_MAC_TABLE_4					0x80
#define DEFAULT_DEV_CONFIG_MAC_TABLE_5					0xA1


	p_eth_res->config.gate[0] = DEFAULT_DEV_CONFIG_GATE_TABLE_0;//p_ctl->dev_config.rec.eth_gate[0];
	p_eth_res->config.gate[1] = DEFAULT_DEV_CONFIG_GATE_TABLE_1;//p_ctl->dev_config.rec.eth_gate[1];
	p_eth_res->config.gate[2] = DEFAULT_DEV_CONFIG_GATE_TABLE_2;//p_ctl->dev_config.rec.eth_gate[2];
	p_eth_res->config.gate[3] = DEFAULT_DEV_CONFIG_GATE_TABLE_3;//p_ctl->dev_config.rec.eth_gate[3];

	p_eth_res->config.ip[0] = 192;//DEFAULT_DEV_CONFIG_IP_TABLE_0;//p_ctl->dev_config.rec.eth_ip[0];
	p_eth_res->config.ip[1] = 168;//DEFAULT_DEV_CONFIG_IP_TABLE_1;//p_ctl->dev_config.rec.eth_ip[1];
	p_eth_res->config.ip[2] = 1;//DEFAULT_DEV_CONFIG_IP_TABLE_2;//p_ctl->dev_config.rec.eth_ip[2];
	p_eth_res->config.ip[3] = 131;//DEFAULT_DEV_CONFIG_IP_TABLE_3;//p_ctl->dev_config.rec.eth_ip[3];

	p_eth_res->config.mask[0] = DEFAULT_DEV_CONFIG_APWIFI_MASK_TABLE_0;//p_ctl->dev_config.rec.eth_mask[0];
	p_eth_res->config.mask[1] = DEFAULT_DEV_CONFIG_APWIFI_MASK_TABLE_1;//p_ctl->dev_config.rec.eth_mask[1];
	p_eth_res->config.mask[2] = DEFAULT_DEV_CONFIG_APWIFI_MASK_TABLE_2;//p_ctl->dev_config.rec.eth_mask[2];
	p_eth_res->config.mask[3] = DEFAULT_DEV_CONFIG_APWIFI_MASK_TABLE_3;//p_ctl->dev_config.rec.eth_mask[3];

	p_eth_res->config.mac[0] = DEFAULT_DEV_CONFIG_MAC_TABLE_0;//p_ctl->dev_config.rec.eth_mac[0];
	p_eth_res->config.mac[1] = DEFAULT_DEV_CONFIG_MAC_TABLE_1;//p_ctl->dev_config.rec.eth_mac[1];
	p_eth_res->config.mac[2] = DEFAULT_DEV_CONFIG_MAC_TABLE_2;//p_ctl->dev_config.rec.eth_mac[2];
	p_eth_res->config.mac[3] = DEFAULT_DEV_CONFIG_MAC_TABLE_3;//p_ctl->dev_config.rec.eth_mac[3];
	p_eth_res->config.mac[4] = DEFAULT_DEV_CONFIG_MAC_TABLE_4;//p_ctl->dev_config.rec.eth_mac[4];
	p_eth_res->config.mac[5] = DEFAULT_DEV_CONFIG_MAC_TABLE_5;//p_ctl->dev_config.rec.eth_mac[5];


	osThreadNew(lwip_fone_thread, &eth_res, NULL);
}
#endif
//int load_spi_nand(sunxi_spi_t *spi, image_info_t *image)

static uint8_t page[64];

uint32_t addr;



int load_spi_nand(sunxi_spi_t *spi){
unsigned int		   size;
uint64_t			   start, time;
uint8_t tst[64];

uint8_t* p_mem;
uint32_t pos=0;
p_mem = (uint8_t*)0x20000000;

    uint32_t *data = (uint32_t *)page;

	memset(page,0,sizeof(page));
	addr = 0x8000;

	spi->info.id.mfr = 00;//0xc8;
	spi->info.page_size = 32;

	spi_nand_read(spi,page,addr,64);
	memcpy(p_mem,page,64);
	p_mem += 64;
	addr += 64;

    // 1. Проверка магии "eGON.BT0"
    // 'eGON' = 0x4E4F4765 (в little-endian это 0x65, 0x47, 0x4F, 0x4E)
    // '.BT0' = 0x3054422E (в little-endian это 0x2E, 0x42, 0x54, 0x30)
    if (data[1] != 0x4E4F4765 || data[2] != 0x3054422E) {
        return -1; // Это не eGON файл
    }

    // 2. Проверка адекватности длины
    uint32_t length = data[4]; // Офсет 0x10
    if (length < 32 || length < 0) {//проверка!!!
        return -2; // Длина подозрительная
    }

    // 3. Проверка CRC
    uint32_t target_sum = data[3]; // Офсет 0x0C
    uint32_t calc_sum = 0;
    uint8_t b_start=0;

    for (uint32_t i = 0; i < (length / 4); i++) {
    	if(!b_start){
    		if (i == 3){
    			pos++;
    			continue; // Пропускаем поле самой суммы
    		}
    	}
        calc_sum += data[pos++];
        if(pos == (64/4)){
        	spi_nand_read(spi,page,addr,64);
        	memcpy(p_mem,page,64);
        	p_mem += 64;
        	addr += 64;
        	pos = 0;
        	b_start = 1;
        }
    }
    calc_sum += 0x5F0A6C39;

    if (calc_sum != target_sum) {
        return -3; // CRC битая
    }

    return 0; // Всё чётко!
}

void sdelay(unsigned long loops){
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n"
			  "bne 1b":"=r" (loops):"0"(loops));
};

//
//struct gpio_t_ led_gpio[] = {
//	{
//		.gpio = GPIOD,
//		.pin = BV(22),
//		.mode = GPIO_MODE_OUTPUT,
//		.drv = GPIO_DRV_3,
//	},
//};



//struct gpio_t_ led_gpio[] = {
//	{
//		.gpio = GPIOC,
//		.pin = BV(4),
//		.mode = GPIO_MODE_OUTPUT,
//		.drv = GPIO_DRV_3,
//	},
//};

//для старой макетки!!!
//struct gpio_t_ led_gpio[] = {
//	{
//		.gpio = GPIOD,
//		.pin = (22),
//		.mode = GPIO_MODE_OUTPUT,
//		.drv = GPIO_DRV_3,
//	},
//};

struct gpio_t_ led_gpio[] = {
	{
		.gpio = GPIOB,
		.pin = (7),
		.mode = GPIO_MODE_OUTPUT,
		.drv = GPIO_DRV_3,
	},
};

uint32_t SystemCoreClock = 16000000;

void sunxi_clk_init(void);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cmd_h_add_to_cell_table_ext_size(	SettingCell_t *p_cell,
								uint32_t num,
								void *p_v,
								void *p_func_read,
								void *p_func_write,
								uint32_t attr,
								uint8_t size,
								const char *p_name){
SettingCell_t 	cell;
uint32_t 		i;
	for(i=0;i<COMPONENT_MAX_TABLE_SIZE;i++){
		if(!p_cell[i].CellNumber) break;
	}
	if( i>= COMPONENT_MAX_TABLE_SIZE) return;

	cell.CellAttr = attr;
	cell.CellType = size;
	cell.CellNumber = num;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)p_func_read;
	cell.WriteProc = (void*)(void*)p_func_write;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_v;
	cell.DescriptStr = p_name;

	p_cell[i] = cell;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t test_add_debug_table(control_struct *p_ctl, uint32_t pos){

		memset(&p_ctl->comp_table[0],0,sizeof(p_ctl->comp_table));
		p_ctl->p_comp_table = &p_ctl->comp_table[0];
//		uint32_t driver_eth_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, eth_resourse_s	*p_ctl, const char *p_name){
#ifdef FIRST_APP
		pos = driver_eth_add_to_cell_table(&p_ctl->comp_table[0],pos,p_ctl->p_eth_res,"net if");
#endif
//#ifdef SINGLE
#if defined(SINGLE) || defined(FIRST_APP)
		pos = tpd_add_to_cell_table(&p_ctl->comp_table[0],pos,p_ctl);
#endif
		p_ctl->comp_table_pos = pos;
	//	name_add_to_cell_table(&p_ctl->comp_table[0],pos++,"test");
		return pos;
}
////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef UDP_SENDER

#define VE_ENGINE_ENC_H264_BASE			0xb00

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef	FSBL
void tpd_dbg_start_grab(uint32_t res32, uint32_t tmp1, uint32_t tmp2, uint8_t *ptr){
	control_struct *p_ctl;
	p_ctl = (control_struct *)res32;
	cedar_start(&cedrus);//, cedrus.jpeg_data_buf,cedrus.width,cedrus.width,cedrus.height, &out_width, &out_height,100);
	return;
}
void tpd_dbg_continue_grab(uint32_t res32, uint32_t tmp1, uint32_t tmp2, uint8_t *ptr){
	control_struct *p_ctl;
	p_ctl = (control_struct *)res32;
	cedar_restart(&cedrus);//, cedrus.jpeg_data_buf,cedrus.width,cedrus.width,cedrus.height, &out_width, &out_height,100);
	return;
}
void tpd_dbg_send_jpeg_file(uint32_t res32, uint32_t tmp1, uint32_t tmp2, uint8_t *ptr){
	control_struct *p_ctl;
	p_ctl = (control_struct *)res32;
	cedar_send_jpeg_data(&cedrus);
	return;
}

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tpd_dbg_set_auto_grab(uint32_t res32, uint32_t tmp1, uint32_t tmp2, uint8_t *ptr){
	control_struct *p_ctl;
	p_ctl = (control_struct *)res32;
	p_ctl->b_start_auto = *ptr;
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* tpd_dbg_get_auto_grab(uint32_t res32, uint32_t tmp1, uint32_t tmp2, uint8_t *ptr){
	control_struct *p_ctl;
	p_ctl = (control_struct *)res32;
	return &p_ctl->b_start_auto;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tpd_dbg_set_en_send(uint32_t res32, uint32_t tmp1, uint32_t tmp2, uint8_t *ptr){
	control_struct *p_ctl;
	p_ctl = (control_struct *)res32;
	p_ctl->b_en_send = *ptr;
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* tpd_dbg_get_en_send(uint32_t res32, uint32_t tmp1, uint32_t tmp2, uint8_t *ptr){
	control_struct *p_ctl;
	p_ctl = (control_struct *)res32;
	return &p_ctl->b_en_send;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tpd_dbg_set_data_reg(uint32_t res32, uint32_t tmp1, uint32_t tmp2, uint8_t *ptr){
	control_struct *p_ctl;
	uint32_t data;// = *(uint32_t*)ptr;
	memcpy(&data,ptr,4);
	writel(data, tmp1);
	return;
}
static uint32_t tvp_d_data;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* tpd_dbg_get_data_reg(uint32_t res32, uint32_t tmp1, uint32_t tmp2){
	control_struct *p_ctl;
	p_ctl = (control_struct *)res32;
	tvp_d_data = readl(tmp1);
	return (uint8_t*)&tvp_d_data;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef	FSBL
uint32_t tpd_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, control_struct	*p_ctl){
SettingCell_t 	cell;

//	return num;


	p_cell = name_add_to_cell_table(p_cell,num++,"Tpd");



	p_cell++;

//	cell.CellAttr = WR_Att + RD_Att;
//	cell.CellType = 1;
//	cell.VarPtr = &p_ctl->phi_reg_addr;
//	cell.ReadProc = 0;
//	cell.WriteProc = 0;
//	cell.HighLim = 0;
//	cell.DefaultValue = 0;
//	cell.CellNumber = num++;
//	cell.LowLim = 0;
//	cell.DescriptStr = "phi reg addr";
//	*p_cell++ = cell;

	cell.CellAttr = Action_Att + Default_Attr + WR_Att;
	cell.CellType = 1;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)tpd_dbg_start_grab;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "start_grab";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + Default_Attr + WR_Att;
	cell.CellType = 1;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)tpd_dbg_continue_grab;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "continue_grab";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + Default_Attr + WR_Att;
	cell.CellType = 1;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)tpd_dbg_send_jpeg_file;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "send_grab";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 1;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_auto_grab;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_auto_grab;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "tvd auto en";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 1;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_en_send;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_en_send;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "tvd send en";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 1;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)cedrus_dbg_get_q_luma;
	cell.WriteProc = (void*)(void*)cedrus_dbg_set_q_luma;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)&cedrus;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "jpeg q luma";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 1;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)cedrus_dbg_get_q_chroma;
	cell.WriteProc = (void*)(void*)cedrus_dbg_set_q_chroma;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)&cedrus;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "jpeg q chroma";
	*p_cell++ = cell;


	return num;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C00000 + 0x00;
	cell.DescriptStr = "TVD_TOP_MAP 0x00";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C00000 + 0x24;
	cell.DescriptStr = "TVD_TOP_CTL 0x24";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C00000 + 0x28;
	cell.DescriptStr = "TVD_ADC_CTL 0x28";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C00000 + 0x2C;
	cell.DescriptStr = "TVD_ADC_CFG 0x2C";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x00;
	cell.DescriptStr = "TVD_EN 0x00";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x04;
	cell.DescriptStr = "TVD_MODE 0x04";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x08;
	cell.DescriptStr = "TVD_CLAMP_AGC1 0x08";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0C;
	cell.DescriptStr = "TVD_CLAMP_AGC2 0x0С";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x10;
	cell.DescriptStr = "HLOCK1 0x10";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x14;
	cell.DescriptStr = "HLOCK2 0x14";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x18;
	cell.DescriptStr = "HLOCK3 0x18";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x1C;
	cell.DescriptStr = "HLOCK4 0x1C";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x20;
	cell.DescriptStr = "HLOCK5 0x20";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0024;
	cell.DescriptStr = "TVD_VLOCK1 0x24";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0028;
	cell.DescriptStr = "TVD_VLOCK2 0x28";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0030;
	cell.DescriptStr = "TVD_CLOCK1 0x30";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0034;
	cell.DescriptStr = "TVD_CLOCK2 0x34";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0040;
	cell.DescriptStr = "TVD_YC_SEP1 0x40";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0044;
	cell.DescriptStr = "TVD_YC_SEP2 0x44";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0050;
	cell.DescriptStr = "TVD_ENHANCE1 0x50";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0054;
	cell.DescriptStr = "TVD_ENHANCE2 0x54";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0058;
	cell.DescriptStr = "TVD_ENHANCE3 0x58";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0060;
	cell.DescriptStr = "TVD_WB1 0x60";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0064;
	cell.DescriptStr = "TVD_WB2 0x64";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0068;
	cell.DescriptStr = "TVD_WB3 0x68";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x006C;
	cell.DescriptStr = "TVD_WB4 0x6C";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0080;
	cell.DescriptStr = "TVD_IRQ_CTL 0x80";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0090;
	cell.DescriptStr = "TVD_IRQ_STATUS 0x90";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0100;
	cell.DescriptStr = "TVD_DEBUG1 0x100";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0180;
	cell.DescriptStr = "TVD_STATUS1 0x180";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0184;
	cell.DescriptStr = "TVD_STATUS2 0x184";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0188;
	cell.DescriptStr = "TVD_STATUS3 0x188";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x018C;
	cell.DescriptStr = "TVD_STATUS4 0x18C";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0190;
	cell.DescriptStr = "TVD_STATUS5 0x190";
	*p_cell++ = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tpd_dbg_get_data_reg;
	cell.WriteProc = (void*)(void*)tpd_dbg_set_data_reg;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0x05C01000 + 0x0194;
	cell.DescriptStr = "TVD_STATUS6 0x194";
	*p_cell++ = cell;

	return num;
}
#endif
#ifndef	FSBL
void cntrl_udp_sender_start_init(control_struct *p_ctl){
err_t res;

	p_ctl->udp_sender_data_ptr = sender_line_buf;

	p_ctl->udp_sender_send_port = 27015;

	p_ctl->header.header = 0x345612;
	p_ctl->header.height = 576;//240;//576;
	p_ctl->header.width = 720;//GRABBER_DEF_WIDTH;
	p_ctl->header.num = 0;
	p_ctl->header.lines = 0;

	p_ctl->v_pcb = udp_new();

	IP4_ADDR(&p_ctl->udp_sender_dest_ip, 192, 168, 1, 1);

}
#endif
#endif

void ve_clk_init(void) {

	CCU->VE_CLK_REG |= UINT32_C(1) << 31;	// VE_SCLK_GATING
	(void) CCU->VE_CLK_REG;

	CCU->VE_BGR_REG |= UINT32_C(1) << 0;	// VE_GATING
	(void) CCU->VE_BGR_REG;
	CCU->VE_BGR_REG &= ~ (UINT32_C(1) << 16);	// VE_RST
	(void) CCU->VE_BGR_REG;
	CCU->VE_BGR_REG |= (UINT32_C(1) << 16);	// VE_RST
	(void) CCU->VE_BGR_REG;

}

void csi_clk_init(void) {
uint32_t reg_val = 0;
	reg_val |= (1U << 31);    // TVD_CLK_GATING = ON
	reg_val |= (1 << 24);     // CLK_SRC_SEL = PLL_VIDEO0
	reg_val |= (1 << 0);      // FACTOR_M = 1 (делитель на 2)


	CCU->CSI_CLK_REG = reg_val;
	udelay(100);

	CCU->CSI_BGR_REG |= (1 << 0) | (1 << 16);
	udelay(100);

	CCU->CSI_MASTER_CLK_REG = reg_val;
	udelay(100);
}

void iommu_init(void) {
uint32_t reg_val = 0;
	CCU->IOMMU_BGR_REG = (1 << 0);
	udelay(100);
	writel(0x01, 0x02010000 + 0x20);//en iommu
	udelay(100);
}

void CSI_DMA_IRQHandler(void){

	uint32_t status = readl(0x05809000 + 0x0054);

	if (status & 0x02) {
		writel(0x02, 0x05809000 + 0x0054);// irq clear

//		writel((uint32_t)video_luma_buf + 0x20000000, 0x05809000 + 0x20);//data addr luma
//		writel((uint32_t)video_chroma_buf + 0x20000000, 0x05809000 + 0x28);//data addr hroma

//		writel((uint32_t)video_luma_buf1, 0x05809000 + 0x20);//data addr luma
//		writel((uint32_t)video_chroma_buf1, 0x05809000 + 0x28);//data addr hroma


		control.udp_sender_buf_luma_csi = video_luma_buf_csi;//video_luma_buf_grab;
		control.udp_sender_buf_chroma_csi = video_chroma_buf_csi;//video_luma_buf;
		if(control.b_start_auto){
			osEventFlagsSet(control.cmd_evt_id ,main_cntrl_thread_event_frame_csi);
		}
	}
}
void csi_internal_clk_init(void) {
uint32_t reg_val = 0;

struct gpio_t_ gpio_pin;

gpio_pin.gpio = GPIOE;
gpio_pin.mode = GPIO_MODE_FNC2;
gpio_pin.drv = GPIO_DRV_3;
gpio_pin.pupd = GPIO_PUPD_UP;

gpio_pin.pin = 0;//HSYNC
gpio_init_pin_sun(&gpio_pin);

gpio_pin.pin = 1;//VSYNC
gpio_init_pin_sun(&gpio_pin);

gpio_pin.pin = 2;//PCLK
gpio_init_pin_sun(&gpio_pin);

gpio_pin.pin = 4;//D0
gpio_init_pin_sun(&gpio_pin);

gpio_pin.pin = 5;//D1
gpio_init_pin_sun(&gpio_pin);

gpio_pin.pin = 6;//D2
gpio_init_pin_sun(&gpio_pin);

gpio_pin.pin = 7;//D3
gpio_init_pin_sun(&gpio_pin);

gpio_pin.pin = 8;//D4
gpio_init_pin_sun(&gpio_pin);

gpio_pin.pin = 9;//D5
gpio_init_pin_sun(&gpio_pin);

gpio_pin.pin = 10;//D6
gpio_init_pin_sun(&gpio_pin);

gpio_pin.pin = 11;//D7
gpio_init_pin_sun(&gpio_pin);

gpio_pin.pin = 12;//FID
gpio_pin.mode = GPIO_MODE_FNC3;//!!!!!
gpio_init_pin_sun(&gpio_pin);

udelay(100);

	writel(0x80000000, 0x05800000 + 0);//en clk
	udelay(100);
	writel(0x01, 0x05800000 + 0x04);//en clk parser 0
	udelay(100);
	writel(0x00010001, 0x05800000 + 0x0C);//включить Post0 и Bank 0
	udelay(100);

	writel(0, 0x05801000 + 0x00);//parser 0 en
	udelay(100);
	writel(0, 0x05801000 + 0x0C);// ch0 en
	udelay(100);
	writel(0x70000000, 0x05809000 + 0x00);//

	writel(0, 0x05809000 + 0x04);//
	udelay(100);
	writel(0, 0x05800800 + 0x00);//csi top en
	udelay(100);

//	writel(0x01, 0x05800800 + 0x00);//csi top en


	writel(0x02D00000, 0x05801000 + 0x28);// обрезка кадра - 720 на 0
	writel(0x02400016, 0x05801000 + 0x2C);//


	reg_val = 0;

	reg_val |= 0x08<<24;//delay frame test
	reg_val |= 1<<20;//interlace
//		reg_val |= 1<<13;//ddr
		reg_val |= 0x01<<14;//f sync
//		reg_val |= 1<<19;//F
//		reg_val |= 1<<18;
//		reg_val |= 1<<17;

	reg_val |= 2<<6;

//		Биты 23:20 = 0x1 (Interlaced)
//		Бит 19 = 0x0 (Field polarity)
//		Бит 18 = 0x1 (VRef Positive)
//		Бит 17 = 0x1 (HRef Positive)
//		Бит 16 = 0x0 (PCLK Rising)
//		Биты 7:6 = 0x2 (UYVY)
//		Биты 4:0 = 0x0 (YUV Separate Sync)

	writel(reg_val, 0x05801000 + 0x04);//

	udelay(100);



	udelay(100);

//	reg_val = 0;
//	reg_val |= 0x10000;// (NCSIC_EN)
//	reg_val |= 0x08000;// (PCLK_EN)
//	reg_val |= 0x00001;// (PRS_EN)
//	writel(reg_val, 0x05801000 + 0x00);//parser 0 en


	udelay(100);


//		reg_val = 0;
//
//		reg_val |= 0x08<<24;//delay frame test
//		reg_val |= 1<<20;//interlace
////		reg_val |= 1<<13;//ddr
////		reg_val |= 0x02<<14;//v sync
////		reg_val |= 1<<19;
////		reg_val |= 1<<18;
////		reg_val |= 1<<17;
//
//		reg_val |= 2<<6;
//
////		Биты 23:20 = 0x1 (Interlaced)
////		Бит 19 = 0x0 (Field polarity)
////		Бит 18 = 0x1 (VRef Positive)
////		Бит 17 = 0x1 (HRef Positive)
////		Бит 16 = 0x0 (PCLK Rising)
////		Биты 7:6 = 0x2 (UYVY)
////		Биты 4:0 = 0x0 (YUV Separate Sync)
//
//		writel(reg_val, 0x05801000 + 0x04);//
//
//		udelay(100);

//		0x0024 Parser Channel_0 Input Format Register (Default Value:0x0000_0003) default yuv422


//		writel(1, 0x05801000 + 0x3C);//interlace

	writel(0x02400000, 0x05809000 + 0x14);//вертикальная обрезка
	writel(0x02D00000, 0x05809000 + 0x10);// горизонтальная обрезка кадра - 720 на 0

	reg_val = 0;
	reg_val |= 2<<10;
	reg_val |= 7<<16;//0111: frame planar YCbCr 422 UV combined (UV sequence)
	writel(reg_val, 0x05809000 + 0x04);//здесь оба типа полей

	udelay(100);

		writel(0x02, 0x05809000 + 0x0054);// irq clear
		IRQ_SetHandler(CSI_DMA0_IRQn, CSI_DMA_IRQHandler);
		IRQ_Enable(CSI_DMA0_IRQn);
		writel(0x02, 0x05809000 + 0x0050);// irq enable

	writel(0x02D002D0, 0x05809000 + 0x0038);// buflen
//		writel(0x70000017, 0x05809000 + 0x00);// start
	writel(0x70000087, 0x05809000 + 0x00);// start


//		memset(video_luma_buf,0,sizeof(video_luma_buf));
//		memset(video_chroma_buf,0,sizeof(video_chroma_buf));

//		udelay(100);
//		writel((uint32_t)video_luma_buf + 0x20000000, 0x05809000 + 0x20);//data addr luma
//		udelay(100);
//		writel((uint32_t)video_chroma_buf + 0x20000000, 0x05809000 + 0x28);//data addr hroma
//		udelay(100);
//		writel((uint32_t)video_chroma_buf + 0x20000000 + 0x20000, 0x05809000 + 0x30);//data addr hroma
//		udelay(100);

//	asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r" (0xFFFFFFFF));

//		memset((void*)0x40000000,0,64);

//		memset(video_luma_buf,0,sizeof(video_luma_buf));
//		memset(video_chroma_buf,0,sizeof(video_chroma_buf));

//	memset((void*)0x40200000 ,0,sizeof(video_luma_buf));
//	memset(video_chroma_buf,0,sizeof(video_chroma_buf));
//
//
//	udelay(100);
//	writel((uint32_t)0x40200000, 0x05809000 + 0x20);//data addr luma
//	udelay(100);
//	writel((uint32_t)video_chroma_buf+ 0x20000000, 0x05809000 + 0x28);//data addr hroma
//	udelay(100);
//	writel((uint32_t)video_chroma_buf+ 0x20000+ 0x20000000, 0x05809000 + 0x30);//data addr hroma
//	udelay(100);
uint32_t tmp;
		tmp = (uint32_t)video_luma_buf_csi;
		tmp &= 0x0FFFFFFF;
		tmp = tmp >> 2;

		udelay(100);
		writel(tmp, 0x05809000 + 0x20);//data addr luma
		udelay(100);
		tmp = (uint32_t)video_chroma_buf_csi;
		tmp &= 0x0FFFFFFF;
		tmp = tmp >> 2;

		writel(tmp, 0x05809000 + 0x28);//data addr hroma
		udelay(100);
//		writel((uint32_t)video_chroma_buf+ 0x20000+ 0x20000000, 0x05809000 + 0x30);//data addr hroma
//		udelay(100);


//		writel((uint32_t)video_luma_buf + 0x20000000 + 0x1000, 0x05809000 + 0x80);//data addr luma
//		udelay(100);
//		writel((uint32_t)video_luma_buf + 0x20000000+ 0x2000, 0x05809000 + 0x80);//data addr luma
//		udelay(100);
//		writel((uint32_t)video_luma_buf + 0x20000000+ 0x3000, 0x05809000 + 0x80);//data addr luma
//		udelay(100);
//		writel((uint32_t)video_luma_buf + 0x20000000+ 0x4000, 0x05809000 + 0x80);//data addr luma
//		udelay(100);
//
//		writel((uint32_t)video_chroma_buf + 0x20000000 + 0x1000, 0x05809000 + 0x84);//data addr hroma
//		udelay(100);
//		writel((uint32_t)video_chroma_buf + 0x20000000+ 0x2000, 0x05809000 + 0x84);//data addr hroma
//		udelay(100);
//		writel((uint32_t)video_chroma_buf + 0x20000000+ 0x3000, 0x05809000 + 0x84);//data addr hroma
//		udelay(100);
//		writel((uint32_t)video_chroma_buf + 0x20000000+ 0x4000, 0x05809000 + 0x84);//data addr hroma
//		udelay(100);
//
//		writel((uint32_t)video_chroma_buf + 0x20000000+ 0x5000, 0x05809000 + 0x88);//data addr hroma
//		udelay(100);
//		writel((uint32_t)video_chroma_buf + 0x20000000+ 0x6000, 0x05809000 + 0x88);//data addr hroma
//		udelay(100);
//		writel((uint32_t)video_chroma_buf + 0x20000000+ 0x7000, 0x05809000 + 0x88);//data addr hroma
//		udelay(100);
//		writel((uint32_t)video_chroma_buf + 0x20000000+ 0x8000, 0x05809000 + 0x88);//data addr hroma
//		udelay(100);
//

		udelay(100);

		writel(0x70000017, 0x05809000 + 0x00);// start

		reg_val = 0;
		reg_val |= 0x10000;// (NCSIC_EN)
		reg_val |= 0x08000;// (PCLK_EN)
	//	reg_val |= 0x00002;// (PRS_MODE = MCSI)
		reg_val |= 0x00001;// (PRS_EN)

		udelay(100);
		writel(0x2, 0x05801000 + 0x0C);// ch0 en
		udelay(100);
		writel(reg_val, 0x05801000 + 0x00);//parser 0 en
		udelay(100);
		writel(0x01, 0x05800800 + 0x00);//csi top en
//		writel(0x70000017, 0x05809000 + 0x00);// start

		udelay(100);

//		0x70000031
//		0x70000000 (Default Software modes)
//		0x00000020 (Frame Count En)
//		0x00000010 (DMA En)
//		0x00000001 (BK Top En)


}

//void veavc_sdram_index(uint32_t index)
//{
//  	uint32_t regs = 0x01C0E000;
//	writel(index, regs + VE_AVC_SRAM_PORT_OFFSET);
//}
//
//void veavc_jpeg_quantization(uint16_t *tableY, uint16_t *tableC, uint32_t length)
//{
//	uint32_t data;
//	int i;
//	uint32_t regs = 0x01C0E000;
//
//	veavc_sdram_index(0x0);
//
///*
//	When compared to libjpeg, there are still rounding errors in the
//	coefficients values (around 1 unit of difference).
//*/
//	for(i = 0; i < length; i++)
//	{
//		data  = 0x0000ffff & (0xffff / tableY[i]);
//		data |= 0x00ff0000 & (((tableY[i] + 1) / 2) << 16);
//		writel(data, regs + VE_AVC_SRAM_PORT_DATA);
//	}
//	for(i = 0; i < length; i++)
//	{
//		data  = 0x0000ffff & (0xffff / tableC[i]);
//		data |= 0x00ff0000 & (((tableC[i] + 1) / 2) << 16);
//		writel(data, regs + VE_AVC_SRAM_PORT_DATA);
//	}
//}
#ifndef	FSBL
//void ve_jpg_init(void) {
//	memset(jpeg_data_buf,0,sizeof(jpeg_data_buf));
//
//	cedrus.io_base = 0x01C0E000;
//	cedrus_enc_format_coded_configure(&cedrus);
//
//	cedrus.bytesperline = 720/16;
//	cedrus.width = 720;//768;//720;
//	cedrus.height = 576;
//	cedrus.num_components = 3;
////	//исправить адреса!!!! на 40
//
//	memset(video_luma_buf_grab,0,sizeof(video_luma_buf_grab));
//	memset(video_chroma_buf,0,sizeof(video_chroma_buf));
//	memset(jpeg_data_buf,0,sizeof(jpeg_data_buf));
//
//	cedrus.luma_addr = (uint32_t)video_luma_buf_grab;
//	cedrus.chroma_addr = (uint32_t)video_chroma_buf;
//	cedrus.jpeg_data_buf = jpeg_data_buf;
//	cedrus.size_jpeg_buf = sizeof(jpeg_data_buf);
//
//	cedrus.cmd_evt_id = 0;
//	cedrus.event_flag = 0;
//
//}
#endif



void ccu_enable_csi(void) {
    // Выбираем PLL_VIDEO0 как источник, ставим делитель (например, 1)
	CCU->CSI_CLK_REG = (1U << 31) | (1U << 24); // Enable + Source PLL_VIDEO0
    // Включаем Gating и снимаем Reset
	CCU->CSI_BGR_REG |= (1 << 0) | (1 << 16);
}

void tvd_clk_init(void) {
    uint32_t reg_val = 0;
    reg_val |= (1U << 31);    // TVD_CLK_GATING = ON
    reg_val |= (1 << 24);     // CLK_SRC_SEL = PLL_VIDEO0
    reg_val |= (7 << 0);      // 27 МГЦ!!!!!

    CCU->TVD_CLK_REG = reg_val;
	CCU->TVD_BGR_REG |= (1 << 0) | (1 << 16) | (1 << 1) | (1 << 17);
}

uint8_t t1=0;

void TVD_IRQHandler(void){

    if (TVD0->TVD_IRQ_STATUS & (1<<24)) {

        // --- ПАВОВКА / ОБРАБОТКА ---
        // Здесь меняем адреса буферов (Ping-Pong)
        // TVD0->TVD_WB3 = next_buffer_phys;

        // 2. СБРОС ФЛАГА (W1C)
        // Если не сбросить, прерывание будет висеть вечно
    	TVD0->TVD_IRQ_STATUS = 1 << 24;

    	if(control.udp_sender_buf_luma_tvd != video_luma_buf){
    		TVD0->TVD_WB3 = (uint32_t)video_luma_buf_grab + 0x20000000;
    		TVD0->TVD_WB4 = (uint32_t)video_chroma_buf_grab + 0x20000000;
    		control.udp_sender_buf_luma_tvd = video_luma_buf_grab;//video_luma_buf;
    		control.udp_sender_buf_chroma_tvd = video_chroma_buf_grab;//video_luma_buf;
    	}
    	else{
    		TVD0->TVD_WB3 = (uint32_t)video_luma_buf + 0x20000000;
    		TVD0->TVD_WB4 = (uint32_t)video_chroma_buf + 0x20000000;
    		control.udp_sender_buf_luma_tvd = video_luma_buf;//video_luma_buf_grab;
    		control.udp_sender_buf_chroma_tvd = video_chroma_buf;//video_luma_buf;
    	}
    	if(control.b_start_auto){
//			   gpio_set_sun(led_gpio,GPIO_SET);
    		osEventFlagsSet(control.cmd_evt_id ,main_cntrl_thread_event_frame_tvd);
   	}


//    	if(control.b_start_frame){
//    		control.b_start_frame = 0;
//    		TVD0->TVD_WB3 = (uint32_t)video_luma_buf + 0x20000000;
//    		osEventFlagsSet(control.cmd_evt_id ,main_cntrl_thread_event_frame);
//    	}
//
//    	if(control.b_start_grab){
//    		control.b_start_frame = 1;
//    		control.b_start_grab = 0;
//    		TVD0->TVD_WB3 = (uint32_t)video_luma_buf_grab + 0x20000000;
//    	}

    	TVD0->TVD_WB1 |= 1 << 8;

//		   if(t1){
//			   gpio_set_sun(led_gpio,GPIO_RESET);
//			   t1=0;
//		   }
//		   else{
//			   gpio_set_sun(led_gpio,GPIO_SET);
//			   t1=1;
//		   }

    }

}

void tvd_start_init(void) {


	control.b_en_send = 0;
	control.b_start_auto = 0;
//	udelay(100);
	osDelay(10);
	TVD_TOP->TVD_TOP_MAP = 1;

	TVD_TOP->TVD_ADC_CTL_0 = 3;//0x28
	TVD_TOP->TVD_ADC_CFG_0 = 0x0004AAAA;//0x3C

//	TVD_TOP->TVD_ADC_CFG_0 = 3;//0x3C
//	udelay(100);
	osDelay(10);

	TVD0->TVD_EN = 1;
//	udelay(100);
	osDelay(10);

	memset(video_luma_buf,0,sizeof(video_luma_buf));
	memset(video_chroma_buf,0,sizeof(video_chroma_buf));

	TVD0->TVD_WB1 = 0;
//	udelay(100);
	osDelay(5);

	TVD0->TVD_VLOCK1 =  0x02400221;//!!!!!!!!!!!


	TVD0->TVD_IRQ_STATUS = 1 << 24;
	IRQ_SetHandler(TVD_IRQn, TVD_IRQHandler);
	IRQ_Enable(TVD_IRQn);





	TVD0->TVD_WB4 = (uint32_t)video_chroma_buf + 0x20000000;
	TVD0->TVD_WB3 = (uint32_t)video_luma_buf + 0x20000000;
	TVD0->TVD_WB2 = 0x012002D0;//0x024002D0;//то 576 строк (0x240) и 720 пикселей (0x2D0)
//	udelay(100);
	osDelay(10);

	TVD0->TVD_WB1 = 0x01;
//	udelay(100);
	osDelay(10);
//	TVD0->TVD_WB1 = 0x101;

//	TVD0->TVD_WB1 = 0x02D00121;

	TVD0->TVD_IRQ_CTL = 1 << 24;

	TVD0->TVD_WB1 = (0x02D0 << 16) | 0x0123;

//	TVD0->TVD_WB1 = 0x02D00103;

//	TVD0->TVD_ENHANCE3 = 0x11000100;




	/*
	 * Для стандартного захвата PAL/NTSC в память (формат YUV422, прогрессивный кадр) используй такие настройки:
WB_EN (бит 0) = 1 — Главный рубильник записи.
WB_FMT (бит 1) = 1 — Выбираем YUV422. Это «родной» формат для TVD, он даст лучшее качество.
WB_FRAME_MODE (бит 5) = 1 — Писать целый кадр (склеивать поля).
WB_ADDR_VALID (бит 8) = 1 — КРИТИЧНО! Пока этот бит в нуле, контроллер игнорирует адреса, которые ты записал в 0x68 и 0x6C.
HACT_STRIDE (биты 27:16) = 0x2D0 (720) — Шаг строки. Для YUV422 (Semi-planar) шаг яркости равен ширине.
WB_MB_MODE (бит 4) = 0 — Оставляем Planar mode (так проще всего открывать файлы потом).
	 *
	 */

}
void tve_bus_init(void) {
    uint32_t reg_val = 0;
    reg_val |= (1U << 31);    // TVD_CLK_GATING = ON
    reg_val |= (1 << 24);     // CLK_SRC_SEL = PLL_VIDEO0
    reg_val |= (1 << 0);      // FACTOR_M = 1 (делитель на 2)

    *(volatile uint32_t *)CCU->TVE_CLK_REG = reg_val;

	CCU->TVE_BGR_REG |= (1 << 0) | (1 << 16) | (1 << 1) | (1 << 17);

    // Небольшая задержка, чтобы логика стабилизировалась после сброса
    for(volatile int i = 0; i < 1000; i++);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void udp_sending_grabber_copy_raw_image (control_struct* p_ctl, uint8_t* p_base_src, uint32_t len){
//uint16_t wrt_now_line_num;
//uint8_t* p_src;
//uint8_t* p_dst;
//uint16_t tmp;
//
//	tmp = p_ctl->header.lines;
//	if(control.grabber.next_type_frame == grabber_frame_0){
//		p_ctl->header.lines |= 0x8000;
//	}
//
//	wrt_now_line_num = len/p_ctl->header.width/2;//!!!!!!!
//	memcpy(p_ctl->udp_sender_data_ptr, &p_ctl->header,sizeof(p_ctl->header));
//	p_ctl->header.lines = tmp;
//
//	p_dst = p_ctl->udp_sender_data_ptr;
//	p_dst += sizeof(p_ctl->header);
//
//	p_ctl->udp_sender_send_size = sizeof(p_ctl->header) + wrt_now_line_num * p_ctl->header.width;
//
//	while(wrt_now_line_num){
//
//				len = p_ctl->header.width;
//				p_src = p_base_src;
//				p_src++;
//					while(len){
//						*p_dst = *p_src;
//						p_src+=4;
//						p_dst++;
//						len--;
//					}
//					wrt_now_line_num--;
//					p_ctl->header.lines++;
//					p_base_src += p_ctl->header.width*4;
//					if(p_ctl->header.lines>=p_ctl->header.height){
//						p_ctl->grabber.hdma_dcmi.Instance->CR &= ~(DMA_IT_TC | DMA_IT_HT);
//						control.header.num++;
//						control.header.lines=0;
//						if(control.grabber.next_type_frame == grabber_frame_0){
//							control.grabber.file_cntr ++;
//						}
//					}
//	}
//}
//p_ctl->on_send_udp(p_ctl->p_on_send_udp_ctl,p_ctl->p_on_send_udp_data_ptr,p_ctl->p_on_send_udp_data_size);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
err_t cntrl_udp_sender_send(control_struct *p_ctl, uint8_t* dataptr, uint16_t size){
err_t res;

	res = netbuf_ref(p_ctl->udp_sender_net,dataptr,size);
	if(res != ERR_OK){
//		printf("err net %d/r/n",res);
		return res;
	}
	res = netconn_sendto(p_ctl->p_udp_sender,p_ctl->udp_sender_net,&p_ctl->udp_sender_dest_ip,p_ctl->udp_sender_send_port);
	if(res != ERR_OK){
//		printf("err net send %i/r/n",res);
	}
	netbuf_free(p_ctl->udp_sender_net);
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RECEIVER_EXT_MODE
#ifndef NOT_VIDEO_RECEIVER
void cntrl_sel_recv_mode(control_struct *p_ctl, recv_mode_e recv_mode){
recv_mode_e old_mode;
	old_mode = p_ctl->recv_mode;
	p_ctl->recv_mode = recv_mode;
	if(p_ctl->i2c_recv_3_4.p_cfg->freq_ch_mode == i2c_recv_ch_mode){
		p_ctl->i2c_recv_3_4.freq = p_ctl->i2c_recv_3_4.table_freq_ptr[p_ctl->i2c_recv_3_4.ch_cntr];
	}
	if(p_ctl->i2c_recv_4_5.p_cfg->freq_ch_mode == i2c_recv_ch_mode){
		p_ctl->i2c_recv_4_5.freq = p_ctl->i2c_recv_4_5.table_freq_ptr[p_ctl->i2c_recv_4_5.ch_cntr];
	}

	switch(recv_mode){
	case cntr_recv_mode_1_2:
		TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);
		TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);//off!!!

		i2c_recv_power_on(&p_ctl->i2c_recv_3_4);
		i2c_recv_power_off(&p_ctl->i2c_recv_4_5);

		i2c_recv_set_freq(&p_ctl->i2c_recv_3_4,p_ctl->i2c_recv_3_4.freq);

		p_ctl->recv_active = cmd_recv_active_1_2;

	break;
	case cntr_recv_mode_5_8:

		TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);
		TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);//off!!!

		i2c_recv_power_off(&p_ctl->i2c_recv_3_4);
		i2c_recv_power_on(&p_ctl->i2c_recv_4_5);

		i2c_recv_set_freq(&p_ctl->i2c_recv_4_5,p_ctl->i2c_recv_4_5.freq);
		p_ctl->recv_active = cmd_recv_active_5_8;
	break;
	case cntr_recv_mode_1_2_5_8:
		TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);
		TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);

		i2c_recv_power_on(&p_ctl->i2c_recv_3_4);
		i2c_recv_power_on(&p_ctl->i2c_recv_4_5);

		i2c_recv_set_freq(&p_ctl->i2c_recv_3_4,p_ctl->i2c_recv_3_4.freq);
		i2c_recv_set_freq(&p_ctl->i2c_recv_4_5,p_ctl->i2c_recv_4_5.freq);

		p_ctl->recv_active = cmd_recv_active_1_2;
	break;
	case cntr_recv_mode_5_8_1_2:
		TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);
		TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);

		i2c_recv_power_on(&p_ctl->i2c_recv_3_4);
		i2c_recv_power_on(&p_ctl->i2c_recv_4_5);

		i2c_recv_set_freq(&p_ctl->i2c_recv_3_4,p_ctl->i2c_recv_3_4.freq);
		i2c_recv_set_freq(&p_ctl->i2c_recv_4_5,p_ctl->i2c_recv_4_5.freq);

		p_ctl->recv_active = cmd_recv_active_5_8;
	break;
	case cntr_recv_mode_1_2_and_5_8:
		TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);
		TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);
		i2c_recv_power_on(&p_ctl->i2c_recv_3_4);
		i2c_recv_power_on(&p_ctl->i2c_recv_4_5);

		i2c_recv_set_freq(&p_ctl->i2c_recv_3_4,p_ctl->i2c_recv_3_4.freq);
		i2c_recv_set_freq(&p_ctl->i2c_recv_4_5,p_ctl->i2c_recv_4_5.freq);

		p_ctl->recv_active = cmd_recv_active_1_2;
	break;

	}
}
#endif
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RECEIVER_BASE_MODE
#ifndef NOT_VIDEO_RECEIVER
void cntrl_sel_recv_mode(control_struct *p_ctl, recv_mode_e recv_mode){
recv_mode_e old_mode;
	old_mode = p_ctl->recv_mode;
	p_ctl->recv_mode = recv_mode;
#ifdef RECV_1_2
	if(p_ctl->recv_1_2.p_cfg->freq_ch_mode == recv_1_2_ch_mode){
		if(p_ctl->recv_1_2.ver == recv_1_2_ver_1_2){
			p_ctl->recv_1_2.freq = ch_1_2_table_freq_[p_ctl->recv_1_2.ch_cntr];
		}
		if(p_ctl->recv_1_2.ver == recv_1_2_ver_1_5_){
			p_ctl->recv_1_2.freq = ch_1_5_table_freq[p_ctl->recv_1_2.ch_cntr];
		}
	}
#endif
#ifdef		RTC6705
	if(recv_mode != cntr_recv_mode_emit){
#endif
#ifdef RTC6715
		if(p_ctl->rtc6715.p_cfg->freq_ch_mode == recv_5_8_ch_mode){
			p_ctl->rtc6715.freq = ch_5_8_freq[p_ctl->rtc6715.ch_cntr];
		}
#endif
#ifdef		RTC6705
	}
#endif
	switch(recv_mode){
	case cntr_recv_mode_1_2:
		TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);
#ifndef		NO_SEC_CH
		TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);//off!!!
#endif
#ifdef RECV_1_2
		recv_1_2_power_on(&p_ctl->recv_1_2);
#endif
#ifdef RTC6715
		rtc6715_power_off(&p_ctl->rtc6715);
#endif
#ifdef RECV_2_4
		recv_1_2_power_off(&p_ctl->recv_2_4);
#endif


		osDelay(100);
#ifdef RECV_1_2
		recv_1_2_set_freq(&p_ctl->recv_1_2,p_ctl->recv_1_2.freq);
#endif

		p_ctl->recv_active = cmd_recv_active_1_2;

	break;
	case cntr_recv_mode_5_8:
#ifdef		RTC6705
		if(old_mode == cntr_recv_mode_emit){
			rtc6705_power_off(&p_ctl->rtc6705);
		}
#endif
		TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);
#ifndef		NO_SEC_CH
		TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);//off!!!
#endif
#ifdef RECV_1_2
		recv_1_2_power_off(&p_ctl->recv_1_2);
#endif
#ifdef RTC6715
		rtc6715_power_on(&p_ctl->rtc6715);
#endif
#ifdef RECV_2_4
		recv_1_2_power_on(&p_ctl->recv_2_4);
#endif

		osDelay(100);
#ifdef RTC6715
		rtc6715_set_freq(&p_ctl->rtc6715,p_ctl->rtc6715.freq);
#endif
#ifdef RECV_2_4
		recv_1_2_set_freq(&p_ctl->recv_2_4,p_ctl->recv_2_4.freq);
#endif

		p_ctl->recv_active = cmd_recv_active_5_8;
	break;
	case cntr_recv_mode_1_2_5_8:
		TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);
#ifndef		NO_SEC_CH
		TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);
#endif
#ifdef RECV_1_2
		recv_1_2_power_on(&p_ctl->recv_1_2);
#endif
#ifdef RTC6715
		rtc6715_power_on(&p_ctl->rtc6715);
#endif
#ifdef RECV_2_4
		recv_1_2_power_on(&p_ctl->recv_2_4);
#endif
		osDelay(100);
#ifdef RECV_1_2
		recv_1_2_set_freq(&p_ctl->recv_1_2,p_ctl->recv_1_2.freq);
#endif
#ifdef RTC6715
		rtc6715_set_freq(&p_ctl->rtc6715,p_ctl->rtc6715.freq);
#endif
#ifdef RECV_2_4
		recv_1_2_set_freq(&p_ctl->recv_2_4,p_ctl->recv_2_4.freq);
#endif
		p_ctl->recv_active = cmd_recv_active_1_2;
	break;
	case cntr_recv_mode_5_8_1_2:
		TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);
#ifndef		NO_SEC_CH
		TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);
#endif
#ifdef RECV_1_2
		recv_1_2_power_on(&p_ctl->recv_1_2);
#endif
#ifdef RTC6715
		rtc6715_power_on(&p_ctl->rtc6715);
#endif
#ifdef RECV_2_4
		recv_1_2_power_on(&p_ctl->recv_2_4);
#endif

		osDelay(100);
#ifdef RECV_1_2
		recv_1_2_set_freq(&p_ctl->recv_1_2,p_ctl->recv_1_2.freq);
#endif
#ifdef RTC6715
		rtc6715_set_freq(&p_ctl->rtc6715,p_ctl->rtc6715.freq);
#endif
#ifdef RECV_2_4
		recv_1_2_set_freq(&p_ctl->recv_2_4,p_ctl->recv_2_4.freq);
#endif
		p_ctl->recv_active = cmd_recv_active_5_8;
	break;
	case cntr_recv_mode_1_2_and_5_8:
		TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);
#ifndef		NO_SEC_CH
		TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);
#endif
#ifdef RECV_1_2
		recv_1_2_power_on(&p_ctl->recv_1_2);
#endif
#ifdef RTC6715
		rtc6715_power_on(&p_ctl->rtc6715);
#endif
#ifdef RECV_2_4
		recv_1_2_power_on(&p_ctl->recv_2_4);
#endif
		osDelay(100);
#ifdef RECV_1_2
		recv_1_2_set_freq(&p_ctl->recv_1_2,p_ctl->recv_1_2.freq);
#endif
#ifdef RTC6715
		rtc6715_set_freq(&p_ctl->rtc6715,p_ctl->rtc6715.freq);
#endif
#ifdef RECV_2_4
		recv_1_2_set_freq(&p_ctl->recv_2_4,p_ctl->recv_2_4.freq);
#endif

		p_ctl->recv_active = cmd_recv_active_1_2;
	break;
#ifdef		RTC6705
	case cntr_recv_mode_emit:

			printf("emit mode =%d \r\n",p_ctl->rtc6715.freq);

			p_ctl->rtc6705.freq = p_ctl->rtc6715.freq;
			rtc6705_power_on(&p_ctl->rtc6705);

//			control.freq_mode = cntrl_freq_mode_manual;
//			control.auto_mode = cntrl_auto_mode_dis;
//
//			switch_fdc_on(&p_ctl->rtc6705.sw_3v,fdc_on);
//			switch_fdc_set_data(&p_ctl->rtc6705.sw_3v);
//
//			osDelay(50);
//
//			rtc6705_write_reg(&p_ctl->rtc6705,p_ctl->rtc6705.reg.reg7.word,7);
//			rtc6705_write_reg(&p_ctl->rtc6705,p_ctl->rtc6705.reg.reg4.word,4);
//
//			rtc6705_set_freq(&p_ctl->rtc6705,p_ctl->rtc6715.freq);
//
//			switch_fdc_on(&p_ctl->rtc6705.sw_24v,fdc_on);
//			switch_fdc_set_data(&p_ctl->rtc6705.sw_24v);

	break;
#endif



	}

}
#endif
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void app_cmd (void *argument) {
control_struct *p_ctl;
uint32_t event;
osStatus_t status;
cntrl_dev_sys_msg_que_type_s dev_sys_msg;
	p_ctl = &control;
	while (1) {

		   event = osEventFlagsWait(p_ctl->main_ctrl_thread_ev, 0x7FFFFFFF, osFlagsWaitAny, 500);
		   if(!(event & 0x80000000)){
				  if (event & cntrl_thread_event_recv_5_8_rssi){
		#ifdef RECV_5_8_RSSI
					  recv_5_8_continue_rssi(&p_ctl->rtc6715);
		#endif
				  }
//				  if (event & cntrl_thread_event_tvp_slave){
//		#ifndef		NO_SEC_CH
//					  cntrl_process_auto_detect_slave(p_ctl);
//						if(p_ctl->auto_mode == cntrl_auto_mode_en){
//							TVP_interrut_start(&control.tvp_5150_slave);
//						}
//		#endif
//					  /*
//					  if(TVP_interrut_event_check(&control.tvp_5150_slave)){
//						  osDelay(100);
//						  if(TVP_interrut_event_check(&control.tvp_5150_slave)){
//							 printf("alarm slave \r\n");
//
//							 control.freq_mode = cntrl_freq_mode_manual;
//							 control.auto_mode = cntrl_auto_mode_dis;
//
//							 switch(control.recv_mode){
//							 case  cntr_recv_mode_1_2_5_8:
//								p_ctl->recv_mode = cntr_recv_mode_5_8_1_2;
//								TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);
//								TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);
//								break;
//							 case  cntr_recv_mode_5_8_1_2:
//								p_ctl->recv_mode = cntr_recv_mode_1_2_5_8;
//								TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);//+
//								TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);
//								break;
//							 }
//						  }
//						  else{
//							  TVP_interrut_start(&control.tvp_5150_slave);
//						  }
//					  }
//					  else{
//						  osDelay(100);
//						  printf("alarm slave fake!!!! \r\n");
//						  TVP_interrut_start(&control.tvp_5150_slave);
//					  }
//					  */
//				  }
//		#ifndef NOT_VIDEO_RECEIVER
//				  if (event & cntrl_thread_event_tvp_master){
//
//						cntrl_process_auto_detect_master(p_ctl);
//						if(p_ctl->auto_mode == cntrl_auto_mode_en){
//							TVP_interrut_start(&control.tvp_5150_master);
//						}
//
//					  /*
//					  if(TVP_interrut_event_check(&control.tvp_5150_master)){
//						  osDelay(100);
//						  if(TVP_interrut_event_check(&control.tvp_5150_master)){
//							 printf("alarm master \r\n");
//							 control.freq_mode = cntrl_freq_mode_manual;
//							 control.auto_mode = cntrl_auto_mode_dis;
//						  }
//						  else{
//							  TVP_interrut_start(&control.tvp_5150_master);
//						  }
//					  }
//					  else{
//						  printf("alarm master fake!!!! \r\n");
//						  osDelay(100);
//						  TVP_interrut_start(&control.tvp_5150_master);
//					  }
//					  */
//				  }
//		#endif
				if (event & cntrl_thread_get_cmd){
					do{
						 status = osMessageQueueGet(p_ctl->main_ctrl_cmd.msg_que_id, &dev_sys_msg, NULL, 0);
						if (status == osOK){
							main_cntrl_cmd_parser(p_ctl,&dev_sys_msg);
						}
					}
					while(status == osOK);
				}
		#ifdef LORA
				  if (event & cntrl_thread_get_lora_cmd){
					 do{
						msg_ev = osMessageGet(control.lora.ctrl_cmd.cntrl_msg_que_id,0);
						if (msg_ev.status == osEventMessage){
							p_dev_sys_msg = (cntrl_dev_sys_msg_que_type_s*) msg_ev.value.v;
							lora_cntrl_cmd_parser(&control.lora,p_dev_sys_msg);
						}
						}
					while(msg_ev.status == osEventMessage);
				  }
		#endif

		   }
			  else{
		/*
					if((control.grabber.speed >= 52) && (control.grabber.height == 240)){
						cntrl_make_new_height(p_ctl,200);
					}
					if((control.grabber.speed <= 51) && (control.grabber.height == 200)){
						cntrl_make_new_height(p_ctl,240);
					}
		*/
		#ifndef NOT_VIDEO_RECEIVER
		#ifdef			 RECEIVER_EXT_MODE
					  receiver_process_ext(&control,500);
		#endif
		#ifdef			 RECEIVER_BASE_MODE
					receiver_process(&control,500);
		#endif


		#endif
		#ifdef RECV_5_8_RSSI
					if(b_start){
						recv_5_8_send_status(&control.rtc6715);
					}
		#endif
			  }


//
//		  ev = osSignalWait(0,500);//������ ��� ��������� ��� ����� ������!!!!
//		  if (ev.status == osEventSignal) {
//
//			  if (ev.value.signals & cntrl_thread_event_recv_5_8_rssi){
//	#ifdef RECV_5_8_RSSI
//				  recv_5_8_continue_rssi(&p_ctl->rtc6715);
//	#endif
//			  }
//
//
//			  if (ev.value.signals & cntrl_thread_event_tvp_slave){
//	#ifndef		NO_SEC_CH
//				  cntrl_process_auto_detect_slave(p_ctl);
//					if(p_ctl->auto_mode == cntrl_auto_mode_en){
//						TVP_interrut_start(&control.tvp_5150_slave);
//					}
//	#endif
//				  /*
//				  if(TVP_interrut_event_check(&control.tvp_5150_slave)){
//					  osDelay(100);
//					  if(TVP_interrut_event_check(&control.tvp_5150_slave)){
//						 printf("alarm slave \r\n");
//
//						 control.freq_mode = cntrl_freq_mode_manual;
//						 control.auto_mode = cntrl_auto_mode_dis;
//
//						 switch(control.recv_mode){
//						 case  cntr_recv_mode_1_2_5_8:
//							p_ctl->recv_mode = cntr_recv_mode_5_8_1_2;
//							TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);
//							TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);
//							break;
//						 case  cntr_recv_mode_5_8_1_2:
//							p_ctl->recv_mode = cntr_recv_mode_1_2_5_8;
//							TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);//+
//							TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);
//							break;
//						 }
//					  }
//					  else{
//						  TVP_interrut_start(&control.tvp_5150_slave);
//					  }
//				  }
//				  else{
//					  osDelay(100);
//					  printf("alarm slave fake!!!! \r\n");
//					  TVP_interrut_start(&control.tvp_5150_slave);
//				  }
//				  */
//			  }
//	#ifndef NOT_VIDEO_RECEIVER
//			  if (ev.value.signals & cntrl_thread_event_tvp_master){
//
//					cntrl_process_auto_detect_master(p_ctl);
//					if(p_ctl->auto_mode == cntrl_auto_mode_en){
//						TVP_interrut_start(&control.tvp_5150_master);
//					}
//
//				  /*
//				  if(TVP_interrut_event_check(&control.tvp_5150_master)){
//					  osDelay(100);
//					  if(TVP_interrut_event_check(&control.tvp_5150_master)){
//						 printf("alarm master \r\n");
//						 control.freq_mode = cntrl_freq_mode_manual;
//						 control.auto_mode = cntrl_auto_mode_dis;
//					  }
//					  else{
//						  TVP_interrut_start(&control.tvp_5150_master);
//					  }
//				  }
//				  else{
//					  printf("alarm master fake!!!! \r\n");
//					  osDelay(100);
//					  TVP_interrut_start(&control.tvp_5150_master);
//				  }
//				  */
//			  }
//	#endif
//			  if (ev.value.signals & cntrl_thread_get_cmd){
//				 do{
//					msg_ev = osMessageGet(control.main_ctrl_cmd.cntrl_msg_que_id,0);
//					if (msg_ev.status == osEventMessage){
//						p_dev_sys_msg = (cntrl_dev_sys_msg_que_type_s*) msg_ev.value.v;
//						main_cntrl_cmd_parser(&control,p_dev_sys_msg);
//					}
//					}
//				while(msg_ev.status == osEventMessage);
//			  }
//	#ifdef LORA
//			  if (ev.value.signals & cntrl_thread_get_lora_cmd){
//				 do{
//					msg_ev = osMessageGet(control.lora.ctrl_cmd.cntrl_msg_que_id,0);
//					if (msg_ev.status == osEventMessage){
//						p_dev_sys_msg = (cntrl_dev_sys_msg_que_type_s*) msg_ev.value.v;
//						lora_cntrl_cmd_parser(&control.lora,p_dev_sys_msg);
//					}
//					}
//				while(msg_ev.status == osEventMessage);
//			  }
//	#endif
//
//		  }
//		  else{
//	/*
//				if((control.grabber.speed >= 52) && (control.grabber.height == 240)){
//					cntrl_make_new_height(p_ctl,200);
//				}
//				if((control.grabber.speed <= 51) && (control.grabber.height == 200)){
//					cntrl_make_new_height(p_ctl,240);
//				}
//	*/
//	#ifndef NOT_VIDEO_RECEIVER
//	#ifdef			 RECEIVER_EXT_MODE
//				  receiver_process_ext(&control,500);
//	#endif
//	#ifdef			 RECEIVER_BASE_MODE
//				receiver_process(&control,500);
//	#endif
//
//
//	#endif
//	#ifdef RECV_5_8_RSSI
//				if(b_start){
//					recv_5_8_send_status(&control.rtc6715);
//				}
//	#endif
//		  }
	}
}


void app_main (void *argument) {
enum gpio_state_t state;
control_struct *p_ctl;
volatile uint32_t flag;
static volatile uint64_t tmp__=0;
p_ctl = &control;

//	uart_phone_thread_cfg_s conf;

//    cntrl_init(&control);
//
//    while(1){
//
//    };

	p_ctl->conf.event_flag= osEventFlagsNew(NULL);
	p_ctl->conf.p_uart_buf = p_uart_driver_array;

    debug_interface_start_init(&printf_dbg,dbg_printf_cntrl);
#ifdef	TEST_BOARD
    printf_dbg.p_pars_uart = &p_ctl->uart_parser3;
#endif

#ifdef MURKA_BOARD
    printf_dbg.p_pars_uart = &p_ctl->uart_parser0;
#endif
#ifdef SCANER_BOARD
    printf_dbg.p_pars_uart = &p_ctl->uart_parser0;
#endif



#ifdef	USE_UART1
    printf_dbg.p_pars_uart = &p_ctl->uart_parser1;
#endif

	osThreadNew(uart_driver_fone_thread, (void*)&p_ctl->conf, NULL);

	p_ctl->cmd_evt_id = osEventFlagsNew(NULL);

#ifdef	USE_UART1
	p_ctl->p_uart1 = &Driver_USART1;
#endif
#ifdef	TEST_BOARD
	p_ctl->p_uart3 = &Driver_USART3;
#endif

#ifdef	MURKA_BOARD
	p_ctl->p_uart0 = &Driver_USART0;
#endif
#ifdef	MURKA_BOARD
	uart_cmd_parser_start_init(&p_ctl->uart_parser0,p_ctl->p_uart0,main_cntrl_thread_event_uart_parser_0,p_ctl->cmd_evt_id,p_ctl->conf.event_flag);
#endif
#ifdef	SCANER_BOARD
	p_ctl->p_uart0 = &Driver_USART0;
#endif
#ifdef	SCANER_BOARD
	uart_cmd_parser_start_init(&p_ctl->uart_parser0,p_ctl->p_uart0,main_cntrl_thread_event_uart_parser_0,p_ctl->cmd_evt_id,conf.event_flag);
#endif



#ifdef	TEST_BOARD
	uart_cmd_parser_start_init(&p_ctl->uart_parser3,p_ctl->p_uart3,main_cntrl_thread_event_uart_parser_3,p_ctl->cmd_evt_id,conf.event_flag);
#endif
#ifdef	USE_UART1
	uart_cmd_parser_start_init(&p_ctl->uart_parser1,p_ctl->p_uart1,main_cntrl_thread_event_uart_parser_1,p_ctl->cmd_evt_id,conf.event_flag);
#endif

	volatile int tmp;

//	dma_init();
//	dma_test();

	osDelay(10);


//	if (sunxi_spi_init(&sunxi_spi0) != 0) {
//		printf("SPI: init failed\r\n");
//	}

//	nic_initialize();

#ifdef ETHR_GMII

	cntrl_init_i2c_test_board(p_ctl);
//
//
//	sunxi_i2c_init(&i2c_3, 3);
//
//	struct i2c_msg msgs[2];
//
//uint8_t i=0;
//uint8_t write_data[8];
//uint8_t read_data[2];
//
////output all
//write_data[0] = 6;//addr
//write_data[1] = 0x00;
//write_data[2] = 0x00;
//msgs[0].addr  = 0x24;
//msgs[0].flags = 0;       // Запись
//msgs[0].len   = 3;
//msgs[0].buf   = &write_data[0];
//sunxi_i2c_xfer(&i2c_3,msgs,1);
//
//osDelay(10);
//
////output all zero
//write_data[0] = 2;//addr
//write_data[1] = 0x00;
//write_data[2] = 0x00;
//msgs[0].addr  = 0x24;
//msgs[0].flags = 0;       // Запись
//msgs[0].len   = 3;
//msgs[0].buf   = &write_data[0];
//sunxi_i2c_xfer(&i2c_3,msgs,1);
//
//osDelay(100);
//
////output all one
//write_data[0] = 2;//addr
//write_data[1] = 0xFF;
//write_data[2] = 0xFF;
//msgs[0].addr  = 0x24;
//msgs[0].flags = 0;       // Запись
//msgs[0].len   = 3;
//msgs[0].buf   = &write_data[0];
//sunxi_i2c_xfer(&i2c_3,msgs,1);
//
//osDelay(100);
#endif

//	load_spi_nand(&sunxi_spi0);
#ifdef LWIP_EN
#ifdef FIRST_APP
	cntrl_start_lwip(p_ctl);
#endif
#endif

#ifdef LWIP_EN
#ifdef SINGLE
	cntrl_start_lwip(p_ctl);
#endif
#endif

//	while(1){
//		osDelay(1000);
//	}
#ifdef MURKA_BOARD
	test_add_debug_table(p_ctl,1);
#endif

//#ifdef FIRST_APP
//	p_ctl->list_param.height = 100;
//	p_ctl->list_param.width = 100;
//	p_ctl->list_param.top_x = 1;
//	p_ctl->list_param.top_y = 1;
//	p_ctl->list_param.flag = 1;
//
//	for(int i=0;i<MAX_NUM_FUNC_TABLE;i++){
//		memset(&p_ctl->table_list[i],0,sizeof(p_ctl->table_list[i]));
////		p_ctl->table_list[i].p_param = &p_ctl->dev_config.rec.list_param[i];
//		p_ctl->table_list[i].p_param = &p_ctl->list_param;
//
//		if (!i) p_ctl->table_list[i].p_cell_param = (cmd_cell_param_s*) &main_cell_param;
//		else p_ctl->table_list[i].p_cell_param = (cmd_cell_param_s*) &table_cell_param;
//	}
//uint8_t list_pos=0;
//	main_cntrl_make_html_cntrl_table(p_ctl,&p_ctl->table_list[list_pos++]);
//#endif


	uint8_t t1;

#ifdef MURKA_BOARD
	tvd_clk_init();
	ccu_enable_csi();
	tve_bus_init();

	tvd_start_init();

	ve_clk_init();
	ve_jpg_init(p_ctl->cmd_evt_id, main_cntrl_thread_event_jpeg);
#endif
//	csi_clk_init();


#ifdef MURKA_BOARD
//	cntrl_make_fat_fs_start_init();
	cntrl_init(p_ctl);
#endif

#ifdef SCANER_BOARD
	cntrl_scaner_init(p_ctl);
#endif

	uint8_t b_cedr_busy=0;
	uint8_t* p_luma = NULL;
	uint8_t* p_chroma = NULL;
	uint8_t b_cedr_tvd = 0;
	uint8_t b_cedr_csi = 0;
	uint8_t b_cedr_tvd_wait = 0;
	uint8_t b_cedr_csi_wait = 0;

//	uint8_t b_cedr_busy=0;

  for (;;) {

	   flag = osEventFlagsWait(p_ctl->cmd_evt_id, 0x7FFFFFFF, osFlagsWaitAny, 2000);
	   if(!(flag & 0x80000000)){
#ifdef	USE_UART1
		   if (flag & p_ctl->uart_parser1.sig_get_cmd){
			 printf_dbg.p_pars_uart = &p_ctl->uart_parser1;
			 universal_cmd_parser(&p_ctl->uart_parser1.packet);
		   }
#endif
#ifdef	TEST_BOARD
		   if (flag & p_ctl->uart_parser3.sig_get_cmd){
			 printf_dbg.p_pars_uart = &p_ctl->uart_parser3;
			 universal_cmd_parser(&p_ctl->uart_parser3.packet);
		   }
#endif
#ifdef	MURKA_BOARD
		   if (flag & p_ctl->uart_parser0.sig_get_cmd){
			 printf_dbg.p_pars_uart = &p_ctl->uart_parser0;
			 universal_cmd_parser(&p_ctl->uart_parser0.packet);
		   }
#endif
#ifdef	SCANER_BOARD
		   if (flag & p_ctl->uart_parser0.sig_get_cmd){
			 printf_dbg.p_pars_uart = &p_ctl->uart_parser0;
			 universal_cmd_parser(&p_ctl->uart_parser0.packet);
		   }
#endif


//#ifdef FIRST_APP
//		   if (flag & main_cntrl_thread_event_tcp_stack_init){
//				httpd_init(p_ctl->cmd_evt_id,main_cntrl_thread_http_cmd,arm_eth_driver_event_http_ack_get);
//				httpd_cgi_init();
//				 json_parser_init();
//		   }
#ifdef HTTP_EN
			 if (flag & main_cntrl_thread_http_cmd){
//				 httpd_sys_cmd_call();
				 osEventFlagsSet(httpd_event_flag, arm_eth_driver_event_http_ack_get);
			 }
#endif
//#endif

//#ifdef SINGLE
#if defined(SINGLE) || defined(FIRST_APP)
		   if (flag & main_cntrl_thread_event_tcp_stack_init){

			   p_ctl->tcp_8080.event = main_cntrl_thread_event_8080;
			   p_ctl->tcp_8080.event_flag_id = p_ctl->cmd_evt_id;
			   cntrl_start_8080(&p_ctl->tcp_8080);

#ifdef UDP_SENDER
			   cntrl_udp_sender_start_init(p_ctl);
#endif
			   cedar_udp_start_init(&cedrus);
#ifdef TFTP_EN
			   tftpd_init();
#endif
#ifdef HTTP_EN
				httpd_init(p_ctl->cmd_evt_id,main_cntrl_thread_http_cmd,arm_eth_driver_event_http_ack_get);
				httpd_cgi_init();
				json_parser_init();
#endif

		        osThreadNew(http_cgi_81_thread, NULL, NULL);    // Create application main thread

//		        p_ctl->b_start_auto = 1;
			}
#ifdef MURKA_BOARD
		   if (flag & main_cntrl_thread_event_frame_csi){

			   if(!b_cedr_busy){
				   b_cedr_busy = 1;
				   p_luma = 0;
				   p_chroma = 0;
					b_cedr_tvd = 1;
				   cedar_convert(&cedrus,(uint8_t*)p_ctl->udp_sender_buf_luma_csi,(uint8_t*)p_ctl->udp_sender_buf_chroma_csi);
			   }
			   else{
				   if(p_luma == NULL){
					   b_cedr_tvd_wait = 1;
					   p_luma = (uint8_t*)p_ctl->udp_sender_buf_luma_csi;
					   p_chroma = (uint8_t*)p_ctl->udp_sender_buf_chroma_csi;
				   }
			   }

//			   if(control.b_en_send){
//		uint8_t* p_mem;
//					p_ctl->header.lines = 0;
//					p_mem = (uint8_t* )p_ctl->udp_sender_buf;//video_luma_buf_grab;
//					gpio_set_sun(led_gpio, GPIO_SET);
//					struct pbuf *p_hdr = pbuf_alloc(PBUF_TRANSPORT, sizeof(p_ctl->header), PBUF_RAM);
//					struct pbuf *p_vid = pbuf_alloc(PBUF_TRANSPORT, 720 * 2, PBUF_REF);
//					pbuf_cat(p_hdr, p_vid); // Склеили их навсегда (в рамках этого кадра)
//					void *p_hdr_payload_backup = p_hdr->payload;
//
//					for(uint32_t i = 0; i < 288; i++) {
//						p_hdr->payload = p_hdr_payload_backup;
//						p_hdr->len = sizeof(p_ctl->header);
//						p_hdr->tot_len = sizeof(p_ctl->header) + 1440;
//						p_ctl->header.lines = i * 2;
//						memcpy(p_hdr->payload, &p_ctl->header, sizeof(p_ctl->header));
//						p_vid->payload = p_mem; // Подменяем адрес строки в DDR
//						p_mem += 720 * 2;
//						LOCK_TCPIP_CORE();
//						udp_sendto(p_ctl->v_pcb, p_hdr, &p_ctl->udp_sender_dest_ip, p_ctl->udp_sender_send_port);
//						UNLOCK_TCPIP_CORE();
//					}
//					pbuf_free(p_hdr); // Вот теперь освобождаем цепочку до следующего кадра
//					gpio_set_sun(led_gpio, GPIO_RESET);
//			   }
		   }
#endif
#ifdef MURKA_BOARD
		   if (flag & main_cntrl_thread_event_frame_tvd){

			   if(!b_cedr_busy){
				   b_cedr_busy = 1;
				   p_luma = 0;
				   p_chroma = 0;
				   b_cedr_csi = 1;
				   cedar_convert(&cedrus,(uint8_t*)p_ctl->udp_sender_buf_luma_tvd,(uint8_t*)p_ctl->udp_sender_buf_chroma_tvd);
			   }
			   else{
				   if(p_luma == NULL){
					   b_cedr_csi_wait = 1;
					   p_luma = (uint8_t*)p_ctl->udp_sender_buf_luma_tvd;
					   p_chroma = (uint8_t*)p_ctl->udp_sender_buf_chroma_tvd;
				   }
			   }

//			   if(control.b_en_send){
//		uint8_t* p_mem;
//					p_ctl->header.lines = 0;
//					p_mem = (uint8_t* )p_ctl->udp_sender_buf;//video_luma_buf_grab;
//					gpio_set_sun(led_gpio, GPIO_SET);
//					struct pbuf *p_hdr = pbuf_alloc(PBUF_TRANSPORT, sizeof(p_ctl->header), PBUF_RAM);
//					struct pbuf *p_vid = pbuf_alloc(PBUF_TRANSPORT, 720 * 2, PBUF_REF);
//					pbuf_cat(p_hdr, p_vid); // Склеили их навсегда (в рамках этого кадра)
//					void *p_hdr_payload_backup = p_hdr->payload;
//
//					for(uint32_t i = 0; i < 288; i++) {
//						p_hdr->payload = p_hdr_payload_backup;
//						p_hdr->len = sizeof(p_ctl->header);
//						p_hdr->tot_len = sizeof(p_ctl->header) + 1440;
//						p_ctl->header.lines = i * 2;
//						memcpy(p_hdr->payload, &p_ctl->header, sizeof(p_ctl->header));
//						p_vid->payload = p_mem; // Подменяем адрес строки в DDR
//						p_mem += 720 * 2;
//						LOCK_TCPIP_CORE();
//						udp_sendto(p_ctl->v_pcb, p_hdr, &p_ctl->udp_sender_dest_ip, p_ctl->udp_sender_send_port);
//						UNLOCK_TCPIP_CORE();
//					}
//					pbuf_free(p_hdr); // Вот теперь освобождаем цепочку до следующего кадра
//					gpio_set_sun(led_gpio, GPIO_RESET);
//			   }
		   }
#endif
#ifdef MURKA_BOARD
		   if (flag & main_cntrl_thread_event_jpeg){
			   cedar_finish(&cedrus);
			   if(b_cedr_csi){
				   b_cedr_csi = 0;
				   cedrus.udp_sender_send_port = 27025;
//				   if(p_ctl->tcp_8080.p_a_conn_1){
//
//					   char frame_header[100];
//					   memset(frame_header,0,100);
//					   int header_len = sprintf(frame_header,
//					       "--frame_boundary\r\n"
//					       "Content-Type: image/jpeg\r\n"
//					       "Content-Length: %u\r\n\r\n",
//					       (unsigned int)cedrus.size_jpeg_data_file);
//
//					   // 1. Шлем заголовок кадра (строку)
//					   netconn_write(p_ctl->tcp_8080.p_a_conn_1, frame_header, header_len, NETCONN_COPY);
//
//					   netconn_write(p_ctl->tcp_8080.p_a_conn_1, cedrus.jpeg_data_buf, cedrus.size_jpeg_data_file, NETCONN_NOCOPY);
//					   netconn_write(p_ctl->tcp_8080.p_a_conn_1, "\r\n", 2, NETCONN_COPY);
//				   }
			   }
			   if(b_cedr_tvd){
				   b_cedr_tvd = 0;
				   cedrus.udp_sender_send_port = 27026;
				   if(p_ctl->tcp_8080.p_a_conn_1){

					   char frame_header[100];
					   memset(frame_header,0,100);
					   int header_len = sprintf(frame_header,
					       "--frame_boundary\r\n"
					       "Content-Type: image/jpeg\r\n"
					       "Content-Length: %u\r\n\r\n",
					       (unsigned int)cedrus.size_jpeg_data_file);

					   // 1. Шлем заголовок кадра (строку)
					   netconn_write(p_ctl->tcp_8080.p_a_conn_1, frame_header, header_len, NETCONN_COPY);

					   netconn_write(p_ctl->tcp_8080.p_a_conn_1, cedrus.jpeg_data_buf, cedrus.size_jpeg_data_file, NETCONN_NOCOPY);
					   netconn_write(p_ctl->tcp_8080.p_a_conn_1, "\r\n", 2, NETCONN_COPY);
				   }
			   }
//			   cedar_send_jpeg_data(&cedrus);
			   b_cedr_busy = 0;
			   if(p_luma){
				   b_cedr_busy = 1;
					if(b_cedr_tvd_wait){
						b_cedr_tvd_wait = 0;
						b_cedr_tvd = 1;
					}
					if(b_cedr_csi_wait){
						b_cedr_csi_wait = 0;
						b_cedr_csi = 1;
					}
				   cedar_convert(&cedrus,p_luma,p_chroma);
				   p_luma = NULL;
				   p_chroma = NULL;
			   }
		   }
		   if (flag & main_cntrl_thread_event_8080){
			   tcp_cmd_ethernet_get_msg(&p_ctl->tcp_8080);
		   }
#endif
#endif
//		   2. Как понять, что перепутаны поля (Четное/Нечетное)
//		   Если поля (Fields) собраны неверно или не угадана полярность сигнала FIELD (бит 19 в 0x05801004):
//		   Симптом 1 («Расческа»): При движении объектов края выглядят как зубья пилы. Это нормально для Interlace, но если зубья слишком длинные — поля перепутаны местами (Top-Field-First vs Bottom-Field-First).
//		   Симптом 2 («Дрожание»): Статичная картинка мелко подрагивает вверх-вниз на одну строку. Это значит, что ты записываешь четное поле в адрес нечетного.
//		   Симптом 3 (Сдвиг на строку): Вся картинка смещена вертикально, и сверху/снизу видна черная полоса в 1 пиксель.
//		   Как это вылечить в регистрах:
//		   В Парсере (0x05801004):
//		   Бит 16 (CLK_POL): Попробуй переключить (0 <-> 1). Ищи вариант с минимальным шумом.
//		   Бит 19 (FIELD_POL): Если картинка «дрожит» или «пилит» — инвертируй его. Это поменяет порядок строк (0-2-4... и 1-3-5...).
//		   Бит 20 (SRC_TYPE): Убедись, что он в 1 (Interlace), раз у тебя PAL.
//		   В DMA (0x05800008):
//		   Бит 10 (FIELD_SEL): Посмотри, как настроен захват. Обычно для PAL ставят 0 (Both fields).
//		   Режим записи: Если ты используешь Frame Planar (бит 3:0 = 0111), DMA сам должен «переплетать» строки из разных полей в один кадр. Если он этого не делает, проверь настройки Stride (0x0040) — для Interlace он должен учитывать, что строки пишутся через одну.
//		   Совет по 70 Мбит:
//		   Если поля перепутаны, JPEG-кодер сходит с ума, пытаясь сжать «кашу» из строк. Как только ты их синхронизируешь, картинка станет резкой, а поток (битрейт) при том же качестве упадет еще на 15-20%, потому что исчезнет паразитный высокочастотный шум от «дрожания» строк.
//		   Что в итоге? После инверсии бита 19 (Field Pol) дрожание строк пропало?
	   }
	   else{
//		   if(t1){
//			   gpio_set_sun(led_gpio,GPIO_RESET);
//			   t1=0;
//		   }
//		   else{
//			   gpio_set_sun(led_gpio,GPIO_SET);
//			   t1=1;
//		   }
//		   printf("Test! \r\n");
		   tmp=0;
//		   sun8i_emac_eth_send(&emac_dev,p_ctl->cmd_evt_id,500);
	   }

  }
}

/*
133'631 bytes of readonly  code memory
 11'229 bytes of readonly  data memory
3'365'229 bytes of readwrite data memory

    136'350 bytes of readonly  code memory
     11'353 bytes of readonly  data memory
  3'362'829 bytes of readwrite data memory

    136'351 bytes of readonly  code memory
     11'337 bytes of readonly  data memory
  3'362'801 bytes of readwrite data memory

*/

int sys_dram_init(void);
//#pragma section="HEAP"
//
//void check_start() {
//    char * h_start = __section_begin("HEAP");
//    char * h_end   = __section_end("HEAP");
//    unsigned int heap_size = (unsigned int)h_end - (unsigned int)h_start;
//    memset(h_start,0xAA,heap_size);
//    // Теперь можно печатать или проверять
//}
//
//void check_heap() {
//    char * h_start = __section_begin("HEAP");
//    char * h_end   = __section_end("HEAP");
//    unsigned int heap_size = (unsigned int)h_end - (unsigned int)h_start;
//
//    // Теперь можно печатать или проверять
//}

//https://wiki.st.com/stm32mpu/wiki/Cortex-M_remote_processor_management_overview
//https://linux-sunxi.org/VE_Register_guide#AVC_Encoder_Engine_Registers
void main(void){
//        gd = &_gd;
//	check_start();

//        sunxi_sram_init();
volatile uint32_t tmp;

#ifndef FIRST_APP
#ifndef	FSBL
//        irq_init();
#endif

#ifndef	FSBL
        __asm volatile("cpsid i");

        mmu_enable();

        sunxi_clk_init();

    	timer_init();

        sunxi_clk_dump();
        
        sys_dram_init();

        gpio_init_pin_sun(led_gpio);
        gpio_set_sun(led_gpio,GPIO_RESET);

        irq_init();//Перенес на тест!!

#endif
#endif

#ifdef	FIRST_APP

        irq_init();
        __asm volatile("cpsid i");

//        irq_init();
//        mmu_enable();
uint32_t num = 0;
		while(1){
			for(int i=0; i<1000;i++){
				udelay(200);
			}
			gpio_set_sun(led_gpio,GPIO_RESET);
			for(int i=0; i<1000;i++){
				udelay(200);
			}
			gpio_set_sun(led_gpio,GPIO_SET);
			num++;
			if(num>20) break;
		}

#endif

#ifdef	FSBL

        mmu_enable();

volatile int size;

        sunxi_clk_init();
        gpio_init_pin_sun(led_gpio);
        gpio_set_sun(led_gpio,GPIO_RESET);

        timer_init();
        sunxi_clk_dump();
        size = sys_dram_init();
//        return;

#endif

#ifdef	FSBL

		if (sunxi_spi_init(&sunxi_spi0) != 0) {
	//		printf("SPI: init failed\r\n");
		}
int res_load;
        res_load= load_spi_nand(&sunxi_spi0);
        if(!res_load){
        	void (*app_entry)(void) = (void (*)(void))0x20000100;
        	app_entry();
        	while(1){

        	}
        }
//        if(!size){
//			while(1){
//				gpio_set_sun(led_gpio,GPIO_RESET);
//				gpio_set_sun(led_gpio,GPIO_SET);
//
//			};
//        }
        else{
        	uint32_t nn=0;
			while(1){
				for(int i=0; i<1000;i++){
					udelay(1000);
				}
				gpio_set_sun(led_gpio,GPIO_RESET);
				for(int i=0; i<1000;i++){
					udelay(1000);
				}
				gpio_set_sun(led_gpio,GPIO_SET);
//				nn++;
//				if(nn>10) break;
			}
        }
#endif

    	L1C_InvalidateICacheAll();
    	L1C_InvalidateDCacheAll();

    	L1C_CleanDCacheAll();


//    	tvd_clk_init();
//    	ccu_enable_csi();
//    	tve_bus_init();
//
//    	tvd_start_init();
//
//    	ve_clk_init();
//    	ve_jpg_init(p_ctl->cmd_evt_id, main_cntrl_thread_event_jpeg);




static volatile        osStatus_t res;

		memset(&osRtxInfo,0,sizeof(osRtxInfo));
		osRtxInfo.os_id = osRtxKernelId;//, .version = osRtxVersionKernel, .kernel.state = osRtxKernelInactive
		osRtxInfo.version = osRtxVersionKernel;
		osRtxInfo.kernel.state = osRtxKernelInactive;
        res = osKernelInitialize();                 // Initialize CMSIS-RTOS


        const osThreadAttr_t thread2_attributes = {
            .name = "MyThread2",               // Имя потока (для отладчика)
            .attr_bits = 0U,                   // Дополнительные биты атрибутов (по умолчанию 0)
            .cb_mem = NULL,                    // Память для блока управления потоком (динамически)
            .cb_size = 0U,
            .stack_mem = NULL,                 // Память для стека (динамически)
            .stack_size = 1024 * 2,            // Размер стека в байтах (2 КБ)
            .priority = osPriorityHigh,        // Приоритет (выше обычного)
            .tz_module = 0U,
        };

        osThreadNew(app_main, NULL, &thread2_attributes);    // Create application main thread
        osKernelStart();
}
//
//void vApplicationMallocFailedHook(void)
//{
//	uart_printf("malloc fejld\n");
//
//	while(1);
//
//}
//void vApplicationIdleHook( void )
//{
////	uart_printf("k\n");
//}


//https://github.com/minilogic/f1c_nonos?ysclid=mk9z8ost3g168736436
//https://github.com/uli/allwinner-bare-metal/tree/jh/lwip


//	while(1){
//
//
//		write_data[0] = 2;//addr
//		write_data[1] = 0x00;
//		write_data[2] = 0x00;
//		msgs[0].addr  = 0x24;
//		msgs[0].flags = 0;       // Запись
//		msgs[0].len   = 3;
//		msgs[0].buf   = &write_data[0];
//
//		res_i2c = sunxi_i2c_xfer(&i2c_3,msgs,1);
//
//		osDelay(1000);
//
//		write_data[0] = 2;//addr
//		write_data[1] = 0xFF;
//		write_data[2] = 0xFF;
//		msgs[0].addr  = 0x24;
//		msgs[0].flags = 0;       // Запись
//		msgs[0].len   = 3;
//		msgs[0].buf   = &write_data[0];
//
//		res_i2c = sunxi_i2c_xfer(&i2c_3,msgs,1);
//
//		osDelay(1000);
//
////
////
////	// 1. Сообщение: ЗАПИСЬ адреса регистра
////	write_data[0] = 6;
////	msgs[0].addr  = 0x24;
////	msgs[0].flags = 0;       // Запись
////	msgs[0].len   = 1;
////	msgs[0].buf   = &write_data[0];
////
////	// 2. Сообщение: ЧТЕНИЕ данных
////	msgs[1].addr  = 0x24;
////	msgs[1].flags = I2C_M_RD; // Чтение
////	msgs[1].len   = 2;        // Читаем сразу Port 0 и Port 1
////	msgs[1].buf   = read_data;
////
////	res_i2c = sunxi_i2c_xfer(&i2c_3,msgs,2);
////	printf("resw i2c0=%x %x %x \r\n",res_i2c,read_data[0],read_data[1]);
////
////	osDelay(1000);
//	}





//	while(1){
//
//		msg.flags = 0;
//		msg.len = 2;
//		msg.buf = data;
//
//		data[0] = 0x06;
//		data[1] = i++;
//		res_i2c = sunxi_i2c_xfer(&i2c_3,&msg,1);
//		printf("resw i2c0=%x %x %x %x %x %x %x %x %x \r\n",res_i2c,data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
//		osDelay(1000);
//
//
//		msg.flags = I2C_M_RD;
//		msg.len = 2;
//		msg.buf = data;
//
//		data[0] = 0x06;
//		data[1] = 0xFF;
//		res_i2c = sunxi_i2c_xfer(&i2c_3,&msg,1);
//		printf("resr i2c0=%x %x %x %x %x %x %x %x %x \r\n",res_i2c,data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
//		osDelay(1000);
////		data[0] = 0x01;
////		data[1] = 0xFF;
////		printf("res i2c1=%x %x %x\r\n",res_i2c,data[0],data[1]);
//////		printf("res i2c=%x\r\n",res_i2c);
////		osDelay(1000);
//	}


//
//uint8_t data[16];
//data[0] = 0x06;
//data[1] = 0xFF;
////	msg.addr = 0x24;
////	msg.flags = 0;//I2C_M_RD;
////	msg.len = 8;
////	msg.buf = data;
//
//osDelay(100);
//
//int res_i2c;

//	res_i2c = sunxi_i2c_xfer(&i2c_3,&msg,1);
//	printf("res i2c=%x\r\n",res_i2c);
//	osDelay(1000);
//
//	msg.flags = I2C_M_RD;
//	data[0] = 0x00;
//	data[1] = 0xFF;
//	res_i2c = sunxi_i2c_xfer(&i2c_3,&msg,1);
//	printf("res i2c=%x\r\n",res_i2c);

//	msg.flags = 0;
//	msg.len = 1;
//	msg.buf = data;
//
//	data[0] = 0x02;
//	data[1] = 0xAA;
//	res_i2c = sunxi_i2c_xfer(&i2c_3,&msg,1);
//
//
//	msg.flags = 0;
//	msg.len = 1;
//	msg.buf = data;
//
//	data[0] = 0x03;
//	data[1] = 0x55;
//	res_i2c = sunxi_i2c_xfer(&i2c_3,&msg,1);
//
//	msg.flags = 0;
//	msg.len = 1;
//	msg.buf = data;
//
//	data[0] = 0x04;
//	data[1] = 0xA5;
//	res_i2c = sunxi_i2c_xfer(&i2c_3,&msg,1);
//
//
//	msg.flags = 0;
//	msg.len = 1;
//	msg.buf = data;
//
//	data[0] = 0x05;
//	data[1] = 0x5A;
//	res_i2c = sunxi_i2c_xfer(&i2c_3,&msg,1);






//Фрагмент кода
//.global _start
//.global irq_handler_asm
//
//_start:
//    b   reset_handler
//    b   .               @ Undefined Instruction
//    b   .               @ Software Interrupt (SWI)
//    b   .               @ Prefetch Abort
//    b   .               @ Data Abort
//    b   .               @ Reserved
//    b   irq_handler_asm @ IRQ Vector (Offset 0x18)
//    b   .               @ FIQ
//
//@ --- Вход в прерывание ---
//irq_handler_asm:
//    @ 1. Сохраняем контекст (R0-R3, R12, LR)
//    @ LR в режиме IRQ содержит адрес возврата + 4, поэтому корректируем его:
//    sub     lr, lr, #4
//    push    {r0-r3, r12, lr}
//
//    @ 2. Вызываем C-функцию обработчика
//    bl      c_irq_handler
//
//    @ 3. Восстанавливаем контекст
//    pop     {r0-r3, r12, lr}
//
//    @ 4. Возврат из прерывания (копирует SPSR в CPSR)
//    movs    pc, lr
//2. Полный код на C (main.c)
//Этот код объединяет настройку CP15 (таймера) и GIC.
//
//C
//#include <stdint.h>
//
///* --- Адреса регистров (подставьте свои для вашего SoC) --- */
#define GIC_DIST_BASE   (GIC_BASE + 0x1000)
#define GIC_CPU_BASE    (GIC_BASE + 0x2000)
//
///* --- Функции GIC (упрощенные) --- */
#define MMIO32(addr) (*(volatile uint32_t *)(addr))

void gic_setup_timer_irq(void) {
    // 1. Включаем GIC CPU Interface
    MMIO32(GIC_CPU_BASE + 0x000) = 1; // GICC_CTLR
    MMIO32(GIC_CPU_BASE + 0x004) = 0xFF; // GICC_PMR (разрешить все приоритеты)

    // 2. Включаем GIC Distributor
    MMIO32(GIC_DIST_BASE + 0x000) = 1; // GICD_CTLR

    // 3. Разрешаем ID 30 (PPI Timer)
    // ID 30 находится в регистре GICD_ISENABLER0 (bit 30)
    MMIO32(GIC_DIST_BASE + 0x100) = (1 << 29);

    // Примечание: Для PPI ID 30 обычно не нужно настраивать Target CPU,
    // так как оно локально. Приоритет по умолчанию тоже подойдет.
}

/* --- Глобальная переменная счетчика (для теста) --- */
volatile uint32_t system_ticks = 0;

/* --- Обработчик прерывания (вызывается из asm) --- */
void c_irq_handler(void) {
	enum gpio_state_t state;
    // 1. Читаем ID прерывания (Acknowledge)
    uint32_t iar = MMIO32(GIC_CPU_BASE + 0x00C); // GICC_IAR
    uint32_t id = iar & 0x3FF;

    if (id == 29) {
        // --- Логика Таймера ---
        system_ticks++;


//        state = gpio_get(led_gpio);
//        if(state == GPIO_RESET) {
//      	  gpio_set(led_gpio,GPIO_SET);
//        }
//        else{
//      	  gpio_set(led_gpio,GPIO_RESET);
//        }

        // КРИТИЧНО ВАЖНО: Перезарядка таймера!
        // Если этого не сделать, уровень сигнала прерывания останется HIGH,
        // и прерывание сработает снова сразу после выхода.
        uint32_t freq = 24000000;
        PL1_SetLoadValue(freq / 1000); // Снова заводим на 10 мс
    }

    // 2. Сообщаем GIC, что закончили (End Of Interrupt)
    MMIO32(GIC_CPU_BASE + 0x010) = iar; // GICC_EOIR
}

/* --- Функция запуска таймера --- */
void start_timer(void) {
    uint32_t freq = 24000000;
    PL1_SetControl(0);
    PL1_SetLoadValue(freq / 100);
    PL1_SetControl(1);
}
/*
Самые частые ошибки (Checklist)
Забыли cpsie i: Вы настроили GIC, настроили таймер, но само ядро процессора игнорирует линию IRQ, потому что бит I в CPSR установлен в 1 (Masked) при сбросе.

Забыли обновить CNTP_TVAL в обработчике: Generic Timer работает как компаратор. Когда Count >= Cval, линия прерывания становится HIGH и остается такой, пока вы не запишете новое значение в TVAL (что отодвинет CVAL в будущее). Без этого вы получите "interrupt storm" (бесконечный цикл входа в прерывание).

Неверная таблица векторов: Если регистр VBAR (Vector Base Address Register) указывает не туда, где лежит ваша инструкция b irq_handler_asm, процессор прыгнет "в никуда".

Secure vs Non-Secure: Если вы работаете в Secure mode (например, сразу после ресета без загрузки Linux), убедитесь, что вы используете правильные регистры. Иногда доступ к Non-Secure таймеру из Secure режима требует настройки регистра SCR (Secure Configuration Register), но обычно CNTP (Physical Timer) работает нормально.

Следующий шаг
Система настроена и прерывания тикают. Хотите узнать, как на базе этого таймера реализовать простой планировщик задач (Context Switching) для переключения между двумя потоками?

Почему у меня работает на таймере 29 прерывание

Если у вас таймер срабатывает на прерывании ID 29, а не ID 30, это означает, что вы используете Secure Physical Timer (защищенный физический таймер).

Это абсолютно нормальная ситуация для bare-metal программирования, но она указывает на то, в каком режиме работает ваш процессор и какие регистры вы, вероятно, используете.

Вот подробное объяснение, почему так происходит.

1. Карта прерываний таймера (PPI)
В архитектуре ARMv7-A (Cortex-A7) каждому логическому таймеру присвоен свой жесткий ID прерывания (PPI):

Таймер  Регистры CP15  PPI ID  GIC ID  Описание
Non-Secure Physical  CNTP_* (c14, c2)  14  30  Обычный таймер для ОС (Linux, Android).
Secure Physical  CNTPS_* (c14, c5)  13  29  Таймер для Secure World (TrustZone).
Virtual Timer  CNTV_* (c14, c3)  11  27  Для гостевых ОС под гипервизором.
Hypervisor Timer  CNTHP_* (c14, c12)  10  26  Для самого гипервизора.
2. Почему у вас работает ID 29?
Есть две основные причины, почему система генерирует именно это прерывание:

Причина А: Вы используете регистры Secure Timer (Чаще всего)
Проверьте ваш ассемблерный код или макросы записи в регистры таймера. Если в команде MCR используется c5 вместо c2, вы физически обращаетесь к Secure-таймеру.

Код для ID 30 (Non-Secure):

C
// c14, c2, 0 -> CNTP_TVAL (Non-Secure)
__asm volatile("mcr p15, 0, %0, c14, c2, 0" : : "r"(val));
Код для ID 29 (Secure):

C
// c14, c5, 0 -> CNTPS_TVAL (Secure)
__asm volatile("mcr p15, 0, %0, c14, c5, 0" : : "r"(val));
Причина Б: Вы находитесь в Secure Mode (TrustZone)
При старте платы (Cold Boot) процессор Cortex-A7 всегда просыпается в режиме Secure SVC (Supervisor). Если вы не переключали процессор в Non-Secure mode (обычно это делает загрузчик U-Boot перед запуском Linux), вы находитесь в "защищенном мире".

В Secure World вы имеете доступ к обоим таймерам.

Если вы настроили Secure Timer (CNTPS), сработает ID 29.

Если вы настроили Non-Secure Timer (CNTP), сработает ID 30.

Однако, некоторые специфичные реализации SoC или настройки регистра SCR (Secure Configuration Register) могут перенаправлять запросы или маскировать Non-Secure прерывания, поэтому в Secure-режиме часто надежнее использовать именно Secure Timer (ID 29).

*/



//	typedef struct {
//	  const char                   *name;   ///< name of the message queue
//	  uint32_t                 attr_bits;   ///< attribute bits
//	  void                      *cb_mem;    ///< memory for control block
//	  uint32_t                   cb_size;   ///< size of provided memory for control block
//	  void                      *mq_mem;    ///< memory for data storage
//	  uint32_t                   mq_size;   ///< size of provided memory for data storage
//	} osMessageQueueAttr_t;

//	osMessageQueueAttr_t msg_attr;
//	static osMessageQueueId_t myQueueId;

//#define MSG_COUNT 10            // Максимальное количество сообщений
//#define MSG_SIZE  sizeof(uint32_t) // Размер одного сообщения (например, 4 байта)
//
//// 2. Резервируем память под управляющий блок (Control Block)
//static osMessageQueueId_t myQueueId;
//static uint32_t myQueue_cb[osRtxMessageQueueCbSize / 4]; // Размер CB фиксирован в CMSIS
//
//// 3. Резервируем память под сами данные очереди
//// Внимание: размер должен быть (msg_count * msg_size)
//static uint8_t myQueue_mem[MSG_COUNT * MSG_SIZE];
////	uint32_t 			os_message[4 + UNI_PROT_MSG_BUF_SIZE];
////	osMessageQDef_t		os_message_def;
////	osMessageQId    	os_message_write_id;
//
//    osMessageQueueAttr_t queue_attr = {
//        .name = "MyStaticQueue",
//        .cb_mem = &myQueue_cb,         // Указатель на память управляющего блока
//        .cb_size = sizeof(myQueue_cb), // Размер блока управления
//        .mq_mem = &myQueue_mem,         // Указатель на буфер данных
//        .mq_size = sizeof(myQueue_mem)  // Общий размер буфера данных
//    };

/*
    static uint32_t myQueue_cb[osRtxMessageQueueCbSize / 4];
    static uint32_t myQueue_mem[(MSG_COUNT * (((MSG_SIZE + 3U) & ~3UL) + sizeof(osRtxMessage_t))) / 4];

    const osMessageQueueAttr_t queue_attr = {
        .cb_mem = myQueue_cb,
        .cb_size = sizeof(myQueue_cb),
        .mq_mem = myQueue_mem,
        .mq_size = sizeof(myQueue_mem)
    };

osMemoryPoolNew

	myQueueId = osMessageQueueNew(MSG_COUNT, MSG_SIZE, NULL);
	osStatus_t s;
	uint32_t data = 0xABCDEF12;
	s = osMessageQueuePut(myQueueId, &data, 0, osWaitForever);
	s = osMessageQueuePut(myQueueId, &data, 0, osWaitForever);
	s = osMessageQueuePut(myQueueId, &data, 0, osWaitForever);

	uint32_t data1;
	s = osMessageQueueGet(myQueueId, &data1, 0, 0);
	s = osMessageQueueGet(myQueueId, &data1, 0, 0);
	s = osMessageQueueGet(myQueueId, &data1, 0, 0);
	s = osMessageQueueGet(myQueueId, &data1, 0, 0);
*/

//
//#define POOL_BLOCKS  10               // Количество блоков
//#define BLOCK_SIZE   32               // Размер одного блока в байтах
//
//// 1. Память для управляющего блока (Control Block)
//#pragma location=".ddr_data"
//static uint32_t myPool_cb[osRtxMemoryPoolCbSize / 4];
//
//// 2. Память для данных пула (массив блоков)
//// Важно: каждый блок должен быть выровнен.
//// Используем uint32_t для автоматического выравнивания.
////#pragma location=".ddr_data"
//static uint32_t myPool_mem[(POOL_BLOCKS * ((BLOCK_SIZE + 3) & ~3)) / 4];
//
//osMemoryPoolId_t myPoolId;
//
//    osMemoryPoolAttr_t pool_attr = {
//        .name = "MyStaticPool",
//        .cb_mem = myPool_cb,
//        .cb_size = sizeof(myPool_cb),
//        .mp_mem = myPool_mem,
//        .mp_size = sizeof(myPool_mem)
//    };
//
//    // Создание пула
//    // Параметры: кол-во блоков, размер блока, атрибуты
//    myPoolId = osMemoryPoolNew(POOL_BLOCKS, BLOCK_SIZE, &pool_attr);
//
//    if (myPoolId == NULL) {
//        // Ошибка создания
//    }
//    void* p_tmp;
//
//    p_tmp = osMemoryPoolAlloc(myPoolId, 0);
//    p_tmp = osMemoryPoolAlloc(myPoolId, 0);
//
//    osMemoryPoolFree(myPoolId, p_tmp);
//
//	osDelay(50);
/*
133'630 bytes of readonly  code memory
 11'221 bytes of readonly  data memory
3'362'181 bytes of readwrite data memory
*/


/*
1. Отключаем софтовый CRC в lwipopts.h
Найди и замени (или добавь) следующие макросы:
c
#define CHECKSUM_GEN_IP       0
#define CHECKSUM_GEN_UDP      0
#define CHECKSUM_GEN_TCP      0
#define CHECKSUM_CHECK_IP     0
#define CHECKSUM_CHECK_UDP    0
#define CHECKSUM_CHECK_TCP    0
Используйте код с осторожностью.

2. Включаем аппаратный CRC в драйвере EMAC (T113-S4)
Контроллер EMAC в T113 умеет сам вставлять контрольные суммы в IP и UDP пакеты при передаче. Это настраивается в дескрипторах передачи (TX Descriptors).
В твоем драйвере (где происходит заполнение дескрипторов перед отправкой, например в sun8i_emac_eth_send или low_level_output_ring), нужно выставить соответствующие биты.
В структуре дескриптора (emac_dma_desc):
Обычно это поле status или size (зависит от конкретной реализации драйвера). Для T113 это биты в status:
c
// Находим дескриптор
struct emac_dma_desc *desc_p = &priv->tx_chain[desc_num];

// Включаем аппаратную вставку CRC для IP и UDP
// EMAC_TX_ST_CSUM_EN — обычно это биты 27-28 в статусном слове дескриптора
desc_p->status |= (0x3 << 27); // 0x3 включает Full Checksum Offload (IP/TCP/UDP)
Используйте код с осторожностью.

3. Настройка регистров EMAC (Global)
Убедись, что при инициализации сетевой карты включена поддержка TX Checksum в регистре конфигурации:
Регистр: EMAC_TX_CTL_1 (Offset 0x08)
Бит 18 (TX_CHKSUM_EN): Должен быть установлен в 1.
🕵️‍♂️ Что это даст на практике:
Процессор (CPU): Больше не заходит внутрь каждого байта видеоданных. Он просто говорит сетевухе: «Вот адрес в DDR, вот длина, посчитай CRC сама по дороге в кабель».
Скорость: Скорость передачи видео должна мгновенно прыгнуть с твоих 28 Мбит к пределу 100-мегабитного линка (~90-94 Мбит).
Температура: Процессор перестанет греться, так как расчет CRC — это одна из самых тяжелых операций для него при сетевом обмене.
🏁 Контрольная проверка (Wireshark):
Когда запустишь — глянь Wireshark на компе.
Если Checksum: 0x0000 или Incorrect — значит, софт выключил, а железо не включил.
Если Checksum: OK — поздравляю, ты реализовал True Zero-Copy!
Какая сейчас загрузка сети после выключения CHECKSUM_GEN_UDP? Должна ощутимо вырасти стабильность картинки.
Важно: Не забудь сделать L1C_CleanInvalidateDCacheRange всего кадра перед циклом, иначе аппаратный CRC может посчитать сумму по «грязным» данным в памяти, и пакеты будут дропаться на ПК как битые.
*/


/*
 * 3. Настройка регистров EMAC (Global)
Убедись, что при инициализации сетевой карты включена поддержка TX Checksum в регистре конфигурации:
Регистр: EMAC_TX_CTL_1 (Offset 0x08)
Бит 18 (TX_CHKSUM_EN): Должен быть установлен в 1.
🕵️‍♂️ Что это даст на практике:
Процессор (CPU): Больше не заходит внутрь каждого байта видеоданных. Он просто говорит сетевухе: «Вот адрес в DDR, вот длина, посчитай CRC сама по дороге в кабель».
Скорость: Скорость передачи видео должна мгновенно прыгнуть с твоих 28 Мбит к пределу 100-мегабитного линка (~90-94 Мбит).
Температура: Процессор перестанет греться, так как расчет CRC — это одна из самых тяжелых операций для него при сетевом обмене.
🏁 Контрольная проверка (Wireshark):
Когда запустишь — глянь Wireshark на компе.
Если Checksum: 0x0000 или Incorrect — значит, софт выключил, а железо не включил.
Если Checksum: OK — поздравляю, ты реализовал True Zero-Copy!
Какая сейчас загрузка сети после выключения CHECKSUM_GEN_UDP? Должна ощутимо вырасти стабильность картинки.
Важно: Не забудь сделать L1C_CleanInvalidateDCacheRange всего кадра перед циклом, иначе аппаратный CRC может посчитать сумму по «грязным» данным в памяти, и пакеты будут дропаться на ПК как битые.



или пакеты дропаются изза физики гигабита - таймингов емака
Вполне вероятно. Когда ты на Baremetal «разгоняешь» стек до реальных 100 Мбит на гигабитном PHY, физика начинает играть против тебя.
Вот три основные причины, почему гигабит на T113-S4 может дропать пакеты при интенсивном потоке:
1. Несоответствие таймингов RGMII (TX Delay)
Гигабитный интерфейс (RGMII) требует очень точной задержки сигналов TX_CLK относительно данных.
Проблема: Если задержка (Delay) в регистре EMAC_CLK_REG (в блоке CCU) или в настройках самого PHY выставлена неверно, на низких скоростях (ARP, пинги) всё будет работать, но при плотном потоке видео пойдут ошибки контрольных сумм или просто потеря битов.
Что проверить: Регистр управления частотой EMAC в CCU. Там есть биты TX_DELAY (обычно от 0 до 7). Попробуй их поменять — иногда «магическое» значение 3 или 4 полностью убирает дропы.
2. Паузы между пакетами (Inter-Packet Gap)
Твой цикл for на 288 итераций «пуляет» пакеты один за другим со скоростью процессора (1.2 ГГц).
Проблема: DMA сетевой карты не успевает освобождать дескрипторы, или принимающая сторона (ПК) захлебывается от такого «шторма».
Решение: Добавь в цикл микро-паузу. Не osDelay(1) (это слишком долго), а простой цикл nop или delay_us(10) после каждого пакета. Это даст PHY время «продышаться» между кадрами Ethernet.
3. Checksum Offload и дропы на ПК
Если ты выключил софтовый CRC, но недонастроил аппаратный в дескрипторах:
Симптом: Wireshark видит пакеты, но помечает их красным (Bad Checksum). ОС Windows/Linux такие пакеты дропает сразу и не отдает твоему плееру. Ты видишь в статистике «скорость есть», а видео нет.
Проверка: В Wireshark нажми на UDP заголовок. Если там Checksum: 0x0000 — это значит, аппаратный оффлоад не включился.
4. TX FIFO Threshold
В регистрах EMAC есть порог запуска передачи (TX Threshold).
Если он стоит слишком низко, EMAC начинает гнать данные в кабель, пока они еще подкачиваются из DDR. Если шина DDR в этот момент занята (например, TVD пишет видео), случается TX Underflow.
Решение: В регистре EMAC_TX_CTL_1 установи режим Store-and-Forward (бит 8). Тогда сетевуха сначала полностью выкачает пакет в свой внутренний буфер и только потом плюнет его в гигабит.
🛠 Что сделать прямо сейчас:
Проверь Wireshark: Если пакеты «битые» по CRC — видео не пойдет.
Замедли цикл: Попробуй слать видео не 25 кадров, а 1 кадр в секунду. Если при 1 FPS картинка на ПК идеальная — значит, у тебя проблемы с «захлебыванием» DMA или таймингами RGMII.
Увеличь количество дескрипторов: Если их 16, они кончаются за доли миллисекунды. Сделай 256.
Какое значение TX_DELAY сейчас в настройках тактирования EMAC? (Часто именно там зарыта причина «глюков» на больших скоростях).


 */

//стправить регистр А08 JEPG!!!!
//ffplay -i udp://0.0.0.0:27025 -f mjpeg
//http://nskhuman.ru/allwinner/apst/apst.php
//https://github.com/aodzip/sun6i-mipi-csi/blob/master/sun6i_csi.c


//Вердикт:
//Для 24-летней «девочки» (99-й) сейчас важнее, чтобы поворотники мигали стабильно и масса не отваливалась. А 200-мегагерцовый ЦАП подождет своего часа — когда появится либо готовый драйвер в Linux, из которого можно выдрать логику, либо лишний месяц свободного времени.
//Если вдруг решишься — начни с исходников ядра Linux (ветка sunxi). Там в драйверах drivers/video/fbdev/sunxi/disp2/ можно подсмотреть, как инициализируется эта цепочка.
//Удачи с «зубилом», пусть электрика больше не капризничает!
//+79002250848 тел секретариата на гараб


//Быстрый совет "на засыпку":
//Если гребенка сильно мешает «продать» результат или показать работу, попробуй в настройках CSI включить режим Field к Frame. Если повезет, контроллер сам склеит поля, хотя на быстром движении «ступеньки» могут остаться.
//Какая у тебя камера? (AHD через декодер или какой-нибудь старый PAL/NTSC сенсор?)
//Если используешь CSI, глянь в регистры — там часто есть флаг FIELD_COMBINE. Это не полноценный деинтерлейс, но гребенку делает чуть менее выраженной.
//Если решишь «прибивать» программно — я могу подсказать формулу для быстрого смешивания строк (blending) на NEON-инструкциях T113, это будет летать.
//Нужна помощь с регистрами DI или попробуем сначала программный "костыль"?
