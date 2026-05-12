
#ifndef RTC6715_H_
#define RTC6715_H_

//#include "comm.h"
#include "switch_fdc.h"
//#include "rffc5071.h"
//#include "uni_prot.h"
//#include "tcp_cmd.h"

#define RECV_5_8_EXT_CH				1

typedef enum{
	RTC6715_not_owerwrite_register = 0,
	RTC6715_must_owerwrite_register = 1
} lmx2492_register_rewrite_e;



//#define RTC6715_TABLE_MODE 		1



#define RTC6715_RSSI_START_AZIMUT 0
#define RTC6715_RSSI_STOP_AZIMUT 350
#define RTC6715_RSSI_NUM_AZIMUT_POINT  ((RTC6715_RSSI_STOP_AZIMUT - RTC6715_RSSI_START_AZIMUT)/10)

#define RTC6715_RSSI_START_FREQ 4840
#define RTC6715_RSSI_STOP_FREQ  6080
#define RTC6715_RSSI_NUM_FREQ_POINT  ((RTC6715_RSSI_STOP_FREQ - RTC6715_RSSI_START_FREQ)/10)

//#define RECV_5_8_RSSI_BUF_SIZE			128
#define RECV_5_8_RSSI_BUF_SIZE			RTC6715_RSSI_NUM_FREQ_POINT

#define RTC6715_MIN_FREQ 		4900
#define RTC6715_MAX_FREQ 		6100

#define RTC6715_START_FREQ 		5825
#define RTC6715_AUTO_FREQ 		10
#define RTC6715_MANUAL_FREQ 	4


#ifdef RECV_5_8_BASE_CH
#define RECV_5_8_MAX_NUM_CH 	96
#define RECV_5_8_MAX_NUM_LIT 	12
#endif


#ifdef RECV_5_8_SUB_3_3

#define RECV_5_8_MAX_NUM_CH 	(56)
#define RECV_5_8_MAX_NUM_LIT 	7

#else

#ifdef RECV_5_8_EXT_CH
#define RECV_5_8_MAX_NUM_CH 	(96+24)
#define RECV_5_8_MAX_NUM_LIT 	15
#endif
#endif

#define RECV_5_8_MAX_NUM_BLANK_CH		8


#define RECV_5_8_MIN_FREQ 		RTC6715_MIN_FREQ
#define RECV_5_8_MAX_FREQ 		RTC6715_MAX_FREQ
#define RECV_5_8_START_FREQ 	RTC6715_START_FREQ
#define RECV_5_8_AUTO_FREQ 		RTC6715_AUTO_FREQ
#define RECV_5_8_MANUAL_FREQ 	RTC6715_MANUAL_FREQ

#define RTC6715_MUX_INT_PRIORITY							4

#define RTC6715_NOT_FOUND_ERROR_BIT							1
#define RTC6715_SOFTWARE_ERROR_BIT							4

#define RTC6715_MAX_BUF_OUT_TIME_MS							500

#define RTC6715_MAX_NUM_REGISTERS							11

typedef union RTC6715_reg_0_t{
	struct{
		uint32_t SYN_RF_R_REG 	: 15;
		uint32_t zero 			: 17;
	} bits;
	uint32_t word;
} RTC6715_reg_0_t;

typedef union RTC6715_reg_1_t{
	struct{
		uint32_t SYN_RF_A_REG 		: 7;
		uint32_t SYN_RF_N_REG 		: 13;
		uint32_t zero 				: 12;
	} bits;
	uint32_t word;
}RTC6715_reg_1_t;

typedef union RTC6715_reg_2_t{
	struct{
		uint32_t PRES_FT 			: 3;
		uint32_t MOUT 				: 2;
		uint32_t SC_CTL 			: 1;
		uint32_t CP_FT				: 3;
		uint32_t CP_5GLO			: 3;
		uint32_t CC_VCO 			: 2;
		uint32_t AGC_6M5 			: 3;
		uint32_t AGC_6M 			: 3;
		uint32_t zero 				: 12;
	} bits;
	uint32_t word;
}RTC6715_reg_2_t;

