/*
 * povorot.c
 *
 *  Created on: 28 сент. 2023 г.
 *      Author: Petr
 */

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "povorot.h"
#include "control.h"

#define FACK_485_POVOROT		1

//osMessageQDef(povorot_cmd_sys_msg_que, CONTROL_DEV_QUE_MSG_POOL_SIZE, cntrl_dev_sys_msg_que_type_s); // Define message queue
//osPoolDef(povorot_cmd_sys_msg_que_pool, CONTROL_DEV_QUE_MSG_POOL_SIZE, cntrl_dev_sys_msg_que_type_s); // Define memory pool
//
//extern const osThreadDef_t os_thread_def_povorot_thread;

#define POVOROT_MSG_COUNT 10
#define POVOROT_MSG_SIZE  sizeof(cntrl_dev_sys_msg_que_type_s) // Теперь размер — это размер структуры

// 2. Рассчитываем память (с учетом заголовка RTX5)
#define POVOROT_FULL_BLOCK_SIZE (((POVOROT_MSG_SIZE + 3U) & ~3UL) + 12U)
#pragma location=".ddr_data"
static uint32_t povorot_myQueue_mem[(POVOROT_MSG_COUNT * POVOROT_FULL_BLOCK_SIZE) / 4];
#pragma location=".ddr_data"
static uint32_t povorot_myQueue_cb[osRtxMessageQueueCbSize / 4];


