/*
 * control.h
 *
 *  Created on: 24 дек. 2025 г.
 *      Author: Petr
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#include "board_config.h"
#include "Driver_UART.h"
#include "Driver_ETH.h"
#include "uart_cmd_parser.h"
#include "debug_interface.h"
#include "debug_cells_lib.h"
#include "lwip/ip_addr.h"
#include "lwip/api.h"
#include "comm.h"

#ifdef	TVP_MASTER_HAVE
#include "tvp5150_reg.h"
#endif
#ifdef RECV_1_2
#include "recv_1_2.h"
#endif

#ifdef RTC6715
#include "rtc6715.h"
#endif
#ifdef POVOROT
#include "povorot.h"
#endif

#include "devconfig.h"

#define UDP_SENDER			1
//#define MAX_NUM_FUNC_TABLE								7

typedef enum cntrl_error_e{
	cntrl_error_tvp_master = 1<<0,
}cntrl_error_e;


typedef enum cntrl_thread_event_e
{
	cntrl_thread_event_time_out		= 1,
	cntrl_thread_get_cmd			= 2,
	cntrl_thread_get_data			= 4,
	cntrl_thread_event_tvp_master	= 8,
	cntrl_thread_event_tvp_slave	= 16,
	cntrl_thread_event_recv_5_8_rssi = 32,
	cntrl_thread_get_lora_cmd			= 64,
}cntrl_thread_event_e;

typedef enum{
	main_cntrl_thread_event_uart_parser_0 = 				0x01,
	main_cntrl_thread_event_uart_parser_3 =					0x02,
	main_cntrl_thread_event_timer_50ms = 					0x04,
	main_cntrl_thread_event_ack_msg = 						0x08,
	main_cntrl_thread_event_frame_tvd = 					0x10,
	main_cntrl_thread_event_ext_wrt = 						0x20,
	main_cntrl_thread_event_tcp_stack_init = 				0x40,
	main_cntrl_thread_http_cmd = 							0x80,
	main_cntrl_thread_event_jpeg = 							0x100,
	main_cntrl_thread_event_frame_csi = 					0x200,
	main_cntrl_thread_event_8080 = 							0x400,
//	main_cntrl_thread_event_ad9912 = 						0x10,
//	main_cntrl_thread_event_lmx2492 = 						0x20,
//	main_cntrl_thread_event_adf4159 = 						0x40,
} main_cntrl_thread_event_e;

typedef enum main_cntrl_cmd_e{
	main_cntrl_cmd_auto_scan_ch_en =			1,
	main_cntrl_cmd_auto_scan_ch_dis =			2,
	main_cntrl_cmd_auto_scan_rotate_en =		3,
	main_cntrl_cmd_auto_scan_rotate_dis =		4,
	main_cntrl_cmd_auto_scan_en =				5,
	main_cntrl_cmd_auto_scan_dis =				6,
	main_cntrl_cmd_image_en =					7,
	main_cntrl_cmd_image_dis =					8,
	main_cntrl_cmd_freq_up =					9,
	main_cntrl_cmd_freq_down =					10,
	main_cntrl_cmd_sel_recv_mode =				11,
	main_cntrl_cmd_freq =						12,
	main_cntrl_cmd_liter_up =					13,
	main_cntrl_cmd_liter_down =					14,
	main_cntrl_cmd_ch_up =						15,
	main_cntrl_cmd_ch_down =					16,
	main_cntrl_cmd_emit_freq =					17,
	main_cntrl_cmd_emit_pwr =					18,
#ifdef RECV_5_8_RSSI
//	main_cntrl_cmd_start_rssi =					17,
//	main_cntrl_cmd_stop_rssi =					18,
#endif

//	main_cntrl_cmd_sel_12_mode =				12,
//	main_cntrl_start_collect_mode =				13,
//	main_cntrl_stop_collect_mode =				14,
}main_cntrl_cmd_e;

typedef struct { // Message object structure
	uint8_t			cmd;
	void*			p_v;
}tcp_cmd_ethernet_int_msg_type;



typedef enum{
	tcp_cmd_send_plus_zero				=   30,
	tcp_cmd_recv_plus_zero				=   31,
	tcp_cmd_recv_plus_non_zero			=   32,
	tcp_cmd_call_back_timer				=   33,
}tcp_cmd_e;

typedef enum {
	cntr_recv_mode_1_2		=	0,
	cntr_recv_mode_5_8		=	1,
	cntr_recv_mode_1_2_5_8	=	2,
	cntr_recv_mode_5_8_1_2	=	3,
	cntr_recv_mode_1_2_and_5_8 =	4,
#ifdef RTC6705
	cntr_recv_mode_emit =		5
#endif
}recv_mode_e;

typedef struct { // Message object structure
	struct netconn 	*p_l_conn;
	struct netconn 	*p_a_conn_1;
	struct netconn 	*p_a_conn_2;
	osMessageQueueId_t	msg_que_id;
	osEventFlagsId_t	event_flag_id;
	uint32_t			event;
}tcp_cmd_8080_t;

typedef struct im_header{
	uint32_t header;
	uint16_t num;
	uint16_t width;
	uint16_t height;
	uint16_t lines;
//	uint8_t	odd_even;

}im_header;

typedef enum{
	cmd_recv_active_1_2 = 					0,
	cmd_recv_active_5_8 = 					1
} cntrl_active_rec_e;

typedef enum{
	cntrl_freq_mode_manual 					= 				0x00,
	cntrl_freq_mode_auto 						= 				0x01,
} cntrl_freq_mode;

typedef enum{
	cntrl_alarm_dis							= 				0x00,
	cntrl_alarm_en 							= 				0x01,
} cntrl_alarm_mode;

typedef enum{
	cntrl_auto_mode_dis						= 				0x00,
	cntrl_auto_mode_en 						= 				0x01,
} cntrl_auto_mode;


typedef struct control_struct{
#ifdef	MURKA_BOARD
	ARM_DRIVER_UART				*p_uart0;
	uart_cmd_parser				uart_parser0;
#endif
#ifdef	SCANER_BOARD
	ARM_DRIVER_UART				*p_uart0;
	uart_cmd_parser				uart_parser0;
#endif

#ifdef USE_UART1
ARM_DRIVER_UART				*p_uart1;
#endif
#ifdef TEST_BOARD
ARM_DRIVER_UART				*p_uart3;
#endif
osEventFlagsId_t 			cmd_evt_id;                        // event flags id 				thread_id;                                          // thread id
//osThreadId_t 				tid_uart_driver_fone_thread;

#ifdef USE_UART1
uart_cmd_parser				uart_parser1;
#endif
#ifdef	TEST_BOARD
uart_cmd_parser				uart_parser3;
#endif
//
//	cntrl_cmd_eth_link_e	eth_link;
//
uint16_t 						comp_table_pos;
SettingCell_t 					comp_table[COMPONENT_MAX_TABLE_SIZE];
ctrl_table_list_struct			table_list[MAX_NUM_FUNC_TABLE];
SettingCell_t 					*p_comp_table;


#ifdef UDP_SENDER

struct udp_pcb*		v_pcb;

struct netconn*		p_udp_sender;
ip_addr_t 			udp_sender_dest_ip;
struct netbuf*		udp_sender_net;

uint8_t*			udp_sender_data_ptr;
uint16_t			udp_sender_send_size;
uint16_t			udp_sender_send_port;
im_header			header;
uint8_t				b_start_grab;
uint8_t				b_start_frame;
uint8_t				b_start_auto;
uint8_t				b_en_send;
volatile uint8_t*	udp_sender_buf_luma_csi;
volatile uint8_t*	udp_sender_buf_chroma_csi;

volatile uint8_t*	udp_sender_buf_luma_tvd;
volatile uint8_t*	udp_sender_buf_chroma_tvd;

#endif



////
////
////uint32_t 				error;
//#ifdef EXT_FLASH_SUPPORT
//w25q64_struct			w25q64;
//#endif
//
//dev_config_struct		dev_config;
SettingCell_t 			main_config_table[25+6];
//
//ack_msg_s				ack;
////povorot_struct			povorot;
////ARM_DRIVER_UART			*p_uart485;
//
////osThreadId 				own_thread_id;          // thread id
//
//osMessageQId 			cmd_ack_msg_que_id;
//osPoolId				cmd_ack_msg_pool_id;
//
//uint32_t 				os_signal_get_ack_msg;
//uint32_t				max_wait_ack_time_ms;
//
//uint32_t				ch_ack_time;
////uint8_t					cap_count;
////uint8_t					ch_cntr;
////uint8_t					ch_max_scan_time;
////uint8_t*				ch_blank;
//
////uint8_t					b_skip_but;
cntrl_auto_mode			auto_mode;
cntrl_freq_mode			freq_mode;
cntrl_alarm_mode		alarm_mode;
recv_mode_e						recv_mode;
cntrl_active_rec_e				recv_active;
uint32_t				ch_time;
uint32_t				ch_max_time;
//cntrl_ant_mode		ant_mode;
uint32_t				ant_time;
uint32_t				ant_max_time;


uint8_t					b_udp_send;
uint8_t					b_http_file_open;
uint8_t					b_wait_udp_send;

////cntrl_alarm_mode		alarm_mode;
////uint32_t				ch_time;
////uint32_t				ch_max_time;
//////cntrl_ant_mode		ant_mode;
////uint32_t				ant_time;
////uint32_t				ant_max_time;
////
////switch_fdc_struct		sw_on_cpu_hold;
////switch_fdc_struct		sw_work_led_cntrl;
////switch_fdc_struct		sw_status_led_cntrl;
////switch_fdc_struct		sw_24_POW_cntrl;
////switch_fdc_struct		sw_24_SYNT_cntrl;
////switch_fdc_struct		sw_FAN_cntrl;
////switch_fdc_struct		sw_on_heat_ctrl;
////switch_fdc_struct		sw_off_heat_ctrl;
//
////switch_fdc_struct		sw_down;
//
//switch_fdc_struct		sw_24_POW_cntrl;
//
cmd_dev_s				main_ctrl_cmd;
ack_msg_s				*main_p_ack;
osEventFlagsId_t 		main_ctrl_thread_ev;

//uint8_t					main_id;
//osThreadId				main_thread_id;
//
////uint8_t					b_udp_send;
////uint8_t					b_wait_udp_send;
//
//uint16_t				adc_ring_buf[8];
//uint16_t				adc_ring_buf_2[2];
//
//voltage_cntrl_struct	rssi_0;
//voltage_cntrl_struct	rssi_1;
////voltage_cntrl_struct	voltage_heat;
////voltage_cntrl_struct	voltage_sensor;
//
////button_struct					button_power;
////collect_button_struct_s			collect_button;
//
////hdc2080_struct			hdc_2080;
//
//uint8_t					time_to_off;
////pwr_cntrl_s				pwr_ch[CLIMATE_MAX_NUM_PWR_CNTRL];
//
//osMessageQId			cntrl_msg_que_id;
//osPoolId				cntrl_msg_pool_id;
//
//cntrl_state_mode		state;
//
//light_led_struct 		led_mode;
//light_led_struct 		led_alarm;
//
//char					tmp_str[32];
//cntrl_power_off_src		off_src;
//cntrl_power_off_src		off_fault_src;
//
//shift_reg_struct		led_reg;
//
////light_led_struct 		led_vd4_green;
////light_led_struct 		led_vd4_blue;
////light_led_struct 		led_vd3_red;
//
//dac_voltage_cntrl_struct	dac;
	eth_resourse_s 		*p_eth_res;

	ctrl_table_list_param_struct list_param;

#ifdef	TVP_MASTER_HAVE
	tvp5150_struct tvp_5150_master;
#endif
#ifdef	TVP_SLAVE_HAVE
	tvp5150_struct tvp_5150_slave;
#endif
#ifdef	RTC6715
	rtc6715_struct	rtc6715;
#endif
#ifdef	RECV_1_2
	recv_1_2_struct	recv_1_2;
#endif
#ifdef	POVOROT
	povorot_struct	povorot;
#endif

	struct sunxi_i2c 			*p_hi2c_tvp;
	struct sunxi_i2c 			*p_hi2c_1_2;
	uint32_t					error;
uint32_t addr;
uint32_t data;
dev_config_struct		dev_config;

uart_phone_thread_cfg_s	conf;

tcp_cmd_8080_t			tcp_8080;

//struct netconn 						*p_l_conn;
//struct netconn 						*p_conn;

uint32_t	file_cntr;

switch_fdc_struct			sw_adc_in;
uint8_t					adc_time;

}control_struct;

extern control_struct control;
void cntrl_start_8080(tcp_cmd_8080_t *p_ctl);
void tcp_cmd_ethernet_get_msg(tcp_cmd_8080_t *p_ctl);


void main_cntrl_init_que(control_struct *p_ctl);

void app_main (void *argument);
void app_cmd (void *argument);
#endif /* CONTROL_H_ */
