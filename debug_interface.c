/*! @file ***************************************************************************************
 *
 * @brief
 * 			������ ����������� ����������
 *
 * $Revision: 1 $
 * $Date: 2012-06-21 $
 *
 *
 *************************************************************************************************/


//*-----------------------------------------------------------------------------------------------
//*			������������ �����
//*-----------------------------------------------------------------------------------------------
#include "stdbool.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "debug_interface.h"
//#include "debug_cells.h"
#include "debug_cells_lib.h"
#include "common_proto_cmd.h"

//*-----------------------------------------------------------------------------------------------
//*			���������
//*-----------------------------------------------------------------------------------------------
#define DBG_ANS		0
#define DEBUG_CMD	1
#define ANS			1
#define DBG_DATA	2

#pragma location=".ddr_data"
debug_info_struct	printf_dbg;

typedef union
{
	uint32_t	u32_;
	uint16_t	u16_[2];
	uint8_t		u8_[4];
} union32_t;

typedef union
{
	uint16_t	u16_;
	uint8_t		u8_[2];
} union16_t;

SettingCell_t * select_cell_table(void);

#pragma location=".ddr_data"
static uint8_t  debug_data_buffer[300];
static uint8_t * debug_data_answer_ptr;
static uint16_t debug_answer_len;
static SettingCell_t *DebugCellTable_ptr;
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////
void debug_interface_start_init(debug_info_struct *p_d, dbg_printf_cntr_state init){
	p_d->p_pars_uart = 0;
#ifdef RTE_ETH
	p_d->p_pars_udp = 0;
#endif
#ifdef CAN_DEBUG
	p_d->p_pars_can = 0;
#endif
	p_d->b_printf_ack_en = init;//dbg_printf_cntrl;//dbg_printf_disable;
	p_d->dest_addr = DEBUG_INTERFACE_START_STATE_DEST_ADDR;
	p_d->p_buf = p_d->send_buf;
	p_d->buf_len = sizeof(p_d->send_buf);
	p_d->src_addr = DEBUG_INTERFACE_START_STATE_ACK_EN;
}
//*-----------------------------------------------------------------------------------------------
/**			���������� ���������� ������
 *
 * @param rx_data_struct	- ��������� �� ��������� ��������� ������
 * @return 					- ��������� ���������� �������										*/
