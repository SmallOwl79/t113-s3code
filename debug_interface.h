/*! @file ***************************************************************************************
 * @brief
 * 			��������� ������ ����������� ����������
 *
 *************************************************************************************************/
#ifndef DEBUG_INTERFACE_H_
#define DEBUG_INTERFACE_H_

#include "uart_cmd_parser.h"
#ifdef RTE_ETH
#include "eth_udp_cmd_parser.h"
#endif
#ifdef CAN_DEBUG
#include "can_cmd_parser.h"
#endif

typedef enum dbg_printf_cntr_state{
	dbg_printf_disable = 0,
	dbg_printf_special = 1,
	dbg_printf_cntrl = 2,
} dbg_printf_cntr_state;


#define DEBUG_INTERFACE_START_STATE_ACK_EN		dbg_printf_disable
#define DEBUG_INTERFACE_START_STATE_SRC_ADDR	0
#define DEBUG_INTERFACE_START_STATE_DEST_ADDR	0


#define DEBUG_INTERFACE_SEND_BUF_LEN			260
#define DEBUG_INTERFACE_TMP_BUF_LEN				260


typedef struct debug_info_struct{
dbg_printf_cntr_state 	b_printf_ack_en;
uint8_t		src_addr;		// ����� ��������� ������ (������������� ���������)
uint8_t		dest_addr;		// ����� ���������� ������ (������������� ���������)
uint8_t 	*p_buf;
uint16_t 	buf_len;
uart_cmd_parser *p_pars_uart;
#ifdef RTE_ETH
eth_cmd_udp_parser *p_pars_udp;
#endif
#ifdef CAN_DEBUG
can_cmd_parser		*p_pars_can;
#endif
uint8_t send_buf[DEBUG_INTERFACE_SEND_BUF_LEN];
uint8_t	tmp_buf[DEBUG_INTERFACE_TMP_BUF_LEN];
}debug_info_struct;

extern debug_info_struct	printf_dbg;

//*-----------------------------------------------------------------------------------------------
//*			�������
//*-----------------------------------------------------------------------------------------------
// ���������� �������� ������.
cmd_result_t cmd_debug_parser(universal_interface_struct_t * rx_data_struct);
void debug_interface_start_init(debug_info_struct *p_d, dbg_printf_cntr_state init);
void debug_interface_send_string(debug_info_struct *p_d);
cmd_result_t dbg_write_printf_param(debug_info_struct *p_d, universal_interface_struct_t * rx_data_struct);
size_t debug_write_data(debug_info_struct *p_debug, uint8_t *p_c,size_t size);

//*-----------------------------------------------------------------------------------------------
//*			��������� ��������� �������
//*-----------------------------------------------------------------------------------------------
cmd_result_t dbg_read_cell_type(universal_interface_struct_t * rx_data_struct);
cmd_result_t dbg_read_cell_list(universal_interface_struct_t * rx_data_struct);
cmd_result_t dbg_read_cell(universal_interface_struct_t * rx_data_struct);
cmd_result_t dbg_write_cell(universal_interface_struct_t * rx_data_struct);
cmd_result_t dbg_ext_read_cell(universal_interface_struct_t * rx_data_struct);
cmd_result_t dbg_ext_write_cell(universal_interface_struct_t * rx_data_struct);

cmd_result_t dbg_default_cell(universal_interface_struct_t * rx_data_struct);
cmd_result_t dbg_read_ram(universal_interface_struct_t * rx_data_struct);
cmd_result_t dbg_write_ram(universal_interface_struct_t * rx_data_struct);

#endif /* DEBUG_INTERFACE_H_ */

