/*
 * rtc6715.c
 *
 *  Created on: 17.04.2014
 *      Author: krasikov
 */
#include "stdio.h"
#include "string.h"
//#include "comm.h"
#include "rtc6715.h"
#include "control.h"

#ifdef RECV_5_8_BASE_CH
const  uint16_t ch_5_8_freq[RECV_5_8_MAX_NUM_CH] = {
		 	 	 	 	 5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725,//A
		 	 	 	 	 5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866,//B
						 5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945,//E
						 5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880,//F
						 5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917,//R
						 5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621,//L
						 5653, 5693, 5733, 5773, 5813, 5853, 5893, 5933,//H
						 5333, 5373, 5413, 5453, 5493, 5533, 5573, 5613,//l
						 5325, 5348, 5366, 5384, 5402, 5420, 5438, 5456,//U
						 5474, 5492, 5510, 5528, 5546, 5564, 5582, 5600,//O
						 4990, 5020, 5050, 5080, 5110, 5140, 5170, 5200,//X
						 6002, 6028, 6054, 6080, 6106, 6132, 6158, 6184//Z
 	 	 	 	 	 	 };
 const  char *ch_5_8_table[RECV_5_8_MAX_NUM_CH] = {
		 	 	 	 	 "A1","A2","A3","A4","A5","A6","A7","A8", //1
						 "B1","B2","B3","B4","B5","B6","B7","B8",//2
						 "E1","E2","E3","E4","E5","E6","E7","E8",//3
						 "F1","F2","F3","F4","F5","F6","F7","F8",//4
						 "R1","R2","R3","R4","R5","R6","R7","R8",//5
						 "L1","L2","L3","L4","L5","L6","L7","L8",//6
						 "H1","H2","H3","H4","H5","H6","H7","H8",//7
						 "l1","l2","l3","l4","l5","l6","l7","l8",//8
						 "U1","U2","U3","U4","U5","U6","U7","U8",//9
						 "O1","O2","O3","O4","O5","O6","O7","O8",//10
						 "X1","X2","X3","X4","X5","X6","X7","X8",//11
						 "Z1","Z2","Z3","Z4","Z5","Z6","Z7","Z8",//12
 };
 const  char *ch_5_8_lit_table[RECV_5_8_MAX_NUM_LIT] = {
		 	 	 	 	 "A","B","E","F","R","L","H","l","U","O","X","Z" };
#endif

#ifdef RECV_5_8_SUB_3_3

#ifdef RECV_5_8_EXT_CH
const  uint16_t ch_5_8_freq[RECV_5_8_MAX_NUM_CH] = {
		 	 	 	 	 3200, 3220, 3240, 3260, 3280, 3300, 3320, 3340,//A
		 	 	 	 	 3360, 3380, 3400, 3420, 3440, 3460, 3480, 3500,//B
						 3520, 3540, 3560, 3580, 3600, 3620, 3640, 3680,//C
						 3210, 3250, 3290, 3330, 3370, 3410, 3450, 3490,//D
						 3230, 3290, 3350, 3410, 3470, 3530, 3590, 3700,//E
						 3110, 3120, 3130, 3140, 3150, 3160, 3180, 3190,//D
						 3710, 3720, 3730, 3740, 3750, 3760, 3780, 3790,//L
//						 5653, 5693, 5733, 5773, 5813, 5853, 5893, 5933,//H
//						 5333, 5373, 5413, 5453, 5493, 5533, 5573, 5613,//l
//						 5325, 5348, 5366, 5384, 5402, 5420, 5438, 5456,//U
//						 5474, 5492, 5510, 5528, 5546, 5564, 5582, 5600,//O
//						 4990, 5020, 5050, 5080, 5110, 5140, 5170, 5200,//X
//						 4867, 4884, 4921, 4958, 4995, 5032, 5069, 5099,//J
//						 5960, 5980, 6000, 6020, 6040, 6060, 6080, 6100,//K
//						 6002, 6028, 6054, 6080, 6106, 6132, 6158, 6184//Z
 	 	 	 	 	 	 };
 const  char *ch_5_8_table[RECV_5_8_MAX_NUM_CH] = {
		 	 	 	 	 "A1","A2","A3","A4","A5","A6","A7","A8", //1
						 "B1","B2","B3","B4","B5","B6","B7","B8",//2
						 "C1","C2","C3","C4","C5","C6","C7","C8",//3
						 "D1","D2","D3","D4","D5","D6","D7","D8",//4
						 "E1","E2","E3","E4","E5","E6","E7","E8",//5
						 "F1","F2","F3","F4","F5","F6","F7","F8",//6
						 "G1","G2","G3","G4","G5","G6","G7","G8",//7
//						 "H1","H2","H3","H4","H5","H6","H7","H8",//8
//						 "l1","l2","l3","l4","l5","l6","l7","l8",//9
//						 "U1","U2","U3","U4","U5","U6","U7","U8",//10
//						 "O1","O2","O3","O4","O5","O6","O7","O8",//11
//						 "X1","X2","X3","X4","X5","X6","X7","X8",//12
//						 "J1","J2","J3","J4","J5","J6","J7","J8",//13
//						 "K1","K2","K3","K4","K5","K6","K7","K8",//14
//						 "Z1","Z2","Z3","Z4","Z5","Z6","Z7","Z8",//15
 };
 const  char *ch_5_8_lit_table[RECV_5_8_MAX_NUM_LIT] = {
//		 	 	 	 	 "A","B","E","F","R","D","L","H","l","U","O","X","J","K","Z" };
		 	 	 	 	 "A","B","C","D","E","F","G"};
#endif

#else
#ifdef RECV_5_8_EXT_CH
const  uint16_t ch_5_8_freq[RECV_5_8_MAX_NUM_CH] = {
		 	 	 	 	 5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725,//A
		 	 	 	 	 5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866,//B
						 5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945,//E
						 5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880,//F
						 5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917,//R
//						 5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621,//D
						 5210, 5230, 5250, 5270, 5300, 5330, 5350, 5370,//D
						 5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621,//L
						 5653, 5693, 5733, 5773, 5813, 5853, 5893, 5933,//H
						 5333, 5373, 5413, 5453, 5493, 5533, 5573, 5613,//l
						 5325, 5348, 5366, 5384, 5402, 5420, 5438, 5456,//U
						 5474, 5492, 5510, 5528, 5546, 5564, 5582, 5600,//O
						 4990, 5020, 5050, 5080, 5110, 5140, 5170, 5200,//X
						 4867, 4884, 4921, 4958, 4995, 5032, 5069, 5099,//J
						 5960, 5980, 6000, 6020, 6040, 6060, 6080, 6100,//K
						 6002, 6028, 6054, 6080, 6106, 6132, 6158, 6184//Z
 	 	 	 	 	 	 };
 const  char *ch_5_8_table[RECV_5_8_MAX_NUM_CH] = {
		 	 	 	 	 "A1","A2","A3","A4","A5","A6","A7","A8", //1
						 "B1","B2","B3","B4","B5","B6","B7","B8",//2
						 "E1","E2","E3","E4","E5","E6","E7","E8",//3
						 "F1","F2","F3","F4","F5","F6","F7","F8",//4
						 "R1","R2","R3","R4","R5","R6","R7","R8",//5
						 "D1","D2","D3","D4","D5","D6","D7","D8",//6
						 "L1","L2","L3","L4","L5","L6","L7","L8",//7
						 "H1","H2","H3","H4","H5","H6","H7","H8",//8
						 "l1","l2","l3","l4","l5","l6","l7","l8",//9
						 "U1","U2","U3","U4","U5","U6","U7","U8",//10
						 "O1","O2","O3","O4","O5","O6","O7","O8",//11
						 "X1","X2","X3","X4","X5","X6","X7","X8",//12
						 "J1","J2","J3","J4","J5","J6","J7","J8",//13
						 "K1","K2","K3","K4","K5","K6","K7","K8",//14
						 "Z1","Z2","Z3","Z4","Z5","Z6","Z7","Z8",//15
 };
 const  char *ch_5_8_lit_table[RECV_5_8_MAX_NUM_LIT] = {
		 	 	 	 	 "A","B","E","F","R","D","L","H","l","U","O","X","J","K","Z" };