//*-----------------------------------------------------------------------------------------------
cmd_result_t cmd_debug_parser(universal_interface_struct_t * rx_data_struct){
	// ���� ���������� �� ������ ���� ������
	if (rx_data_struct->data_length == 0)
		return cmd_len_error;

	DebugCellTable_ptr = select_cell_table();

	cmd_result_t cmd_result = cmd_not_supported;
	uint8_t command = rx_data_struct->data[DEBUG_CMD];
//	printf("dbg cmd =%x \r\n",command);
	switch (command){
		// ��������������� ���������� �������
		case CMD_DEBUG_READ_CELL_TYPE: 	cmd_result = dbg_read_cell_type(rx_data_struct);	break;
		case CMD_DEBUG_READ_CELL_LIST: 	cmd_result = dbg_read_cell_list(rx_data_struct);	break;
		case CMD_DEBUG_READ_CELL: 		cmd_result = dbg_read_cell(rx_data_struct); 		break;
		case CMD_DEBUG_WRITE_CELL: 		cmd_result = dbg_write_cell(rx_data_struct); 		break;
		case CMD_DEBUG_EXT_READ_CELL: 	cmd_result = dbg_ext_read_cell(rx_data_struct); 	break;
		case CMD_DEBUG_EXT_WRITE_CELL:	cmd_result = dbg_ext_write_cell(rx_data_struct); 	break;
		case CMD_DEBUG_DEFAULT_CELL: 	cmd_result = dbg_default_cell(rx_data_struct); 		break;
		case CMD_DEBUG_READ_RAM: 		cmd_result = dbg_read_ram(rx_data_struct); 			break;
		case CMD_DEBUG_WRITE_RAM: 		cmd_result = dbg_write_ram(rx_data_struct); 		break;
		case CMD_DEBUG_PRINTF_PARAM:	cmd_result = dbg_write_printf_param(&printf_dbg, rx_data_struct); 		break;

		default:						cmd_result = cmd_not_supported; break;
	};

	// �������� ���������� ������
	switch(cmd_result)	{
	// ������� ��������� �������, ������ ����� ��
	case cmd_ok:
		rx_data_struct->data[DBG_ANS] = ANS_DEBUG_INFO;
		rx_data_struct->data[ANS] = ANS_DBG_OK;
		rx_data_struct->data[DBG_DATA+0] = command;
		rx_data_struct->data_length = 3;

		rx_data_struct->transmit_ack(rx_data_struct->p_ack);

		break;
	// ���������� �� ������ ������������ ������� (������)
	case cmd_busy:
		rx_data_struct->data[DBG_ANS] = ANS_DEBUG_INFO;
		rx_data_struct->data[ANS] = ANS_ERROR;
		rx_data_struct->data[DBG_DATA+0] = command;
		rx_data_struct->data[DBG_DATA+1] = ERROR_BUSY_DEVICE;
		rx_data_struct->data_length = 4;

		rx_data_struct->transmit_ack(rx_data_struct->p_ack);
		break;

	// ������ ����������
	case cmd_param_error:
		rx_data_struct->data[DBG_ANS] = ANS_DEBUG_INFO;
		rx_data_struct->data[DEBUG_CMD] = ANS_ERROR;
		rx_data_struct->data[DBG_DATA+0] = command;
		rx_data_struct->data[DBG_DATA+1] = ERROR_PARAMETER;
		rx_data_struct->data_length = 4;

		rx_data_struct->transmit_ack(rx_data_struct->p_ack);
		break;

	// ������ ����� �������
	case cmd_len_error:
		rx_data_struct->data[DBG_ANS] = ANS_DEBUG_INFO;
		rx_data_struct->data[ANS] = ANS_ERROR;
		rx_data_struct->data[DBG_DATA+0] = command;
		rx_data_struct->data[DBG_DATA+1] = ERROR_LENGTH_FIELD;
		rx_data_struct->data_length = 4;

		rx_data_struct->transmit_ack(rx_data_struct->p_ack);
		break;

	// ������� ���������� �������, ����� ����������� � �����������
	case cmd_no_answer:
		break;

	// ���������� ������ ���������� �������
	case cmd_hard_error:
		rx_data_struct->data[DBG_ANS] = ANS_DEBUG_INFO;
		rx_data_struct->data[ANS] = ANS_ERROR;
		rx_data_struct->data[DBG_DATA+0] = command;
		rx_data_struct->data[DBG_DATA+1] = ERROR_HARDWARE;
		rx_data_struct->data_length = 4;

		rx_data_struct->transmit_ack(rx_data_struct->p_ack);
		break;

	// ������� ����������� ��� ������� ������
	case cmd_illegal:
		rx_data_struct->data[DBG_ANS] = ANS_DEBUG_INFO;
		rx_data_struct->data[ANS] = ANS_ERROR;
		rx_data_struct->data[DBG_DATA+0] = command;
		rx_data_struct->data[DBG_DATA+1] = ERROR_ILLEGAL_CMD;
		rx_data_struct->data_length = 4;

		rx_data_struct->transmit_ack(rx_data_struct->p_ack);
		break;

	// ��������� ����� debug_data_buffer ������ debug_answer_len
	case cmd_send_buff:
		rx_data_struct->data[DBG_ANS] = ANS_DEBUG_INFO;
		rx_data_struct->data[ANS] = command;
		memcpy(&rx_data_struct->data[DBG_DATA+0], debug_data_answer_ptr, debug_answer_len);
		rx_data_struct->data_length = debug_answer_len + 2;

		rx_data_struct->transmit_ack(rx_data_struct->p_ack);
		break;

	// ������������� ������� (��������� ������ ���� ��������� �����)
	case cmd_send_buff_not_standard:
		rx_data_struct->transmit_ack(rx_data_struct->p_ack);
		break;

	// ���������������� �������
	default:
		rx_data_struct->data[DBG_ANS] = ANS_DEBUG_INFO;
		rx_data_struct->data[ANS] = ANS_ERROR;
		rx_data_struct->data[DBG_DATA+0] = command;
		rx_data_struct->data[DBG_DATA+1] = ERROR_ILLEGAL_CMD; //ERROR_NOT_SUPPORTED;
		rx_data_struct->data_length = 4;

		rx_data_struct->transmit_ack(rx_data_struct->p_ack);
		break;
	}
	return cmd_no_answer;
}
//*-----------------------------------------------------------------------------------------------
/**			������� �������� ������
 *
 * @param rx_data_struct	- ��������� �� ��������� ��������� ������
 * @return 					- ��������� ���������� �������										*/