typedef union RTC6715_reg_3_t{
	struct{
		uint32_t SYN_RZ 			: 8;
		uint32_t SYN_CZ 			: 3;
		uint32_t SYN_C3 			: 3;
		uint32_t zero 				: 18;
	} bits;
	uint32_t word;
}RTC6715_reg_3_t;

typedef union RTC6715_reg_4_t{
	struct{
		uint32_t VCO6M5_EX_CAP 		: 5;
		uint32_t VCO6M_EX_CAP		: 5;
		uint32_t VCO480_EX_CAP 		: 5;
		uint32_t RFVCO_EX_CAP 		: 5;
		uint32_t zero 				: 12;
	} bits;
	uint32_t word;
}RTC6715_reg_4_t;

typedef union RTC6715_reg_5_t{
	struct{
		uint32_t AUDFC_OVP 			: 3;
		uint32_t DFC480_OVP			: 3;
		uint32_t VCODFC_OVP 		: 3;
		uint32_t OK_6M5 			: 1;
		uint32_t OK_6M  			: 1;
		uint32_t OK_IF   			: 1;
		uint32_t OK_RF   			: 1;
		uint32_t R   				: 6;
		uint32_t EN_RECAL			: 1;
		uint32_t zero 				: 12;
	} bits;
	uint32_t word;
}RTC6715_reg_5_t;

typedef union RTC6715_reg_6_t{
	struct{
		uint32_t M6_RZ 				: 8;
		uint32_t M6_CZ				: 3;
		uint32_t M6_C3 				: 3;
		uint32_t M6_ICP 			: 6;
		uint32_t zero 				: 12;
	} bits;
	uint32_t word;
}RTC6715_reg_6_t;

typedef union RTC6715_reg_7_t{
	struct{
		uint32_t M65_RZ 			: 8;
		uint32_t M65_CZ				: 3;
		uint32_t M65_C3 			: 3;
		uint32_t M65_ICP 			: 6;
		uint32_t zero 				: 12;
	} bits;
	uint32_t word;
}RTC6715_reg_7_t;

typedef union RTC6715_reg_8_t{
	struct{
		uint32_t VAMP_GN 			: 8;
		uint32_t IFA_GN				: 3;
		uint32_t CP_MIXER 			: 3;
		uint32_t Reserved 			: 3;
		uint32_t zero 				: 15;
	} bits;
	uint32_t word;
}RTC6715_reg_8_t;

typedef union RTC6715_reg_9_t{
	struct{
		uint32_t RSSI_SQUELCH_D 	: 8;
		uint32_t BC					: 3;
		uint32_t REGIF_VADJ 		: 3;
		uint32_t REGBS_VADJ 		: 3;
		uint32_t IFAF_GN 			: 3;
		uint32_t zero 				: 12;
	} bits;
	uint32_t word;
}RTC6715_reg_9_t;

typedef union RTC6715_reg_A_t{
	struct{
		uint32_t PD_PLL1D8 			: 1;
		uint32_t PD_DIV80			: 1;
		uint32_t PD_MIXER 			: 1;
		uint32_t PD_IFABF 			: 1;
		uint32_t PD_REG1D8 			: 1;
		uint32_t PD_6M5 			: 1;
		uint32_t PD_AU6M5 			: 1;
		uint32_t PD_6M 				: 1;
		uint32_t PD_AU6M 			: 1;
		uint32_t PD_SYN 			: 1;
		uint32_t PD_5GVCO 			: 1;
		uint32_t PD_DIV4			: 1;
		uint32_t PD_BC				: 1;
		uint32_t PD_REGIF			: 1;
		uint32_t PD_REGBS			: 1;
		uint32_t PD_RSSI_SQUELCH	: 1;
		uint32_t PD_IFAF			: 1;
		uint32_t PD_IF_DEMOD		: 1;
		uint32_t PD_VAMP			: 1;
		uint32_t PD_VCLAMP			: 1;
		uint32_t zero 				: 12;
	} bits;
	uint32_t word;
}RTC6715_reg_A_t;

