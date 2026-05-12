/*
 * main_cntrl.c
 *
 *  Created on: 3 нояб. 2023 г.
 *      Author: Petr
 */


#include "string.h"
#include "control.h"


#define MAIN_CNTRL_MSG_COUNT 24
#define MAIN_CNTRL_MSG_SIZE  sizeof(cntrl_dev_sys_msg_que_type_s) // Теперь размер — это размер структуры


#define MAIN_CNTRL_FULL_BLOCK_SIZE (((MAIN_CNTRL_MSG_SIZE + 3U) & ~3UL) + 12U)
#pragma location=".ddr_data"
static uint32_t main_cntrl_myQueue_mem[(MAIN_CNTRL_MSG_COUNT * MAIN_CNTRL_FULL_BLOCK_SIZE) / 4];
#pragma location=".ddr_data"
static uint32_t main_cntrl_myQueue_cb[osRtxMessageQueueCbSize / 4];


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main_cntrl_init_que(control_struct *p_ctl){
const osMessageQueueAttr_t queue_attr = {
	.name = NULL,
	.cb_mem = main_cntrl_myQueue_cb,
	.cb_size = sizeof(main_cntrl_myQueue_cb),
	.mq_mem = main_cntrl_myQueue_mem,
	.mq_size = sizeof(main_cntrl_myQueue_mem)
};

	p_ctl->main_ctrl_cmd.msg_que_id = osMessageQueueNew(MAIN_CNTRL_MSG_COUNT, MAIN_CNTRL_MSG_SIZE, &queue_attr);
	p_ctl->main_ctrl_cmd.ack_msg_que_id = NULL;

	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint32_t main_cntrl_make_ctrl_cmd(control_struct *p_ctl,uint8_t cmd, uint8_t len, uint8_t *p_data, dev_cntrl_ack_response_e have_ack){
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

//	ack_msg_on_send_cmd (p_ctl->p_ack, p_buf, p_ctl->id);

	if (osMessageQueuePut(p_ctl->main_ctrl_cmd.msg_que_id,p_buf,0, 1000) != osOK){
//		p_ctl->error |= povorot_err_soft;
//		ack_msg_on_clear_send_cmd(p_ctl->p_ack, p_buf, p_ctl->id);
		res = 2;
	}
	else{
		osEventFlagsSet(p_ctl->main_ctrl_thread_ev,cntrl_thread_get_cmd);
	}
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef NOT_VIDEO_RECEIVER
void cntrl_process_auto_detect_master(control_struct *p_ctl){
uint8_t back;
	if(p_ctl->auto_mode == cntrl_auto_mode_en){
		back = TVP_Read(&p_ctl->tvp_5150_master, 0x88);
//		if(back & 0x20){//50 hz
//			if(p_ctl->header.height == 200){
//				cntrl_make_new_height(p_ctl,240);
//			}
//		}
//		else{			//60 hz
//			if(p_ctl->header.height == 240){
//				cntrl_make_new_height(p_ctl,200);
//			}
//		}
		back &= 0x06;
		if(back == 0x06){
			back = TVP_Read(&p_ctl->tvp_5150_master, 0x88);
			back &= 0x06;
			if(back == 0x06){
//				printf("Alarm!!!!! Detect!!! \r\n");

				povorot_cmd_send_cmd_stop(&p_ctl->povorot);

				if(p_ctl->freq_mode == cntrl_freq_mode_auto){
					p_ctl->freq_mode = cntrl_freq_mode_manual;
				}
				p_ctl->auto_mode = cntrl_auto_mode_dis;
//				cntrl_stop_auto_detect(p_ctl);
				p_ctl->alarm_mode = cntrl_alarm_en;
			}
		}
	}
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RECEIVER_BASE_MODE
#ifndef		NO_SEC_CH
void cntrl_process_auto_detect_slave(control_struct *p_ctl){
uint8_t back;
	if((p_ctl->recv_mode == cntr_recv_mode_1_2) || (p_ctl->recv_mode == cntr_recv_mode_5_8)){
		return;
	}
	if(p_ctl->auto_mode == cntrl_auto_mode_en){
		back = TVP_Read(&p_ctl->tvp_5150_slave, 0x88);
		back &= 0x06;
		if(back == 0x06){
			back = TVP_Read(&p_ctl->tvp_5150_slave, 0x88);
			back &= 0x06;
			if(back == 0x06){
//				printf("Alarm!!!!! Detect!!! slave \r\n");

				switch(p_ctl->recv_mode){
				case cntr_recv_mode_1_2_5_8:
					TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);
					TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);
//					p_ctl->recv_active = cntr_recv_mode_5_8_1_2;
					p_ctl->recv_active = cmd_recv_active_5_8;
				break;
				case cntr_recv_mode_5_8_1_2:
					TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);
					TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);
//					p_ctl->recv_active = cntr_recv_mode_1_2_5_8;
					p_ctl->recv_active = cmd_recv_active_1_2;
				break;
				case cntr_recv_mode_1_2_and_5_8:
					if(p_ctl->recv_active == cmd_recv_active_1_2){
						TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);
						TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);
						p_ctl->recv_active = cmd_recv_active_5_8;
					}
					else{
						TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);
						TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);
						p_ctl->recv_active = cmd_recv_active_1_2;
					}
				break;
				}
				povorot_cmd_send_cmd_stop(&p_ctl->povorot);

				if(p_ctl->freq_mode == cntrl_freq_mode_auto){
					p_ctl->freq_mode = cntrl_freq_mode_manual;
				}
				p_ctl->auto_mode = cntrl_auto_mode_dis;
				p_ctl->alarm_mode = cntrl_alarm_en;
			}
		}
	}
}
#endif
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RECEIVER_EXT_MODE
void cntrl_process_auto_detect_slave(control_struct *p_ctl){
uint8_t back;
	if((p_ctl->recv_mode == cntr_recv_mode_1_2) || (p_ctl->recv_mode == cntr_recv_mode_5_8)){
		return;
	}
	if(p_ctl->auto_mode == cntrl_auto_mode_en){
		back = TVP_Read(&p_ctl->tvp_5150_slave, 0x88);
		back &= 0x06;
		if(back == 0x06){
			back = TVP_Read(&p_ctl->tvp_5150_slave, 0x88);
			back &= 0x06;
			if(back == 0x06){
//				printf("Alarm!!!!! Detect!!! slave \r\n");

				switch(p_ctl->recv_mode){
				case cntr_recv_mode_1_2_5_8:
					TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);//+
					TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);//+
//					p_ctl->recv_active = cntr_recv_mode_5_8_1_2;
					p_ctl->recv_active = cmd_recv_active_5_8;
				break;
				case cntr_recv_mode_5_8_1_2:
					TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);//+
					TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);//+
//					p_ctl->recv_active = cntr_recv_mode_1_2_5_8;
					p_ctl->recv_active = cmd_recv_active_1_2;
				break;
				case cntr_recv_mode_1_2_and_5_8:
					if(p_ctl->recv_active == cmd_recv_active_1_2){
						TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);//+
						TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);//+
						p_ctl->recv_active = cmd_recv_active_5_8;
					}
					else{
						TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);//+
						TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);//+
						p_ctl->recv_active = cmd_recv_active_1_2;
					}
				break;
				}
				povorot_cmd_send_cmd_stop(&p_ctl->povorot);

				if(p_ctl->freq_mode == cntrl_freq_mode_auto){
					p_ctl->freq_mode = cntrl_freq_mode_manual;
				}
				p_ctl->auto_mode = cntrl_auto_mode_dis;
				p_ctl->alarm_mode = cntrl_alarm_en;
			}
		}
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void ch_process(control_struct *p_ctl, uint32_t time){
uint8_t res;
	p_ctl->ch_ack_time += time;
	if(p_ctl->ch_ack_time>=1000){
		cntrl_process_auto_detect(p_ctl);
		p_ctl->ch_ack_time = 0;
	}

//	if(p_ctl->ant_mode == cntrl_ant_mode_manual){
//		p_ctl->ant_time = 0;

		if(p_ctl->ch_mode == cntrl_ch_mode_manual){
			p_ctl->ch_time = 0;
		}
		else{
			p_ctl->ch_time += time;
			if(p_ctl->ch_time >= p_ctl->ch_max_time){
				p_ctl->ch_time = 0;
				cntrl_ch_up(p_ctl);
				if(p_ctl->b_collect){
					if(p_ctl->recv_mode==cntr_recv_mode_5_8){
						p_ctl->recv_mode = cntr_recv_mode_1_2;
						TVP_Write(&p_ctl->tvp_5150_master, 0x00,0x00);
					}
					else{
						p_ctl->recv_mode = cntr_recv_mode_5_8;
						TVP_Write(&p_ctl->tvp_5150_master, 0x00,0x02);
					}
				}
			}
		}
}
*/
#ifndef NOT_VIDEO_RECEIVER
#ifdef RECEIVER_BASE_MODE
void receiver_process(control_struct *p_ctl, uint32_t time){
uint8_t res;

		if(p_ctl->freq_mode == cntrl_freq_mode_auto){
			switch(p_ctl->recv_mode){
			case cntr_recv_mode_1_2:
#ifdef RECV_1_2
				recv_1_2_time_process(&p_ctl->recv_1_2,time);
#endif
			break;
			case cntr_recv_mode_5_8:
#ifdef RTC6715
				recv_5_8_time_process(&p_ctl->rtc6715,time);
#endif
#ifdef RECV_2_4
				recv_1_2_time_process(&p_ctl->recv_2_4,time);
#endif

			break;
			case cntr_recv_mode_1_2_5_8:
			case cntr_recv_mode_5_8_1_2:
#ifdef RECV_1_2
				recv_1_2_time_process(&p_ctl->recv_1_2,time);
#endif


#ifdef RTC6715
				recv_5_8_time_process(&p_ctl->rtc6715,time);
#endif
#ifdef RECV_2_4
				recv_1_2_time_process(&p_ctl->recv_2_4,time);
#endif


			break;
			case cntr_recv_mode_1_2_and_5_8:

					if(p_ctl->recv_active == cmd_recv_active_1_2){
#ifdef RECV_1_2
							if(recv_1_2_time_process(&p_ctl->recv_1_2,time)){
								p_ctl->recv_active = cmd_recv_active_5_8;
								TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);
#ifndef NO_SEC_CH
								TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);
#endif
							}
#endif
					}
					else{
#ifdef RTC6715
							if(recv_5_8_time_process(&p_ctl->rtc6715,time)){
#endif
#ifdef RECV_2_4
//							if(recv_1_2_time_process(&p_ctl->recv_2_4,time)){
#endif
//#ifdef					NO_SEC_CH
//							if(0){
//#endif

								p_ctl->recv_active = cmd_recv_active_1_2;
								TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);
#ifndef		NO_SEC_CH
								TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);
#endif
							}
						}

//					}
			break;
			}
		}
}
#endif