//*-----------------------------------------------------------------------------------------------
cmd_result_t dbg_read_cell_type(universal_interface_struct_t * rx_data_struct){
	if (rx_data_struct->data_length < 2+1)
		return cmd_len_error;

	cell_cmd_result_t result = debug_cell_read_type(rx_data_struct->data[DBG_DATA+0],
													debug_data_buffer,
													DebugCellTable_ptr);
	debug_answer_len = debug_data_buffer[0];
	debug_data_answer_ptr = &debug_data_buffer[1];

//	for(int i=0;i<debug_answer_len;i++){
//		printf(" %x",debug_data_answer_ptr[i]);
//	}
//	printf("\r\n");

	if ((result == cell_data_ready) && (debug_answer_len))
		return cmd_send_buff;
	else
		return cmd_param_error;
}
//*-----------------------------------------------------------------------------------------------
/**			������� ������ �������������� ���������� �����
 *
 * @param rx_data_struct	- ��������� �� ��������� ��������� ������
 * @return 					- ��������� ���������� �������										*/
//*-----------------------------------------------------------------------------------------------
cmd_result_t dbg_read_cell_list(universal_interface_struct_t * rx_data_struct){
	uint8_t skip_cells = 0;

	// rx_data_struct->data[1] - ����� ������ �������� ������
	if (rx_data_struct->data_length > 2)
		skip_cells = rx_data_struct->data[DBG_DATA+0];

	cell_cmd_result_t result = debug_cell_read_list(skip_cells,debug_data_buffer,DebugCellTable_ptr);
	debug_answer_len = debug_data_buffer[0];
	debug_data_answer_ptr = &debug_data_buffer[1];

	if ((result == cell_data_ready) && (debug_answer_len))
		return cmd_send_buff;
	else
		return cmd_param_error;
}
//*-----------------------------------------------------------------------------------------------
/**			��������� ���������� ������
 *
 * @param rx_data_struct	- ��������� �� ��������� ��������� ������
 * @return 					- ��������� ���������� �������										*/
//*-----------------------------------------------------------------------------------------------
cmd_result_t dbg_read_cell(universal_interface_struct_t * rx_data_struct){
	// rx_data_struct->data[DBG_DATA+0..] - ������ �������� �����

	// ������ ���� ������ ���� �� ���� �����
	if (rx_data_struct->data_length < 3)
		return cmd_len_error;

	// ��������� ������ ������� ������ (������� ����� ������)
	rx_data_struct->data[rx_data_struct->data_length] = 0;

	cell_cmd_result_t result = debug_cell_read(&rx_data_struct->data[DBG_DATA+0],
												debug_data_buffer,
												DebugCellTable_ptr);
	debug_answer_len = debug_data_buffer[0];
	debug_data_answer_ptr = &debug_data_buffer[1];

	if ((result == cell_data_ready) && (debug_answer_len))
		return cmd_send_buff;
	else
		return cmd_param_error;
}
//*-----------------------------------------------------------------------------------------------
/**			��������� ���������� ������
 *
 * @param rx_data_struct	- ��������� �� ��������� ��������� ������
 * @return 					- ��������� ���������� �������										*/
//*-----------------------------------------------------------------------------------------------
cmd_result_t dbg_ext_read_cell(universal_interface_struct_t * rx_data_struct){
	// rx_data_struct->data[DBG_DATA+0..] - ������ �������� �����

	// ������ ���� ������ ���� �� ���� �����
	if (rx_data_struct->data_length < 5)
		return cmd_len_error;

	cell_cmd_result_t result = debug_ext_cell_read(&rx_data_struct->data[DBG_DATA+0],
												debug_data_buffer,
												DebugCellTable_ptr);
	debug_answer_len = debug_data_buffer[0];
	debug_data_answer_ptr = &debug_data_buffer[1];

	if ((result == cell_data_ready) && (debug_answer_len))
		return cmd_send_buff;
	else
		return cmd_param_error;
}
//*-----------------------------------------------------------------------------------------------
/**			�������� ���������� ������
 *
 * @param rx_data_struct	- ��������� �� ��������� ��������� ������
 * @return 					- ��������� ���������� �������										*/