#pragma pack(push,1)
typedef union{
	struct
	{
		RTC6715_reg_0_t			reg0;
		RTC6715_reg_1_t			reg1;
		RTC6715_reg_2_t			reg2;
		RTC6715_reg_3_t			reg3;
		RTC6715_reg_4_t			reg4;
		RTC6715_reg_5_t			reg5;
		RTC6715_reg_6_t			reg6;
		RTC6715_reg_7_t			reg7;
		RTC6715_reg_8_t			reg8;
		RTC6715_reg_9_t			reg9;
		RTC6715_reg_A_t			regA;
	};
	uint32_t bytes[RTC6715_MAX_NUM_REGISTERS];
} rtc6715_state_t;
#pragma pack(pop)

typedef enum {
	recv_5_8_ch_mode					=	0,
#ifdef	RTC6715_FREQ_MODE
	recv_5_8_freq_mode					=	1,
#endif
#ifdef	RTC6715_TABLE_MODE
	recv_5_8_table_mode					=	2
#endif

}recv_5_8_freq_ch_mode_e;


typedef enum {
	recv_5_8_view_mode_normal		=	0,
	recv_5_8_view_mode_inv			=	1,
	recv_5_8_view_mode_inv_norm		=	2,
}recv_5_8_view_mode_e;

typedef enum {
	recv_5_8_lit_A_e				=	0,
	recv_5_8_lit_B_e				=	1,
	recv_5_8_lit_E_e				=	2,
	recv_5_8_lit_F_e				=	3,
	recv_5_8_lit_R_e				=	4,
	recv_5_8_lit_L_e				=	5,
	recv_5_8_lit_H_e				=	6,
	recv_5_8_lit_l_e				=	7,
	recv_5_8_lit_U_e				=	8,
	recv_5_8_lit_O_e				=	9,
	recv_5_8_lit_X_e				=	10,
	recv_5_8_lit_Z_e				=	11
}recv_5_8_lit_name_e;

#define RECV_5_8_MAX_NUM_DISABLE_POINT	4

typedef struct recv_5_8_cfg_struct{
uint16_t		en_lit;
uint16_t		min_freq;
uint16_t		max_freq;
uint16_t		manual_step_freq;
uint16_t		auto_step_freq;

//uint16_t		dis_start_freq[RECV_5_8_MAX_NUM_DISABLE_POINT];
//uint16_t		dis_len_freq[RECV_5_8_MAX_NUM_DISABLE_POINT];

uint8_t					time;
recv_5_8_view_mode_e	view_mode;
recv_5_8_freq_ch_mode_e	freq_ch_mode;

uint8_t			b_en_fast_scan;
uint8_t			b_en_only_scan;
uint8_t			lvl;

char			ch_blank[RECV_5_8_MAX_NUM_BLANK_CH][2];


}recv_5_8_cfg_struct;

typedef enum {
	recv_5_8_view_mode_now_normal		=	0,
	recv_5_8_view_mode_now_inv			=	1,
}recv_5_8_view_mode_now_e;

typedef enum {
	recv_5_8_rssi_state_stop			=	0,
	recv_5_8_rssi_state_start			=	1,
	recv_5_8_rssi_state_continue		=	2,
	recv_5_8_rssi_check_video			=	3
}recv_5_8_rssi_state_e;


typedef enum {
	recv_5_8_rssi_mode_slow			=	0,
	recv_5_8_rssi_mode_mix			=	1,
}recv_5_8_rssi_mode_e;

typedef enum {
	recv_5_8_rssi_now_mode_slow			=	0,
	recv_5_8_rssi_now_mode_fast			=	1,
	recv_5_8_rssi_now_mode_start_fast	=	2,
}recv_5_8_rssi_now_mode_e;


#pragma pack(push,1)
typedef struct recv_5_8_status_s {
	uint16_t								freq;
	uint16_t								azimut;
}recv_5_8_status_s;
#pragma pack(pop)


