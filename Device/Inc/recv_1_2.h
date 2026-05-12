/*
 * recv_1_2.h
 *
 *  Created on: 6 февр. 2024 г.
 *      Author: Petr
 */

#ifndef RECV_1_2_H_
#define RECV_1_2_H_

#include "comm.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "gpio_sun.h"
#include "i2c-sunxi.h"
#include "switch_fdc.h"

#define RECV_2_4_MIN_FREQ 		2000
#define RECV_2_4_MAX_FREQ 		2700
#define RECV_2_4_AUTO_FREQ 		10
#define RECV_2_4_MANUAL_FREQ 	2



#define RECV_1_2_MIN_FREQ 		800
//#define RECV_1_2_MAX_FREQ 		1400
#define RECV_1_2_MAX_FREQ 		1700
#define RECV_1_2_START_FREQ 	1120
#define RECV_1_5_START_FREQ 	1405
#define RECV_2_4_START_FREQ 	2410
#define RECV_1_2_AUTO_FREQ 		10
#define RECV_1_2_MANUAL_FREQ 	2

//#define RECV_1_2_MAX_NUM_CH 	16
#ifdef RECV_1_2_EXT
#define RECV_1_2_MAX_NUM_CH 	30
#else
#define RECV_1_2_MAX_NUM_CH 	23
#endif
#define RECV_1_5_MAX_NUM_CH 	12
#define RECV_2_4_MAX_NUM_CH 	18

#define RECV_1_2_MAX_NUM_BLANK_CH		RECV_1_2_MAX_NUM_CH

typedef enum {
	recv_1_2_view_mode_normal		=	0,
	recv_1_2_view_mode_inv			=	1,
	recv_1_2_view_mode_inv_norm		=	2,
}recv_1_2_view_mode_e;

typedef enum {
	recv_1_2_view_mode_now_normal		=	0,
	recv_1_2_view_mode_now_inv			=	1,
}recv_1_2_view_mode_now_e;

typedef enum {
	recv_1_2_ver_1_2					=	0,
	recv_1_2_ver_1_5_					=	1,
	recv_1_2_ver_2_4_					=	2
}recv_1_2_ver_e;


typedef enum {
	recv_1_2_ch_mode					=	0,
	recv_1_2_freq_mode					=	1
}recv_1_2_freq_ch_mode_e;

#define RECV_1_2_MAX_NUM_DISABLE_POINT	4

typedef struct recv_1_2_cfg_struct{
uint16_t		min_freq;
uint16_t		max_freq;
uint16_t		manual_step_freq;
uint16_t		auto_step_freq;

uint16_t		dis_start_freq[RECV_1_2_MAX_NUM_DISABLE_POINT];
uint16_t		dis_len_freq[RECV_1_2_MAX_NUM_DISABLE_POINT];

uint8_t					time;
recv_1_2_view_mode_e	view_mode;
recv_1_2_freq_ch_mode_e	freq_ch_mode;

uint8_t			ch_blank[RECV_1_2_MAX_NUM_BLANK_CH + 1];

}recv_1_2_cfg_struct;

typedef struct recv_1_2_struct{

struct sunxi_i2c 				*p_hi2c;
switch_fdc_struct				sw_pwr_5v;
switch_fdc_struct				sw_pwr_8v;
//switch_fdc_struct				sw_da6_en;
switch_fdc_struct				sw_da6_in;

uint8_t							i2c_addr;
uint8_t							reg_addr;
uint8_t							reg_data;
uint8_t							ch_cntr;
uint8_t							max_num_ch;
uint8_t							dis_en_ch;

uint16_t						freq;

//uint16_t						min_freq;
//uint16_t						max_freq;

//uint16_t						manual_freq_step;
//uint16_t						auto_freq_step;

SettingCell_t 					html_table[32+7];

recv_1_2_cfg_struct				cfg;
recv_1_2_cfg_struct				*p_cfg;

uint8_t							tmp[4];

uint32_t						time;


//	GPIO_TypeDef 				*CS_port;
//	uint16_t					CS_pin;
//
//
//	GPIO_TypeDef 				*mosi_port;
//	uint16_t					mosi_pin;
//
//	GPIO_TypeDef 				*clk_port;
//	uint16_t					clk_pin;
//
//	GPIO_TypeDef 				*RSSI_port;
//	uint16_t					RSSI_pin;
//
//	ARM_DRIVER_SPI				*p_spi;
//	ARM_SPI_DRIVER_DEVICE_STRUCT		driver_config;
//
//	uint32_t					os_mutex[4];
//	osMutexDef_t 				osmutexdef;
//	osMutexId           		mutex_write_id;
//
//	volatile uint8_t			wr_cntr;
//
//	uint32_t					all_data_out_time_ms;
//
//	uint32_t					error;
//	osThreadId 					os_thread_id;
//	uint32_t 					os_signal_all_data_send;
//	uint32_t 					os_signal_mux_change;
//	volatile uint8_t			b_os_signal_data_send;
//
////	void                  		(*on_mux_change)( void *p_v);
////	void                  		(*on_all_data_out)( void *p_v);
////	void						(*on_error)( void *p_v);
////
////	void                  		*pv_on_mux_change;
////	void                  		*pv_on_all_data_out;
////	void						*pv_on_error;
//
//	uint8_t						addr;
//	uint32_t					reg_data;
//
//	uint32_t					freq;
//
//	rtc6715_state_t				reg;

recv_1_2_view_mode_now_e		view_mode;
recv_1_2_ver_e					ver;

}recv_1_2_struct;
extern const uint16_t ch_1_2_table_freq_[RECV_1_2_MAX_NUM_CH];
extern const uint16_t ch_1_5_table_freq[RECV_1_5_MAX_NUM_CH];

void recv_1_2_start_init(recv_1_2_struct *p_ctl);

void recv_1_2_default_init(recv_1_2_cfg_struct *p_cfg);
void recv_2_4_default_init(recv_1_2_cfg_struct *p_cfg);

uint8_t recv_1_2_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, recv_1_2_struct *p_ctl, const char *p_name);

uint32_t recv_1_2_set_freq_up(recv_1_2_struct *p_ad, uint16_t freq);
uint32_t recv_1_2_set_freq_up_auto(recv_1_2_struct *p_ctl, uint16_t freq);

uint32_t recv_1_2_set_freq_down(recv_1_2_struct *p_ad, uint16_t freq);
void recv_1_2_power_on(recv_1_2_struct *p_ctl);
void recv_1_2_power_off(recv_1_2_struct *p_ctl);
uint32_t recv_1_2_time_process(recv_1_2_struct *p_ctl, uint32_t time);
void recv_1_2_make_view_mode(recv_1_2_struct *p_ctl, recv_1_2_view_mode_now_e mode);
uint32_t recv_1_2_set_freq(recv_1_2_struct *p_ad, uint16_t freq);
void recv_1_2_make_html_cntrl_table(recv_1_2_struct* p_ctl, ctrl_table_list_struct* p_list);
void recv_1_2_invert_video(recv_1_2_struct *p_ctl);

#endif /* RECV_1_2_H_ */