//*-----------------------------------------------------------------------------------------------
cmd_result_t dbg_write_cell(universal_interface_struct_t * rx_data_struct){
	// rx_data_struct->data[DBG_DATA+0] - ����� ������
	// rx_data_struct->data[DBG_DATA+1..] - ������������ ������

	// ������ ���� ������ ���� �� ���� ����� � ���� ������
	if (rx_data_struct->data_length < 4)
		return cmd_len_error;

	cell_cmd_result_t result = debug_cell_write(rx_data_struct->data[DBG_DATA+0],
												&rx_data_struct->data[DBG_DATA+1],
												rx_data_struct->data_length - 3,
												DebugCellTable_ptr);
	if (result == cell_cmd_ok)
		return cmd_ok;
	else
		return cmd_param_error;
}
//*-----------------------------------------------------------------------------------------------
/**			�������� ���������� ������
 *
 * @param rx_data_struct	- ��������� �� ��������� ��������� ������
 * @return 					- ��������� ���������� �������										*/
//*-----------------------------------------------------------------------------------------------
cmd_result_t dbg_ext_write_cell(universal_interface_struct_t * rx_data_struct){
	// rx_data_struct->data[DBG_DATA+0] - ����� ������
	// rx_data_struct->data[DBG_DATA+1..] - ������������ ������

	// ������ ���� ������ ���� �� ���� ����� � ���� ������
	if (rx_data_struct->data_length < 6)
		return cmd_len_error;

	cell_cmd_result_t result = debug_ext_cell_write(rx_data_struct->data[DBG_DATA+2],
												&rx_data_struct->data[DBG_DATA+3],
												rx_data_struct->data_length - 5,
												DebugCellTable_ptr);
	if (result == cell_cmd_ok)
		return cmd_ok;
	else
		return cmd_param_error;
}

//*-----------------------------------------------------------------------------------------------
/**			���������������� ���������� ������ ��������� �� ���������
 *
 * @param rx_data_struct	- ��������� �� ��������� ��������� ������
 * @return 					- ��������� ���������� �������										*/
//*-----------------------------------------------------------------------------------------------
cmd_result_t dbg_default_cell(universal_interface_struct_t * rx_data_struct){
	// rx_data_struct->data[DBG_DATA+0] - ����� ������

	if (rx_data_struct->data_length != 3)
		return cmd_len_error;

	cell_cmd_result_t result = debug_cell_set_default(rx_data_struct->data[DBG_DATA+0], DebugCellTable_ptr);

	if (result == cell_cmd_ok)
		return cmd_ok;
	else
		return cmd_param_error;
}


//*-----------------------------------------------------------------------------------------------
/**			��������� ������
 *
 * @param rx_data_struct	- ��������� �� ��������� ��������� ������
 * @return 					- ��������� ���������� �������										*/
//*-----------------------------------------------------------------------------------------------
cmd_result_t dbg_read_ram(universal_interface_struct_t * rx_data_struct){
	if (rx_data_struct->data_length < 7)
		return cmd_len_error;

	// ������� �����
	union32_t RAM_Addr;
	RAM_Addr.u8_[0] = rx_data_struct->data[DBG_DATA+0];
	RAM_Addr.u8_[1] = rx_data_struct->data[DBG_DATA+1];
	RAM_Addr.u8_[2] = rx_data_struct->data[DBG_DATA+2];
	RAM_Addr.u8_[3] = rx_data_struct->data[DBG_DATA+3];

	// ������� �����
	uint8_t RAM_Read_Len = rx_data_struct->data[DBG_DATA+4];

	// ��������� �����
	debug_answer_len = RAM_Read_Len + 4; // + 4 ����� ������ ������
	debug_data_buffer[0] = rx_data_struct->data[DBG_DATA+0];
	debug_data_buffer[1] = rx_data_struct->data[DBG_DATA+1];
	debug_data_buffer[2] = rx_data_struct->data[DBG_DATA+2];
	debug_data_buffer[3] = rx_data_struct->data[DBG_DATA+3];
	debug_data_answer_ptr = &debug_data_buffer[0];

	// ��������� ���� ������
	if (debug_read_ram((uint8_t *)&debug_data_buffer[4], (uint8_t *)RAM_Addr.u32_, RAM_Read_Len))
		return cmd_send_buff;
	else
		return cmd_param_error;
}
//*-----------------------------------------------------------------------------------------------
/**			�������� ������
 *
 * @param rx_data_struct	- ��������� �� ��������� ��������� ������
 * @return 					- ��������� ���������� �������										*/