#pragma pack(push,1)
typedef struct recv_5_8_rssi_main_status_s {
	uint16_t								start_azimut;
	uint16_t								stop_azimut;
	uint16_t								num_azimut_point;
	uint16_t								start_freq;
	uint16_t								stop_freq;
	uint16_t								num_freq_point;
}recv_5_8_rssi_main_status_s;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct recv_5_8_rssi_freq_status_s {
	uint16_t								start_freq;
	uint16_t								stop_freq;
	uint16_t								num_freq_point;
	uint16_t								delta_freq;
	uint16_t								azimut;
}recv_5_8_rssi_freq_status_s;
#pragma pack(pop)

#define RTC_6715_FAST_RSSI_NUM	16

typedef struct rtc6715_fast_rssi_struct{
uint8_t		num_freq_point;
uint16_t	freq[RTC_6715_FAST_RSSI_NUM];
uint8_t		data[RTC_6715_FAST_RSSI_NUM];
uint8_t		fast_cntr;
uint8_t		slow_cntr;
uint8_t		max_slow_cntr;
}rtc6715_fast_rssi_struct;

#ifdef RECV_5_8_RSSI
typedef struct rtc6715_rssi_struct{

	recv_5_8_rssi_main_status_s global_cntrl;
	recv_5_8_rssi_freq_status_s meas_cntrl;

//uint16_t	start_freq;
//uint16_t	stop_freq;
//uint16_t	delta_freq;

uint16_t	base_freq;
uint16_t	freq;
uint16_t	azimut;
uint8_t		data_buf[RECV_5_8_RSSI_BUF_SIZE+2];
uint8_t		disable_buf[RTC6715_RSSI_NUM_FREQ_POINT*RTC6715_RSSI_NUM_AZIMUT_POINT/8+1];
uint16_t	delay_answer;
uint16_t	buf_cntr;
uint16_t	num_point;
uint32_t 				os_timer[6];
osTimerDef_t 			os_timer_def;
osTimerId 				timer_id;
osThreadId 				thread_id;
uint32_t				signal;

ADC_HandleTypeDef*    p_adc;
uint16_t		adc_buf_num_point;
uint16_t*		p_adc_buf;
recv_5_8_rssi_state_e	state;
uint8_t		b_continue_restart;
uint8_t		b_check_video;
uint8_t		b_en;
recv_5_8_rssi_mode_e	rssi_mode;
recv_5_8_rssi_now_mode_e	rssi_now_mode;

//uni_prot_s* p_uni;

tcp_cmd_eth_parser *p_tcp_cmd;

rtc6715_fast_rssi_struct	fast;
ip_addr_t			dest_ip;
uint16_t			dest_port;

}rtc6715_rssi_struct;
#endif


typedef struct rtc6715_struct{

	switch_fdc_struct			sw_pwr_5v;

//	switch_fdc_struct			sw_da1_en;
	switch_fdc_struct			sw_da1_in;

	struct gpio_t_ 			cs_pin;
	struct gpio_t_ 			mosi_pin;
	struct gpio_t_ 			clk_pin;

//	GPIO_TypeDef 				*cs_port;
//	uint16_t					cs_pin;
//
//	GPIO_TypeDef 				*mosi_port;
//	uint16_t					mosi_pin;
//
//	GPIO_TypeDef 				*clk_port;
//	uint16_t					clk_pin;
//
//	GPIO_TypeDef 				*rssi_port;
//	uint16_t					rssi_pin;

#ifdef RECV_5_8_RSSI
	rtc6715_rssi_struct			rssi;
#endif

//	ARM_DRIVER_SPI				*p_spi;
//	ARM_SPI_DRIVER_DEVICE_STRUCT		driver_config;

//	uint32_t					os_mutex[4];
//	osMutexDef_t 				osmutexdef;
//	osMutexId           		mutex_write_id;

//	volatile uint8_t			wr_cntr;

//	uint32_t					all_data_out_time_ms;

	uint32_t					error;
//	osThreadId 					os_thread_id;
//	uint32_t 					os_signal_all_data_send;
//	uint32_t 					os_signal_mux_change;
//	volatile uint8_t			b_os_signal_data_send;

//	void                  		(*on_mux_change)( void *p_v);
//	void                  		(*on_all_data_out)( void *p_v);
//	void						(*on_error)( void *p_v);
//
//	void                  		*pv_on_mux_change;
//	void                  		*pv_on_all_data_out;
//	void						*pv_on_error;

	uint8_t						addr;
	uint32_t					reg_data;

	rtc6715_state_t				reg;
//	short_time_msg_s			short_time;


	uint16_t					freq;
//	uint16_t					min_freq;
//	uint16_t					max_freq;
//	uint16_t					manual_freq_step;
//	uint16_t					auto_freq_step;


	SettingCell_t 				html_table[30];
//	SettingCell_t 				html_table[30];
	recv_5_8_cfg_struct				*p_cfg;
	recv_5_8_cfg_struct				cfg;

	uint8_t							tmp[4];

	uint32_t					time;

	//uint8_t							max_num_ch;
	char							dis_en_ch_buf[2];
	uint8_t							ch_cntr;
	recv_5_8_view_mode_now_e		view_mode;
#ifdef RECV_5_8_SUB_3_3
	rffc5071_struct		*p_rff5071;
#endif

}rtc6715_struct;