#ifdef RECEIVER_EXT_MODE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void receiver_process_ext(control_struct *p_ctl, uint32_t time){
uint8_t res;

		if(p_ctl->freq_mode == cntrl_freq_mode_auto){
			switch(p_ctl->recv_mode){
			case cntr_recv_mode_1_2:

#ifdef	RECV_3_4_I2C
			i2c_recv_time_process(&p_ctl->i2c_recv_3_4,time);
#endif
			case cntr_recv_mode_5_8:
#ifdef	RECV_4_5_I2C
			i2c_recv_time_process(&p_ctl->i2c_recv_4_5,time);
#endif

			break;
			case cntr_recv_mode_1_2_5_8:
			case cntr_recv_mode_5_8_1_2:
#ifdef	RECV_3_4_I2C
			i2c_recv_time_process(&p_ctl->i2c_recv_3_4,time);
#endif
#ifdef	RECV_4_5_I2C
			i2c_recv_time_process(&p_ctl->i2c_recv_4_5,time);
#endif
			break;
			case cntr_recv_mode_1_2_and_5_8:

					if(p_ctl->recv_active == cmd_recv_active_1_2){
#ifdef RECV_3_4_I2C
							if(i2c_recv_time_process(&p_ctl->i2c_recv_3_4,time)){
								p_ctl->recv_active = cmd_recv_active_5_8;
								TVP_Write(&p_ctl->tvp_5150_master,0x00,0x00);//+
								TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x02);//+
							}
#endif
					}
					else{
#ifdef RECV_4_5_I2C
							if(i2c_recv_time_process(&p_ctl->i2c_recv_4_5,time)){
#endif
								p_ctl->recv_active = cmd_recv_active_1_2;
								TVP_Write(&p_ctl->tvp_5150_master,0x00,0x02);//+
								TVP_Write(&p_ctl->tvp_5150_slave,0x00,0x00);//+
							}


					}
			break;
			}
		}
}
#endif

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef NOT_VIDEO_RECEIVER
static uint8_t* main_cntrl_dbg_get_frame_cntr(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	return (uint8_t*)&p_ctl->file_cntr;
}
#endif
#ifdef	RTC6705
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* main_cntrl_dbg_get_emit_status(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
volatile float v_in, v_out,cur;
	p_ctl = (control_struct *)adf32;
	memset(p_ctl->povorot.ack_dbg_tmp,0,sizeof(p_ctl->povorot.ack_dbg_tmp));
	voltage_callback_meashure(&control.vol_24v_in,0);
	voltage_callback_meashure(&control.vol_24v_out,0);
	voltage_callback_meashure(&control.cur_24v,0);

	v_in = control.vol_24v_in.voltage;
	v_out = control.vol_24v_out.voltage;

	if(control.cur_24v.voltage < 2500) control.cur_24v.voltage = 2500;
	control.cur_24v.voltage -= 2500;

	cur = control.cur_24v.voltage;
	cur = cur * 0.01;
//
	v_in = v_in / 1000.0;
	v_out = v_out / 1000.0;
//
//	cur= cur / 1000.0;
//	if(cur < 2.5) cur = 2.5;
//
//	cur = cur - 2.5;
//	cur = cur * 0.1;

	sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"In=%2.1f Out=%2.1f Cur=%1.1f ",v_in,v_out,cur);
	return (uint8_t*)&p_ctl->povorot.ack_dbg_tmp[0];
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RECEIVER_EXT_MODE
static uint8_t* main_cntrl_dbg_get_freq(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;

	memset(p_ctl->povorot.ack_dbg_tmp,0,sizeof(p_ctl->povorot.ack_dbg_tmp));

	switch(p_ctl->recv_mode){
	case cntr_recv_mode_1_2:


		sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)]," %d",p_ctl->i2c_recv_3_4.freq);
		if(p_ctl->i2c_recv_3_4.p_cfg->freq_ch_mode == i2c_recv_ch_mode){
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%s",ch_3_4_table[p_ctl->i2c_recv_3_4.ch_cntr]);

		}

#ifdef RECV_1_2
		sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->recv_1_2.freq);
		if(p_ctl->recv_1_2.p_cfg->freq_ch_mode == recv_1_2_ch_mode){
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%d ",p_ctl->recv_1_2.ch_cntr);
		}
#endif
	break;

	case cntr_recv_mode_5_8:

		sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)]," %d",p_ctl->i2c_recv_4_5.freq);
		if(p_ctl->i2c_recv_4_5.p_cfg->freq_ch_mode == i2c_recv_ch_mode){
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%s",ch_4_5_table[p_ctl->i2c_recv_4_5.ch_cntr]);

		}

#ifdef RTC6715
#ifdef RECV_5_8_RSSI
		if((p_ctl->rtc6715.p_cfg->b_en_fast_scan) && (p_ctl->rtc6715.rssi.state != recv_5_8_rssi_state_stop)){
			if((p_ctl->rtc6715.rssi.state == recv_5_8_rssi_state_continue) || (p_ctl->rtc6715.rssi.state == recv_5_8_rssi_state_start)){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%s","Scan");
			}
			if(p_ctl->rtc6715.rssi.state == recv_5_8_rssi_check_video) {
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->rtc6715.freq);
			}
		}
#else
		if(0){

		}
#endif
		else{
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->rtc6715.freq);
			if(p_ctl->rtc6715.p_cfg->freq_ch_mode == recv_5_8_ch_mode){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%s ",ch_5_8_table[p_ctl->rtc6715.ch_cntr]);
			}
		}
#endif

	break;
	case cntr_recv_mode_1_2_5_8:

	case cntr_recv_mode_5_8_1_2:

	case cntr_recv_mode_1_2_and_5_8:
		if(p_ctl->recv_active == cmd_recv_active_1_2){

			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)]," %d",p_ctl->i2c_recv_3_4.freq);
			if(p_ctl->i2c_recv_3_4.p_cfg->freq_ch_mode == i2c_recv_ch_mode){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%s",ch_3_4_table[p_ctl->i2c_recv_3_4.ch_cntr]);

			}
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)]," %d",p_ctl->i2c_recv_4_5.freq);
			if(p_ctl->i2c_recv_4_5.p_cfg->freq_ch_mode == i2c_recv_ch_mode){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%s",ch_4_5_table[p_ctl->i2c_recv_4_5.ch_cntr]);
			}




			#ifdef RECV_1_2
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->recv_1_2.freq);
			if(p_ctl->recv_1_2.p_cfg->freq_ch_mode == recv_1_2_ch_mode){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%d",p_ctl->recv_1_2.ch_cntr);
			}
#endif
#ifdef RTC6715
#ifdef RECV_5_8_RSSI
			if((p_ctl->rtc6715.p_cfg->b_en_fast_scan) && (p_ctl->rtc6715.rssi.state != recv_5_8_rssi_state_stop)){
				if((p_ctl->rtc6715.rssi.state == recv_5_8_rssi_state_continue) || (p_ctl->rtc6715.rssi.state == recv_5_8_rssi_state_start)){
					sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],"%s","Scan");
				}
				if(p_ctl->rtc6715.rssi.state == recv_5_8_rssi_check_video) {
					sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],"%d",p_ctl->rtc6715.freq);
				}
			}
#else
			if(0){

			}
#endif
			else{
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)]," %d",p_ctl->rtc6715.freq);
				if(p_ctl->rtc6715.p_cfg->freq_ch_mode == recv_5_8_ch_mode){
					sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%s",ch_5_8_table[p_ctl->rtc6715.ch_cntr]);
				}
			}
#endif
		}
		else{

			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)]," %d",p_ctl->i2c_recv_4_5.freq);
			if(p_ctl->i2c_recv_4_5.p_cfg->freq_ch_mode == i2c_recv_ch_mode){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%s",ch_4_5_table[p_ctl->i2c_recv_4_5.ch_cntr]);
			}

			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)]," %d",p_ctl->i2c_recv_3_4.freq);
			if(p_ctl->i2c_recv_3_4.p_cfg->freq_ch_mode == i2c_recv_ch_mode){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%s",ch_3_4_table[p_ctl->i2c_recv_3_4.ch_cntr]);

			}


#ifdef RTC6715
#ifdef RECV_5_8_RSSI
			if((p_ctl->rtc6715.p_cfg->b_en_fast_scan) && (p_ctl->rtc6715.rssi.state != recv_5_8_rssi_state_stop)){
				if((p_ctl->rtc6715.rssi.state == recv_5_8_rssi_state_continue) || (p_ctl->rtc6715.rssi.state == recv_5_8_rssi_state_start)){
					sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%s","Scan");
				}
				if(p_ctl->rtc6715.rssi.state == recv_5_8_rssi_check_video) {
					sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->rtc6715.freq);
				}
			}
#else
			if(0){

			}
#endif
			else{
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->rtc6715.freq);
				if(p_ctl->rtc6715.p_cfg->freq_ch_mode == recv_5_8_ch_mode){
					sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%s",ch_5_8_table[p_ctl->rtc6715.ch_cntr]);
				}
			}
#endif

#ifdef RECV_1_2
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)]," %d",p_ctl->recv_1_2.freq);
			if(p_ctl->recv_1_2.p_cfg->freq_ch_mode == recv_1_2_ch_mode){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%d",p_ctl->recv_1_2.ch_cntr);
			}
#endif

//			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d ( %d )",p_ctl->rtc6715.freq,p_ctl->recv_1_2.freq);
		}
	break;
	}
	return (uint8_t*)&p_ctl->povorot.ack_dbg_tmp[0];
}
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RECEIVER_BASE_MODE
static uint8_t* main_cntrl_dbg_get_freq(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;

	memset(p_ctl->povorot.ack_dbg_tmp,0,sizeof(p_ctl->povorot.ack_dbg_tmp));

	switch(p_ctl->recv_mode){
	case cntr_recv_mode_1_2:
#ifdef RECV_1_2
		sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->recv_1_2.freq);
		if(p_ctl->recv_1_2.p_cfg->freq_ch_mode == recv_1_2_ch_mode){
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%d ",p_ctl->recv_1_2.ch_cntr);
		}
#endif
	break;
#ifdef	RTC6705
	case cntr_recv_mode_emit:
#endif
	case cntr_recv_mode_5_8:
#ifdef RTC6715
#ifdef RECV_5_8_RSSI
		if((p_ctl->rtc6715.p_cfg->b_en_fast_scan) && (p_ctl->rtc6715.rssi.state != recv_5_8_rssi_state_stop)){
			if((p_ctl->rtc6715.rssi.state == recv_5_8_rssi_state_continue) || (p_ctl->rtc6715.rssi.state == recv_5_8_rssi_state_start)){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%s","Scan");
			}
			if(p_ctl->rtc6715.rssi.state == recv_5_8_rssi_check_video) {
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->rtc6715.freq);
			}
		}
#else
		if(0){

		}
#endif
		else{
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->rtc6715.freq);
			if(p_ctl->rtc6715.p_cfg->freq_ch_mode == recv_5_8_ch_mode){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%s ",ch_5_8_table[p_ctl->rtc6715.ch_cntr]);
			}
		}
#endif
#ifdef RECV_2_4
		sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->recv_2_4.freq);
		if(p_ctl->recv_2_4.p_cfg->freq_ch_mode == recv_1_2_ch_mode){
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%d ",p_ctl->recv_2_4.ch_cntr);
		}
#endif

	break;
	case cntr_recv_mode_1_2_5_8:

	case cntr_recv_mode_5_8_1_2:

	case cntr_recv_mode_1_2_and_5_8:
		if(p_ctl->recv_active == cmd_recv_active_1_2){
#ifdef RECV_1_2
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->recv_1_2.freq);
			if(p_ctl->recv_1_2.p_cfg->freq_ch_mode == recv_1_2_ch_mode){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%d",p_ctl->recv_1_2.ch_cntr);
			}
#endif
#ifdef RTC6715
#ifdef RECV_5_8_RSSI
			if((p_ctl->rtc6715.p_cfg->b_en_fast_scan) && (p_ctl->rtc6715.rssi.state != recv_5_8_rssi_state_stop)){
				if((p_ctl->rtc6715.rssi.state == recv_5_8_rssi_state_continue) || (p_ctl->rtc6715.rssi.state == recv_5_8_rssi_state_start)){
					sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],"%s","Scan");
				}
				if(p_ctl->rtc6715.rssi.state == recv_5_8_rssi_check_video) {
					sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],"%d",p_ctl->rtc6715.freq);
				}
			}