static void povorot_start_modbus(void *p_data);
static void povorot_stop_modbus(void *p_data);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_start_init(povorot_struct *p_ctl, ARM_DRIVER_UART *p_uart, uint8_t addr, osEventFlagsId_t uart_thread_ev){
	p_ctl->error = 0;
	p_ctl->recv_ack = 0;
	p_ctl->p_uart = p_uart;
	p_ctl->addr = addr;
	p_ctl->rotate_speed = 30;
	p_ctl->tilt_speed = 30;
	p_ctl->preset_cmd = 70;
	memset(&p_ctl->data[0],0,8);
	p_ctl->data[0] = 0xFF;
	p_ctl->data[1] = p_ctl->addr;
	p_ctl->data[6] = 0xAF;

	p_ctl->uart_buf_len = 0;
	p_ctl->uart_buf_cntr = 0;

	p_ctl->scan_cmd_error_time = 0;

	p_ctl->input_len = 0;
	p_ctl->receive_state = POVOROT_PARSER_DEFAULT_STATE;

	p_ctl->rotate_angle = 0.0;
	p_ctl->rotate_own_angle = 0.0;
	p_ctl->tilt_angle = 0.0;

	p_ctl->cmd_rotate_angle = 90.0;
	p_ctl->cmd_tilt_angle = 5.0;

	const osMessageQueueAttr_t queue_attr = {
	    .name = NULL,
	    .cb_mem = povorot_myQueue_cb,
	    .cb_size = sizeof(povorot_myQueue_cb),
	    .mq_mem = povorot_myQueue_mem,
	    .mq_size = sizeof(povorot_myQueue_mem)
	};

	p_ctl->ctrl_cmd.msg_que_id = osMessageQueueNew(POVOROT_MSG_COUNT, POVOROT_MSG_SIZE, &queue_attr);
	p_ctl->ctrl_cmd.ack_msg_que_id = NULL;

	gpio_init_pin_sun(&p_ctl->rs485_pin);

//	GPIO_InitTypeDef GPIO_InitStructure;
//
//	GPIO_InitStructure.Pin = VID3_EN485_PIN;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStructure.Pull = GPIO_PULLUP;
//	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
//	HAL_GPIO_Init(VID3_EN485_PORT, &GPIO_InitStructure);

//	HAL_GPIO_WritePin(VID3_EN485_PORT, VID3_EN485_PIN, GPIO_PIN_RESET);

	gpio_set_sun(&p_ctl->rs485_pin,GPIO_RESET);

	p_ctl->parser_thread_ev = osEventFlagsNew(NULL);
	p_ctl->p_uart->Initialize();
	p_ctl->p_uart->Configure(9600,8,0,0,0);
	p_ctl->p_uart->IoCtrl(ARM_UART_SET_START_STOP_CALLBACK_PARAM,(uint32_t)p_ctl);
	p_ctl->p_uart->IoCtrl(ARM_UART_SET_START_CALLBACK_FUNC,(uint32_t)&povorot_start_modbus);
	p_ctl->p_uart->IoCtrl(ARM_UART_SET_STOP_CALLBACK_FUNC,(uint32_t)&povorot_stop_modbus);

//	p_ctl->thread_id = osThreadCreate (osThread(povorot_thread), p_ctl);
	osThreadNew(povorot_thread, p_ctl,NULL);
	p_ctl->p_uart->EventConfigure(p_ctl->parser_thread_ev,uart_thread_ev,p_parser_event_get_data,uart_driver_register(p_ctl->p_uart));

//	writel(0x83,0x02500C0C);
//udelay(10);
////	ptr_uart->Instance->UART_LCR = 0x83;
//
//	// 2. В ЭТОТ МОМЕНТ (пока DLAB=1) пробуем задать режим RS485
////	ptr_uart->Instance->UART_MCR = (0x02 << 6);
//		writel(0xA0,0x02500C10);
//		udelay(10);
//
//	// 3. Выходим из режима конфигурирования
////	ptr_uart->Instance->UART_LCR = 0x03;
//		writel(0x03,0x02500C0C);
//		udelay(10);
//
//	// 4. СБРАСЫВАЕМ FIFO (обязательно для активации авто-логики)
////	ptr_uart->Instance->IIR_FCR = 0x01 | 0x06;
//		writel(0x01 | 0x06,0x02500C08);
//		udelay(10);


//	writel(0x80,0x02500C10);
	p_ctl->p_uart->StartReceive();

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_start_modbus(void *p_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct*)p_data;
//	HAL_GPIO_WritePin(VID3_EN485_PORT, VID3_EN485_PIN, GPIO_PIN_SET);
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	gpio_set_sun(&p_ctl->rs485_pin,GPIO_SET);
	//osDelay(2);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_stop_modbus(void *p_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct*)p_data;
//	udelay(1000);
	gpio_set_sun(&p_ctl->rs485_pin,GPIO_RESET);
//	HAL_GPIO_WritePin(VID3_EN485_PORT, VID3_EN485_PIN, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t povorot_send_cmd_start_scan(povorot_struct *p_ctl){
uint32_t res=0;
	res |= povorot_make_ctrl_cmd(p_ctl,povorot_cntrl_cmd_start_scan,0,NULL,dev_cntrl_ack_not_response);
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t povorot_send_cmd_stop(povorot_struct *p_ctl){
uint32_t res=0;
	res |= povorot_make_ctrl_cmd(p_ctl,povorot_cntrl_cmd_stop,0,NULL,dev_cntrl_ack_not_response);
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t povorot_make_ctrl_cmd(povorot_struct *p_ctl,uint8_t cmd, uint8_t len, uint8_t *p_data, dev_cntrl_ack_response_e have_ack){
cntrl_dev_sys_msg_que_type_s* p_buf;
cntrl_dev_sys_msg_que_type_s buf;
uint32_t res=0;
	p_buf = &buf;
	p_buf->cmd = cmd;
	p_buf->have_ack = have_ack;
	p_buf->time = osKernelGetTickCount();
	if(len){
		memcpy(&p_buf->buf[0],p_data,len);
	}

	ack_msg_on_send_cmd (p_ctl->p_ack, p_buf, p_ctl->id);

	if (osMessageQueuePut(p_ctl->ctrl_cmd.msg_que_id,p_buf,0, 1000) != osOK){
		p_ctl->error |= povorot_err_soft;
		ack_msg_on_clear_send_cmd(p_ctl->p_ack, p_buf, p_ctl->id);
		res = 2;
	}
	else{
		osEventFlagsSet(p_ctl->parser_thread_ev,p_parser_event_get_cmd);
	}
	return res;
//cntrl_dev_sys_msg_que_type_s *p_buf;
//uint32_t res=0;
//	p_buf = osPoolAlloc(p_ctl->ctrl_cmd.cntrl_msg_pool_id);
//	if(p_buf == NULL) {
//		p_ctl->error |= povorot_err_soft;
//		res = 1;
//	}
//	else{
//		p_buf->cmd = cmd;
//		p_buf->have_ack = have_ack;
//		p_buf->time = osKernelSysTick();
//		if(len){
//			memcpy(&p_buf->buf[0],p_data,len);
//		}
//
//		ack_msg_on_send_cmd (p_ctl->p_ack, p_buf, p_ctl->id);
//
//		if (osMessagePut(p_ctl->ctrl_cmd.cntrl_msg_que_id, (uint32_t)p_buf, 1000) != osOK){
//
//			p_ctl->error |= povorot_err_soft;
//
//			ack_msg_on_clear_send_cmd(p_ctl->p_ack, p_buf, p_ctl->id);
//
//			osPoolFree(p_ctl->ctrl_cmd.cntrl_msg_pool_id,p_buf);
//
//			res = 2;
//		}
//		else{
//			osSignalSet(p_ctl->thread_id,p_parser_event_get_cmd);
//		}
//	}
//	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_make_cmd_ack(povorot_struct *p_ctl,uint8_t cmd, uint8_t ack){
//control_sys_msg_que_type_s *p_buf;
//	p_buf = osPoolAlloc(p_ctl->ctrl_cmd.cntrl_ack_msg_pool_id);
//	if(p_buf == NULL) {
//		p_ctl->error |= povorot_err_soft;
//	}
//	else{
//		p_buf->id = p_ctl->id;
//		p_buf->ack_cmd = cmd;
//		p_buf->ack = ack;
//		p_buf->time = osKernelSysTick();
//
//		if (osMessagePut(p_ctl->ctrl_cmd.cntrl_ack_msg_que_id, (uint32_t)p_buf, 0) != osOK){
//			p_ctl->error |= povorot_err_soft;
//			osPoolFree(p_ctl->ctrl_cmd.cntrl_ack_msg_pool_id,p_buf);
//		}
//		osSignalSet(p_ctl->ctrl_cmd.cmd_thread_id,p_ctl->ctrl_cmd.cntrl_ack_msg_event);
//	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void povorot_send_data(povorot_struct *p_ctl){
////	p_ctl->data[7] = 0;
////	for(int i=0;i<7;i++){
////		p_ctl->data[7] ^= p_ctl->data[i];
////	}
////	p_ctl->p_uart->WriteData(p_ctl->data,8);
////	return;
//
//	p_ctl->data[6] = 0;
//	for(int i=1;i<6;i++){
////		p_ctl->data[7] ^= p_ctl->data[i];
//		p_ctl->data[6] += p_ctl->data[i];
//	}
//	p_ctl->p_uart->WriteData(p_ctl->data,7);
//	return;
//
//
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_make_cmd_get_rotate_angle(povorot_struct *p_ctl){
//uint8_t tmp[8];
//	p_ctl->cmd = cmd;
//	tmp[0] = 0xA0;
//	tmp[1] = p_ctl->addr;
//	tmp[2] = 0x00;
//	tmp[3] = cmd;//02
//	tmp[4] = p_ctl->rotate_speed;
//	tmp[5] = p_ctl->tilt_speed;
//	tmp[6] = 0xAF;
//	tmp[7] = 0;
//	for(int i=0;i<7;i++){
//		tmp[7] ^= tmp[i];
//	}
//	p_ctl->p_uart->WriteData(tmp,8);
//	return;
	uint8_t tmp[10];
		tmp[0] = 0xFF;
		tmp[1] = p_ctl->addr;
		tmp[2] = 0x00;
		tmp[3] = 0x51;//02
		tmp[4] = 0;
		tmp[5] = 0;
		tmp[6] = 0;
		for(int i=1;i<6;i++){
			tmp[6] += tmp[i];
		}
#ifdef FACK_485_POVOROT
	tmp[7] = 0x00;
	tmp[8] = 0x00;
	p_ctl->p_uart->WriteData(tmp,8);
#else
	p_ctl->p_uart->WriteData(tmp,7);
#endif
//		p_ctl->p_uart->WriteData(tmp,7);
		return;

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_make_cmd_get_tilt_angle(povorot_struct *p_ctl){
//uint8_t tmp[8];
//	p_ctl->cmd = cmd;
//	tmp[0] = 0xA0;
//	tmp[1] = p_ctl->addr;
//	tmp[2] = 0x00;
//	tmp[3] = cmd;//02
//	tmp[4] = p_ctl->rotate_speed;
//	tmp[5] = p_ctl->tilt_speed;
//	tmp[6] = 0xAF;
//	tmp[7] = 0;
//	for(int i=0;i<7;i++){
//		tmp[7] ^= tmp[i];
//	}
//	p_ctl->p_uart->WriteData(tmp,8);
//	return;
	uint8_t tmp[10];
		tmp[0] = 0xFF;
		tmp[1] = p_ctl->addr;
		tmp[2] = 0x00;
		tmp[3] = 0x53;//02
		tmp[4] = 0;
		tmp[5] = 0;
		tmp[6] = 0;
		for(int i=1;i<6;i++){
			tmp[6] += tmp[i];
		}
#ifdef FACK_485_POVOROT
	tmp[7] = 0x00;
	tmp[8] = 0x00;
	p_ctl->p_uart->WriteData(tmp,8);
#else
	p_ctl->p_uart->WriteData(tmp,7);
#endif
//		p_ctl->p_uart->WriteData(tmp,7);
		return;

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_make_cmd(povorot_struct *p_ctl, uint8_t cmd){
//uint8_t tmp[8];
//	p_ctl->cmd = cmd;
//	tmp[0] = 0xA0;
//	tmp[1] = p_ctl->addr;
//	tmp[2] = 0x00;
//	tmp[3] = cmd;//02
//	tmp[4] = p_ctl->rotate_speed;
//	tmp[5] = p_ctl->tilt_speed;
//	tmp[6] = 0xAF;
//	tmp[7] = 0;
//	for(int i=0;i<7;i++){
//		tmp[7] ^= tmp[i];
//	}
//	p_ctl->p_uart->WriteData(tmp,8);
//	return;
	uint8_t tmp[10];
		p_ctl->cmd = cmd;
		tmp[0] = 0xFF;
		tmp[1] = p_ctl->addr;
		tmp[2] = 0x00;
		tmp[3] = cmd;//02
		tmp[4] = p_ctl->rotate_speed;
		tmp[5] = p_ctl->tilt_speed;
		tmp[6] = 0;
		for(int i=1;i<6;i++){
			tmp[6] += tmp[i];
		}
#ifdef FACK_485_POVOROT
	tmp[7] = 0xff;
	p_ctl->p_uart->WriteData(tmp,8);
#else
	p_ctl->p_uart->WriteData(tmp,7);
#endif
//		p_ctl->p_uart->WriteData(tmp,7);
		return;

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_make_cmd_preset(povorot_struct *p_ctl){
uint8_t tmp[10];
	tmp[0] = 0xA0;
	tmp[1] = p_ctl->addr;
	tmp[2] = 0x00;
	tmp[3] = 0x07;//02
	tmp[4] = 0x00;
	tmp[5] = p_ctl->preset_cmd;
	tmp[6] = 0xAF;
	tmp[7] = 0;
	for(int i=0;i<7;i++){
		tmp[7] ^= tmp[i];
	}
#ifdef FACK_485_POVOROT
	tmp[8] = 0xff;
	p_ctl->p_uart->WriteData(tmp,9);
#else
	p_ctl->p_uart->WriteData(tmp,8);
#endif
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_make_cmd_set_rotate_angle(povorot_struct *p_ctl, float angle){
uint8_t tmp[9];
float tmp_f;
uint16_t tmp_16;

//	printf("cmd an=%f\r\n",angle);

	p_ctl->recv_ack_old = p_ctl->recv_ack;

	p_ctl->cmd_rotate_angle = angle;

	angle = angle - p_ctl->correct_angle;
	if (angle < 0.0)  angle = 360.0 + angle;

	angle = fmod(angle,360.0);

//	printf("rot an=%f\r\n",angle);

	if(angle > 350.0) angle = 350.0;

	tmp_f = angle * 100.0;
	tmp_16 = (int)(tmp_f);
	tmp[0] = 0xFF;
	tmp[1] = p_ctl->addr;
	tmp[2] = 0x00;
	tmp[3] = 0x4B;//02
	tmp[4] = tmp_16>>8;//(int)(p_ctl->cmd_rotate_angle);
	tmp[5] = tmp_16;
	tmp[6] = 0;
	for(int i=1;i<6;i++){
		tmp[6] += tmp[i];
	}
#ifdef FACK_485_POVOROT
	tmp[7] = 0xff;
	p_ctl->p_uart->WriteData(tmp,8);
#else
	p_ctl->p_uart->WriteData(tmp,7);
#endif
//	p_ctl->p_uart->WriteData(tmp,7);
	osDelay(20);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_make_cmd_set_tilt_angle(povorot_struct *p_ctl, float angle){
uint8_t tmp[9];
float t;
float tmp_f;
uint16_t tmp_16;

	p_ctl->cmd_tilt_angle = angle;
	tmp_f = p_ctl->cmd_tilt_angle * 100.0;
	tmp_16 = (int)(tmp_f);
	tmp[0] = 0xFF;
	tmp[1] = p_ctl->addr;
	tmp[2] = 0x00;
	tmp[3] = 0x4D;//02
	tmp[4] = tmp_16>>8;//(int)(p_ctl->cmd_rotate_angle);
//	t = p_ctl->cmd_rotate_angle - tmp[4];
//	t = t * 100;
	//tmp[5] = (int)(t);
	tmp[5] = tmp_16;
//	tmp[4] = (int)(p_ctl->cmd_tilt_angle);
//	t = p_ctl->cmd_tilt_angle - tmp[4];
//	t = t * 100;
//	tmp[5] = (int)(t);
	tmp[6] = 0;
	for(int i=1;i<6;i++){
		tmp[6] += tmp[i];
	}
#ifdef FACK_485_POVOROT
	tmp[7] = 0xff;
	p_ctl->p_uart->WriteData(tmp,8);
#else
	p_ctl->p_uart->WriteData(tmp,7);
#endif
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_get_rotate_state(uint32_t adf32, uint32_t cmd, uint32_t tmp){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	memset(p_ctl->ack_dbg_tmp,0,sizeof(p_ctl->ack_dbg_tmp));
	sprintf((char*)p_ctl->ack_dbg_tmp,"%.1f  %.1f ",p_ctl->rotate_angle,p_ctl->tilt_angle);
	if(p_ctl->work_mode==povorot_work_mode_scan){
		if(p_ctl->scan_direct == povorot_scan_direct_up)
			sprintf((char*)&p_ctl->ack_dbg_tmp[strlen(p_ctl->ack_dbg_tmp)],"up");
		else
			sprintf((char*)&p_ctl->ack_dbg_tmp[strlen(p_ctl->ack_dbg_tmp)],"down");

	}
	return &p_ctl->ack_dbg_tmp[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_get_send_data(uint32_t adf32, uint32_t cmd, uint32_t tmp){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return &p_ctl->data[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_get_rotate_angle(uint32_t adf32, uint32_t cmd, uint32_t tmp){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->rotate_angle;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_get_tilt_angle(uint32_t adf32, uint32_t cmd, uint32_t tmp){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->tilt_angle;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_cmd_dbg_get_base_angle(uint32_t adf32, uint32_t cmd, uint32_t tmp){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->rotate_own_angle;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  povorot_dbg_set_cmd_rotate_angle(uint32_t adf32, uint32_t cmd, uint32_t tmp, uint8_t* p_data){
povorot_struct *p_ctl;
float t;
	p_ctl = (povorot_struct *)adf32;
	memcpy(&t,p_data,4);
	povorot_make_cmd_set_rotate_angle(p_ctl,t);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_get_cmd_rotate_angle(uint32_t adf32, uint32_t cmd, uint32_t tmp, uint8_t* p_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->cmd_rotate_angle;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_get_cmd_tilt_angle(uint32_t adf32, uint32_t cmd, uint32_t tmp, uint8_t* p_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->cmd_tilt_angle;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  povorot_dbg_set_cmd_tilt_angle(uint32_t adf32, uint32_t cmd, uint32_t tmp, uint8_t* p_data){
povorot_struct *p_ctl;
float t;
	p_ctl = (povorot_struct *)adf32;
	memcpy(&t,p_data,4);
	povorot_make_cmd_set_tilt_angle(p_ctl,t);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void povorot_dbg_set_send_data(uint32_t adf32, uint32_t cmd, uint32_t tmp, uint8_t* p8_data){
//povorot_struct *p_adf;
//	p_adf = (povorot_struct *)adf32;
//	memcpy(p_adf->data,p8_data,7);
//	povorot_send_data(p_adf);
//	return;
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_get_active_cmd(uint32_t adf32, uint32_t cmd, uint32_t tmp){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return &p_ctl->cmd;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_dbg_set_def_cmd(uint32_t adf32, uint32_t cmd, uint32_t tmp, uint8_t* p8_data){
povorot_struct *p_adf;
	p_adf = (povorot_struct *)adf32;
	povorot_make_cmd(p_adf,cmd);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_dbg_write_tilt_speed(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	p_ctl->tilt_speed = *p8_data;
	povorot_make_cmd(p_ctl,p_ctl->cmd);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_dbg_write_rotate_speed(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	p_ctl->rotate_speed = *p8_data;
	povorot_make_cmd(p_ctl,p_ctl->cmd);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_dbg_set_preset_cmd(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	p_ctl->preset_cmd = *p8_data;
	povorot_make_cmd_preset(p_ctl);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_get_preset_cmd(uint32_t adf32, uint32_t tmp0, uint32_t tmp){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return &p_ctl->preset_cmd;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_read_rotate_speed(uint32_t adf32, uint32_t tmp0, uint32_t tmp){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return &p_ctl->rotate_speed;

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_read_tilt_speed(uint32_t adf32, uint32_t tmp0, uint32_t tmp){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return &p_ctl->tilt_speed;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_get_cmd_start_scan_angle(uint32_t adf32, uint32_t tmp0, uint32_t tmp){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->scan_down_angle;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_get_cmd_stop_scan_angle(uint32_t adf32, uint32_t tmp0, uint32_t tmp){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->scan_up_angle;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_get_cmd_num_scan_point(uint32_t adf32, uint32_t tmp0, uint32_t tmp){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->scan_num_step;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_dbg_get_cmd_num_scan_point_delay(uint32_t adf32, uint32_t tmp0, uint32_t tmp){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->scan_step_time_max;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_dbg_set_cmd_start_scan_angle(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	povorot_make_ctrl_cmd(p_ctl,povorot_cntrl_set_angle_start_scan,4,p8_data,dev_cntrl_ack_not_response);
return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_dbg_set_cmd_stop_scan_angle(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	povorot_make_ctrl_cmd(p_ctl,povorot_cntrl_set_angle_stop_scan,4,p8_data,dev_cntrl_ack_not_response);
return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_dbg_set_cmd_num_scan_point(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	povorot_make_ctrl_cmd(p_ctl,povorot_cntrl_set_num_step_scan,1,p8_data,dev_cntrl_ack_not_response);
return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_dbg_set_cmd_num_scan_point_delay(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	povorot_make_ctrl_cmd(p_ctl,povorot_cntrl_set_time_step_scan,1,p8_data,dev_cntrl_ack_not_response);
return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t povorot_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, povorot_struct *p_ad, const char *p_name){
	SettingCell_t 	cell;
	uint32_t 		i;
	//char tmp[32];
		i=0;
		for(i=0;i<COMPONENT_MAX_TABLE_SIZE;i++){
			if(!p_cell[i].CellNumber) break;
		}

		if( i>= COMPONENT_MAX_TABLE_SIZE) return 0;

		cell.CellAttr = Action_Att + RD_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)povorot_dbg_read_tilt_speed;
		cell.WriteProc = (void*)(void*)povorot_dbg_write_tilt_speed;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "tilt speed";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + RD_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)povorot_dbg_read_rotate_speed;
		cell.WriteProc = (void*)(void*)povorot_dbg_write_rotate_speed;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "rotate speed";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + RD_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_active_cmd;
		cell.WriteProc = (void*)(void*)povorot_dbg_set_def_cmd;
		cell.HighLim = povorot_cmd_off;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = povorot_cmd_off;
		cell.DescriptStr = "cmd st";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + RD_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_active_cmd;
		cell.WriteProc = (void*)(void*)povorot_dbg_set_def_cmd;
		cell.HighLim = povorot_cmd_rotate_right;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = povorot_cmd_rotate_right;
		cell.DescriptStr = "cmd right";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + RD_Att + WR_Att +Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_active_cmd;
		cell.WriteProc = (void*)(void*)povorot_dbg_set_def_cmd;
		cell.HighLim = povorot_cmd_rotate_left;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = povorot_cmd_rotate_left;
		cell.DescriptStr = "cmd left";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + RD_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_active_cmd;
		cell.WriteProc = (void*)(void*)povorot_dbg_set_def_cmd;
		cell.HighLim = povorot_cmd_tilt_up;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = povorot_cmd_tilt_up;
		cell.DescriptStr = "cmd up";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + RD_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_active_cmd;
		cell.WriteProc = (void*)(void*)povorot_dbg_set_def_cmd;
		cell.HighLim = povorot_cmd_tilt_down;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = povorot_cmd_tilt_down;
		cell.DescriptStr = "cmd down";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + RD_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_preset_cmd;
		cell.WriteProc = (void*)(void*)povorot_dbg_set_preset_cmd;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "cmd preset";
		p_cell[i++] = cell;

//		cell.CellAttr = Action_Att + RD_Att + WR_Att;
//		cell.CellType = FixByteArrayCellType + 7;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)povorot_dbg_get_send_data;
//		cell.WriteProc = (void*)(void*)povorot_dbg_set_send_data;
//		cell.HighLim = 0;
//		cell.DefaultValue = (uint32_t)p_ad;
//		cell.CellNumber = num++;
//		cell.LowLim = 0;
//		cell.DescriptStr = "cmd raw data";
//		p_cell[i++] = cell;

		cell.CellAttr = RD_Att;
		cell.CellType = Float4Cell;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_rotate_angle;
		cell.WriteProc = NULL;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "get angle";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att;
		cell.CellType = Float4Cell;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_tilt_angle;
		cell.WriteProc = NULL;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "get tilt angle";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = Float4Cell;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_cmd_rotate_angle;
		cell.WriteProc = (void*)(void*)povorot_dbg_set_cmd_rotate_angle;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "cmd rot. angle";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = Float4Cell;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_cmd_tilt_angle;
		cell.WriteProc = (void*)(void*)povorot_dbg_set_cmd_tilt_angle;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "cmd tilt angle";
		p_cell[i++] = cell;

		cell.CellAttr = Default_Attr + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = NULL;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)povorot_make_cmd_get_rotate_angle;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "req rotate angle";
		p_cell[i++] = cell;

		cell.CellAttr = Default_Attr + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = NULL;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)povorot_make_cmd_get_tilt_angle;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "req tilt angle";
		p_cell[i++] = cell;

		cell.CellAttr = Default_Attr + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = NULL;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)povorot_send_cmd_start_scan;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "cmd scan";
		p_cell[i++] = cell;

		cell.CellAttr = Default_Attr + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = NULL;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)povorot_send_cmd_stop;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "cmd stop";
		p_cell[i++] = cell;


		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = Float4Cell;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_cmd_start_scan_angle;
		cell.WriteProc = (void*)(void*)povorot_dbg_set_cmd_start_scan_angle;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "start scan angle";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = Float4Cell;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_cmd_stop_scan_angle;
		cell.WriteProc = (void*)(void*)povorot_dbg_set_cmd_stop_scan_angle;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "stop scan angle";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_cmd_num_scan_point;
		cell.WriteProc = (void*)(void*)povorot_dbg_set_cmd_num_scan_point;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "num scan point";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)povorot_dbg_get_cmd_num_scan_point_delay;
		cell.WriteProc = (void*)(void*)povorot_dbg_set_cmd_num_scan_point_delay;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ad;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "scan point time";
		p_cell[i++] = cell;

		return num;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t povorot_cmd_ack_decoder(povorot_struct *p_ctl){
//	printf("get ack pov %x \r\n",p_ctl->data_buf[2]);
float a;
	switch(p_ctl->data_buf[2]){
		case 0x59:

			p_ctl->recv_ack++;

			p_ctl->rotate_angle = (p_ctl->data_buf[3]<<8) + p_ctl->data_buf[4];
			p_ctl->rotate_angle = p_ctl->rotate_angle / 100;

			p_ctl->rotate_own_angle = p_ctl->rotate_angle;

			a = p_ctl->correct_angle;
			if(a < 0.0){
				a = 360.0 + a;
			}
			a = p_ctl->rotate_angle + a;
			p_ctl->rotate_angle = fmod(a,360.0);
//			p_ctl->rotate_angle = a;

		break;
		case 0x5B:
			p_ctl->tilt_angle = (p_ctl->data_buf[3]<<8) + p_ctl->data_buf[4];
			p_ctl->tilt_angle = p_ctl->tilt_angle / 100.0;
		break;

	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t povorot_cmd_parser_data(povorot_struct *p_pars, uint8_t *p_data, uint8_t *get_len){
uint8_t data;
uint8_t crc;
	while (*get_len){

		*get_len = *get_len - 1;
		data = *p_data++;
//		p_pars->crc ^= data;

		switch(p_pars->receive_state){
//		case UART_CMD_PARSER_DEFAULT_STATE:
//			break;
		// ��������
		case POVOROT_PARSER_DEFAULT_STATE:
			if (data == POVOROT_PARSER_START_PACKET_BYTE){
				p_pars->input_len = 0;
				p_pars->receive_state = 1;
				osTimerStart(p_pars->timer_id,POVOROT_PARSER_RX_TIMEOUT);
			}
			break;
		// ����� ���������
		case 1:
			if (p_pars->input_len < POVOROT_CMD_PARSER_INPUT_DATA_BUF_LEN){
				p_pars->data_buf[p_pars->input_len++] = data;
				if(p_pars->input_len>=6){
					crc = 0;
					for(int i=0;i<5;i++){
						crc += p_pars->data_buf[i];
					}
					if(crc == p_pars->data_buf[5]){
						povorot_cmd_ack_decoder(p_pars);
					}
					else{
//						printf("crc fail \r\n");
					}
					osTimerStop(p_pars->timer_id);
					p_pars->receive_state = POVOROT_PARSER_DEFAULT_STATE;
				}
			}
		break;
		}
	}
	return 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_parser_process(povorot_struct *p_pars){
uint32_t len,num_data,res;

	num_data = p_pars->p_uart->DataAvailable();
	do{
		  if(num_data || p_pars->uart_buf_len ){
			  if(num_data > (POVOROT_CMD_PARSER_UART_BUF_LEN - p_pars->uart_buf_len)) num_data = POVOROT_CMD_PARSER_UART_BUF_LEN - p_pars->uart_buf_len;
			  	 if(num_data)
			  		 p_pars->uart_buf_len += p_pars->p_uart->ReadData(&p_pars->uart_buf[p_pars->uart_buf_cntr],num_data);

			  	 len = p_pars->uart_buf_len;
			  	 res = povorot_cmd_parser_data(p_pars,&p_pars->uart_buf[p_pars->uart_buf_cntr],&p_pars->uart_buf_len);
			  	 if(!p_pars->uart_buf_len){
					p_pars->uart_buf_cntr = 0;
			  	 }
				else{
					p_pars->uart_buf_cntr += len - p_pars->uart_buf_len;
				}
		  }
		  num_data = p_pars->p_uart->DataAvailable();
	}
	while (num_data);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_parser_timer_callback(void *arg){
	povorot_struct *p_parser;
	p_parser = (povorot_struct*)arg;
//	osSignalSet(p_parser->thread_id,p_parser_event_time_out);
	osEventFlagsSet(p_parser->parser_thread_ev, p_parser_event_time_out);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_cmd_stop(povorot_struct *p_ctl, cntrl_dev_sys_msg_que_type_s *p_msg){
dev_cntrl_ack_e ack_res;
	ack_res = dev_cntrl_ack_ok;

//	printf("stop scan\r\n");

	p_ctl->work_mode = povorot_work_mode_stop;

	povorot_make_cmd(p_ctl,povorot_cmd_off);

	if(p_msg->have_ack == dev_cntrl_ack_response){
		povorot_make_cmd_ack(p_ctl,p_msg->cmd, ack_res);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_cmd_start_scan(povorot_struct *p_ctl, cntrl_dev_sys_msg_que_type_s *p_msg){
dev_cntrl_ack_e ack_res;
	ack_res = dev_cntrl_ack_ok;

	p_ctl->work_mode = povorot_work_mode_scan;
//	printf("start scan\r\n");

	if(p_ctl->rotate_angle >= p_ctl->scan_up_angle){
		p_ctl->scan_dest_angle = p_ctl->scan_down_angle;
		p_ctl->scan_direct = povorot_scan_direct_down;
	}
	if(p_ctl->rotate_angle <= p_ctl->scan_down_angle){
		p_ctl->scan_dest_angle = p_ctl->scan_up_angle;
		p_ctl->scan_direct = povorot_scan_direct_up;
	}

	if((p_ctl->rotate_angle > p_ctl->scan_down_angle) && (p_ctl->rotate_angle < p_ctl->scan_up_angle)){
		if(p_ctl->scan_direct == povorot_scan_direct_up){
			p_ctl->scan_dest_angle = p_ctl->scan_up_angle;
		}
		else{
			p_ctl->scan_dest_angle = p_ctl->scan_down_angle;
		}
	}
//add step mode!!!! check!!!!
//	printf("cmd scan to %f\r\n",p_ctl->scan_dest_angle);
	p_ctl->scan_step_time = 0;
	povorot_make_cmd_set_rotate_angle(p_ctl,p_ctl->scan_dest_angle);

	if(p_msg->have_ack == dev_cntrl_ack_response){
		povorot_make_cmd_ack(p_ctl,p_msg->cmd, ack_res);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//static void povorot_cmd_set_mode_scan(povorot_struct *p_ctl, cntrl_dev_sys_msg_que_type_s *p_msg){
//dev_cntrl_ack_e ack_res;
//	ack_res = dev_cntrl_ack_ok;
//	if(p_msg->have_ack == dev_cntrl_ack_response){
//		povorot_make_cmd_ack(p_ctl,p_msg->cmd, ack_res);
//	}
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_cmd_set_num_step_scan(povorot_struct *p_ctl, cntrl_dev_sys_msg_que_type_s *p_msg){
dev_cntrl_ack_e ack_res;
	ack_res = dev_cntrl_ack_ok;

	p_ctl->scan_num_step = p_msg->buf[0];

	control.dev_config.rec.rotate_num_point = p_ctl->scan_num_step;
	dev_config_write(&control.dev_config);


	if(!p_ctl->scan_num_step) p_ctl->scan_num_step = 1;
	if(p_ctl->work_mode == povorot_work_mode_scan){
		povorot_cmd_start_scan(p_ctl, p_msg);
	}
	if(p_msg->have_ack == dev_cntrl_ack_response){
		povorot_make_cmd_ack(p_ctl,p_msg->cmd, ack_res);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_cmd_set_time_step_scan(povorot_struct *p_ctl, cntrl_dev_sys_msg_que_type_s *p_msg){
dev_cntrl_ack_e ack_res;
	ack_res = dev_cntrl_ack_ok;

	p_ctl->scan_step_time_max = p_msg->buf[0];

	control.dev_config.rec.rotate_scan_time = p_ctl->scan_step_time_max;
	dev_config_write(&control.dev_config);

	if(!p_ctl->scan_step_time_max) p_ctl->scan_step_time_max = 1;
	p_ctl->scan_step_time_max = p_ctl->scan_step_time_max * 1000;
	if(p_ctl->work_mode == povorot_work_mode_scan){
		povorot_cmd_start_scan(p_ctl, p_msg);
	}

	if(p_msg->have_ack == dev_cntrl_ack_response){
		povorot_make_cmd_ack(p_ctl,p_msg->cmd, ack_res);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_cmd_set_angle_start_scan(povorot_struct *p_ctl, cntrl_dev_sys_msg_que_type_s *p_msg){
dev_cntrl_ack_e ack_res;
float angle;
	ack_res = dev_cntrl_ack_ok;

	memcpy(&angle,&p_msg->buf[0],sizeof(angle));

	control.dev_config.rec.rotate_start = angle;
	dev_config_write(&control.dev_config);

	if(angle < p_ctl->scan_up_angle){
		p_ctl->scan_down_angle = angle;
		if(p_ctl->work_mode == povorot_work_mode_scan){
			povorot_cmd_start_scan(p_ctl, p_msg);
		}
	}
	if(p_msg->have_ack == dev_cntrl_ack_response){
		povorot_make_cmd_ack(p_ctl,p_msg->cmd, ack_res);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_cmd_set_angle_correct(povorot_struct *p_ctl, cntrl_dev_sys_msg_que_type_s *p_msg){
dev_cntrl_ack_e ack_res;
float angle;
	ack_res = dev_cntrl_ack_ok;

	memcpy(&angle,&p_msg->buf[0],sizeof(angle));

	if(angle > 360.0) angle = 360.0;
	if(angle < -360.0) angle = -360.0;

	angle = angle - p_ctl->rotate_own_angle;

	control.dev_config.rec.rotate_calibrate = angle;
	dev_config_write(&control.dev_config);

	p_ctl->correct_angle = angle;
/*
	float a;

	a = 360.0 - p_ctl->correct_angle;
	a = p_ctl->rotate_angle + a;
	a = fmod(a,360.0);
	p_ctl->rotate_angle = a;
*/
	if(p_msg->have_ack == dev_cntrl_ack_response){
		povorot_make_cmd_ack(p_ctl,p_msg->cmd, ack_res);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_cmd_set_angle_stop_scan(povorot_struct *p_ctl, cntrl_dev_sys_msg_que_type_s *p_msg){
dev_cntrl_ack_e ack_res;
float angle;
	ack_res = dev_cntrl_ack_ok;
	memcpy(&angle,&p_msg->buf[0],sizeof(angle));

	control.dev_config.rec.rotate_stop = angle;
	dev_config_write(&control.dev_config);

	if(angle > p_ctl->scan_down_angle){
		p_ctl->scan_up_angle = angle;
		if(p_ctl->work_mode == povorot_work_mode_scan){
			povorot_cmd_start_scan(p_ctl, p_msg);
		}
	}
	if(p_msg->have_ack == dev_cntrl_ack_response){
		povorot_make_cmd_ack(p_ctl,p_msg->cmd, ack_res);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_process_scan_mode(povorot_struct *p_ctl, uint32_t time){
float angle;
float stop_angle;
uint8_t	b_time_wait=0;
	if(p_ctl->work_mode==povorot_work_mode_scan){
		if(p_ctl->scan_mode == povorot_scan_mode_cont){
			if(p_ctl->scan_dest_angle > p_ctl->rotate_angle){
				angle = p_ctl->scan_dest_angle - p_ctl->rotate_angle;
			}
			else{
				angle = p_ctl->rotate_angle - p_ctl->scan_dest_angle;
			}
			if(angle<=POVOROT_SCAN_STOP_ERROR_ANGLE){
				if(p_ctl->scan_direct == povorot_scan_direct_up){
					p_ctl->scan_direct = povorot_scan_direct_down;
				}
				else{
					p_ctl->scan_direct = povorot_scan_direct_up;
				}
				if(p_ctl->scan_direct == povorot_scan_direct_up){
					p_ctl->scan_dest_angle = p_ctl->scan_up_angle;
				}
				else{
					p_ctl->scan_dest_angle = p_ctl->scan_down_angle;
				}
				osDelay(30);
				povorot_make_cmd_set_rotate_angle(p_ctl,p_ctl->scan_dest_angle);
			}
		}
		if(p_ctl->scan_mode == povorot_scan_mode_step){

//			printf("angle=%f cmd=%f %d \r\n",p_ctl->rotate_angle,p_ctl->scan_dest_angle,p_ctl->recv_ack);

			if(p_ctl->scan_dest_angle > p_ctl->rotate_angle){
				angle = p_ctl->scan_dest_angle - p_ctl->rotate_angle;
			}
			else{
				angle = p_ctl->rotate_angle - p_ctl->scan_dest_angle;
			}
			if(angle <= POVOROT_SCAN_STOP_ERROR_ANGLE){
				b_time_wait = 1;
			}
			else{
				if((p_ctl->recv_ack - p_ctl->recv_ack_old)>=5){
					b_time_wait = 1;
	//				printf("restart long stuff\r\n");
				}
			}

//			if(p_ctl->recv_ack_old != p_ctl->recv_ack){
//				p_ctl->recv_ack_cntr++;
//			}
//			else{
//				p_ctl->recv_ack_cntr = 0;
//			}
//
//			p_ctl->recv_ack_old = p_ctl->recv_ack;
//			uint32_t				recv_ack_cntr;

//			if(angle <= POVOROT_SCAN_STOP_ERROR_ANGLE){
			if(b_time_wait){
				if(!p_ctl->scan_step_time){
					p_ctl->scan_step_time = p_ctl->scan_step_time_max;
				}
				else{
					if(p_ctl->scan_step_time >= time) p_ctl->scan_step_time -= time;
					else p_ctl->scan_step_time = 0;
					if(!p_ctl->scan_step_time){
						if(p_ctl->scan_direct == povorot_scan_direct_up){
							stop_angle = p_ctl->scan_up_angle;
						}
						else{
							stop_angle = p_ctl->scan_down_angle;
						}
						if(p_ctl->scan_dest_angle > stop_angle){
							angle = p_ctl->scan_dest_angle - stop_angle;
						}
						else{
							angle = stop_angle - p_ctl->scan_dest_angle;
						}
						if(angle<=POVOROT_SCAN_STOP_ERROR_ANGLE){ //change direction
							if(p_ctl->scan_direct == povorot_scan_direct_up){
								p_ctl->scan_direct = povorot_scan_direct_down;
								p_ctl->scan_dest_angle = p_ctl->scan_up_angle;
							}
							else{
								p_ctl->scan_direct = povorot_scan_direct_up;
								p_ctl->scan_dest_angle = p_ctl->scan_down_angle;
							}
						}
						angle = (p_ctl->scan_up_angle - p_ctl->scan_down_angle)/p_ctl->scan_num_step;
						if(p_ctl->scan_direct == povorot_scan_direct_down){
							angle = -angle;
						}
						p_ctl->scan_dest_angle += angle;
						osDelay(20);
						povorot_make_cmd_set_rotate_angle(p_ctl,p_ctl->scan_dest_angle);
						p_ctl->scan_cmd_error_time = 0;
//						else{
//
//						}

					}
				}
			}
			else{
				p_ctl->scan_cmd_error_time += time;
				if(p_ctl->scan_cmd_error_time>=20000){
					osDelay(20);
					povorot_make_cmd_set_rotate_angle(p_ctl,p_ctl->scan_dest_angle);
					p_ctl->scan_cmd_error_time = 0;
//					printf("alarm restart\r\n");
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////	povorot_cntrl_cmd_speed_scan  =					 			3,
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_cmd_parser(povorot_struct *p_ctl, cntrl_dev_sys_msg_que_type_s *p_msg){
	float tmp1;
	switch(p_msg->cmd){
	case povorot_cntrl_cmd_stop:
		povorot_cmd_stop(p_ctl,p_msg);
	break;
	case povorot_cntrl_cmd_direction:
		control.freq_mode = cntrl_freq_mode_manual;
		p_ctl->cmd_direct = (povorot_cmd_direction)p_msg->buf[0];
		p_ctl->work_mode = povorot_work_mode_stop;
		povorot_make_cmd(p_ctl,povorot_cmd_off);
		osDelay(100);

	break;
	case povorot_cntrl_cmd_exec_direction:
		p_ctl->work_mode = povorot_work_mode_cmd;
		control.freq_mode = cntrl_freq_mode_manual;
		povorot_make_cmd(p_ctl,p_ctl->cmd_direct);
	break;
	case povorot_cntrl_cmd_start_scan:
		povorot_cmd_start_scan(p_ctl,p_msg);
	break;
//	case povorot_cntrl_set_mode_scan:
//		povorot_cmd_set_mode_scan(p_ctl,p_msg);
//	break;
	case povorot_cntrl_set_num_step_scan:
		povorot_cmd_set_num_step_scan(p_ctl,p_msg);
	break;
	case povorot_cntrl_set_time_step_scan:
		povorot_cmd_set_time_step_scan(p_ctl,p_msg);
	break;
	case povorot_cntrl_set_angle_start_scan:
		povorot_cmd_set_angle_start_scan(p_ctl,p_msg);
	break;
	case povorot_cntrl_set_angle_stop_scan:
		povorot_cmd_set_angle_stop_scan(p_ctl,p_msg);
	break;
	case povorot_cntrl_cmd_angle_pos:
		p_ctl->work_mode = povorot_work_mode_cmd;
		control.freq_mode = cntrl_freq_mode_manual;
		memcpy(&tmp1,&p_msg->buf[0],sizeof(float));
		povorot_make_cmd_set_rotate_angle(p_ctl,tmp1);
	break;
	case povorot_cntrl_cmd_tilt_pos:
		p_ctl->work_mode = povorot_work_mode_cmd;
		control.freq_mode = cntrl_freq_mode_manual;
		memcpy(&tmp1,&p_msg->buf[0],sizeof(float));
		povorot_make_cmd_set_tilt_angle(p_ctl,tmp1);
	break;

	case povorot_cntrl_cmd_step_left:
		p_ctl->work_mode = povorot_work_mode_stop;
		povorot_make_cmd(p_ctl,povorot_cmd_rotate_left);
		osDelay(100);
		povorot_make_cmd(p_ctl,povorot_cmd_off);
	break;
	case povorot_cntrl_cmd_step_right:
		p_ctl->work_mode = povorot_work_mode_stop;
		povorot_make_cmd(p_ctl,povorot_cmd_rotate_right);
		osDelay(100);
		povorot_make_cmd(p_ctl,povorot_cmd_off);
	break;
	case povorot_cntrl_cmd_step_up:
		p_ctl->work_mode = povorot_work_mode_stop;
		povorot_make_cmd(p_ctl,povorot_cmd_tilt_up);
		osDelay(100);
		povorot_make_cmd(p_ctl,povorot_cmd_off);
	break;
	case povorot_cntrl_cmd_step_down:
		p_ctl->work_mode = povorot_work_mode_stop;
		povorot_make_cmd(p_ctl,povorot_cmd_tilt_down);
		osDelay(100);
		povorot_make_cmd(p_ctl,povorot_cmd_off);
	break;
	case povorot_cntrl_set_angle_correct:
		povorot_cmd_set_angle_correct(p_ctl,p_msg);
	break;

//	case povorot_cntrl_cmd_start_scan:
//		povorot_cmd_start_scan(p_ctl,p_msg);
//	break;
	}

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_thread(void *argument) {
povorot_struct *p_ctl;
uint32_t rotate_time_cntr;
uint32_t tilt_time_cntr;
uint32_t flag;
cntrl_dev_sys_msg_que_type_s dev_sys_msg;
osStatus_t status;
#ifdef TEST_MEM_USAGE
	test_usage_add_name("Povorot");
#endif
	p_ctl = (povorot_struct*)argument;

//	p_ctl->os_timer_def.ptimer = povorot_parser_timer_callback;//&p_parser->os_timer_cb_timer_name;
//	p_ctl->os_timer_def.timer = p_ctl->os_timer;
//	p_ctl->timer_id = osRtxTimerObject(object) (&p_ctl->os_timer_def, osTimerOnce, p_ctl);
//	if (p_ctl->timer_id == NULL) {	// Periodic timer created fail
////		printf("error parser init \r\n");
//	}

	p_ctl->timer_id = osTimerNew(povorot_parser_timer_callback, osTimerOnce, p_ctl, NULL);// (&p_pars->os_timer_def, osTimerOnce, p_pars);

	p_ctl->receive_state = POVOROT_PARSER_DEFAULT_STATE;

	rotate_time_cntr = 0;
	tilt_time_cntr = 0;

	p_ctl->scan_mode = povorot_scan_mode_step;//povorot_scan_mode_cont;//povorot_scan_mode_step;//povorot_scan_mode_cont;
	p_ctl->work_mode = povorot_work_mode_stop;


	p_ctl->scan_dest_angle = 10.0;
	p_ctl->correct_angle = control.dev_config.rec.rotate_calibrate;
	p_ctl->scan_up_angle = control.dev_config.rec.rotate_stop;
	p_ctl->scan_down_angle = control.dev_config.rec.rotate_start;
	p_ctl->scan_step_time_max = control.dev_config.rec.rotate_scan_time;
	p_ctl->scan_step_time_max = p_ctl->scan_step_time_max * 1000;
	p_ctl->scan_num_step = control.dev_config.rec.rotate_num_point;

	float a;

	a = 360.0 - p_ctl->correct_angle;
	a = p_ctl->rotate_angle + a;
	a = fmod(a,360.0);
	p_ctl->rotate_angle = a;


		while (1) {
		   flag = osEventFlagsWait(p_ctl->parser_thread_ev, 0x7FFFFFFF, osFlagsWaitAny, 1000);
		   if (flag == (uint32_t)osErrorTimeout) {
				  rotate_time_cntr += 1000;
				  tilt_time_cntr += 1000;
				  if(rotate_time_cntr>=POVOROT_AZIMUT_REQ_TIME){
					  rotate_time_cntr = 0;
					  osDelay(20);
					  povorot_make_cmd_get_rotate_angle(p_ctl);
					  osDelay(20);

				  }
				  if(tilt_time_cntr>=POVOROT_TILT_REQ_TIME){
					  tilt_time_cntr = 0;
					  osDelay(20);
					  povorot_make_cmd_get_tilt_angle(p_ctl);
					  osDelay(20);
				  }

				  povorot_process_scan_mode(p_ctl,1000);
		   }
		   else{
			  if (flag & p_parser_event_get_data){
				  povorot_parser_process(p_ctl);
			  }
			  if (flag & u_parser_event_time_out){
				  p_ctl->receive_state = POVOROT_PARSER_DEFAULT_STATE;
				  osTimerStop(p_ctl->timer_id);
			  }
			  if (flag & p_parser_event_get_cmd){
				do{
					 status = osMessageQueueGet(p_ctl->ctrl_cmd.msg_que_id, &dev_sys_msg, NULL, 0);
					if (status == osOK){
						povorot_cmd_parser(p_ctl,&dev_sys_msg);
					}
				}
				while(status == osOK);
			  }
		   }
		}
//
//	while (1) {
//		  ev = osSignalWait(0,1000);//������ ��� ��������� ��� ����� ������!!!!
//// TODO timer for timeout input data stream !!!!
//		  if (ev.status == osEventSignal) {
//			  if (ev.value.signals & p_parser_event_get_data){
//				  povorot_parser_process(p_ctl);
//			  }
//			  if (ev.value.signals & p_parser_event_time_out){
//				  p_ctl->receive_state = POVOROT_PARSER_DEFAULT_STATE;
//				  osTimerStop(p_ctl->timer_id);
//			  }
//			  if (ev.value.signals & p_parser_event_get_cmd){
//
//					 do{
//						msg_ev = osMessageGet(p_ctl->ctrl_cmd.cntrl_msg_que_id,0);
//						if (msg_ev.status == osEventMessage){
//
//							p_dev_sys_msg = (cntrl_dev_sys_msg_que_type_s*) msg_ev.value.v;
//
//							povorot_cmd_parser(p_ctl,p_dev_sys_msg);
//
//						}
//					}
//					while(msg_ev.status == osEventMessage);
//			  }
//
//////				  uart_cmd_parser_process(p_parser);
////			  }
////			  if (ev.value.signals & u_parser_event_time_out){
//////				  p_parser->receive_state = UART_CMD_PARSER_DEFAULT_STATE;
//////				  osTimerStop(p_parser->timer_id);
////			  }
//		  }
//		  else{
//			  rotate_time_cntr += 1000;
//			  tilt_time_cntr += 1000;
//			  if(rotate_time_cntr>=POVOROT_AZIMUT_REQ_TIME){
//				  rotate_time_cntr = 0;
//				  osDelay(20);
//				  povorot_make_cmd_get_rotate_angle(p_ctl);
//				  osDelay(20);
//
//			  }
//			  if(tilt_time_cntr>=POVOROT_TILT_REQ_TIME){
//				  tilt_time_cntr = 0;
//				  osDelay(20);
//				  povorot_make_cmd_get_tilt_angle(p_ctl);
//				  osDelay(20);
//			  }
//
//			  povorot_process_scan_mode(p_ctl,1000);
//		  }
//	}
}