//*-----------------------------------------------------------------------------------------------
cmd_result_t dbg_write_ram(universal_interface_struct_t * rx_data_struct){
	if (rx_data_struct->data_length < 7)
		return cmd_len_error;

	// ������� �����
	union32_t RAM_Addr;
	RAM_Addr.u8_[0] = rx_data_struct->data[DBG_DATA+0];
	RAM_Addr.u8_[1] = rx_data_struct->data[DBG_DATA+1];
	RAM_Addr.u8_[2] = rx_data_struct->data[DBG_DATA+2];
	RAM_Addr.u8_[3] = rx_data_struct->data[DBG_DATA+3];

	// ������� �����
	uint8_t RAM_Read_Len = rx_data_struct->data_length - 6;

	if (debug_write_ram((uint8_t *)RAM_Addr.u32_, &rx_data_struct->data[DBG_DATA+4], RAM_Read_Len))
		return cmd_ok;
	else
		return cmd_param_error;
}
//*-----------------------------------------------------------------------------------------------
/**			�������� ������
 *
 * @param rx_data_struct	- ��������� �� ��������� ��������� ������
 * @return 					- ��������� ���������� �������										*/
//*-----------------------------------------------------------------------------------------------
cmd_result_t dbg_write_printf_param(debug_info_struct *p_d, universal_interface_struct_t * rx_data_struct){

	p_d->b_printf_ack_en = (dbg_printf_cntr_state)rx_data_struct->data[DBG_DATA+0];
	p_d->src_addr = rx_data_struct->src_addr;
	p_d->dest_addr = rx_data_struct->dest_addr;
	return cmd_ok;
}
size_t debug_write_data(debug_info_struct *p_debug, uint8_t *p_c,size_t size){
uint8_t tmp[6];
int i,num=0;
uint8_t crc = 0;
#ifdef RTE_ETH
struct pbuf *pb;
err_t	ret_udp;
#endif
//	return 0;
		if(size > 100) return 0;

		switch( p_debug->b_printf_ack_en){

		case dbg_printf_disable: break;
		case dbg_printf_cntrl:

			if (p_debug->p_pars_uart){
				crc ^= 1;
				crc ^= 127;
				crc ^= size + 3;
				crc ^= 0xF0;
				crc ^= 0x8E;

				tmp[0] = UART_CMD_PARSER_START_PACKET_BYTE;
				tmp[1] = 1;
				tmp[2] = 127;
				tmp[3] = size + 3;
				tmp[4] = 0xF0;
				tmp[5] = 0x8E;

				for(i=0;i<size;i++){
					crc ^= p_c[i];
				}
				osMutexAcquire(p_debug->p_pars_uart->mutex_write_id, osWaitForever);
				p_debug->p_pars_uart->p_uart->WriteData(tmp,6);
				p_debug->p_pars_uart->p_uart->WriteData(p_c,size);
				p_debug->p_pars_uart->p_uart->WriteData(&crc,1);
				osMutexRelease (p_debug->p_pars_uart->mutex_write_id);
			}
#ifdef CAN_DEBUG
			if (p_debug->p_pars_can){
				can_cmd_parser_make_packet(p_debug->p_pars_can, p_c,size,p_debug->p_pars_can->src_lit,p_debug->p_pars_can->src_ch);
			}
#endif
			else{
#ifdef RTE_ETH
				if(p_debug->p_pars_udp){
					p_debug->tmp_buf[0] = size+4;
					p_debug->tmp_buf[1] = (size+4) >> 8;

					p_debug->tmp_buf[2] = 0xF0;
					p_debug->tmp_buf[3] = 0x8E;
					if((size + 4) < DEBUG_INTERFACE_TMP_BUF_LEN){
						memcpy(&p_debug->tmp_buf[4],p_c,size);

						pb = pbuf_alloc(PBUF_TRANSPORT, 512, PBUF_REF);
						pb->payload = (void*)&p_debug->tmp_buf[0];
						pb->len = pb->tot_len = size+4;

						ret_udp = udp_sendto(p_debug->p_pars_udp->pcb, pb, (ip_addr_t*)&p_debug->p_pars_udp->dst_ip,p_debug->p_pars_udp->udp_port);
						if(ret_udp != ERR_OK){
//							printf("udp send error \r\n");
						}
						pbuf_free(pb);
					}
				}
#endif
			}
			break;
			break;
		case dbg_printf_special:
			if (p_debug->p_pars_uart){
				num = p_debug->p_pars_uart->p_uart->WriteData((uint8_t*)p_c,size);
			}
			break;
		}
	return num;
}