extern const  uint16_t ch_5_8_freq[RECV_5_8_MAX_NUM_CH];
extern const  char *ch_5_8_table[RECV_5_8_MAX_NUM_CH];

//void rtc6715_call_back_start_send_data(void *p);
//void rtc6715_call_back_stop_send_data(void *p);

void recv_5_8_default_init(recv_5_8_cfg_struct *p_cfg);
#ifdef RECV_5_8_RSSI
void recv_5_8_rssi_init(rtc6715_struct *p_ctl,osThreadId thread_id,uint32_t signal);
#endif

uint32_t rtc6715_write_reg(rtc6715_struct *p_ad, uint32_t reg_data, uint8_t reg_addr);
uint32_t rtc6715_read_reg(rtc6715_struct *p_ad, uint32_t *reg_data, uint8_t reg_addr);

void rtc6715_power_off(rtc6715_struct *p_ctl);
void rtc6715_power_on(rtc6715_struct *p_ctl);
void recv_5_8_make_html_cntrl_table(rtc6715_struct* p_ctl, ctrl_table_list_struct* p_list);

uint32_t rtc6715_set_freq(rtc6715_struct *p_ad, uint16_t freq);

void rtc6715_start_init(rtc6715_struct *p_ad);
void rtc6715_start_pin_init(rtc6715_struct *p_ad);

uint32_t rtc6715_wait_all_out(rtc6715_struct *p_ad, uint32_t time_ms);
uint8_t rtc6715_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, rtc6715_struct *, const char *p_name);

uint32_t recv_5_8_set_freq_up(rtc6715_struct *p_ad, uint16_t freq);
uint32_t recv_5_8_set_freq_up_auto(rtc6715_struct *p_ctl, uint16_t freq);
uint32_t recv_5_8_set_freq_down(rtc6715_struct *p_ad, uint16_t freq);

uint32_t recv_5_8_set_liter_up(rtc6715_struct *p_ctl);
uint32_t recv_5_8_set_liter_down(rtc6715_struct *p_ctl);
uint32_t recv_5_8_set_ch_up(rtc6715_struct *p_ctl);
uint32_t recv_5_8_set_ch_down(rtc6715_struct *p_ctl);

void recv_5_8_start_rssi(rtc6715_struct* p_ctl, uint8_t b_restart);
void recv_5_8_stop_rssi(rtc6715_struct* p_ctl);
uint8_t recv_5_8_get_rssi(rtc6715_struct* p_ctl);
void recv_5_8_continue_rssi(rtc6715_struct* p_ctl);
void recv_5_8_send_status(rtc6715_struct* p_ctl);

void recv_5_8_write_cfg(rtc6715_struct *p_ctl);

#endif /* LMX2492_H_ */
