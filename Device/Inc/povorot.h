/*
 * adf4002.h
 *
 *  Created on: 17.04.2014
 *      Author: krasikov
 */

#ifndef POVOROT_H_
#define POVOROT_H_
#include "common_proto_info.h"
//#include "stm32f4xx_hal.h"
#include "comm.h"
#include "debug_cells_lib.h"
#include "Driver_UART.h"


typedef enum{
	povorot_cmd_off = 			0,
	povorot_cmd_rotate_right = 	2,
	povorot_cmd_rotate_left = 	4,
	povorot_cmd_tilt_up = 		8,
	povorot_cmd_tilt_down = 	16,
} povorot_cmd_direction;

#define POVOROT_SCAN_STOP_ERROR_ANGLE			2.5

#define POVOROT_PARSER_DEFAULT_STATE			0
#define POVOROT_PARSER_START_PACKET_BYTE		0xFF

#define POVOROT_AZIMUT_REQ_TIME				1000
#define POVOROT_TILT_REQ_TIME				3500

#define POVOROT_PARSER_RX_TIMEOUT				200	// ������� �����, ��������� �����


#define POVOROT_CMD_PARSER_INPUT_DATA_BUF_LEN			260
#define POVOROT_CMD_PARSER_UART_BUF_LEN					32

typedef enum povorot_thread_event_e
{
	p_parser_event_time_out		= 1,
	p_parser_event_get_cmd		= 2,
	p_parser_event_get_data		= 4,
}povorot_thread_event_t;

typedef enum povorot_error_e{
	povorot_err_no =					 							0,
	povorot_err_soft = 												1,
}povorot_error_e;

typedef enum povorot_work_mode_e{
	povorot_work_mode_stop =			 							0,
	povorot_work_mode_cmd =											1,
	povorot_work_mode_scan =										2,
}povorot_work_mode_e;

typedef enum povorot_scan_mode_e{
	povorot_scan_mode_cont =			 							0,
	povorot_scan_mode_step =										1,
}povorot_scan_mode_e;

typedef enum povorot_scan_direct_e{
	povorot_scan_direct_up =			 							0,
	povorot_scan_direct_down =										1,
}povorot_scan_direct_e;


typedef enum povorot_cntrl_cmd_e{
	povorot_cntrl_cmd_stop =					 				1,
	povorot_cntrl_cmd_start_scan  =					 			2,
	povorot_cntrl_cmd_speed_scan  =					 			3,
	povorot_cntrl_set_angle_start_scan  =			 			4,
	povorot_cntrl_set_angle_stop_scan  =			 			5,
	povorot_cntrl_set_mode_scan  =					 			6,
	povorot_cntrl_set_num_step_scan  =					 		7,
	povorot_cntrl_set_time_step_scan  =					 		8,
	povorot_cntrl_cmd_direction =					 			9,
	povorot_cntrl_cmd_exec_direction =					 		10,
	povorot_cntrl_cmd_angle_pos =				 				11,
	povorot_cntrl_cmd_tilt_pos = 								12,
	povorot_cntrl_cmd_step_left = 								13,
	povorot_cntrl_cmd_step_right = 								14,
	povorot_cntrl_cmd_step_up = 								15,
	povorot_cntrl_cmd_step_down = 								16,
	povorot_cntrl_set_angle_correct = 							17

}povorot_cntrl_cmd_e;

typedef struct povorot_struct{
	povorot_cmd_direction	cmd_direct;
	povorot_scan_direct_e	scan_direct;
	povorot_scan_mode_e		scan_mode;
	povorot_work_mode_e		work_mode;

	float					scan_dest_angle;
	float					scan_up_angle;
	float					scan_down_angle;
	float					correct_angle;

	uint32_t				scan_cmd_error_time;
	uint32_t				recv_ack;
	uint32_t				recv_ack_old;
	uint32_t				recv_ack_cntr;
	uint32_t				scan_step_time;
	uint32_t				scan_step_time_max;
	uint32_t				scan_num_step;

	uint8_t					data[8];
	uint8_t					addr;
	uint8_t					cmd;
	uint8_t					preset_cmd;
	uint8_t					tilt_speed;
	uint8_t					rotate_speed;
	ARM_DRIVER_UART			*p_uart;
//	osThreadId 				thread_id;		//id ������ ���������� ������
	osEventFlagsId_t 		parser_thread_ev;

//	uint16_t 							input_len;
//	uint16_t 							cntr;
//	ARM_DRIVER_UART						*p_uart;
	uint8_t 							data_buf[POVOROT_CMD_PARSER_INPUT_DATA_BUF_LEN];
	uint8_t 							uart_buf[POVOROT_CMD_PARSER_UART_BUF_LEN];
	uint8_t 							uart_buf_len;
	uint8_t 							uart_buf_cntr;
//	uint8_t					crc;
	uint8_t								input_len;
	uint8_t								receive_state;

//	uint32_t 							os_timer[6];
//	osTimerDef_t 						os_timer_def;
	osTimerId_t 							timer_id;

	float								rotate_angle;
	float								rotate_own_angle;
	float								tilt_angle;

	float								cmd_rotate_angle;
	float								cmd_tilt_angle;

	struct gpio_t_ 						rs485_pin;

	uint8_t					error;
	cmd_dev_s				ctrl_cmd;
	ack_msg_s				*p_ack;
	uint8_t					id;

	SettingCell_t 						html_table[22+1+1+2];
	uint8_t								ack_dbg_tmp[128];
}povorot_struct;

void povorot_start_init(povorot_struct *p_ctl, ARM_DRIVER_UART *p_uart, uint8_t addr, osEventFlagsId_t uart_thread_ev);
void povorot_make_cmd(povorot_struct *p_ctl, uint8_t cmd);
uint32_t povorot_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, povorot_struct *p_ad, const char *p_name);
void povorot_thread(void *argument);
void povorot_make_html_cntrl_table(povorot_struct* p_ctl, ctrl_table_list_struct* p_list);
void povorot_cmd_make_html_cntrl_table(povorot_struct* p_ctl, ctrl_table_list_struct* p_list);
uint8_t*  povorot_dbg_get_rotate_angle(uint32_t adf32, uint32_t cmd, uint32_t tmp);
uint8_t*  povorot_dbg_get_tilt_angle(uint32_t adf32, uint32_t cmd, uint32_t tmp);
uint8_t*  povorot_cmd_dbg_get_base_angle(uint32_t adf32, uint32_t cmd, uint32_t tmp);
uint8_t*  povorot_dbg_get_rotate_state(uint32_t adf32, uint32_t cmd, uint32_t tmp);

uint32_t povorot_cmd_make_ctrl_cmd(povorot_struct *p_ctl,uint8_t cmd, uint8_t len, uint8_t *p_data, dev_cntrl_ack_response_e have_ack);
uint32_t povorot_make_ctrl_cmd(povorot_struct *p_ctl,uint8_t cmd, uint8_t len, uint8_t *p_data, dev_cntrl_ack_response_e have_ack);

uint32_t povorot_cmd_send_cmd_stop(povorot_struct *p_ctl);

#endif /* POVOROT_H_ */