#else
			if(0){

			}
#endif
			else{
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)]," %d",p_ctl->rtc6715.freq);
				if(p_ctl->rtc6715.p_cfg->freq_ch_mode == recv_5_8_ch_mode){
					sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%s",ch_5_8_table[p_ctl->rtc6715.ch_cntr]);
				}
			}
#endif
#ifdef RECV_2_4
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)]," %d",p_ctl->recv_2_4.freq);
			if(p_ctl->recv_2_4.p_cfg->freq_ch_mode == recv_1_2_ch_mode){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%d",p_ctl->recv_2_4.ch_cntr);
			}

#endif
		}
		else{

#ifdef RTC6715
#ifdef RECV_5_8_RSSI
			if((p_ctl->rtc6715.p_cfg->b_en_fast_scan) && (p_ctl->rtc6715.rssi.state != recv_5_8_rssi_state_stop)){
				if((p_ctl->rtc6715.rssi.state == recv_5_8_rssi_state_continue) || (p_ctl->rtc6715.rssi.state == recv_5_8_rssi_state_start)){
					sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%s","Scan");
				}
				if(p_ctl->rtc6715.rssi.state == recv_5_8_rssi_check_video) {
					sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->rtc6715.freq);
				}
			}
#else
			if(0){

			}
#endif
			else{
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->rtc6715.freq);
				if(p_ctl->rtc6715.p_cfg->freq_ch_mode == recv_5_8_ch_mode){
					sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%s",ch_5_8_table[p_ctl->rtc6715.ch_cntr]);
				}
			}
#endif
#ifdef RECV_2_4
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d",p_ctl->recv_2_4.freq);
			if(p_ctl->recv_2_4.p_cfg->freq_ch_mode == recv_1_2_ch_mode){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%d",p_ctl->recv_2_4.ch_cntr);
			}
#endif

#ifdef RECV_1_2
			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)]," %d",p_ctl->recv_1_2.freq);
			if(p_ctl->recv_1_2.p_cfg->freq_ch_mode == recv_1_2_ch_mode){
				sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[strlen((char*)p_ctl->povorot.ack_dbg_tmp)],":ch=%d",p_ctl->recv_1_2.ch_cntr);
			}
#endif