#endif
#endif


 uint32_t recv_5_8_chec_dis_lit_ch(rtc6715_struct *p_ctl, uint8_t ch);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void rtc6715_start_init(rtc6715_struct *p_ad){

	p_ad->error = 0;
	p_ad->addr = 0;

	rtc6715_start_pin_init(p_ad);

	p_ad->reg.reg0.word =  0x08;
	p_ad->reg.reg1.word =  0x02A05;
	p_ad->reg.reg2.word =  0xFFE44;
	p_ad->reg.reg3.word =  0x03980;
	p_ad->reg.reg4.word =  0x7ABEF;
	p_ad->reg.reg5.word =  0x7E1D2;
	p_ad->reg.reg6.word =  0x82408;
	p_ad->reg.reg7.word =  0x82408;
	p_ad->reg.reg8.word =  0x0FF80;
	p_ad->reg.reg9.word =  0xB2007;
	p_ad->reg.regA.word =  0x10C13;

	p_ad->freq = RTC6715_START_FREQ;
	p_ad->ch_cntr = 0;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void rtc6715_call_back_stop_send_data(void *p){
//rtc6715_struct *p_ad;
//	if(p!= NULL){
//		p_ad = p;
//		HAL_GPIO_WritePin(p_ad->CS_port, p_ad->CS_pin, GPIO_PIN_SET);
//		osMutexWait (p_ad->mutex_write_id, osWaitForever);
//		if (p_ad->wr_cntr) {
//			p_ad->wr_cntr--;
//			if((!p_ad->wr_cntr) && (p_ad->b_os_signal_data_send) && (p_ad->os_signal_all_data_send)){
//				osSignalSet(p_ad->os_thread_id,p_ad->os_signal_all_data_send);
//			}
//		}
//		osMutexRelease (p_ad->mutex_write_id);
//	}
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void rtc6715_call_back_start_send_data(void *p){
//rtc6715_struct *p_ad;
//	if(p!= NULL){
//		p_ad = p;
//		HAL_GPIO_WritePin(p_ad->CS_port, p_ad->CS_pin, GPIO_PIN_RESET);
//	}
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void rtc6715_start_pin_init(rtc6715_struct *p_ad){

	gpio_init_pin_sun((struct gpio_t_ *)&p_ad->cs_pin);
	gpio_init_pin_sun((struct gpio_t_ *)&p_ad->mosi_pin);
	gpio_init_pin_sun((struct gpio_t_ *)&p_ad->clk_pin);

//GPIO_InitTypeDef GPIO_InitStructure;
//
//	GPIO_InitStructure.Pin = p_ad->cs_pin;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStructure.Pull = GPIO_PULLUP;
//	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
//	HAL_GPIO_Init(p_ad->cs_port, &GPIO_InitStructure);
//
//	GPIO_InitStructure.Pin = p_ad->mosi_pin;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStructure.Pull = GPIO_PULLUP;
//	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
//	HAL_GPIO_Init(p_ad->mosi_port, &GPIO_InitStructure);
//
//	GPIO_InitStructure.Pin = p_ad->clk_pin;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStructure.Pull = GPIO_PULLUP;
//	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
//	HAL_GPIO_Init(p_ad->clk_port, &GPIO_InitStructure);

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_default_init(recv_5_8_cfg_struct *p_cfg){

	memset(p_cfg,0,sizeof(recv_5_8_cfg_struct));
	p_cfg->min_freq = RECV_5_8_MIN_FREQ;
	p_cfg->max_freq = RECV_5_8_MAX_FREQ;
	p_cfg->auto_step_freq =  RECV_5_8_AUTO_FREQ;
	p_cfg->manual_step_freq =  RECV_5_8_MANUAL_FREQ;

	p_cfg->time = 5;
	p_cfg->en_lit = 0xFFFF;
	p_cfg->b_en_fast_scan = 1;
	p_cfg->view_mode = recv_5_8_view_mode_normal;

	p_cfg->lvl = 0x30;

}
#ifdef RECV_5_8_RSSI
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_rssi_timer_callback(void const *arg){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *) arg;
	osSignalSet(p_ctl->rssi.thread_id, p_ctl->rssi.signal);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_rssi_init(rtc6715_struct *p_ctl,osThreadId thread_id,uint32_t signal ){

	p_ctl->rssi.global_cntrl.start_azimut = RTC6715_RSSI_START_AZIMUT;
	p_ctl->rssi.global_cntrl.stop_azimut = RTC6715_RSSI_STOP_AZIMUT;
	p_ctl->rssi.global_cntrl.num_azimut_point = RTC6715_RSSI_NUM_AZIMUT_POINT;
	p_ctl->rssi.global_cntrl.start_freq = RTC6715_RSSI_START_FREQ;
	p_ctl->rssi.global_cntrl.stop_freq = RTC6715_RSSI_STOP_FREQ;
	p_ctl->rssi.global_cntrl.num_freq_point = RTC6715_RSSI_NUM_FREQ_POINT;

	p_ctl->rssi.meas_cntrl.start_freq = RTC6715_RSSI_START_FREQ;
	p_ctl->rssi.meas_cntrl.stop_freq = RTC6715_RSSI_STOP_FREQ;
	p_ctl->rssi.meas_cntrl.delta_freq = 10;
	p_ctl->rssi.meas_cntrl.num_freq_point = RTC6715_RSSI_NUM_FREQ_POINT;//110;
	p_ctl->rssi.delay_answer = 40;//msek
	p_ctl->rssi.buf_cntr = 0;
	p_ctl->rssi.num_point = RECV_5_8_RSSI_BUF_SIZE;

	p_ctl->rssi.os_timer_def.ptimer = recv_5_8_rssi_timer_callback;//&p_parser->os_timer_cb_timer_name;
	p_ctl->rssi.os_timer_def.timer = p_ctl->rssi.os_timer;
	p_ctl->rssi.timer_id = osTimerCreate (&p_ctl->rssi.os_timer_def, osTimerOnce, p_ctl);

	p_ctl->rssi.thread_id = thread_id;
	p_ctl->rssi.signal = signal;
	p_ctl->rssi.b_continue_restart = 0;
	p_ctl->rssi.b_check_video = 0;
	p_ctl->rssi.b_en = 0;

	p_ctl->rssi.state = recv_5_8_rssi_state_stop;
//	p_ctl->rssi.p_uni = NULL;
	p_ctl->rssi.p_tcp_cmd = NULL;

	p_ctl->rssi.rssi_mode = recv_5_8_rssi_mode_slow;
	p_ctl->rssi.rssi_now_mode = recv_5_8_rssi_now_mode_slow;

	p_ctl->rssi.fast.fast_cntr = 0;
	p_ctl->rssi.fast.slow_cntr = 0;

	memset(p_ctl->rssi.disable_buf,0,sizeof(p_ctl->rssi.disable_buf));

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t rtc6715_get_freq_from_table(rtc6715_struct *p_ctl, uint16_t* p_freq){
uint32_t res=1;
uint8_t lvl,val;
//uint16_t freq;
uint16_t p,b,o;
	while(p_ctl->rssi.buf_cntr <= RECV_5_8_RSSI_BUF_SIZE){

		p = p_ctl->rssi.meas_cntrl.azimut/10;
		p = p * p_ctl->rssi.meas_cntrl.num_freq_point;
		p = p + p_ctl->rssi.buf_cntr;
		b = p / 8;
		o = p % 8;
//		if(o) b++;
		val = p_ctl->rssi.disable_buf[b] & (1 << o);
		if(!val){
			lvl = p_ctl->rssi.data_buf[p_ctl->rssi.buf_cntr];
			if(lvl > p_ctl->p_cfg->lvl){
				*p_freq = p_ctl->rssi.meas_cntrl.start_freq + p_ctl->rssi.buf_cntr * p_ctl->rssi.meas_cntrl.delta_freq;
				p_ctl->rssi.buf_cntr++;
				return 0;
			}
		}
		else{
//			printf("miss=%d %d %d\r\n",p,b,o);
		}
		p_ctl->rssi.buf_cntr++;
	}
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t recv_5_8_get_rssi(rtc6715_struct* p_ctl) {
uint16_t v,v1,i,j,temp;
volatile uint16_t* arr;
uint8_t n;
	arr = p_ctl->rssi.p_adc_buf;
	n = p_ctl->rssi.adc_buf_num_point;
	if (HAL_ADC_Start_DMA_No_DMA_It(p_ctl->rssi.p_adc,
				(uint32_t *)p_ctl->rssi.p_adc_buf,
				p_ctl->rssi.adc_buf_num_point) != HAL_OK){
	}

	delay_us_del(200);

	for (i = n - 1; i > 0; i--) {
		for (j = 0; j < i; j++){
			v = arr[j];
			v1 = arr[j+1];
			if (v > v1) {
				temp = v;
				arr[j] = v1;
				arr[j + 1] = temp;
			}
		}
	}
	return arr[n/2]>>4;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_start_rssi(rtc6715_struct* p_ctl, uint8_t b_restart){
	p_ctl->rssi.buf_cntr = 0;
	p_ctl->rssi.state = recv_5_8_rssi_state_start;
	p_ctl->rssi.base_freq = p_ctl->freq;
	p_ctl->rssi.freq = p_ctl->rssi.meas_cntrl.start_freq;
	p_ctl->rssi.b_continue_restart = 1;//b_restart;
	p_ctl->rssi.b_check_video = 1;
	p_ctl->rssi.b_en = 1;
	recv_5_8_continue_rssi(p_ctl);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_send_fast_rssi(rtc6715_struct* p_ctl){
struct pbuf *p_buf;
uint8_t tmp[2];
//uint8_t ack_max_len = 255;
uni_prot_addr_param_s addr;
uint16_t len;
uint8_t tmp_data[5];

tcp_cmd_msg_s* p_ack_msg;
uint8_t* p_out_data;
uint16_t out_len;

addr.dst_addr_lit = 127;
addr.src_addr_lit = 1;
addr.version = uni_prot_header_version_1;
addr.dst_ch = 1;
addr.src_ch = 1;

//return;
//	if((p_ctl->rssi.p_uni==NULL)||(!p_ctl->rssi.dest_port)) return;

	if((p_ctl->rssi.p_tcp_cmd==NULL)) return;

	p_ack_msg = tcp_cmd_make_ack_start(p_ctl->rssi.p_tcp_cmd,&p_ctl->rssi.p_tcp_cmd->def_addr);
	if(p_ack_msg == NULL) return;

	p_out_data = &p_ack_msg->data.p_data[p_ack_msg->data.cmd_len];

	p_out_data[0] = 0x8E;
	p_out_data[1] = p_ctl->rssi.fast.num_freq_point;

	memcpy(&p_out_data[2],&p_ctl->rssi.fast.freq,sizeof(p_ctl->rssi.fast.freq));
	out_len = 2 + sizeof(p_ctl->rssi.fast.freq);

	memcpy(&p_out_data[out_len],&p_ctl->rssi.fast.data,sizeof(p_ctl->rssi.fast.data));
	out_len += sizeof(p_ctl->rssi.fast.data);

	p_ack_msg->data.cmd_len = p_ack_msg->data.cmd_len + out_len;
	tcp_cmd_make_ack_finish(p_ctl->rssi.p_tcp_cmd,p_ack_msg,ANS_NO,0,NULL);

	return;


//
//	len = 5 + 2 + sizeof(p_ctl->rssi.fast.freq) + sizeof(p_ctl->rssi.fast.data);
//
//	tmp_data[1] = 0x80;
//	tmp_data[2] = addr.version;
//	tmp_data[3] = addr.src_addr_lit;
//	tmp_data[4] = addr.dst_addr_lit;
//	tmp_data[0] = len;
//	tmp_data[1] |= ((uint32_t)(len))>>8;
//
//	p_buf = pbuf_alloc(PBUF_TRANSPORT,len, PBUF_RAM);
//	if(p_buf != NULL){
//		tmp[0] = 0x8E;
//		tmp[1] = p_ctl->rssi.fast.num_freq_point;
//
//		pbuf_take(p_buf,&tmp_data,5);
//		pbuf_take_at(p_buf,tmp,2,5);
//		pbuf_take_at(p_buf,&p_ctl->rssi.fast.freq,sizeof(p_ctl->rssi.fast.freq),2+5);
//		pbuf_take_at(p_buf,&p_ctl->rssi.fast.data,sizeof(p_ctl->rssi.fast.data),5 + 2 + sizeof(p_ctl->rssi.fast.freq));
//
//		struct netbuf  *xNetBuf = netbuf_new ();
//		if(xNetBuf==NULL) {
//			pbuf_free(p_buf);
//			return;
//		}
//
//		xNetBuf->p = p_buf;
//		xNetBuf->ptr = xNetBuf->p;
//
////		netconn_sendto(p_ctl->rssi.p_uni->p_conn->udp.p_conn,xNetBuf,&p_ctl->rssi.dest_ip,p_ctl->rssi.dest_port);
//		netbuf_delete(xNetBuf);
//		return;
//


//		uni_prot_thread_send_buf(p_ctl->rssi.p_uni,p_buf->len,p_buf->payload,&addr);
//		pbuf_free(p_buf);
//	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_send_rssi(rtc6715_struct* p_ctl){
struct pbuf *p_buf;
uint8_t tmp[2];
uint16_t len;
//uint8_t ack_max_len = 255;
uni_prot_addr_param_s addr;
addr.dst_addr_lit = 127;
addr.src_addr_lit = 1;
addr.version = uni_prot_header_version_1;
addr.dst_ch = 1;
addr.src_ch = 1;
uint8_t tmp_data[5];
tcp_cmd_msg_s* p_ack_msg;
uint8_t* p_out_data;
uint16_t out_len;

//	return;
//	if((p_ctl->rssi.p_uni==NULL)||(!p_ctl->rssi.dest_port)) return;
	if((p_ctl->rssi.p_tcp_cmd ==NULL)) return;

	p_ctl->rssi.meas_cntrl.azimut = (uint16_t)round(control.povorot.rotate_angle);

	p_ack_msg = tcp_cmd_make_ack_start(p_ctl->rssi.p_tcp_cmd,&p_ctl->rssi.p_tcp_cmd->def_addr);
	if(p_ack_msg == NULL) return;

	p_out_data = &p_ack_msg->data.p_data[p_ack_msg->data.cmd_len];

	p_out_data[0] = 0x8B;
	p_out_data[1] = 0x8E;

	memcpy(&p_out_data[2],&p_ctl->rssi.global_cntrl,sizeof(p_ctl->rssi.global_cntrl));
	out_len = 2 + sizeof(p_ctl->rssi.global_cntrl);
	memcpy(&p_out_data[out_len],&p_ctl->rssi.meas_cntrl,sizeof(p_ctl->rssi.meas_cntrl));
	out_len += sizeof(p_ctl->rssi.meas_cntrl);
	memcpy(&p_out_data[out_len],p_ctl->rssi.data_buf,p_ctl->rssi.meas_cntrl.num_freq_point);
	out_len += p_ctl->rssi.meas_cntrl.num_freq_point;

	p_ack_msg->data.cmd_len = p_ack_msg->data.cmd_len + out_len;
	tcp_cmd_make_ack_finish(p_ctl->rssi.p_tcp_cmd,p_ack_msg,ANS_NO,0,NULL);

	return;

//	len = 5 + 2 + sizeof(p_ctl->rssi.global_cntrl)+sizeof(p_ctl->rssi.meas_cntrl) + p_ctl->rssi.meas_cntrl.num_freq_point;
//
//
//
//	tmp_data[1] = 0x80;
//	tmp_data[2] = addr.version;
//	tmp_data[3] = addr.src_addr_lit;
//	tmp_data[4] = addr.dst_addr_lit;
//	tmp_data[0] = len;
//	tmp_data[1] |= ((uint32_t)(len))>>8;
//
////	p_buf = pbuf_alloc(PBUF_TRANSPORT,5 + 2 + sizeof(p_ctl->rssi.global_cntrl)+sizeof(p_ctl->rssi.meas_cntrl) + p_ctl->rssi.meas_cntrl.num_freq_point, PBUF_RAM);
//	p_buf = pbuf_alloc(PBUF_TRANSPORT,len, PBUF_RAM);
//	if(p_buf != NULL){
//		tmp[0] = 0x8B;
//		tmp[1] = 0x8E;
//
//		pbuf_take(p_buf,&tmp_data,5);
//		pbuf_take_at(p_buf,tmp,2,5);
//		pbuf_take_at(p_buf,&p_ctl->rssi.global_cntrl,sizeof(p_ctl->rssi.global_cntrl),2+5);
//		pbuf_take_at(p_buf,&p_ctl->rssi.meas_cntrl,sizeof(p_ctl->rssi.meas_cntrl),5 + 2 + sizeof(p_ctl->rssi.global_cntrl));
//		pbuf_take_at(p_buf,p_ctl->rssi.data_buf,p_ctl->rssi.meas_cntrl.num_freq_point,5 + 2 + sizeof(p_ctl->rssi.global_cntrl)+sizeof(p_ctl->rssi.meas_cntrl));
//
//		struct netbuf  *xNetBuf = netbuf_new ();
//		if(xNetBuf==NULL) {
//			pbuf_free(p_buf);
//			return;
//		}
//
//		xNetBuf->p = p_buf;
//		xNetBuf->ptr = xNetBuf->p;
//
////		netconn_sendto(p_ctl->rssi.p_uni->p_conn->udp.p_conn,xNetBuf,&p_ctl->rssi.dest_ip,p_ctl->rssi.dest_port);
//		netbuf_delete(xNetBuf);
//		return;


//		uni_prot_thread_send_buf(p_ctl->rssi.p_uni,p_buf->len,p_buf->payload,&addr);

//	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_send_status(rtc6715_struct* p_ctl){
//struct pbuf *p_buf;
//uint8_t tmp[2];
//uint8_t ack_max_len = 255;
//uni_prot_addr_param_s addr;
//addr.dst_addr_lit = 127;
//addr.src_addr_lit = 1;
//addr.version = uni_prot_header_version_1;
//addr.dst_ch = 1;
//addr.src_ch = 1;

tcp_cmd_msg_s* p_ack_msg;
uint8_t* p_out_data;
uint16_t out_len;

recv_5_8_status_s status;

//uint16_t len;
//uint8_t tmp_data[5];

	if((p_ctl->rssi.state == recv_5_8_rssi_state_stop) || (p_ctl->rssi.state == recv_5_8_rssi_check_video)) {
		status.freq = p_ctl->freq;
	}
	else{
		status.freq = 0;
	}
	status.azimut = (uint16_t)round(control.povorot.rotate_angle);


//	return;
//	if((p_ctl->rssi.p_uni==NULL)||(!p_ctl->rssi.dest_port)) return;
	if((p_ctl->rssi.p_tcp_cmd==NULL)) return;

	p_ack_msg = tcp_cmd_make_ack_start(p_ctl->rssi.p_tcp_cmd,&p_ctl->rssi.p_tcp_cmd->def_addr);
	if(p_ack_msg == NULL) return;

	p_out_data = &p_ack_msg->data.p_data[p_ack_msg->data.cmd_len];

	p_out_data[0] = 0x8C;
	p_out_data[1] = 0x8E;

	memcpy(&p_out_data[2],&status,sizeof(status));
	out_len = 2 + sizeof(status);

	p_ack_msg->data.cmd_len = p_ack_msg->data.cmd_len + out_len;
	tcp_cmd_make_ack_finish(p_ctl->rssi.p_tcp_cmd,p_ack_msg,ANS_NO,0,NULL);

	return;

	/*





	memcpy(&p_out_data[out_len],&p_ctl->rssi.meas_cntrl,sizeof(p_ctl->rssi.meas_cntrl));
	out_len += sizeof(p_ctl->rssi.meas_cntrl);
	memcpy(&p_out_data[out_len],p_ctl->rssi.data_buf,p_ctl->rssi.meas_cntrl.num_freq_point);
	out_len += p_ctl->rssi.meas_cntrl.num_freq_point;

	p_ack_msg->data.cmd_len = p_ack_msg->data.cmd_len + out_len;
	tcp_cmd_make_ack_finish(p_ctl->rssi.p_tcp_cmd,p_ack_msg,ANS_NO,0,NULL);



	len = 5 + 2 + sizeof(recv_5_8_status_s);

	tmp_data[1] = 0x80;
	tmp_data[2] = addr.version;
	tmp_data[3] = addr.src_addr_lit;
	tmp_data[4] = addr.dst_addr_lit;
	tmp_data[0] = len;
	tmp_data[1] |= ((uint32_t)(len))>>8;

	if((p_ctl->rssi.state == recv_5_8_rssi_state_stop) || (p_ctl->rssi.state == recv_5_8_rssi_check_video)) {
		status.freq = p_ctl->freq;
	}
	else{
		status.freq = 0;
	}
	status.azimut = (uint16_t)round(control.povorot.rotate_angle);

//	if(p_ctl->rssi.p_uni==NULL) return;
	if(p_ctl->rssi.p_tcp_cmd==NULL) return;

	p_buf = pbuf_alloc(PBUF_TRANSPORT,len, PBUF_RAM);
	if(p_buf != NULL){
		tmp[0] = 0x8C;
		tmp[1] = 0x8E;
		pbuf_take(p_buf,&tmp_data,5);
		pbuf_take_at(p_buf,tmp,2,5);
		pbuf_take_at(p_buf,&status,sizeof(status),2+5);

		struct netbuf  *xNetBuf = netbuf_new ();
		if(xNetBuf==NULL) {
			pbuf_free(p_buf);
			return;
		}

		xNetBuf->p = p_buf;
		xNetBuf->ptr = xNetBuf->p;

	//	netconn_sendto(p_ctl->rssi.p_uni->p_conn->udp.p_conn,xNetBuf,&p_ctl->rssi.dest_ip,p_ctl->rssi.dest_port);
		netbuf_delete(xNetBuf);
		return;

//		uni_prot_thread_send_buf(p_ctl->rssi.p_uni,p_buf->len,p_buf->payload,&addr);
//		pbuf_free(p_buf);
	}
	*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_continue_rssi(rtc6715_struct* p_ctl){
	if(p_ctl->rssi.state == recv_5_8_rssi_state_stop) return;
	if(p_ctl->rssi.state == recv_5_8_rssi_state_continue){

		if(p_ctl->rssi.rssi_now_mode == recv_5_8_rssi_now_mode_fast){
			if(p_ctl->rssi.fast.fast_cntr <= p_ctl->rssi.fast.num_freq_point){
				p_ctl->rssi.fast.data[p_ctl->rssi.fast.fast_cntr] = recv_5_8_get_rssi(p_ctl);
				p_ctl->rssi.fast.fast_cntr++;
				if(p_ctl->rssi.fast.fast_cntr >= p_ctl->rssi.fast.num_freq_point){
					recv_5_8_send_fast_rssi(p_ctl);
					p_ctl->rssi.rssi_now_mode = recv_5_8_rssi_now_mode_slow;
					p_ctl->rssi.fast.slow_cntr = 0;
				}
			}
			else{
				p_ctl->rssi.rssi_now_mode = recv_5_8_rssi_now_mode_slow;
			}
		}
		else{
			if(p_ctl->rssi.buf_cntr <= RECV_5_8_RSSI_BUF_SIZE){
				p_ctl->rssi.data_buf[p_ctl->rssi.buf_cntr] = recv_5_8_get_rssi(p_ctl);
	//			printf("rssi freq=%d lvl=%d \r\n",p_ctl->rssi.freq,p_ctl->rssi.data_buf[p_ctl->rssi.buf_cntr]);
				p_ctl->rssi.buf_cntr++;
				p_ctl->rssi.freq += p_ctl->rssi.meas_cntrl.delta_freq;

				if(p_ctl->rssi.rssi_mode == recv_5_8_rssi_mode_mix){
					p_ctl->rssi.fast.slow_cntr++;
					if(p_ctl->rssi.fast.slow_cntr >= p_ctl->rssi.fast.max_slow_cntr){
						p_ctl->rssi.rssi_now_mode = recv_5_8_rssi_now_mode_fast;
						p_ctl->rssi.fast.fast_cntr = 0;
						p_ctl->rssi.fast.slow_cntr = 0;
					}
				}

				if(p_ctl->rssi.freq > p_ctl->rssi.meas_cntrl.stop_freq){
					recv_5_8_send_rssi(p_ctl);
	//				if(p_ctl->rssi.b_check_video){
					if((!p_ctl->p_cfg->b_en_only_scan) && (p_ctl->rssi.rssi_mode == recv_5_8_rssi_mode_slow)){
						p_ctl->rssi.buf_cntr = 0;
						p_ctl->rssi.state = recv_5_8_rssi_check_video;

						switch_fdc_on(&p_ctl->sw_da1_en,fdc_on);
						switch_fdc_set_data(&p_ctl->sw_da1_en);

					}
					else{
						if(p_ctl->rssi.b_continue_restart){
							recv_5_8_start_rssi(p_ctl, 1);
						}
						else{
							recv_5_8_stop_rssi(p_ctl);
//							p_ctl->rssi.state = recv_5_8_rssi_state_stop;
//								switch_fdc_on(&p_ctl->sw_da1_en,fdc_on);
//								switch_fdc_set_data(&p_ctl->sw_da1_en);
						}
					}
				}
			}
			else{
				recv_5_8_stop_rssi(p_ctl);
//					p_ctl->rssi.state = recv_5_8_rssi_state_stop;
//					switch_fdc_on(&p_ctl->sw_da1_en,fdc_on);
//					switch_fdc_set_data(&p_ctl->sw_da1_en);
			}
		}
	}

	if(p_ctl->rssi.state == recv_5_8_rssi_state_stop) {
		rtc6715_set_freq(p_ctl, p_ctl->rssi.base_freq);
		return;
	}

	if(p_ctl->rssi.state == recv_5_8_rssi_check_video) {
//		rtc6715_set_freq(p_ctl, p_ctl->rssi.base_freq);
		return;
	}

if(p_ctl->rssi.state != recv_5_8_rssi_state_continue){
	switch_fdc_on(&p_ctl->sw_da1_en,fdc_off);
	switch_fdc_set_data(&p_ctl->sw_da1_en);
}
	p_ctl->rssi.state = recv_5_8_rssi_state_continue;

	if(p_ctl->rssi.rssi_now_mode == recv_5_8_rssi_now_mode_start_fast){
		p_ctl->rssi.rssi_now_mode = recv_5_8_rssi_now_mode_fast;
		p_ctl->rssi.rssi_mode = recv_5_8_rssi_mode_mix;
		p_ctl->rssi.fast.fast_cntr = 0;
		p_ctl->rssi.fast.slow_cntr = 0;
		p_ctl->rssi.fast.max_slow_cntr = 32 - p_ctl->rssi.fast.num_freq_point;
	}

	if(p_ctl->rssi.rssi_now_mode == recv_5_8_rssi_now_mode_slow){
		rtc6715_set_freq(p_ctl, p_ctl->rssi.freq);
	}
	else{
		rtc6715_set_freq(p_ctl, p_ctl->rssi.fast.freq[p_ctl->rssi.fast.fast_cntr]);
	}
	osTimerStart(p_ctl->rssi.timer_id, p_ctl->rssi.delay_answer);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_stop_rssi(rtc6715_struct* p_ctl){
	p_ctl->rssi.state = recv_5_8_rssi_state_stop;
	p_ctl->rssi.b_continue_restart = 0;
	p_ctl->rssi.b_check_video = 0;
	p_ctl->rssi.b_en = 0;
	switch_fdc_on(&p_ctl->sw_da1_en,fdc_on);
	switch_fdc_set_data(&p_ctl->sw_da1_en);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* recv_5_8_dbg_get_en_rssi_mode(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;

	return (uint8_t*)&p_ctl->p_cfg->b_en_fast_scan;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void recv_5_8_dbg_set_en_rssi_mode(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	if(*p8_data){
		p_ctl->p_cfg->b_en_fast_scan = 1;
	}
	else{
		p_ctl->p_cfg->b_en_fast_scan = 0;
	}
	recv_5_8_write_cfg(p_ctl);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* recv_5_8_dbg_get_rssi_lvl(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;

	return (uint8_t*)&p_ctl->p_cfg->lvl;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void recv_5_8_dbg_set_rssi_lvl(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	p_ctl->p_cfg->lvl = *p8_data;
	recv_5_8_write_cfg(p_ctl);
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* recv_5_8_dbg_get_en_only_scan_mode(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;

	return (uint8_t*)&p_ctl->p_cfg->b_en_only_scan;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void recv_5_8_dbg_set_en_only_scan_mode(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	if(*p8_data){
		p_ctl->p_cfg->b_en_only_scan = 1;
	}
	else{
		p_ctl->p_cfg->b_en_only_scan = 0;
	}
	recv_5_8_write_cfg(p_ctl);
	return;
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_make_view_mode(rtc6715_struct *p_ctl, recv_5_8_view_mode_now_e mode){
	p_ctl->view_mode = mode;

	switch (p_ctl->view_mode){
	case recv_5_8_view_mode_now_normal:
		switch_fdc_on(&p_ctl->sw_da1_in,fdc_off);
	break;
	case recv_5_8_view_mode_now_inv:
		switch_fdc_on(&p_ctl->sw_da1_in,fdc_on);
	break;
	}
	switch_fdc_set_data(&p_ctl->sw_da1_in);

	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_5_8_time_process(rtc6715_struct *p_ctl, uint32_t time){
uint32_t res=0;
	p_ctl->time += time;
	if(p_ctl->time > (uint32_t)p_ctl->p_cfg->time * 1000){
		p_ctl->time = 0;
//		res = 1;
		if(p_ctl->p_cfg->view_mode==recv_5_8_view_mode_inv_norm){
			if(p_ctl->view_mode == recv_5_8_view_mode_now_normal){
				recv_5_8_make_view_mode(p_ctl,recv_5_8_view_mode_now_inv);
			}
			else{
				recv_5_8_make_view_mode(p_ctl,recv_5_8_view_mode_now_normal);
				recv_5_8_set_freq_up_auto(p_ctl,p_ctl->p_cfg->auto_step_freq);
				res = 1;
			}
		}
		else{
			recv_5_8_set_freq_up_auto(p_ctl,p_ctl->p_cfg->auto_step_freq);
			res = 1;
		}
	}
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void rtc6715_power_on(rtc6715_struct *p_ctl){

	switch_fdc_on(&p_ctl->sw_pwr_5v,fdc_on);
	switch_fdc_set_data(&p_ctl->sw_pwr_5v);

#ifdef RECV_5_8_SUB_3_3
	rffc5071_regs_commit(p_ctl->p_rff5071);
#endif

	osDelay(50);

	switch (p_ctl->p_cfg->view_mode ){
	case recv_5_8_view_mode_normal:
		recv_5_8_make_view_mode(p_ctl, recv_5_8_view_mode_now_normal);
	break;
	case recv_5_8_view_mode_inv:
		recv_5_8_make_view_mode(p_ctl, recv_5_8_view_mode_now_inv);
	break;
	case recv_5_8_view_mode_inv_norm:
		recv_5_8_make_view_mode(p_ctl, recv_5_8_view_mode_now_normal);
	break;
	}

	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void rtc6715_power_off(rtc6715_struct *p_ctl){

	switch_fdc_on(&p_ctl->sw_pwr_5v,fdc_off);
	switch_fdc_set_data(&p_ctl->sw_pwr_5v);

#ifdef RECV_5_8_SUB_3_3
	switch_fdc_on(&p_ctl->p_rff5071->sw_en,fdc_off);
	switch_fdc_set_data(&p_ctl->p_rff5071->sw_en);

#endif

	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_write_cfg(rtc6715_struct *p_ctl){
//	dev_config_write(&control.dev_config);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t rtc6715_write_reg(rtc6715_struct *p_ad, uint32_t reg_data, uint8_t reg_addr){
uint32_t data;
//uint8_t tmp_buf[4];
uint32_t res=0;

	if(reg_addr >= RTC6715_MAX_NUM_REGISTERS) return -1;
	p_ad->reg.bytes[reg_addr] = reg_data;

	data = reg_addr & 0xF;
	data |= 0x10;
	data |= reg_data << 5;

//	HAL_GPIO_WritePin(p_ad->cs_port, p_ad->cs_pin, GPIO_PIN_RESET);
	gpio_set_sun(&p_ad->cs_pin,GPIO_RESET);
//	res |= delay_us_msg(&p_ad->short_time,10);
	udelay(10);

	for (int i=0; i<25; i++){
		if(data & 1){
//			HAL_GPIO_WritePin(p_ad->mosi_port, p_ad->mosi_pin, GPIO_PIN_SET);
			gpio_set_sun(&p_ad->mosi_pin,GPIO_SET);
		}
		else{
//			HAL_GPIO_WritePin(p_ad->mosi_port, p_ad->mosi_pin, GPIO_PIN_RESET);
			gpio_set_sun(&p_ad->mosi_pin,GPIO_RESET);
		}
//		res |= delay_us_msg(&p_ad->short_time,10);
		udelay(10);

//		HAL_GPIO_WritePin(p_ad->clk_port, p_ad->clk_pin, GPIO_PIN_SET);
		gpio_set_sun(&p_ad->clk_pin,GPIO_SET);
		udelay(10);
//		res |= delay_us_msg(&p_ad->short_time,10);
		gpio_set_sun(&p_ad->clk_pin,GPIO_RESET);
//		HAL_GPIO_WritePin(p_ad->clk_port, p_ad->clk_pin, GPIO_PIN_RESET);
		udelay(10);
//		res |= delay_us_msg(&p_ad->short_time,10);
		data = data >> 1;

	}

	gpio_set_sun(&p_ad->cs_pin,GPIO_SET);
//	HAL_GPIO_WritePin(p_ad->cs_port, p_ad->cs_pin, GPIO_PIN_SET);

	if(res) p_ad->error |= 1;

	return 0;
}
//******************************************************************************
//* function: calcFrequencyData
//*         : calculates the frequency value for the syntheziser register B of
//*         : the RTC6751 circuit that is used within the RX5808/RX5880 modules.
//*         : this value is inteded to be loaded to register at adress 1 via SPI
//*         :
//*  Formula: frequency = ( N*32 + A )*2 + 479
//******************************************************************************
uint32_t rtc6715_set_freq(rtc6715_struct *p_ad, uint16_t freq){
  unsigned int N;
  unsigned char A;
  p_ad->freq = freq;

#ifdef  RECV_5_8_SUB_3_3
  rffc5071_set_freq(p_ad->p_rff5071,freq);
#else
  freq = (freq - 479) / 2;
  N = freq / 32;
  A = freq % 32;
  rtc6715_write_reg(p_ad,(N << 7) |  A,1);
#endif


//	printf("6715 freq = %d \r\n",p_ad->freq);
  return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t rtc6715_read_reg(rtc6715_struct *p_ad, uint32_t *reg_data, uint8_t reg_addr){
	*reg_data = 0;
	if(reg_addr >= RTC6715_MAX_NUM_REGISTERS) return -1;
	*reg_data = p_ad->reg.bytes[reg_addr];
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_5_8_set_freq_up(rtc6715_struct *p_ctl, uint16_t freq){
#ifdef	RTC6715_FREQ_MODE
	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_freq_mode){
		if((p_ctl->freq + freq) > p_ctl->p_cfg->max_freq){
			rtc6715_set_freq(p_ctl, p_ctl->p_cfg->min_freq);
		}
		else{
			rtc6715_set_freq(p_ctl, p_ctl->freq + freq);
		}
	}
#endif
	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_ch_mode){
uint8_t num=0,b_find;
		while(1){
				p_ctl->ch_cntr++;
				if(p_ctl->ch_cntr >= RECV_5_8_MAX_NUM_CH){
					p_ctl->ch_cntr = 0;
				}
				b_find = recv_5_8_chec_dis_lit_ch(p_ctl,p_ctl->ch_cntr);
				if(!b_find){
					rtc6715_set_freq(p_ctl,ch_5_8_freq[p_ctl->ch_cntr]);
					break;
				}
				num++;
				if (num>150) break;
		}
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_5_8_set_ch_up(rtc6715_struct *p_ctl){
	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_ch_mode){
//uint8_t num=0,b_find;
//		while(1){
				p_ctl->ch_cntr ++;
				if(!(p_ctl->ch_cntr%8)){
					p_ctl->ch_cntr -= 8;
				}
//				b_find = recv_5_8_chec_dis_lit_ch(p_ctl,p_ctl->ch_cntr);
//				if(!b_find){
					rtc6715_set_freq(p_ctl,ch_5_8_freq[p_ctl->ch_cntr]);
//					break;
//				}
//				num++;
//				if (num>150) break;
//		}
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_5_8_set_ch_down(rtc6715_struct *p_ctl){
	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_ch_mode){
//uint8_t num=0,b_find;
//		while(1){
				if(!(p_ctl->ch_cntr%8)){
					p_ctl->ch_cntr += 7;
				}
				else{
					p_ctl->ch_cntr--;
				}
//				b_find = recv_5_8_chec_dis_lit_ch(p_ctl,p_ctl->ch_cntr);
//				if(!b_find){
					rtc6715_set_freq(p_ctl,ch_5_8_freq[p_ctl->ch_cntr]);
//					break;
//				}
//				num++;
//				if (num>150) break;
//		}
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_5_8_set_liter_up(rtc6715_struct *p_ctl){
	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_ch_mode){
uint8_t num=0,b_find;
		while(1){
				p_ctl->ch_cntr += 8;
				if(p_ctl->ch_cntr >= RECV_5_8_MAX_NUM_CH){
					p_ctl->ch_cntr = p_ctl->ch_cntr - RECV_5_8_MAX_NUM_CH;
				}
				b_find = recv_5_8_chec_dis_lit_ch(p_ctl,p_ctl->ch_cntr);
				if(!b_find){
					rtc6715_set_freq(p_ctl,ch_5_8_freq[p_ctl->ch_cntr]);
					break;
				}
				num++;
				if (num>150) break;
		}
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_5_8_set_liter_down(rtc6715_struct *p_ctl){
	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_ch_mode){
uint8_t num=0,b_find;
		while(1){
				if(p_ctl->ch_cntr < 8){
					p_ctl->ch_cntr = RECV_5_8_MAX_NUM_CH - (8-p_ctl->ch_cntr);
				}
				else{
					p_ctl->ch_cntr -= 8;
				}
				b_find = recv_5_8_chec_dis_lit_ch(p_ctl,p_ctl->ch_cntr);
				if(!b_find){
					rtc6715_set_freq(p_ctl,ch_5_8_freq[p_ctl->ch_cntr]);
					break;
				}
				num++;
				if (num>150) break;
		}
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_5_8_chec_dis_lit_ch(rtc6715_struct *p_ctl, uint8_t ch){
	uint32_t b_find = 1;
	uint8_t pos;
	pos = ch / 8;
	if(p_ctl->p_cfg->en_lit & (1<<pos)){
		b_find = 0;
	}
	return b_find;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_5_8_set_freq_up_auto(rtc6715_struct *p_ctl, uint16_t freq){
uint8_t i,b_find;
char* p_ch;
uint8_t num=0;
#ifdef RECV_5_8_RSSI
	if(p_ctl->rssi.state == recv_5_8_rssi_check_video){
		uint16_t freq;
		uint32_t res;
		res = rtc6715_get_freq_from_table(p_ctl,&freq);
		if(!res){
			rtc6715_set_freq(p_ctl, freq);
		}
		else{
			if(p_ctl->rssi.b_continue_restart){
				recv_5_8_start_rssi(p_ctl, 1);
			}
			else{
				recv_5_8_stop_rssi(p_ctl);
//				p_ctl->rssi.state = recv_5_8_rssi_state_stop;
			}
		}
		return 0;
	}
#endif
	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_ch_mode){
		while(1){
			num++;
			p_ctl->ch_cntr++;
			if(p_ctl->ch_cntr >= RECV_5_8_MAX_NUM_CH){
				p_ctl->ch_cntr = 0;
			}
			b_find = 0;
			b_find = recv_5_8_chec_dis_lit_ch(p_ctl,p_ctl->ch_cntr);
			if(!b_find){
				p_ch = (char*)ch_5_8_table[p_ctl->ch_cntr];
				for(i = 0; i<RECV_5_8_MAX_NUM_BLANK_CH;i++){
					if(memcmp(p_ch,&p_ctl->p_cfg->ch_blank[i][0],2)==0){
	//				if(ch == p_ctl->p_cfg->ch_blank[i]){
						b_find = 1;
						break;
					}
				}
			}
			if(!b_find) break;
			if(num>150) break;
		}
		rtc6715_set_freq(p_ctl,ch_5_8_freq[p_ctl->ch_cntr]);
	}
#ifdef	RTC6715_FREQ_MODE
	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_freq_mode){
		if((p_ctl->freq + freq) > p_ctl->p_cfg->max_freq){
			rtc6715_set_freq(p_ctl, p_ctl->p_cfg->min_freq);
		}
		else{
			rtc6715_set_freq(p_ctl, p_ctl->freq + freq);
		}
	}
#endif
//#ifdef	RTC6715_TABLE_MODE
//		if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_table_mode){
//uint16_t freq;
//uint32_t res;
//			res = rtc6715_get_freq_from_table(p_ctl,&freq);
//			if(!res){
//				rtc6715_set_freq(p_ctl, freq);
//			}
//			else{
//
//			}
//		}
//#endif


  return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_5_8_set_freq_down(rtc6715_struct *p_ctl, uint16_t freq){
#ifdef	RTC6715_FREQ_MODE
	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_freq_mode){
		if((p_ctl->freq - freq) < p_ctl->p_cfg->min_freq){
			rtc6715_set_freq(p_ctl, p_ctl->p_cfg->max_freq);
		}
		else{
			rtc6715_set_freq(p_ctl, p_ctl->freq - freq);
		}
	}
#endif
	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_ch_mode){
		uint8_t num=0,b_find;
		while(1){
			if(p_ctl->ch_cntr==0){
				p_ctl->ch_cntr = RECV_5_8_MAX_NUM_CH - 1;
			}
			else{
				p_ctl->ch_cntr--;
			}
			b_find = recv_5_8_chec_dis_lit_ch(p_ctl,p_ctl->ch_cntr);
			if(!b_find){
				rtc6715_set_freq(p_ctl,ch_5_8_freq[p_ctl->ch_cntr]);
				break;
			}
			num++;
			if(num>150) break;
		}
	}
  return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void rtc6715_write_reg_func(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
rtc6715_struct *p_ad;
	p_ad = (rtc6715_struct *)ad32;

//	printf("ad wrt addr = %x, data = %x,\r\n",p_ad->addr, *p8_data);

	rtc6715_write_reg(p_ad,*(uint32_t*)p8_data,p_ad->addr);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* rtc6715_read_reg_func(uint32_t ad32, uint32_t num, uint32_t tmp){
rtc6715_struct *p_ad;
	p_ad = (rtc6715_struct *)ad32;
	rtc6715_read_reg(p_ad,&p_ad->reg_data,p_ad->addr);

	return (uint8_t*)&p_ad->reg_data;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void rtc6715_dbg_write_freq(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
rtc6715_struct *p_ad;
	p_ad = (rtc6715_struct *)ad32;
	uint16_t tmp16;
	memcpy(&tmp16,p8_data,2);

//	control.freq_mode = cntrl_freq_mode_manual;
//	control.auto_mode = cntrl_auto_mode_dis;

	rtc6715_set_freq(p_ad,tmp16);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* rtc6715_dbg_read_freq(uint32_t ad32, uint32_t num, uint32_t tmp){
rtc6715_struct *p_ad;
	p_ad = (rtc6715_struct *)ad32;
	return (uint8_t*)&p_ad->freq;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t rtc6715_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, rtc6715_struct *p_ad, const char *p_name){
SettingCell_t 	cell;
uint32_t 		i;
	i=0;

	for(i=0;i<COMPONENT_MAX_TABLE_SIZE;i++){
		if(!p_cell[i].CellNumber) break;
	}
	if( i>= COMPONENT_MAX_TABLE_SIZE) return num;

	cell.CellAttr = RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 2;
	cell.VarPtr = &p_ad->addr;
	cell.ReadProc = 0;
	cell.WriteProc = 0;
	cell.HighLim = 0;
	cell.DefaultValue = 0;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "rtc6715 addr";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)rtc6715_read_reg_func;
	cell.WriteProc = (void*)(void*)rtc6715_write_reg_func;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ad;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "rtc6715 reg";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 2;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)rtc6715_dbg_read_freq;
	cell.WriteProc = (void*)(void*)rtc6715_dbg_write_freq;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ad;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "rtc6715 freq";
	p_cell[i++] = cell;

	sw_fdc_add_to_cell_table(p_cell, num++, &p_ad->sw_pwr_5v, "recv58 sw 5v");

//	sw_fdc_add_to_cell_table(p_cell, num++, &p_ad->sw_da1_en, "recv58 sw en");
	sw_fdc_add_to_cell_table(p_cell, num++, &p_ad->sw_da1_in, "recv58 sw in");

	return num;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_dbg_set_scan_time(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	p_ctl->p_cfg->time = *p8_data;
	if(!p_ctl->p_cfg->time) p_ctl->p_cfg->time = 1;

	recv_5_8_write_cfg(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_5_8_dbg_get_scan_time(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	return &p_ctl->p_cfg->time;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_dbg_set_min_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	p_ctl->p_cfg->min_freq = *(uint16_t*)p8_data;
	if(p_ctl->p_cfg->min_freq < RECV_5_8_MIN_FREQ) p_ctl->p_cfg->min_freq = RECV_5_8_MIN_FREQ;
	if(p_ctl->p_cfg->min_freq > RECV_5_8_MAX_FREQ) p_ctl->p_cfg->min_freq = RECV_5_8_MIN_FREQ;

	recv_5_8_write_cfg(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_5_8_dbg_get_min_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	return (uint8_t*)&p_ctl->p_cfg->min_freq;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_dbg_set_max_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	p_ctl->p_cfg->max_freq = *(uint16_t*)p8_data;
	if(p_ctl->p_cfg->max_freq < RECV_5_8_MIN_FREQ) p_ctl->p_cfg->max_freq = RECV_5_8_MAX_FREQ;
	if(p_ctl->p_cfg->max_freq > RECV_5_8_MAX_FREQ) p_ctl->p_cfg->max_freq = RECV_5_8_MAX_FREQ;

	recv_5_8_write_cfg(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_5_8_dbg_get_max_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	return (uint8_t*)&p_ctl->p_cfg->max_freq;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_dbg_set_manual_step_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	p_ctl->p_cfg->manual_step_freq = *(uint16_t*)p8_data;
	if(!p_ctl->p_cfg->manual_step_freq) p_ctl->p_cfg->manual_step_freq = 1;

	recv_5_8_write_cfg(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_5_8_dbg_get_manual_step_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	return (uint8_t*)&p_ctl->p_cfg->manual_step_freq;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_dbg_set_auto_step_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	p_ctl->p_cfg->auto_step_freq = *(uint16_t*)p8_data;
	if(!p_ctl->p_cfg->auto_step_freq) p_ctl->p_cfg->auto_step_freq = 1;

	recv_5_8_write_cfg(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_5_8_dbg_get_auto_step_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	return (uint8_t*)&p_ctl->p_cfg->auto_step_freq;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void recv_5_8_dbg_set_disable_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
//	rtc6715_struct *p_ctl;
//	p_ctl = (rtc6715_struct *)ctl32;
//	p_ctl->p_cfg->dis_start_freq[num] = *(uint16_t*)p8_data;
//	if(p_ctl->p_cfg->dis_start_freq[num] < RECV_5_8_MIN_FREQ) p_ctl->p_cfg->dis_start_freq[num] = RECV_5_8_MIN_FREQ;
//	if(p_ctl->p_cfg->dis_start_freq[num] > RECV_5_8_MAX_FREQ) p_ctl->p_cfg->dis_start_freq[num] = RECV_5_8_MAX_FREQ;
//	recv_5_8_write_cfg(p_ctl);
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint8_t* recv_5_8_dbg_get_disable_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
//	rtc6715_struct *p_ctl;
//	p_ctl = (rtc6715_struct *)ctl32;
//	return (uint8_t*)&p_ctl->p_cfg->dis_start_freq[num];
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void recv_5_8_dbg_set_disable_freq_len(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
//	rtc6715_struct *p_ctl;
//	p_ctl = (rtc6715_struct *)ctl32;
//	p_ctl->p_cfg->dis_len_freq[num] = *(uint16_t*)p8_data;
//	recv_5_8_write_cfg(p_ctl);
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint8_t* recv_5_8_dbg_get_disable_freq_len(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
//	rtc6715_struct *p_ctl;
//	p_ctl = (rtc6715_struct *)ctl32;
//	return (uint8_t*)&p_ctl->p_cfg->dis_len_freq[num];
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void recv_5_8_dbg_set_view_mode(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){

	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	p_ctl->p_cfg->view_mode = (recv_5_8_view_mode_e)cmd;

	switch (p_ctl->p_cfg->view_mode ){
	case recv_5_8_view_mode_normal:
		recv_5_8_make_view_mode(p_ctl, recv_5_8_view_mode_now_normal);
	break;
	case recv_5_8_view_mode_inv:
		recv_5_8_make_view_mode(p_ctl, recv_5_8_view_mode_now_inv);
	break;
	case recv_5_8_view_mode_inv_norm:
		recv_5_8_make_view_mode(p_ctl, recv_5_8_view_mode_now_normal);
	break;
	}
	recv_5_8_write_cfg(p_ctl);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void recv_5_8_dbg_set_en_lit(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
//recv_5_8_lit_name_e en_tit;
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;

//	en_tit = (recv_5_8_lit_name_e)cmd;

	p_ctl->p_cfg->en_lit &= ~(1 << cmd);

	if(*p8_data){
		p_ctl->p_cfg->en_lit |= (1 << cmd);
	}

//	printf("en = %d %d %d\r\n",cmd,*p8_data,p_ctl->p_cfg->en_lit);

	recv_5_8_write_cfg(p_ctl);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_5_8_dbg_get_en_lit(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	p_ctl->tmp[0] = 0;
	if(p_ctl->p_cfg->en_lit & (1<< cmd)) 	p_ctl->tmp[0] = 1;
	return (uint8_t*)&p_ctl->tmp[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_5_8_dbg_get_view_mode(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	p_ctl->tmp[0] = 0;
	if(p_ctl->p_cfg->view_mode == cmd){
		p_ctl->tmp[0] = 1;
	}
	return (uint8_t*)&p_ctl->tmp[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef	RTC6715_FREQ_MODE
uint8_t* recv_5_8_dbg_get_freq_mode(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	p_ctl->tmp[0] = 0;

	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_freq_mode){
		p_ctl->tmp[0] = 1;
	}
	return (uint8_t*)&p_ctl->tmp[0];
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef	RTC6715_FREQ_MODE
static void recv_5_8_dbg_set_freq_mode(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)ctl32;
	if(*p8_data){
		p_ctl->p_cfg->freq_ch_mode = recv_5_8_freq_mode;
	}
	else{
		p_ctl->p_cfg->freq_ch_mode = recv_5_8_ch_mode;
	}
	recv_5_8_write_cfg(p_ctl);
	return;
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//static uint8_t* recv_5_8_dbg_get_disable_ch(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
//	rtc6715_struct *p_ctl;
//uint8_t pos,cntr;
//	p_ctl = (rtc6715_struct *)adf32;
//	memset(control.povorot.ack_dbg_tmp,0,sizeof(control.povorot.ack_dbg_tmp));
//	cntr=0;
//	for(int i=0;i<RECV_5_8_MAX_NUM_BLANK_CH;i++){
//		if(p_ctl->p_cfg->ch_blank[i][0]){
//			pos = strlen((char*)control.povorot.ack_dbg_tmp);
//			sprintf((char*)&control.povorot.ack_dbg_tmp[pos],"%c%c ",p_ctl->p_cfg->ch_blank[i][0],p_ctl->p_cfg->ch_blank[i][1]);
//			cntr++;
//			if(cntr>8) break;
//		}
//	}
//	return (uint8_t*)control.povorot.ack_dbg_tmp;
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* recv_5_8_dbg_get_dis_en_ch(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)adf32;
	return (uint8_t*)&p_ctl->dis_en_ch_buf[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_dbg_set_dis_en_ch(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
rtc6715_struct *p_ctl;
	p_ctl = (rtc6715_struct *)adf32;
	memcpy(p_ctl->dis_en_ch_buf,p8_data,2);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_dbg_add_to_disable_ch(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
int i;
	p_ctl = (rtc6715_struct *)adf32;
	for(i = 0; i<RECV_5_8_MAX_NUM_BLANK_CH;i++){
		if(memcmp(p_ctl->dis_en_ch_buf,&p_ctl->p_cfg->ch_blank[i][0],2)==0){
			return;
		}
//		if(p_ctl->dis_en_ch == p_ctl->p_cfg->ch_blank[i]) return;
	}
	for(i = 0; i < RECV_5_8_MAX_NUM_CH; i++){
//		if(p_ctl->dis_en_ch == ch_5_8_table[i]) break;
		if(memcmp(p_ctl->dis_en_ch_buf,ch_5_8_table[i],2)==0){
			break;
		}
//		if(p_ctl->dis_en_ch == ch_5_8_table[i]) break;
	}
	if(i >= RECV_5_8_MAX_NUM_CH) return;
	for(i = 0; i<RECV_5_8_MAX_NUM_CH;i++){
		if(!p_ctl->p_cfg->ch_blank[i][0]){
			memcpy(&p_ctl->p_cfg->ch_blank[i][0],&p_ctl->dis_en_ch_buf[0],2);
//			p_ctl->p_cfg->ch_blank[i] = p_ctl->dis_en_ch;
			recv_5_8_write_cfg(p_ctl);
			return;
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_dbg_remove_from_disable_ch(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	rtc6715_struct *p_ctl;
int i;
	p_ctl = (rtc6715_struct *)adf32;
	for(i = 0; i<RECV_5_8_MAX_NUM_BLANK_CH;i++){
//		if(p_ctl->dis_en_ch == p_ctl->p_cfg->ch_blank[i]){
//			p_ctl->p_cfg->ch_blank[i] = 0;
//			recv_5_8_write_cfg(p_ctl);
//			return;
//		}
		if(memcmp(p_ctl->dis_en_ch_buf,&p_ctl->p_cfg->ch_blank[i][0],2)==0){
			p_ctl->p_cfg->ch_blank[i][0] = 0;
			recv_5_8_write_cfg(p_ctl);
			return;
		}

	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_5_8_make_html_cntrl_table(rtc6715_struct* p_ctl, ctrl_table_list_struct* p_list){
SettingCell_t* p_cell;
uint8_t num=1;
SettingCell_t 	cell;
uint32_t 		i;
	memset(p_ctl->html_table,0,sizeof(p_ctl->html_table));

#ifdef	RECV_5_8_SUB_3_3
	p_list->name = "Config_3_3";
#else
	p_list->name = "Config_5_8";
#endif

	p_list->mode = div_time_att | div_wr_att | div_rd_att;// | div_collect_mode;//div_collect_mode
	//div_all_wr | div_collect_mode;// | div_collect_mode_wr;//div_all_rd
	p_list->p_start_cell = &p_ctl->html_table[0];
	p_cell = p_list->p_start_cell;

	cmd_h_add_to_cell_table_ext_size(
							p_cell,
							num++,
							p_ctl,
							NULL,
							NULL,
							0,
							FixIntegerCellType,

#ifdef	RECV_5_8_SUB_3_3
							"Config_3_3");
#else
							"Config_5_8");
#endif



	i = 1;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)rtc6715_dbg_read_freq;
		cell.WriteProc = (void*)(void*)rtc6715_dbg_write_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "freq";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 1;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_scan_time;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_scan_time;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "auto scan time";
		p_cell[i++] = cell;


	//view mode
		cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
		cell.CellType = FixIntegerCellType + 1,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_view_mode;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_view_mode;
		cell.LowLim = 1;
		cell.HighLim = recv_5_8_view_mode_normal;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "view normal";
		p_cell[i++] = cell;

		cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
		cell.CellType = FixIntegerCellType + 1,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_view_mode;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_view_mode;
		cell.LowLim = 1;
		cell.HighLim = recv_5_8_view_mode_inv;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "view invert";
		p_cell[i++] = cell;

		cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
		cell.CellType = FixIntegerCellType + 1,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_view_mode;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_view_mode;
		cell.LowLim = 1;
		cell.HighLim = recv_5_8_view_mode_inv_norm;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "view inv/norm";
		p_cell[i++] = cell;

#ifdef RECV_5_8_RSSI
		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
		cell.CellType = FixIntegerCellType + 1,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_en_rssi_mode;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_en_rssi_mode;
		cell.LowLim = 1;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Rssi mode";
		p_cell[i++] = cell;

		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
		cell.CellType = FixIntegerCellType + 1,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_en_only_scan_mode;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_en_only_scan_mode;
		cell.LowLim = 1;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Only scan";
		p_cell[i++] = cell;

		cell.CellAttr = WR_Att + Action_Att+ RD_Att,
		cell.CellType = FixIntegerCellType + 1,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_rssi_lvl;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_rssi_lvl;
		cell.LowLim = 1;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Lvl";
		p_cell[i++] = cell;

#endif
#ifdef	RTC6715_FREQ_MODE
		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
		cell.CellType = FixIntegerCellType + 1,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_freq_mode;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_freq_mode;
		cell.LowLim = 1;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Freq mode";
		p_cell[i++] = cell;
#endif
		if(!p_ctl->p_cfg->b_en_fast_scan){
			for(int gg=0;gg<RECV_5_8_MAX_NUM_LIT;gg++){
				cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
				cell.CellType = FixIntegerCellType + 1,
				cell.CellNumber = num++;
				cell.VarPtr = 0;
				cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_en_lit;
				cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_en_lit;
				cell.LowLim = 2;
				cell.HighLim = gg;//recv_5_8_lit_A_e;
				cell.DefaultValue = (uint32_t)p_ctl;
				cell.DescriptStr = ch_5_8_lit_table[gg];
				p_cell[i++] = cell;
			}
		}


#ifdef	RTC6715_FREQ_MODE

	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_freq_mode){

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_min_freq;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_min_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "min freq";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_max_freq;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_max_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "max freq";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_manual_step_freq;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_manual_step_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "manual step freq";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_auto_step_freq;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_auto_step_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "auto step freq";
		p_cell[i++] = cell;
	//1 point
		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_disable_freq;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_disable_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "dis. freq 1";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_disable_freq_len;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_disable_freq_len;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "dis. len 1(MHz)";
		p_cell[i++] = cell;
	//2 point
		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_disable_freq;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_disable_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 1;
		cell.DescriptStr = "dis. freq 2";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_disable_freq_len;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_disable_freq_len;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 1;
		cell.DescriptStr = "dis. len 2(MHz)";
		p_cell[i++] = cell;

	//2 point
		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_disable_freq;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_disable_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 2;
		cell.DescriptStr = "dis. freq 3";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_disable_freq_len;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_disable_freq_len;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 2;
		cell.DescriptStr = "dis. len 3(MHz)";
		p_cell[i++] = cell;

	//3 point
		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_disable_freq;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_disable_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 3;
		cell.DescriptStr = "dis. freq 3";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_disable_freq_len;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_disable_freq_len;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 3;
		cell.DescriptStr = "dis. len 3(MHz)";
		p_cell[i++] = cell;

	}
#endif
	if(p_ctl->p_cfg->freq_ch_mode == recv_5_8_ch_mode){

//		cell.CellAttr = Action_Att + RD_Att,
//		cell.CellType = FixStringCellType + 31,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_disable_ch;
//		cell.WriteProc = NULL;
//		cell.LowLim = 1;
//		cell.HighLim = 12;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch all";
//		p_cell[i++] = cell;

		cell.CellAttr = WR_Att + Action_Att + RD_Att,
		cell.CellType = FixStringCellType + 31,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)recv_5_8_dbg_get_dis_en_ch;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_set_dis_en_ch;
		cell.LowLim = 1;
		cell.HighLim = 11;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "add/remove ch";
		p_cell[i++] = cell;

		cell.CellAttr = WR_Att + Action_Att + Default_Attr,
		cell.CellType = FixIntegerCellType + 1,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_add_to_disable_ch;
		cell.LowLim = 1;
		cell.HighLim = 11;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "add to dis. ch";
		p_cell[i++] = cell;

		cell.CellAttr = WR_Att + Action_Att + Default_Attr,
		cell.CellType = FixIntegerCellType + 1,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = NULL;
		cell.WriteProc = (void*)(void*)recv_5_8_dbg_remove_from_disable_ch;
		cell.LowLim = 1;
		cell.HighLim = 11;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "rem. from dis. ch";
		p_cell[i++] = cell;
	}

}