//			sprintf((char*)&p_ctl->povorot.ack_dbg_tmp[0],"%d ( %d )",p_ctl->rtc6715.freq,p_ctl->recv_1_2.freq);
		}
	break;
	}
	return (uint8_t*)&p_ctl->povorot.ack_dbg_tmp[0];
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* main_cntrl_dbg_get_alarm_state(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	memset(p_ctl->povorot.ack_dbg_tmp,0,sizeof(p_ctl->povorot.ack_dbg_tmp));
	if(p_ctl->alarm_mode == cntrl_alarm_en){
		sprintf((char*)p_ctl->povorot.ack_dbg_tmp,"%s","!!! ALARM !!!");
	}
	else{
		sprintf((char*)p_ctl->povorot.ack_dbg_tmp,"%s","Norm");
	}
	return (uint8_t*)p_ctl->povorot.ack_dbg_tmp;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* main_cntrl_dbg_get_alarm_state_bin(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	p_ctl->povorot.ack_dbg_tmp[0] = 0;
	if(p_ctl->alarm_mode == cntrl_alarm_en){
		p_ctl->povorot.ack_dbg_tmp[0] = 1;
	}
	return (uint8_t*)&p_ctl->povorot.ack_dbg_tmp[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void main_cntrl_dbg_freq_up(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
p_ctl = (control_struct *)adf32;

	main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_freq_up,0,NULL,dev_cntrl_ack_not_response);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void main_cntrl_dbg_liter_up(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
p_ctl = (control_struct *)adf32;

	main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_liter_up,0,NULL,dev_cntrl_ack_not_response);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void main_cntrl_dbg_liter_down(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
p_ctl = (control_struct *)adf32;

	main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_liter_down,0,NULL,dev_cntrl_ack_not_response);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void main_cntrl_dbg_ch_up(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
p_ctl = (control_struct *)adf32;

	main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_ch_up,0,NULL,dev_cntrl_ack_not_response);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void main_cntrl_dbg_ch_down(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
p_ctl = (control_struct *)adf32;

	main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_ch_down,0,NULL,dev_cntrl_ack_not_response);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//static uint8_t* main_cntrl_dbg_get_collect_mode(uint32_t adf32, uint32_t tmp0, uint32_t cmd){
//	control_struct *p_ctl;
//	p_ctl = (control_struct *)adf32;
//
//	return &p_ctl->b_collect;
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void main_cntrl_dbg_freq_down(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
p_ctl = (control_struct *)adf32;

	main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_freq_down,0,NULL,dev_cntrl_ack_not_response);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* main_cntrl_dbg_get_ch_auto_scan(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	return (uint8_t*)&p_ctl->freq_mode;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void main_cntrl_dbg_set_ch_auto_scan(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	if(*p8_data){
		main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_auto_scan_ch_en,0,NULL,dev_cntrl_ack_not_response);
	}
	else{
		main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_auto_scan_ch_dis,0,NULL,dev_cntrl_ack_not_response);
	}

	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* main_cntrl_dbg_get_rotate_auto_scan(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;

	p_ctl->povorot.ack_dbg_tmp[0] = 0;
	if(p_ctl->povorot.work_mode == povorot_work_mode_scan){
		p_ctl->povorot.ack_dbg_tmp[0] = 1;
	}
	return (uint8_t*)&p_ctl->povorot.ack_dbg_tmp[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void main_cntrl_dbg_set_rotate_auto_scan(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	if(*p8_data){
		povorot_cmd_make_ctrl_cmd(&p_ctl->povorot,povorot_cntrl_cmd_start_scan,0,NULL,dev_cntrl_ack_not_response);
	}
	else{
		povorot_cmd_send_cmd_stop(&p_ctl->povorot);
	}

//	if(*p8_data){
//		main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_auto_scan_rotate_en,0,NULL,dev_cntrl_ack_not_response);
//	}
//	else{
//		main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_auto_scan_rotate_dis,0,NULL,dev_cntrl_ack_not_response);
//	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* main_cntrl_dbg_get_auto_scan(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	return (uint8_t*)&p_ctl->auto_mode;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void main_cntrl_dbg_set_auto_scan(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	if(*p8_data){
		main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_auto_scan_en,0,NULL,dev_cntrl_ack_not_response);
	}
	else{
		main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_auto_scan_dis,0,NULL,dev_cntrl_ack_not_response);
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* main_cntrl_dbg_get_en_image(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	return (uint8_t*)&p_ctl->b_udp_send;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void main_cntrl_dbg_set_en_image(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	if(*p8_data){

		if(!p_ctl->b_http_file_open){
			p_ctl->b_udp_send=1;
			p_ctl->b_wait_udp_send = 0;
		}
		else{
			if(p_ctl->b_udp_send==0){
				p_ctl->b_wait_udp_send = 1;
			}
		}
	}
	else{
		p_ctl->b_udp_send = 0;
	}
	return;
}
#ifdef RECV_5_8_RSSI
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//static uint8_t* main_cntrl_dbg_get_en_fast_scan(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
//control_struct *p_ctl;
//	p_ctl = (control_struct *)adf32;
//	return (uint8_t*)&p_ctl->rtc6715.rssi.b_en;
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//static void main_cntrl_dbg_set_en_fast_scan(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
//	control_struct *p_ctl;
//	p_ctl = (control_struct *)adf32;
////todo en 5.8
//	if(*p8_data){
//		main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_start_rssi,0,NULL,dev_cntrl_ack_not_response);	}
//	else{
//		main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_stop_rssi,0,NULL,dev_cntrl_ack_not_response);
//	}
//	return;
//}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void main_cntrl_dbg_set_recv_mode(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_sel_recv_mode,1,(uint8_t*)&cmd,dev_cntrl_ack_not_response);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* main_cntrl_dbg_get_recv_mode(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	p_ctl->povorot.ack_dbg_tmp[0] = 0;
	if(p_ctl->recv_mode == cmd){
		p_ctl->povorot.ack_dbg_tmp[0] = 1;
	}
	return (uint8_t*)p_ctl->povorot.ack_dbg_tmp;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void main_cntrl_dbg_set_rotate_make_step(uint32_t adf32, uint32_t cmd, uint32_t tmp, uint8_t* p8_data){
control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	povorot_cmd_make_ctrl_cmd(&p_ctl->povorot,cmd,0,NULL,dev_cntrl_ack_not_response);
	return;
}
//#ifdef RECV_1_2
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//static void main_cntrl_dbg_set_5_8_freq(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
//	control_struct *p_ctl;
//	p_ctl = (control_struct *)adf32;
//	recv_1_2_set_freq(&p_ctl->recv_1_2,*((uint16_t*)p8_data));
//	return;
//}
//#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifndef NOT_VIDEO_RECEIVER
// void main_cntrl_dbg_set_offset_w(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
//	control_struct *p_ctl;
//	uint16_t w,h;
//
//	p_ctl = (control_struct *)adf32;
//
//	memcpy(&w, p8_data,2);
//	w = w - w%2;
//	if(w>320) w = 320;
//
//	memcpy(&control.dev_config.rec.tmp[2], &w,2);
//	dev_config_write(&control.dev_config);
//
//	cntr_stop_grabb(p_ctl);
//	HAL_DCMI_DisableCrop(&p_ctl->grabber.hdcmi);
//	osDelay(20);
//
//
//	  w = *(uint16_t*)&control.dev_config.rec.tmp[2];
//	  h = *(uint16_t*)&control.dev_config.rec.tmp[4];
//
//	  HAL_DCMI_ConfigCrop(&p_ctl->grabber.hdcmi,w, h, p_ctl->grabber.wigth*2-1, p_ctl->grabber.height);
//	  HAL_DCMI_EnableCrop(&p_ctl->grabber.hdcmi);
//
//	cntr_start_grabb(p_ctl);
//
//
//	return;
//}
//#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifndef NOT_VIDEO_RECEIVER
// void main_cntrl_dbg_set_offset_h(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
//control_struct *p_ctl;
//	uint16_t w,h;
//	p_ctl = (control_struct *)adf32;
//
//
//	memcpy(&h, p8_data,2);
//	if(!h) h = 2;
//	if(h>72) h = 72;
//
//	memcpy(&control.dev_config.rec.tmp[4], &h,2);
//	dev_config_write(&control.dev_config);
//
//	cntr_stop_grabb(p_ctl);
//	HAL_DCMI_DisableCrop(&p_ctl->grabber.hdcmi);
//	osDelay(20);
//
//
//	  w = *(uint16_t*)&control.dev_config.rec.tmp[2];
//	  h = *(uint16_t*)&control.dev_config.rec.tmp[4];
//
//	  HAL_DCMI_ConfigCrop(&p_ctl->grabber.hdcmi,w, h, p_ctl->grabber.wigth*2-1, p_ctl->grabber.height);
//	  HAL_DCMI_EnableCrop(&p_ctl->grabber.hdcmi);
//
//	cntr_start_grabb(p_ctl);
//
//	return;
//}
//#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* main_cntrl_dbg_get_offset_w(uint32_t adf32, uint32_t tmp0, uint32_t cmd){
	control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	return &p_ctl->dev_config.rec.tmp[2];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 uint8_t* main_cntrl_dbg_get_offset_h(uint32_t adf32, uint32_t tmp0, uint32_t cmd){
	control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
	return &p_ctl->dev_config.rec.tmp[4];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* main_cntrl_dbg_get_5_8_freq(uint32_t adf32, uint32_t tmp0, uint32_t cmd){
	control_struct *p_ctl;
	p_ctl = (control_struct *)adf32;
#ifdef RTC6715
	return (uint8_t*)&p_ctl->rtc6715.freq;
#endif
#ifdef RECV_2_4
	return (uint8_t*)&p_ctl->recv_2_4.freq;
#endif

}
#ifdef RECEIVER_BASE_MODE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main_cntrl_make_html_cntrl_table(control_struct* p_ctl, ctrl_table_list_struct* p_list){
SettingCell_t* p_cell;
uint8_t num=1;
SettingCell_t 	cell;
uint32_t 		i;

memset(&p_ctl->main_config_table,0,sizeof(p_ctl->main_config_table));
p_list->name = "Main_cntrl";
p_list->mode = div_time_att;// | div_collect_mode;//div_collect_mode
//div_all_wr | div_collect_mode;// | div_collect_mode_wr;//div_all_rd
p_list->p_start_cell = &p_ctl->main_config_table[0];
p_cell = p_list->p_start_cell;

cmd_h_add_to_cell_table_ext_size(
						p_cell,
						num++,
						p_ctl,
						NULL,
						NULL,
						0,
						FixIntegerCellType,
						"Control");

	i = 1;

	cell.CellAttr = RD_Att;
	cell.CellType = VarStringCellType + 31;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_dbg_get_rotate_state;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)&p_ctl->povorot;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Rotate state";
	p_cell[i++] = cell;

#ifndef NOT_VIDEO_RECEIVER
	cell.CellAttr = RD_Att;
	cell.CellType = VarStringCellType + 31;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_freq;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Freq MHz";
	p_cell[i++] = cell;
#endif

#ifdef	RTC6705

	cell.CellAttr = RD_Att;
	cell.CellType = VarStringCellType + 31;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_status;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Status";
	p_cell[i++] = cell;

#endif


#ifndef NOT_VIDEO_RECEIVER
	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att + Ptr_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_alarm_state_bin;
	cell.WriteProc = NULL;
	cell.LowLim = 5;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Alarm!!!";
	p_cell[i++] = cell;

	cell.CellAttr = RD_Att + Action_Att;
	cell.CellType = FixIntegerCellType + 4,
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_frame_cntr;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Frame cntr";
	p_cell[i++] = cell;
#endif


#ifndef NOT_VIDEO_RECEIVER

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_freq_up;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Up";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_freq_down;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Down";
	p_cell[i++] = cell;
#ifdef	RTC6715
	if(!p_ctl->rtc6715.p_cfg->b_en_fast_scan){
#endif

#ifdef	RECV_5_8_TUNE_CH_CNTRL
	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_liter_up;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Lit. Up";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_liter_down;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Lit. Down";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_ch_up;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Ch Up";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_ch_down;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Ch Down";
	p_cell[i++] = cell;

#endif
#ifdef	RTC6715
	}
#endif


//work mode

#ifndef LORA
#ifndef RTC6705
#ifdef	RECV_1_2
	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_recv_mode;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_recv_mode;
	cell.LowLim = 1;
	cell.HighLim = cntr_recv_mode_1_2;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "sel 1.2";
#ifdef	RECV_1_2_SUB_1_5
	if(p_ctl->recv_1_2.ver == recv_1_2_ver_1_5_){
		cell.DescriptStr = "sel 1.5";
	}
#endif
#ifdef	RECV_1_2_SUB_2_4
	if(p_ctl->recv_1_2.ver == recv_1_2_ver_2_4_){
		cell.DescriptStr = "sel 2.4";
	}
#endif

	p_cell[i++] = cell;
#endif
#endif

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_recv_mode;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_recv_mode;
	cell.LowLim = 1;
	cell.HighLim = cntr_recv_mode_5_8;
	cell.DefaultValue = (uint32_t)p_ctl;

#ifdef RTC6715
#ifdef	RECV_5_8_SUB_3_3
	cell.DescriptStr = "sel 3.3";
#else
	cell.DescriptStr = "sel 5.8";
#endif
#endif
#ifdef RECV_2_4
	cell.DescriptStr = "sel 2.4";
#endif

	p_cell[i++] = cell;

#ifdef RTC6705

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_recv_mode;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_recv_mode;
	cell.LowLim = 1;
	cell.HighLim = cntr_recv_mode_emit;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "sel Emit";
	p_cell[i++] = cell;

#endif

#ifndef RTC6705
	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_recv_mode;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_recv_mode;
	cell.LowLim = 1;
	cell.HighLim = cntr_recv_mode_1_2_5_8;
	cell.DefaultValue = (uint32_t)p_ctl;
#ifdef RECV_2_4
	cell.DescriptStr = "sel 1.5_2.4";
#else

#ifdef	RECV_5_8_SUB_3_3
	cell.DescriptStr = "sel 1.2_3.3";
#else
	cell.DescriptStr = "sel 1.2_5.8";
#endif


#ifdef	RECV_1_2_SUB_2_4
	if(p_ctl->recv_1_2.ver == recv_1_2_ver_2_4_){

#ifdef	RECV_5_8_SUB_3_3
	cell.DescriptStr = "sel 2.4_3.3";
#else
	cell.DescriptStr = "sel 2.4_5.8";
#endif

	}
#endif

#endif


	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_recv_mode;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_recv_mode;
	cell.LowLim = 1;
	cell.HighLim = cntr_recv_mode_5_8_1_2;
	cell.DefaultValue = (uint32_t)p_ctl;
#ifdef RECV_2_4
	cell.DescriptStr = "sel 2.4_1.5";
#else

#ifdef	RECV_5_8_SUB_3_3
	cell.DescriptStr = "sel 3.3_1.2";
#else
	cell.DescriptStr = "sel 5.8_1.2";
#endif


//	cell.DescriptStr = "sel 5.8_1.2";
#ifdef	RECV_1_2_SUB_2_4
	if(p_ctl->recv_1_2.ver == recv_1_2_ver_2_4_){

#ifdef	RECV_5_8_SUB_3_3
	cell.DescriptStr = "sel 3.3_2.4";
#else
	cell.DescriptStr = "sel 5.8_2.4";
#endif



//		cell.DescriptStr = "sel 5.8_2.4";
	}
#endif

#endif

//	cell.DescriptStr = "sel 5.8_1.2";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_recv_mode;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_recv_mode;
	cell.LowLim = 1;
	cell.HighLim = cntr_recv_mode_1_2_and_5_8;
	cell.DefaultValue = (uint32_t)p_ctl;
#ifdef RECV_2_4
	cell.DescriptStr = "sel 2.4+1.5";
#else
#ifdef	RECV_5_8_SUB_3_3
	cell.DescriptStr = "sel 3.3+1.2";
#else
	cell.DescriptStr = "sel 5.8+1.2";
#endif

#ifdef	RECV_1_2_SUB_2_4
	if(p_ctl->recv_1_2.ver == recv_1_2_ver_2_4_){

#ifdef	RECV_5_8_SUB_3_3
	cell.DescriptStr = "sel 3.3+2.4";
#else
	cell.DescriptStr = "sel 5.8+2.4";
#endif






	}
#endif

#endif
	p_cell[i++] = cell;
#endif


//lora
#endif

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_ch_auto_scan;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_ch_auto_scan;
	cell.LowLim = 1;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Auto ch scan en";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_rotate_auto_scan;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_auto_scan;
	cell.LowLim = 1;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Auto rotate scan en";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_auto_scan;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_auto_scan;
	cell.LowLim = 1;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Auto scan";
	p_cell[i++] = cell;


#endif

//	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 1,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_collect_mode;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_collect_mode;
//	cell.LowLim = 1;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Auto 1.2+5.8";
//	p_cell[i++] = cell;
//#ifdef RECV_1_2
//	if(p_ctl->dev_config.rec.tmp[0]){
////		cell.CellAttr = WR_Att + Action_Att + RD_Att,
//		cell.CellAttr = WR_Att + Action_Att,
//		cell.CellType = FixIntegerCellType + 2,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_5_8_freq;
//		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_5_8_freq;
//		cell.LowLim = 0;
//		cell.HighLim = 0;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "freq 5.8 MHz";
//		p_cell[i++] = cell;
//	}
//#endif

	if(p_ctl->dev_config.rec.tmp[1]){
		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
		cell.LowLim = povorot_cntrl_cmd_step_left;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rot st L.";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
		cell.LowLim = povorot_cntrl_cmd_step_right;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rot st R.";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
		cell.LowLim = povorot_cntrl_cmd_step_up;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rot st U.";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
		cell.LowLim = povorot_cntrl_cmd_step_down;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rot st D.";
		p_cell[i++] = cell;

	}

#ifndef NOT_VIDEO_RECEIVER
	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_en_image;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_en_image;
	cell.LowLim = 1;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Image ext";
	p_cell[i++] = cell;
#endif

#ifdef RECV_5_8_RSSI
//	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 1,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_en_fast_scan;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_en_fast_scan;
//	cell.LowLim = 1;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Fast scan";
//	p_cell[i++] = cell;
#endif

}
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef MULTI_EMIT

 void main_cntrl_cmd_parser(control_struct *p_ctl, cntrl_dev_sys_msg_que_type_s *p_msg){

	 rtc6705_multi_struct* p_mul;

	 p_ctl->alarm_mode = cntrl_alarm_dis;

	switch(p_msg->cmd){

	case main_cntrl_cmd_emit_freq:

		p_mul = &p_ctl->mutli_e1;
		if(p_msg->buf[0] >= 1) p_mul = &p_ctl->mutli_e2;

uint16_t freq;
		memcpy(&freq,&p_msg->buf[2],2);
		rtc6705_multi_set_freq(p_mul,freq,p_msg->buf[1],rtc6705_multi_single_write);

	break;

	case main_cntrl_cmd_emit_pwr:

		p_mul = &p_ctl->mutli_e1;
		if(p_msg->buf[0] >= 1) p_mul = &p_ctl->mutli_e2;

		if(p_msg->buf[1]){
			rtc6705_multi_power_on(p_mul);
		}
		else{
			rtc6705_multi_power_off(p_mul);
		}
	break;


	case main_cntrl_cmd_sel_recv_mode:
#ifndef NOT_VIDEO_RECEIVER
		cntrl_sel_recv_mode(p_ctl, (recv_mode_e)p_msg->buf[0]);
#endif
	break;

	case main_cntrl_cmd_freq:
//		printf("cmd freq = %d \r\n",*(uint16_t*)&p_msg->buf[0]);
#ifdef RTC6715
#ifdef RECV_5_8_RSSI
		if(p_ctl->rtc6715.rssi.state != recv_5_8_rssi_state_stop){
			recv_5_8_stop_rssi(&p_ctl->rtc6715);
		}
#endif
		rtc6715_set_freq(&p_ctl->rtc6715, *(uint16_t*)&p_msg->buf[0]);
#endif
#ifdef RECV_2_4
		recv_1_2_set_freq(&p_ctl->recv_2_4, *(uint16_t*)&p_msg->buf[0]);
#endif

	break;
	case main_cntrl_cmd_freq_up:

		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->auto_mode = cntrl_auto_mode_dis;

#ifdef RECV_1_2
		if(p_ctl->recv_active == cmd_recv_active_1_2){
			recv_1_2_set_freq_up(&p_ctl->recv_1_2,p_ctl->recv_1_2.p_cfg->manual_step_freq);
		}
#endif
		if(p_ctl->recv_active == cmd_recv_active_5_8){
#ifdef RTC6715
			recv_5_8_set_freq_up(&p_ctl->rtc6715,p_ctl->rtc6715.p_cfg->manual_step_freq);
#endif
#ifdef RECV_2_4
			recv_1_2_set_freq_up(&p_ctl->recv_2_4,p_ctl->recv_2_4.p_cfg->manual_step_freq);
#endif

		}
	break;
	case main_cntrl_cmd_liter_up:

		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->auto_mode = cntrl_auto_mode_dis;

		if(p_ctl->recv_active == cmd_recv_active_5_8){
#ifdef RTC6715
			recv_5_8_set_liter_up(&p_ctl->rtc6715);
#endif
		}
	break;

	case main_cntrl_cmd_liter_down:

		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->auto_mode = cntrl_auto_mode_dis;

		if(p_ctl->recv_active == cmd_recv_active_5_8){
#ifdef RTC6715
			recv_5_8_set_liter_down(&p_ctl->rtc6715);
#endif
		}
	break;
	case main_cntrl_cmd_ch_down:

		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->auto_mode = cntrl_auto_mode_dis;

		if(p_ctl->recv_active == cmd_recv_active_5_8){
#ifdef RTC6715
			recv_5_8_set_ch_down(&p_ctl->rtc6715);
#endif
		}
	break;
	case main_cntrl_cmd_ch_up:

		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->auto_mode = cntrl_auto_mode_dis;

		if(p_ctl->recv_active == cmd_recv_active_5_8){
#ifdef RTC6715
			recv_5_8_set_ch_up(&p_ctl->rtc6715);
#endif
		}
	break;

	case main_cntrl_cmd_freq_down:

		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->auto_mode = cntrl_auto_mode_dis;
#ifdef RECV_1_2
		if(p_ctl->recv_active == cmd_recv_active_1_2){
			recv_1_2_set_freq_down(&p_ctl->recv_1_2,p_ctl->recv_1_2.p_cfg->manual_step_freq);
		}
#endif
		if(p_ctl->recv_active == cmd_recv_active_5_8){
#ifdef RTC6715
			recv_5_8_set_freq_down(&p_ctl->rtc6715,p_ctl->rtc6715.p_cfg->manual_step_freq);
#endif
#ifdef RECV_2_4
			recv_1_2_set_freq_down(&p_ctl->recv_2_4,p_ctl->recv_2_4.p_cfg->manual_step_freq);
#endif

		}
	break;
	case main_cntrl_cmd_auto_scan_ch_en:
		p_ctl->freq_mode = cntrl_freq_mode_auto;
		p_ctl->ch_time = 0;

		if ((p_ctl->recv_mode == cntr_recv_mode_5_8) || (p_ctl->recv_mode == cntr_recv_mode_1_2_5_8) || (p_ctl->recv_mode == cntr_recv_mode_5_8_1_2) || (p_ctl->recv_mode == cntr_recv_mode_1_2_and_5_8)){

#ifdef	RTC6715
#ifdef 	RECV_5_8_RSSI
			if(p_ctl->rtc6715.p_cfg->b_en_fast_scan){
				recv_5_8_start_rssi(&p_ctl->rtc6715,1);
			}
#endif
#endif
		}

//		//!!!!!!
//		cntrl_stop_collect(p_ctl);
//
//		if(p_ctl->recv_mode == cntr_recv_mode_5_8){
//			p_ctl->ch_mode = cntrl_ch_mode_auto;
//			p_ctl->ch_time = 0;
//			cntrl_recv58_start_det(p_ctl);
//		}
//		if(p_ctl->recv_mode == cntr_recv_mode_1_2){
//			main_cntrl_start_ch_auto_scan(p_ctl);
//		}
	break;

	case main_cntrl_cmd_auto_scan_ch_dis:
		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->ch_time = 0;

#ifdef	RTC6715
#ifdef RECV_5_8_RSSI
		recv_5_8_stop_rssi(&p_ctl->rtc6715);
#endif
#endif
//		if ((p_ctl->recv_mode == cntr_recv_mode_5_8) || (p_ctl->recv_mode == cntr_recv_mode_1_2_5_8) || (p_ctl->recv_mode == cntr_recv_mode_5_8_1_2) || (p_ctl->recv_mode == cntr_recv_mode_1_2_and_5_8)){
//			if(p_ctl->rtc6715.p_cfg->b_en_fast_scan){
//
//			}
//		}

	break;
	case main_cntrl_cmd_auto_scan_en:
		p_ctl->auto_mode = cntrl_auto_mode_en;

		switch(p_ctl->recv_mode){

		case cntr_recv_mode_1_2:
#ifndef NOT_VIDEO_RECEIVER
			TVP_interrut_start(&p_ctl->tvp_5150_master);
#endif
#ifndef		NO_SEC_CH
			TVP_interrut_stop(&p_ctl->tvp_5150_slave);
#endif
		break;

		case cntr_recv_mode_5_8:
#ifndef NOT_VIDEO_RECEIVER
			TVP_interrut_start(&p_ctl->tvp_5150_master);
#endif
#ifndef		NO_SEC_CH
			TVP_interrut_stop(&p_ctl->tvp_5150_slave);
#endif
		break;

		case cntr_recv_mode_1_2_5_8:
#ifndef NOT_VIDEO_RECEIVER
			TVP_interrut_start(&p_ctl->tvp_5150_master);
#endif
#ifndef		NO_SEC_CH
			TVP_interrut_start(&p_ctl->tvp_5150_slave);
#endif
		break;

		case cntr_recv_mode_5_8_1_2:
#ifndef NOT_VIDEO_RECEIVER
			TVP_interrut_start(&p_ctl->tvp_5150_master);
#endif
#ifndef		NO_SEC_CH
			TVP_interrut_start(&p_ctl->tvp_5150_slave);
#endif
		break;

		case cntr_recv_mode_1_2_and_5_8:
#ifndef NOT_VIDEO_RECEIVER
			TVP_interrut_start(&p_ctl->tvp_5150_master);
#endif
#ifndef		NO_SEC_CH
			TVP_interrut_start(&p_ctl->tvp_5150_slave);
#endif
		break;
		}

	break;
	case main_cntrl_cmd_auto_scan_dis:
		p_ctl->auto_mode = cntrl_auto_mode_dis;
	break;
#ifdef		RTC6705
	case main_cntrl_cmd_emit_freq:
uint16_t freq_e;
		freq_e = *(uint16_t*)&p_msg->buf[0];

		if((p_ctl->recv_mode != cntr_recv_mode_emit) && (freq_e)){
			cntrl_sel_recv_mode(p_ctl, cntr_recv_mode_emit);
		}
		else{
			if((p_ctl->recv_mode == cntr_recv_mode_emit) && (!freq_e)){
				cntrl_sel_recv_mode(p_ctl, cntr_recv_mode_5_8);
			}
		}
		if(p_ctl->recv_mode == cntr_recv_mode_emit){
			p_ctl->freq_mode = cntrl_freq_mode_manual;//remove???
			p_ctl->auto_mode = cntrl_auto_mode_dis;

			rtc6705_set_freq(&p_ctl->rtc6705,freq_e);
			rtc6715_set_freq(&p_ctl->rtc6715, freq_e);
		}

	break;
#endif

	}
	osPoolFree(p_ctl->main_ctrl_cmd.cntrl_msg_pool_id,p_msg);
}
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RECEIVER_BASE_MODE

 void main_cntrl_cmd_parser(control_struct *p_ctl, cntrl_dev_sys_msg_que_type_s *p_msg){

	 p_ctl->alarm_mode = cntrl_alarm_dis;

	switch(p_msg->cmd){

	case main_cntrl_cmd_sel_recv_mode:
#ifndef NOT_VIDEO_RECEIVER
		cntrl_sel_recv_mode(p_ctl, (recv_mode_e)p_msg->buf[0]);
#endif
	break;

	case main_cntrl_cmd_freq:
//		printf("cmd freq = %d \r\n",*(uint16_t*)&p_msg->buf[0]);
#ifdef RTC6715
#ifdef RECV_5_8_RSSI
		if(p_ctl->rtc6715.rssi.state != recv_5_8_rssi_state_stop){
			recv_5_8_stop_rssi(&p_ctl->rtc6715);
		}
#endif
		rtc6715_set_freq(&p_ctl->rtc6715, *(uint16_t*)&p_msg->buf[0]);
#endif
#ifdef RECV_2_4
		recv_1_2_set_freq(&p_ctl->recv_2_4, *(uint16_t*)&p_msg->buf[0]);
#endif

	break;
	case main_cntrl_cmd_freq_up:

		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->auto_mode = cntrl_auto_mode_dis;

#ifdef RECV_1_2
		if(p_ctl->recv_active == cmd_recv_active_1_2){
			recv_1_2_set_freq_up(&p_ctl->recv_1_2,p_ctl->recv_1_2.p_cfg->manual_step_freq);
		}
#endif
		if(p_ctl->recv_active == cmd_recv_active_5_8){
#ifdef RTC6715
			recv_5_8_set_freq_up(&p_ctl->rtc6715,p_ctl->rtc6715.p_cfg->manual_step_freq);
#endif
#ifdef RECV_2_4
			recv_1_2_set_freq_up(&p_ctl->recv_2_4,p_ctl->recv_2_4.p_cfg->manual_step_freq);
#endif

		}
	break;
	case main_cntrl_cmd_liter_up:

		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->auto_mode = cntrl_auto_mode_dis;

		if(p_ctl->recv_active == cmd_recv_active_5_8){
#ifdef RTC6715
			recv_5_8_set_liter_up(&p_ctl->rtc6715);
#endif
		}
	break;

	case main_cntrl_cmd_liter_down:

		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->auto_mode = cntrl_auto_mode_dis;

		if(p_ctl->recv_active == cmd_recv_active_5_8){
#ifdef RTC6715
			recv_5_8_set_liter_down(&p_ctl->rtc6715);
#endif
		}
	break;
	case main_cntrl_cmd_ch_down:

		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->auto_mode = cntrl_auto_mode_dis;

		if(p_ctl->recv_active == cmd_recv_active_5_8){
#ifdef RTC6715
			recv_5_8_set_ch_down(&p_ctl->rtc6715);
#endif
		}
	break;
	case main_cntrl_cmd_ch_up:

		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->auto_mode = cntrl_auto_mode_dis;

		if(p_ctl->recv_active == cmd_recv_active_5_8){
#ifdef RTC6715
			recv_5_8_set_ch_up(&p_ctl->rtc6715);
#endif
		}
	break;

	case main_cntrl_cmd_freq_down:

		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->auto_mode = cntrl_auto_mode_dis;
#ifdef RECV_1_2
		if(p_ctl->recv_active == cmd_recv_active_1_2){
			recv_1_2_set_freq_down(&p_ctl->recv_1_2,p_ctl->recv_1_2.p_cfg->manual_step_freq);
		}
#endif
		if(p_ctl->recv_active == cmd_recv_active_5_8){
#ifdef RTC6715
			recv_5_8_set_freq_down(&p_ctl->rtc6715,p_ctl->rtc6715.p_cfg->manual_step_freq);
#endif
#ifdef RECV_2_4
			recv_1_2_set_freq_down(&p_ctl->recv_2_4,p_ctl->recv_2_4.p_cfg->manual_step_freq);
#endif

		}
	break;
	case main_cntrl_cmd_auto_scan_ch_en:
		p_ctl->freq_mode = cntrl_freq_mode_auto;
		p_ctl->ch_time = 0;

		if ((p_ctl->recv_mode == cntr_recv_mode_5_8) || (p_ctl->recv_mode == cntr_recv_mode_1_2_5_8) || (p_ctl->recv_mode == cntr_recv_mode_5_8_1_2) || (p_ctl->recv_mode == cntr_recv_mode_1_2_and_5_8)){

#ifdef	RTC6715
#ifdef 	RECV_5_8_RSSI
			if(p_ctl->rtc6715.p_cfg->b_en_fast_scan){
				recv_5_8_start_rssi(&p_ctl->rtc6715,1);
			}
#endif
#endif
		}

//		//!!!!!!
//		cntrl_stop_collect(p_ctl);
//
//		if(p_ctl->recv_mode == cntr_recv_mode_5_8){
//			p_ctl->ch_mode = cntrl_ch_mode_auto;
//			p_ctl->ch_time = 0;
//			cntrl_recv58_start_det(p_ctl);
//		}
//		if(p_ctl->recv_mode == cntr_recv_mode_1_2){
//			main_cntrl_start_ch_auto_scan(p_ctl);
//		}
	break;

	case main_cntrl_cmd_auto_scan_ch_dis:
		p_ctl->freq_mode = cntrl_freq_mode_manual;
		p_ctl->ch_time = 0;

#ifdef	RTC6715
#ifdef RECV_5_8_RSSI
		recv_5_8_stop_rssi(&p_ctl->rtc6715);
#endif
#endif
//		if ((p_ctl->recv_mode == cntr_recv_mode_5_8) || (p_ctl->recv_mode == cntr_recv_mode_1_2_5_8) || (p_ctl->recv_mode == cntr_recv_mode_5_8_1_2) || (p_ctl->recv_mode == cntr_recv_mode_1_2_and_5_8)){
//			if(p_ctl->rtc6715.p_cfg->b_en_fast_scan){
//
//			}
//		}

	break;
//	case main_cntrl_cmd_auto_scan_en:
//		p_ctl->auto_mode = cntrl_auto_mode_en;
//
//		switch(p_ctl->recv_mode){
//
//		case cntr_recv_mode_1_2:
//#ifndef NOT_VIDEO_RECEIVER
//			TVP_interrut_start(&p_ctl->tvp_5150_master);
//#endif
//#ifndef		NO_SEC_CH
//			TVP_interrut_stop(&p_ctl->tvp_5150_slave);
//#endif
//		break;
//
//		case cntr_recv_mode_5_8:
//#ifndef NOT_VIDEO_RECEIVER
//			TVP_interrut_start(&p_ctl->tvp_5150_master);
//#endif
//#ifndef		NO_SEC_CH
//			TVP_interrut_stop(&p_ctl->tvp_5150_slave);
//#endif
//		break;
//
//		case cntr_recv_mode_1_2_5_8:
//#ifndef NOT_VIDEO_RECEIVER
//			TVP_interrut_start(&p_ctl->tvp_5150_master);
//#endif
//#ifndef		NO_SEC_CH
//			TVP_interrut_start(&p_ctl->tvp_5150_slave);
//#endif
//		break;
//
//		case cntr_recv_mode_5_8_1_2:
//#ifndef NOT_VIDEO_RECEIVER
//			TVP_interrut_start(&p_ctl->tvp_5150_master);
//#endif
//#ifndef		NO_SEC_CH
//			TVP_interrut_start(&p_ctl->tvp_5150_slave);
//#endif
//		break;
//
//		case cntr_recv_mode_1_2_and_5_8:
//#ifndef NOT_VIDEO_RECEIVER
//			TVP_interrut_start(&p_ctl->tvp_5150_master);
//#endif
//#ifndef		NO_SEC_CH
//			TVP_interrut_start(&p_ctl->tvp_5150_slave);
//#endif
//		break;
//		}
//
//	break;
	case main_cntrl_cmd_auto_scan_dis:
		p_ctl->auto_mode = cntrl_auto_mode_dis;
	break;
#ifdef		RTC6705
	case main_cntrl_cmd_emit_freq:
uint16_t freq_e;
		freq_e = *(uint16_t*)&p_msg->buf[0];

		if((p_ctl->recv_mode != cntr_recv_mode_emit) && (freq_e)){
			cntrl_sel_recv_mode(p_ctl, cntr_recv_mode_emit);
		}
		else{
			if((p_ctl->recv_mode == cntr_recv_mode_emit) && (!freq_e)){
				cntrl_sel_recv_mode(p_ctl, cntr_recv_mode_5_8);
			}
		}
		if(p_ctl->recv_mode == cntr_recv_mode_emit){
			p_ctl->freq_mode = cntrl_freq_mode_manual;//remove???
			p_ctl->auto_mode = cntrl_auto_mode_dis;

			rtc6705_set_freq(&p_ctl->rtc6705,freq_e);
			rtc6715_set_freq(&p_ctl->rtc6715, freq_e);
		}

	break;
#endif

	}
}
#endif











#ifdef MULTI_EMIT

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 static void main_cntrl_dbg_set_emit_freq(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
uint8_t tmp[4];
	p_ctl = (control_struct *)adf32;
	tmp[0] = tmp0;
	tmp[1] = cmd;
	tmp[2] = p8_data[0];
	tmp[3] = p8_data[1];
	main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_emit_freq,4,tmp,dev_cntrl_ack_not_response);
	return;
}
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 static uint8_t* main_cntrl_dbg_get_emit_freq(uint32_t adf32, uint32_t tmp0, uint32_t cmd){
control_struct *p_ctl;
rtc6705_multi_struct*		p_mutli;
	p_ctl = (control_struct *)adf32;
	if(tmp0>1) tmp0 = 0;
	if(tmp0==0){
		p_mutli = &p_ctl->mutli_e1;
	}
	else{
		p_mutli = &p_ctl->mutli_e2;
	}

	return (uint8_t*)&p_mutli->rtc[cmd].freq;
}
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 static void main_cntrl_dbg_set_emit_pwr_en(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
control_struct *p_ctl;
uint8_t tmp[4];
	p_ctl = (control_struct *)adf32;

	tmp[0] = tmp0;
	tmp[1] = 0;
	if(*p8_data){
		tmp[1] = 1;
	}
	main_cntrl_make_ctrl_cmd(p_ctl,main_cntrl_cmd_emit_pwr,2,tmp,dev_cntrl_ack_not_response);
	return;
}
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 static uint8_t* main_cntrl_dbg_get_emit_pwr_en(uint32_t adf32, uint32_t tmp0, uint32_t cmd){
control_struct *p_ctl;
rtc6705_multi_struct*		p_mutli;
	p_ctl = (control_struct *)adf32;
	if(tmp0>1) tmp0 = 0;
	if(tmp0==0){
		p_mutli = &p_ctl->mutli_e1;
	}
	else {
		p_mutli = &p_ctl->mutli_e2;
	}

	return (uint8_t*)&p_mutli->emit_mode;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main_cntrl_make_html_cntrl_table(control_struct* p_ctl, ctrl_table_list_struct* p_list){
SettingCell_t* p_cell;
uint8_t num=1;
SettingCell_t 	cell;
uint32_t 		i;

memset(&p_ctl->main_config_table,0,sizeof(p_ctl->main_config_table));
p_list->name = "Main_cntrl";
p_list->mode = div_time_att | div_wr_att | div_rd_att;// | div_collect_mode;//div_collect_mode
//div_all_wr | div_collect_mode;// | div_collect_mode_wr;//div_all_rd
p_list->p_start_cell = &p_ctl->main_config_table[0];
p_cell = p_list->p_start_cell;

cmd_h_add_to_cell_table_ext_size(
						p_cell,
						num++,
						p_ctl,
						NULL,
						NULL,
						0,
						FixIntegerCellType,
						"Control");

	i = 1;

	cell.CellAttr = RD_Att;
	cell.CellType = VarStringCellType + 31;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_dbg_get_rotate_state;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)&p_ctl->povorot;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Rotate state";
	p_cell[i++] = cell;

	cell.CellAttr = 0;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = NULL;
	cell.ReadProc = NULL;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Emit 1 cntrl";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_pwr_en;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_pwr_en;
	cell.LowLim = 0;//ch num
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "En. Emit 1";
	p_cell[i++] = cell;

	cell.CellAttr = WR_Att + Action_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 2,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
	cell.LowLim = 0;//ch num
	cell.HighLim = 0;//freq ch
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Freq 1";
	p_cell[i++] = cell;

	cell.CellAttr = WR_Att + Action_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 2,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
	cell.LowLim = 0;//ch num
	cell.HighLim = 1;//freq ch
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Freq 2";
	p_cell[i++] = cell;

	cell.CellAttr = WR_Att + Action_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 2,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
	cell.LowLim = 0;//ch num
	cell.HighLim = 2;//freq ch
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Freq 3";
	p_cell[i++] = cell;

	cell.CellAttr = WR_Att + Action_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 2,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
	cell.LowLim = 0;//ch num
	cell.HighLim = 3;//freq ch
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Freq 4";
	p_cell[i++] = cell;


	cell.CellAttr = 0;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = NULL;
	cell.ReadProc = NULL;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Emit 2 cntrl";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_pwr_en;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_pwr_en;
	cell.LowLim = 1;//ch num
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "En. Emit 2";
	p_cell[i++] = cell;

	cell.CellAttr = WR_Att + Action_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 2,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
	cell.LowLim = 1;//ch num
	cell.HighLim = 0;//freq ch
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Freq 1";
	p_cell[i++] = cell;

	cell.CellAttr = WR_Att + Action_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 2,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
	cell.LowLim = 1;//ch num
	cell.HighLim = 1;//freq ch
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Freq 2";
	p_cell[i++] = cell;

	cell.CellAttr = WR_Att + Action_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 2,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
	cell.LowLim = 1;//ch num
	cell.HighLim = 2;//freq ch
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Freq 3";
	p_cell[i++] = cell;

	cell.CellAttr = WR_Att + Action_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 2,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
	cell.LowLim = 1;//ch num
	cell.HighLim = 3;//freq ch
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Freq 4";
	p_cell[i++] = cell;


	if(p_ctl->dev_config.rec.tmp[1]){

		cell.CellAttr = 0;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = NULL;
		cell.ReadProc = NULL;
		cell.WriteProc = NULL;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "Rotate tune";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
		cell.LowLim = povorot_cntrl_cmd_step_left;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rot st L.";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
		cell.LowLim = povorot_cntrl_cmd_step_right;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rot st R.";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
		cell.LowLim = povorot_cntrl_cmd_step_up;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rot st U.";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
		cell.LowLim = povorot_cntrl_cmd_step_down;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rot st D.";
		p_cell[i++] = cell;

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* main_cntrl_dbg_read_period(uint32_t ad32, uint32_t num, uint32_t tmp){
control_struct *p_ad;
rtc6705_multi_struct*		p_multi;
	p_ad = (control_struct *)ad32;
	p_multi = &p_ad->mutli_e1;
	if(num)p_multi = &p_ad->mutli_e2;
//	*(uint32_t*)&p_multi->tmp[0] = p_multi->p_htim->Instance->ARR;
//	return (uint8_t*)&p_multi->tmp[0];
	return (uint8_t*)&p_multi->p_cfg->period;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main_cntrl_dbg_save_freq(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p_data){
control_struct *p_ad;
	p_ad = (control_struct *)ad32;

	control.dev_config.rec.freq[0] = p_ad->mutli_e1.rtc[0].freq;
	control.dev_config.rec.freq[1] = p_ad->mutli_e1.rtc[1].freq;
	control.dev_config.rec.freq[2] = p_ad->mutli_e1.rtc[2].freq;
	control.dev_config.rec.freq[3] = p_ad->mutli_e1.rtc[3].freq;

	control.dev_config.rec.freq[4] = p_ad->mutli_e2.rtc[0].freq;
	control.dev_config.rec.freq[5] = p_ad->mutli_e2.rtc[1].freq;
	control.dev_config.rec.freq[6] = p_ad->mutli_e2.rtc[2].freq;
	control.dev_config.rec.freq[7] = p_ad->mutli_e2.rtc[3].freq;

	dev_config_write(&control.dev_config);
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main_cntrl_dbg_write_period(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p_data){
control_struct *p_ad;
rtc6705_multi_struct*		p_multi;
	p_ad = (control_struct *)ad32;

	p_multi = &p_ad->mutli_e1;
	if(num)p_multi = &p_ad->mutli_e2;


	p_multi->p_cfg->period = *(uint32_t*)p_data;
	p_multi->p_htim->Instance->ARR = *(uint32_t*)p_data;

	dev_config_write(&control.dev_config);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* main_cntrl_dbg_read_imp(uint32_t ad32, uint32_t num, uint32_t tmp){
control_struct *p_ad;
rtc6705_multi_struct*		p_multi;
	p_ad = (control_struct *)ad32;
	p_multi = &p_ad->mutli_e1;
	if(num)p_multi = &p_ad->mutli_e2;

//	*(uint32_t*)&p_multi->tmp[0] = p_multi->p_htim->Instance->CCR4__;
//
//	return (uint8_t*)&p_multi->tmp[0];
	return (uint8_t*)&p_multi->p_cfg->imp;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main_cntrl_dbg_write_imp(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p_data){
control_struct *p_ad;
rtc6705_multi_struct*		p_multi;
	p_ad = (control_struct *)ad32;
	p_multi = &p_ad->mutli_e1;
	if(num)p_multi = &p_ad->mutli_e2;
	p_multi->p_cfg->imp = *(uint32_t*)p_data;

	if(p_multi->tim_ch == TIM_CHANNEL_4)
		p_multi->p_htim->Instance->CCR4 = *(uint32_t*)p_data;
	else
		p_multi->p_htim->Instance->CCR2 = *(uint32_t*)p_data;

	dev_config_write(&control.dev_config);
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main_cntrl_make_html_emit_cfg_table(control_struct* p_ctl, ctrl_table_list_struct* p_list){
SettingCell_t* p_cell;
uint8_t num=1;
SettingCell_t 	cell;
uint32_t 		i;

memset(&p_ctl->emit_config_table,0,sizeof(p_ctl->emit_config_table));
p_list->name = "Emit_config";
p_list->mode = div_time_att | div_wr_att | div_rd_att;// | div_collect_mode;//div_collect_mode
//div_all_wr | div_collect_mode;// | div_collect_mode_wr;//div_all_rd
p_list->p_start_cell = &p_ctl->emit_config_table[0];
p_cell = p_list->p_start_cell;

cmd_h_add_to_cell_table_ext_size(
						p_cell,
						num++,
						p_ctl,
						NULL,
						NULL,
						0,
						FixIntegerCellType,
						"Emit config");

	i = 1;

	cell.CellAttr = Action_Att + Default_Attr + WR_Att;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_save_freq;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Save freq";
	p_cell[i++] = cell;

	cell.CellAttr = 0;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = NULL;
	cell.ReadProc = NULL;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Emit 1 config";
	p_cell[i++] = cell;


	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_read_period;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_write_period;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Mod. period";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_read_imp;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_write_imp;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Mod. imp";
	p_cell[i++] = cell;

	cell.CellAttr = 0;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = NULL;
	cell.ReadProc = NULL;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Emit 2 config";
	p_cell[i++] = cell;


	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_read_period;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_write_period;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 1;
	cell.DescriptStr = "Mod. period";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_read_imp;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_write_imp;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 1;
	cell.DescriptStr = "Mod. imp";
	p_cell[i++] = cell;


//
//	cell.CellAttr = RD_Att;
//	cell.CellType = VarStringCellType + 31;
//	cell.VarPtr = NULL;
//	cell.ReadProc = (void*)(void*)povorot_dbg_get_rotate_state;
//	cell.WriteProc = NULL;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)&p_ctl->povorot;
//	cell.CellNumber = num++;
//	cell.LowLim = 0;
//	cell.DescriptStr = "Rotate state";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = 0;
//	cell.CellType = FixIntegerCellType + 1;
//	cell.VarPtr = NULL;
//	cell.ReadProc = NULL;
//	cell.WriteProc = NULL;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.CellNumber = num++;
//	cell.LowLim = 0;
//	cell.DescriptStr = "Emit 1 cntrl";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 1,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_pwr_en;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_pwr_en;
//	cell.LowLim = 0;//ch num
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "En. Emit 1";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = WR_Att + Action_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 2,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
//	cell.LowLim = 0;//ch num
//	cell.HighLim = 0;//freq ch
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Freq 1";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = WR_Att + Action_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 2,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
//	cell.LowLim = 0;//ch num
//	cell.HighLim = 1;//freq ch
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Freq 2";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = WR_Att + Action_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 2,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
//	cell.LowLim = 0;//ch num
//	cell.HighLim = 2;//freq ch
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Freq 3";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = WR_Att + Action_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 2,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
//	cell.LowLim = 0;//ch num
//	cell.HighLim = 3;//freq ch
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Freq 4";
//	p_cell[i++] = cell;
//
//
//	cell.CellAttr = 0;
//	cell.CellType = FixIntegerCellType + 1;
//	cell.VarPtr = NULL;
//	cell.ReadProc = NULL;
//	cell.WriteProc = NULL;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.CellNumber = num++;
//	cell.LowLim = 0;
//	cell.DescriptStr = "Emit 2 cntrl";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 1,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_pwr_en;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_pwr_en;
//	cell.LowLim = 1;//ch num
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "En. Emit 2";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = WR_Att + Action_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 2,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
//	cell.LowLim = 1;//ch num
//	cell.HighLim = 0;//freq ch
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Freq 1";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = WR_Att + Action_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 2,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
//	cell.LowLim = 1;//ch num
//	cell.HighLim = 1;//freq ch
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Freq 2";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = WR_Att + Action_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 2,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
//	cell.LowLim = 1;//ch num
//	cell.HighLim = 2;//freq ch
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Freq 3";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = WR_Att + Action_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 2,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_emit_freq;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_emit_freq;
//	cell.LowLim = 1;//ch num
//	cell.HighLim = 3;//freq ch
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Freq 4";
//	p_cell[i++] = cell;
//
//
//	if(p_ctl->dev_config.rec.tmp[1]){
//
//		cell.CellAttr = 0;
//		cell.CellType = FixIntegerCellType + 1;
//		cell.VarPtr = NULL;
//		cell.ReadProc = NULL;
//		cell.WriteProc = NULL;
//		cell.HighLim = 0;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.CellNumber = num++;
//		cell.LowLim = 0;
//		cell.DescriptStr = "Rotate tune";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
//		cell.CellType = FixIntegerCellType + 1;
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = NULL;
//		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
//		cell.LowLim = povorot_cntrl_cmd_step_left;
//		cell.HighLim = 0;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "Rot st L.";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
//		cell.CellType = FixIntegerCellType + 1;
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = NULL;
//		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
//		cell.LowLim = povorot_cntrl_cmd_step_right;
//		cell.HighLim = 0;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "Rot st R.";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
//		cell.CellType = FixIntegerCellType + 1;
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = NULL;
//		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
//		cell.LowLim = povorot_cntrl_cmd_step_up;
//		cell.HighLim = 0;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "Rot st U.";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
//		cell.CellType = FixIntegerCellType + 1;
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = NULL;
//		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
//		cell.LowLim = povorot_cntrl_cmd_step_down;
//		cell.HighLim = 0;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "Rot st D.";
//		p_cell[i++] = cell;
//
//	}
}
#endif
















#ifdef RECEIVER_EXT_MODE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main_cntrl_make_html_cntrl_table(control_struct* p_ctl, ctrl_table_list_struct* p_list){
SettingCell_t* p_cell;
uint8_t num=1;
SettingCell_t 	cell;
uint32_t 		i;

memset(&p_ctl->main_config_table,0,sizeof(p_ctl->main_config_table));
p_list->name = "Main_cntrl";
p_list->mode = div_time_att;// | div_collect_mode;//div_collect_mode
//div_all_wr | div_collect_mode;// | div_collect_mode_wr;//div_all_rd
p_list->p_start_cell = &p_ctl->main_config_table[0];
p_cell = p_list->p_start_cell;

cmd_h_add_to_cell_table_ext_size(
						p_cell,
						num++,
						p_ctl,
						NULL,
						NULL,
						0,
						FixIntegerCellType,
						"Control");

	i = 1;

	cell.CellAttr = RD_Att;
	cell.CellType = VarStringCellType + 31;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_dbg_get_rotate_state;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)&p_ctl->povorot;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Rotate state";
	p_cell[i++] = cell;

	cell.CellAttr = RD_Att;
	cell.CellType = VarStringCellType + 31;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_freq;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Freq MHz";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att + Ptr_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_alarm_state_bin;
	cell.WriteProc = NULL;
	cell.LowLim = 5;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Alarm!!!";
	p_cell[i++] = cell;

	cell.CellAttr = RD_Att + Action_Att;
	cell.CellType = FixIntegerCellType + 4,
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_frame_cntr;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "Frame cntr";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_freq_up;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Up";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_freq_down;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Down";
	p_cell[i++] = cell;


	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_liter_up;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Lit. Up";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_liter_down;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Lit. Down";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_ch_up;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Ch Up";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_ch_down;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Ch Down";
	p_cell[i++] = cell;

//work mode


	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_recv_mode;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_recv_mode;
	cell.LowLim = 1;
	cell.HighLim = cntr_recv_mode_1_2;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "sel 3.3";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_recv_mode;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_recv_mode;
	cell.LowLim = 1;
	cell.HighLim = cntr_recv_mode_5_8;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "sel 4.2";
	p_cell[i++] = cell;


	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_recv_mode;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_recv_mode;
	cell.LowLim = 1;
	cell.HighLim = cntr_recv_mode_1_2_5_8;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "sel 3.3_4.2";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_recv_mode;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_recv_mode;
	cell.LowLim = 1;
	cell.HighLim = cntr_recv_mode_5_8_1_2;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "sel 4.2_3.3";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_recv_mode;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_recv_mode;
	cell.LowLim = 1;
	cell.HighLim = cntr_recv_mode_1_2_and_5_8;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "sel 4.2+3.3";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_ch_auto_scan;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_ch_auto_scan;
	cell.LowLim = 1;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Auto ch scan en";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_rotate_auto_scan;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_auto_scan;
	cell.LowLim = 1;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Auto rotate scan en";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_auto_scan;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_auto_scan;
	cell.LowLim = 1;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Auto scan";
	p_cell[i++] = cell;


//	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 1,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_collect_mode;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_collect_mode;
//	cell.LowLim = 1;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Auto 1.2+5.8";
//	p_cell[i++] = cell;
//#ifdef RECV_1_2
//	if(p_ctl->dev_config.rec.tmp[0]){
////		cell.CellAttr = WR_Att + Action_Att + RD_Att,
//		cell.CellAttr = WR_Att + Action_Att,
//		cell.CellType = FixIntegerCellType + 2,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_5_8_freq;
//		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_5_8_freq;
//		cell.LowLim = 0;
//		cell.HighLim = 0;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "freq 5.8 MHz";
//		p_cell[i++] = cell;
//	}
//#endif

	if(p_ctl->dev_config.rec.tmp[1]){
		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
		cell.LowLim = povorot_cntrl_cmd_step_left;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rot st L.";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
		cell.LowLim = povorot_cntrl_cmd_step_right;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rot st R.";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
		cell.LowLim = povorot_cntrl_cmd_step_up;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rot st U.";
		p_cell[i++] = cell;

		cell.CellAttr = Action_Att + WR_Att + Default_Attr;
		cell.CellType = FixIntegerCellType + 1;
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_rotate_make_step;
		cell.LowLim = povorot_cntrl_cmd_step_down;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rot st D.";
		p_cell[i++] = cell;

	}

#ifndef NOT_VIDEO_RECEIVER
	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_en_image;
	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_en_image;
	cell.LowLim = 1;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "Image ext";
	p_cell[i++] = cell;
#endif

#ifdef RECV_5_8_RSSI
//	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 1,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_en_fast_scan;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_en_fast_scan;
//	cell.LowLim = 1;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Fast scan";
//	p_cell[i++] = cell;
#endif

}
#endif

 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 #ifdef RECEIVER_EXT_MODE

  void main_cntrl_cmd_parser(control_struct *p_ctl, cntrl_dev_sys_msg_que_type_s *p_msg){

 	 p_ctl->alarm_mode = cntrl_alarm_dis;

 	switch(p_msg->cmd){

 	case main_cntrl_cmd_sel_recv_mode:
 #ifndef NOT_VIDEO_RECEIVER
 		cntrl_sel_recv_mode(p_ctl, (recv_mode_e)p_msg->buf[0]);
 #endif
 	break;

 	case main_cntrl_cmd_freq:
 //		printf("cmd freq = %d \r\n",*(uint16_t*)&p_msg->buf[0]);
 #ifdef RTC6715
 #ifdef RECV_5_8_RSSI
 		if(p_ctl->rtc6715.rssi.state != recv_5_8_rssi_state_stop){
 			recv_5_8_stop_rssi(&p_ctl->rtc6715);
 		}
 #endif
 		rtc6715_set_freq(&p_ctl->rtc6715, *(uint16_t*)&p_msg->buf[0]);
 #endif

 	break;
 	case main_cntrl_cmd_freq_up:

 		p_ctl->freq_mode = cntrl_freq_mode_manual;
 		p_ctl->auto_mode = cntrl_auto_mode_dis;
#ifdef	RECV_3_4_I2C
 		if(p_ctl->recv_active == cmd_recv_active_1_2){
 			i2c_recv_set_freq_up(&p_ctl->i2c_recv_3_4,p_ctl->i2c_recv_3_4.p_cfg->manual_step_freq);
 		}
#endif
#ifdef	RECV_4_5_I2C
 		if(p_ctl->recv_active == cmd_recv_active_5_8){
 			i2c_recv_set_freq_up(&p_ctl->i2c_recv_4_5,p_ctl->i2c_recv_4_5.p_cfg->manual_step_freq);
 		}
#endif
 	break;
 	case main_cntrl_cmd_liter_up:

 		p_ctl->freq_mode = cntrl_freq_mode_manual;
 		p_ctl->auto_mode = cntrl_auto_mode_dis;

 		if(p_ctl->recv_active == cmd_recv_active_1_2){
 #ifdef RECV_4_5_I2C
 			i2c_recv_set_liter_up(&p_ctl->i2c_recv_3_4);
 #endif
 		}
 		if(p_ctl->recv_active == cmd_recv_active_5_8){
 #ifdef RECV_4_5_I2C
 			i2c_recv_set_liter_up(&p_ctl->i2c_recv_4_5);
 #endif
 		}
 	break;

 	case main_cntrl_cmd_liter_down:

 		p_ctl->freq_mode = cntrl_freq_mode_manual;
 		p_ctl->auto_mode = cntrl_auto_mode_dis;

 		if(p_ctl->recv_active == cmd_recv_active_1_2){
 #ifdef RECV_4_5_I2C
 			i2c_recv_set_liter_down(&p_ctl->i2c_recv_3_4);
 #endif
 		}
 		if(p_ctl->recv_active == cmd_recv_active_5_8){
 #ifdef RECV_4_5_I2C
 			i2c_recv_set_liter_down(&p_ctl->i2c_recv_4_5);
 #endif
 		}

//
//
// 		if(p_ctl->recv_active == cmd_recv_active_5_8){
// #ifdef RTC6715
// 			recv_5_8_set_liter_down(&p_ctl->rtc6715);
// #endif
// 		}


 	break;
 	case main_cntrl_cmd_ch_down:

 		p_ctl->freq_mode = cntrl_freq_mode_manual;
 		p_ctl->auto_mode = cntrl_auto_mode_dis;

 		if(p_ctl->recv_active == cmd_recv_active_1_2){
 #ifdef RECV_4_5_I2C
 			i2c_recv_set_ch_down(&p_ctl->i2c_recv_3_4);
 #endif
 		}
 		if(p_ctl->recv_active == cmd_recv_active_5_8){
 #ifdef RECV_4_5_I2C
 			i2c_recv_set_ch_down(&p_ctl->i2c_recv_4_5);
 #endif
 		}

//
// 		if(p_ctl->recv_active == cmd_recv_active_5_8){
// #ifdef RTC6715
// 			recv_5_8_set_ch_down(&p_ctl->rtc6715);
// #endif
// 		}
//

 	break;
 	case main_cntrl_cmd_ch_up:

 		p_ctl->freq_mode = cntrl_freq_mode_manual;
 		p_ctl->auto_mode = cntrl_auto_mode_dis;

 		if(p_ctl->recv_active == cmd_recv_active_1_2){
 #ifdef RECV_4_5_I2C
 			i2c_recv_set_ch_up(&p_ctl->i2c_recv_3_4);
 #endif
 		}
 		if(p_ctl->recv_active == cmd_recv_active_5_8){
 #ifdef RECV_4_5_I2C
 			i2c_recv_set_ch_up(&p_ctl->i2c_recv_4_5);
 #endif
 		}

// 		if(p_ctl->recv_active == cmd_recv_active_5_8){
// #ifdef RTC6715
// 			recv_5_8_set_ch_up(&p_ctl->rtc6715);
// #endif
// 		}


 	break;

 	case main_cntrl_cmd_freq_down:

 		p_ctl->freq_mode = cntrl_freq_mode_manual;
 		p_ctl->auto_mode = cntrl_auto_mode_dis;


 		if(p_ctl->recv_active == cmd_recv_active_1_2){
 #ifdef RECV_4_5_I2C
 			i2c_recv_set_freq_down(&p_ctl->i2c_recv_3_4,p_ctl->i2c_recv_3_4.p_cfg->manual_step_freq);
 #endif
 		}
 		if(p_ctl->recv_active == cmd_recv_active_5_8){
 #ifdef RECV_4_5_I2C
 			i2c_recv_set_freq_down(&p_ctl->i2c_recv_4_5,p_ctl->i2c_recv_4_5.p_cfg->manual_step_freq);
 #endif
 		}
//
// #ifdef RECV_1_2
// 		if(p_ctl->recv_active == cmd_recv_active_1_2){
// 			recv_1_2_set_freq_down(&p_ctl->recv_1_2,p_ctl->recv_1_2.p_cfg->manual_step_freq);
// 		}
// #endif
// 		if(p_ctl->recv_active == cmd_recv_active_5_8){
// #ifdef RTC6715
// 			recv_5_8_set_freq_down(&p_ctl->rtc6715,p_ctl->rtc6715.p_cfg->manual_step_freq);
// #endif
// 		}

 	break;
 	case main_cntrl_cmd_auto_scan_ch_en:
 		p_ctl->freq_mode = cntrl_freq_mode_auto;
 		p_ctl->ch_time = 0;

 		if ((p_ctl->recv_mode == cntr_recv_mode_5_8) || (p_ctl->recv_mode == cntr_recv_mode_1_2_5_8) || (p_ctl->recv_mode == cntr_recv_mode_5_8_1_2) || (p_ctl->recv_mode == cntr_recv_mode_1_2_and_5_8)){

 #ifdef	RTC6715
 #ifdef 	RECV_5_8_RSSI
 			if(p_ctl->rtc6715.p_cfg->b_en_fast_scan){
 				recv_5_8_start_rssi(&p_ctl->rtc6715,1);
 			}
 #endif
 #endif
 		}

 	break;

 	case main_cntrl_cmd_auto_scan_ch_dis:
 		p_ctl->freq_mode = cntrl_freq_mode_manual;
 		p_ctl->ch_time = 0;

 #ifdef	RTC6715
 #ifdef RECV_5_8_RSSI
 		recv_5_8_stop_rssi(&p_ctl->rtc6715);
 #endif
 #endif

 	break;
 	case main_cntrl_cmd_auto_scan_en:
 		p_ctl->auto_mode = cntrl_auto_mode_en;

 		switch(p_ctl->recv_mode){

 		case cntr_recv_mode_1_2:
 #ifndef NOT_VIDEO_RECEIVER
 			TVP_interrut_start(&p_ctl->tvp_5150_master);
 #endif
 #ifndef		NO_SEC_CH
 			TVP_interrut_stop(&p_ctl->tvp_5150_slave);
 #endif
 		break;

 		case cntr_recv_mode_5_8:
 #ifndef NOT_VIDEO_RECEIVER
 			TVP_interrut_start(&p_ctl->tvp_5150_master);
 #endif
 #ifndef		NO_SEC_CH
 			TVP_interrut_stop(&p_ctl->tvp_5150_slave);
 #endif
 		break;

 		case cntr_recv_mode_1_2_5_8:
 #ifndef NOT_VIDEO_RECEIVER
 			TVP_interrut_start(&p_ctl->tvp_5150_master);
 #endif
 #ifndef		NO_SEC_CH
 			TVP_interrut_start(&p_ctl->tvp_5150_slave);
 #endif
 		break;

 		case cntr_recv_mode_5_8_1_2:
 #ifndef NOT_VIDEO_RECEIVER
 			TVP_interrut_start(&p_ctl->tvp_5150_master);
 #endif
 #ifndef		NO_SEC_CH
 			TVP_interrut_start(&p_ctl->tvp_5150_slave);
 #endif
 		break;

 		case cntr_recv_mode_1_2_and_5_8:
 #ifndef NOT_VIDEO_RECEIVER
 			TVP_interrut_start(&p_ctl->tvp_5150_master);
 #endif
 #ifndef		NO_SEC_CH
 			TVP_interrut_start(&p_ctl->tvp_5150_slave);
 #endif
 		break;
 		}

 	break;
 	case main_cntrl_cmd_auto_scan_dis:
 		p_ctl->auto_mode = cntrl_auto_mode_dis;
 	break;
 #ifdef		RTC6705
 	case main_cntrl_cmd_emit_freq:
 uint16_t freq_e;
 		freq_e = *(uint16_t*)&p_msg->buf[0];

 		if((p_ctl->recv_mode != cntr_recv_mode_emit) && (freq_e)){
 			cntrl_sel_recv_mode(p_ctl, cntr_recv_mode_emit);
 		}
 		else{
 			if((p_ctl->recv_mode == cntr_recv_mode_emit) && (!freq_e)){
 				cntrl_sel_recv_mode(p_ctl, cntr_recv_mode_5_8);
 			}
 		}
 		if(p_ctl->recv_mode == cntr_recv_mode_emit){
 			p_ctl->freq_mode = cntrl_freq_mode_manual;//remove???
 			p_ctl->auto_mode = cntrl_auto_mode_dis;

 			rtc6705_set_freq(&p_ctl->rtc6705,freq_e);
 			rtc6715_set_freq(&p_ctl->rtc6715, freq_e);
 		}

 	break;
 #endif

 	}
 	osPoolFree(p_ctl->main_ctrl_cmd.cntrl_msg_pool_id,p_msg);
 }
 #endif
