/*
 * recv_1_2.c
 *
 *  Created on: 6 февр. 2024 г.
 *      Author: Petr
 */

#include "string.h"
#include "recv_1_2.h"
#include "control.h"
//1640 - 840 (810)
#if defined(RECV_1_2) || defined(RECV_2_4)
#ifdef RECV_1_2_EXT
const uint16_t ch_1_2_table_freq_[RECV_1_2_MAX_NUM_CH] = {
		 //0  //1  //2   //3   //4   //5   //6   //7   //8   //9   //A   //B   //C   //D   //E   //F	//10  11	12	  13	14   15    16
		 910, 980, 1010, 1040, 1080, 1120, 1160, 1200, 1240, 1280, 1320, 1380, 1360, 1258, 1100, 1140, 1405, 1430, 1455, 1480, 1505, 1530, 1555, 1580, 1605, 1630, 1640, 840, 860, 880
};

#else
const uint16_t ch_1_2_table_freq_[RECV_1_2_MAX_NUM_CH] = {
		 //0  //1  //2   //3   //4   //5   //6   //7   //8   //9   //A   //B   //C   //D   //E   //F	//10  11	12	  13	14   15    16
		 910, 980, 1010, 1040, 1080, 1120, 1160, 1200, 1240, 1280, 1320, 1380, 1360, 1258, 1100, 1140, 1405, 1430, 1455, 1480, 1505, 1530, 1555
};
#endif
const uint16_t ch_1_5_table_freq[RECV_1_5_MAX_NUM_CH] = {
		 //0  //1  //2   //3   //4   //5   //6   //7   //8   //9   //A   //B
		 1405, 1430, 1455, 1480, 1505, 1530, 1555, 1580, 1605, 1630, 1655, 1680
};
const uint16_t ch_2_4_table_freq[RECV_2_4_MAX_NUM_CH] = {
		 2410, 2430, 2450, 2470, 2490, 2510, 2530, 2550, 2560, 2390, 2370, 2350, 2330, 2310, 2290,2270,2250,2230
};
//2230-2570
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 0-2,410 г/1-2,430 г/2-2,450 г/3-2,470 г/4-2,490 г/5-2,510 г/6-2,390 г/7-2,370 г/8-2,350G/9-2,330 Гц/ч-2,310G/C-2.290G
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RECV_1_2_EXT
const  char *ch_1_2_ch_dis_name[RECV_1_2_MAX_NUM_CH] = {
	"dis ch 0",
	"dis ch 1",
	"dis ch 2",
	"dis ch 3",
	"dis ch 4",
	"dis ch 5",
	"dis ch 6",
	"dis ch 7",
	"dis ch 8",
	"dis ch 9",
	"dis ch 10",
	"dis ch 11",
	"dis ch 12",
	"dis ch 13",
	"dis ch 14",
	"dis ch 15",
	"dis ch 16",
	"dis ch 17",
	"dis ch 18",
	"dis ch 19",
	"dis ch 20",
	"dis ch 21",
	"dis ch 22",
	"dis ch 23",
	"dis ch 24",
	"dis ch 25",
	"dis ch 26",
	"dis ch 27",
	"dis ch 28",
	"dis ch 29"
};
#else
const  char *ch_1_2_ch_dis_name[RECV_1_2_MAX_NUM_CH] = {
	"dis ch 0",
	"dis ch 1",
	"dis ch 2",
	"dis ch 3",
	"dis ch 4",
	"dis ch 5",
	"dis ch 6",
	"dis ch 7",
	"dis ch 8",
	"dis ch 9",
	"dis ch 10",
	"dis ch 11",
	"dis ch 12",
	"dis ch 13",
	"dis ch 14",
	"dis ch 15",
	"dis ch 16",
	"dis ch 17",
	"dis ch 18",
	"dis ch 19",
	"dis ch 20",
	"dis ch 21",
	"dis ch 22"

};

#endif
//1,405 г 1,430 г 1,455 г 1,480 г 1,505 г, 1,530 г, 1,555 г, 1,580 г 1,605 г 1,630 г 1,655 г 1,680 г

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_start_init(recv_1_2_struct *p_ctl){

	p_ctl->max_num_ch = RECV_1_2_MAX_NUM_CH;
	p_ctl->freq = RECV_1_2_START_FREQ;
	p_ctl->ch_cntr = 0;

	if(p_ctl->ver == recv_1_2_ver_1_5_){
		p_ctl->max_num_ch = RECV_1_5_MAX_NUM_CH;
		p_ctl->freq = RECV_1_5_START_FREQ;
	}
	if(p_ctl->ver == recv_1_2_ver_2_4_){
		p_ctl->max_num_ch = RECV_2_4_MAX_NUM_CH;
		p_ctl->freq = RECV_2_4_START_FREQ;
	}


//	p_ctl->ver = ver;
//	p_ctl->ch_cntr = 0;
//
//	p_ctl->i2c_addr = addr;
//	p_ctl->p_i2c = p_i2c;

//	switch_fdc_start_init(&p_ctl->sw_pwr_5v);
//	p_ctl->sw_pwr_5v.b_inveretd_ON = 1;
//	p_ctl->sw_pwr_5v.ON_pin = pwr_5v_pin;
//	p_ctl->sw_pwr_5v.ON_port = pwr_5v_port;
//	switch_fdc_on(&p_ctl->sw_pwr_5v,fdc_on);
//	switch_fdc_set_data(&p_ctl->sw_pwr_5v);
//
//	GPIO_InitStructure.Pin = p_ctl->sw_pwr_5v.ON_pin;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
//	HAL_GPIO_Init(p_ctl->sw_pwr_5v.ON_port, &GPIO_InitStructure);
//
////	switch_fdc_pin_init(&p_ctl->sw_pwr_5v);
//
//	switch_fdc_start_init(&p_ctl->sw_pwr_8v);
//	p_ctl->sw_pwr_8v.b_inveretd_ON = 0;
//	p_ctl->sw_pwr_8v.ON_pin = pwr_8v_pin;
//	p_ctl->sw_pwr_8v.ON_port = pwr_8v_port;
//	switch_fdc_on(&p_ctl->sw_pwr_8v,fdc_on);
//	switch_fdc_set_data(&p_ctl->sw_pwr_8v);
//
//	switch_fdc_pin_init(&p_ctl->sw_pwr_8v);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_default_init(recv_1_2_cfg_struct *p_cfg){

	memset(p_cfg,0,sizeof(recv_1_2_cfg_struct));
	p_cfg->min_freq = RECV_1_2_MIN_FREQ;
	p_cfg->max_freq = RECV_1_2_MAX_FREQ;
	p_cfg->auto_step_freq =  RECV_1_2_AUTO_FREQ;
	p_cfg->manual_step_freq =  RECV_1_2_MANUAL_FREQ;

	p_cfg->time = 5;
	p_cfg->view_mode = recv_1_2_view_mode_normal;

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_2_4_default_init(recv_1_2_cfg_struct *p_cfg){

	memset(p_cfg,0,sizeof(recv_1_2_cfg_struct));
	p_cfg->min_freq = RECV_2_4_MIN_FREQ;
	p_cfg->max_freq = RECV_2_4_MAX_FREQ;
	p_cfg->auto_step_freq =  RECV_2_4_AUTO_FREQ;
	p_cfg->manual_step_freq =  RECV_2_4_MANUAL_FREQ;

	p_cfg->time = 5;
	p_cfg->view_mode = recv_1_2_view_mode_normal;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_1_2_time_process(recv_1_2_struct *p_ctl, uint32_t time){
uint32_t res=0;
	p_ctl->time += time;
	if(p_ctl->time > (uint32_t)p_ctl->p_cfg->time * 1000){
		p_ctl->time = 0;
//		res = 1;
		if(p_ctl->p_cfg->view_mode==recv_1_2_view_mode_inv_norm){
			if(p_ctl->view_mode == recv_1_2_view_mode_now_normal){
				recv_1_2_make_view_mode(p_ctl,recv_1_2_view_mode_now_inv);
			}
			else{
				recv_1_2_make_view_mode(p_ctl,recv_1_2_view_mode_now_normal);
				recv_1_2_set_freq_up_auto(p_ctl,p_ctl->p_cfg->auto_step_freq);
				res = 1;
			}
		}
		else{
			recv_1_2_set_freq_up_auto(p_ctl,p_ctl->p_cfg->auto_step_freq);
			res = 1;
		}
	}
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_power_on(recv_1_2_struct *p_ctl){

	if((p_ctl->ver == recv_1_2_ver_1_2) || (p_ctl->ver == recv_1_2_ver_2_4_)){
		switch_fdc_on(&p_ctl->sw_pwr_5v,fdc_on);
//		switch_fdc_on(&p_ctl->sw_pwr_8v,fdc_on);

		switch_fdc_set_data(&p_ctl->sw_pwr_5v);
//		switch_fdc_set_data(&p_ctl->sw_pwr_8v);
	}

	osDelay(50);

//	if(p_ctl->ver == recv_1_2_ver_1_2){
	if((p_ctl->ver == recv_1_2_ver_1_2) || (p_ctl->ver == recv_1_2_ver_2_4_)){
//		MX_I2C3_Init();
	}

	switch (p_ctl->p_cfg->view_mode ){
	case recv_1_2_view_mode_normal:
		recv_1_2_make_view_mode(p_ctl, recv_1_2_view_mode_now_normal);
	break;
	case recv_1_2_view_mode_inv:
		recv_1_2_make_view_mode(p_ctl, recv_1_2_view_mode_now_inv);
	break;
	case recv_1_2_view_mode_inv_norm:
		recv_1_2_make_view_mode(p_ctl, recv_1_2_view_mode_now_normal);
	break;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_power_off(recv_1_2_struct *p_ctl){
//	if(p_ctl->ver == recv_1_2_ver_1_2){
	if((p_ctl->ver == recv_1_2_ver_1_2) || (p_ctl->ver == recv_1_2_ver_2_4_)){
//		HAL_I2C_DeInit(p_ctl->p_i2c);
	}

	osDelay(50);
//	if(p_ctl->ver == recv_1_2_ver_1_2){
	if((p_ctl->ver == recv_1_2_ver_1_2) || (p_ctl->ver == recv_1_2_ver_2_4_)){
		switch_fdc_on(&p_ctl->sw_pwr_5v,fdc_off);
//		switch_fdc_on(&p_ctl->sw_pwr_8v,fdc_off);

		switch_fdc_set_data(&p_ctl->sw_pwr_5v);
//		switch_fdc_set_data(&p_ctl->sw_pwr_8v);
	}

	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_make_view_mode(recv_1_2_struct *p_ctl, recv_1_2_view_mode_now_e mode){
	p_ctl->view_mode = mode;

	switch (p_ctl->view_mode){
	case recv_1_2_view_mode_now_normal:
		switch_fdc_on(&p_ctl->sw_da6_in,fdc_off);
	break;
	case recv_1_2_view_mode_now_inv:
		switch_fdc_on(&p_ctl->sw_da6_in,fdc_on);
	break;
	}
	switch_fdc_set_data(&p_ctl->sw_da6_in);

	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_write_cfg(recv_1_2_struct *p_ctl){
	dev_config_write(&control.dev_config);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//static uint32_t recv_1_2_write_buffer(I2C_HandleTypeDef *hi2c, uint8_t I2C_ADDRESS, uint8_t *aTxBuffer, uint8_t TXBUFFERSIZE){
//uint8_t err_cntr=0;
//    while(HAL_I2C_Master_Transmit(hi2c, (uint16_t)I2C_ADDRESS<<1, (uint8_t*)aTxBuffer, (uint16_t)TXBUFFERSIZE, (uint32_t)1000)!= HAL_OK) {
//        if (HAL_I2C_GetError(hi2c) == HAL_I2C_ERROR_AF){
//        	err_cntr++;
//        	if(err_cntr>5){
//        		return 2;
//        	}
//        	osDelay(10);
//        }
//        if (HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF){
//        	return 1;
//        }
//    }
//      while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY){
//
//      }
//      return 0;
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint32_t recv_1_2_write_buffer(struct sunxi_i2c  *hi2c, uint8_t I2C_ADDRESS, uint8_t *aTxBuffer, uint8_t TXBUFFERSIZE){
    struct i2c_msg msg;
    int ret;
    int retry = 4; // Не даем процу зависнуть вечно

    // Подготовка сообщения в стиле Sunxi
    msg.addr  = I2C_ADDRESS; // В sunxi_i2c_xfer адрес обычно 7-битный (НЕ сдвигай его!)
    msg.flags = 0;           // 0 - это Write
    msg.len   = TXBUFFERSIZE;
    msg.buf   = aTxBuffer;

    while (retry--) {
        ret = sunxi_i2c_xfer(hi2c, &msg, 1);
        if (ret > 0) {
            return 0; // Все ок, данные улетели
        }
        // Если ошибка (NACK или занято), можно чуть подождать
        // delay_us(100);
        osDelay(10);
    }
    return 1;
//    while(HAL_I2C_Master_Transmit(hi2c, (uint16_t)I2C_ADDRESS<<1, (uint8_t*)aTxBuffer, (uint16_t)TXBUFFERSIZE, (uint32_t)1000)!= HAL_OK) {
//        if (HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF){
//        	return 1;
//            //Error_Handler(3);
////			break;
//        }
//
//    }
//      while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY){
//
//      }
//      return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*void set_12_frequancy(uint16_t f)
{
    Delitel=f*8+3836; //считаем делитель
    DelitelH=Delitel>>8;
    DelitelL=Delitel&0XFF;

    Wire.beginTransmission(0x61);   // стучимся к синтезатору и передаем байты параметров
    Wire.write(DelitelH);
    Wire.write(DelitelL);
    Wire.write(0xCE);
    Wire.write(0x00);
    Wire.endTransmission();
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_1_2_set_freq(recv_1_2_struct *p_ad, uint16_t freq){
uint32_t delitel,delitel_h,delitel_l;
uint8_t buf[8];

	p_ad->freq = freq;

	if(freq < RECV_1_2_MIN_FREQ) freq = RECV_1_2_MIN_FREQ;
//	if(freq > RECV_1_2_MAX_FREQ) freq = RECV_1_2_MAX_FREQ;
	if((p_ad->ver == recv_1_2_ver_1_5_) || (p_ad->ver == recv_1_2_ver_1_2)){
		delitel = freq*8+3836; //считаем делитель
	}
	if((p_ad->ver == recv_1_2_ver_2_4_)){
		delitel = freq*8-3836; //считаем делитель
	}

	delitel_h = delitel>>8;
	delitel_l = delitel & 0xFF;

	buf[0] = delitel_h;
	buf[1] = delitel_l;
	buf[2] = 0xCE;
	buf[3] = 0;

//	printf("dta = %x %x \r\n",delitel_h,delitel_l);

	if((p_ad->ver == recv_1_2_ver_1_5_) || (p_ad->ver == recv_1_2_ver_2_4_)){
		buf[2] = 0x8E;
		buf[3] = 0xF0;
	}
uint32_t res;
	res = recv_1_2_write_buffer(p_ad->p_hi2c, p_ad->i2c_addr, buf, 4);
	if(res){
//		p_ad->i2c_addr = 0x63;
//		recv_1_2_write_buffer(p_ad->p_i2c, p_ad->i2c_addr, buf, 4);
	}

	if((p_ad->ver == recv_1_2_ver_1_5_) || (p_ad->ver == recv_1_2_ver_2_4_)){
		recv_1_2_write_buffer(p_ad->p_hi2c, p_ad->i2c_addr, buf, 4);
	}

  return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 0-2,410 г/1-2,430 г/2-2,450 г/3-2,470 г/4-2,490 г/5-2,510 г/6-2,390 г/7-2,370 г/8-2,350G/9-2,330 Гц/ч-2,310G/C-2.290G
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void recv_1_2_invert_video(recv_1_2_struct *p_ctl){
//	if(p_ctl->sw_da6_in.state==fdc_on){
//		switch_fdc_on(&p_ctl->sw_da6_in,fdc_off);
//	}
//	else{
//		switch_fdc_on(&p_ctl->sw_da6_in,fdc_on);
//	}
//	switch_fdc_set_data(&p_ctl->sw_da6_in);
//  return ;
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_1_2_set_freq_up(recv_1_2_struct *p_ctl, uint16_t freq){
	if(p_ctl->p_cfg->freq_ch_mode == recv_1_2_freq_mode){
		if((p_ctl->freq + freq) > p_ctl->p_cfg->max_freq){
			recv_1_2_set_freq(p_ctl, p_ctl->p_cfg->min_freq);
		}
		else{
			recv_1_2_set_freq(p_ctl, p_ctl->freq + freq);
		}
	}
	if(p_ctl->p_cfg->freq_ch_mode == recv_1_2_ch_mode){
		p_ctl->ch_cntr++;
		if(p_ctl->ch_cntr >= p_ctl->max_num_ch){
			p_ctl->ch_cntr = 0;
		}
		if(p_ctl->ver == recv_1_2_ver_1_2){
			recv_1_2_set_freq(p_ctl,ch_1_2_table_freq_[p_ctl->ch_cntr]);
		}
		if(p_ctl->ver == recv_1_2_ver_1_5_){
			recv_1_2_set_freq(p_ctl,ch_1_5_table_freq[p_ctl->ch_cntr]);
		}
		if(p_ctl->ver == recv_1_2_ver_2_4_){
			recv_1_2_set_freq(p_ctl,ch_2_4_table_freq[p_ctl->ch_cntr]);
		}

	}
  return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_1_2_set_freq_up_auto(recv_1_2_struct *p_ctl, uint16_t freq){
uint8_t b_find;
	if(p_ctl->p_cfg->freq_ch_mode == recv_1_2_ch_mode){

		p_ctl->ch_cntr++;
		if(p_ctl->ch_cntr >= p_ctl->max_num_ch){
			p_ctl->ch_cntr = 0;
		}
		if(control.freq_mode == cntrl_freq_mode_auto){
			for(int i=0;i<p_ctl->max_num_ch;i++){
				b_find = 0;
				for(int j=0;j<RECV_1_2_MAX_NUM_BLANK_CH;j++){
//					if(p_ctl->p_cfg->ch_blank[j] == p_ctl->ch_cntr){
					if(p_ctl->p_cfg->ch_blank[p_ctl->ch_cntr]){
						b_find = 1;
						break;
					}
				}
				if (!b_find) break;
				p_ctl->ch_cntr++;
				if(p_ctl->ch_cntr >= p_ctl->max_num_ch){
					p_ctl->ch_cntr = 0;
				}
			}
		}
//		recv_1_2_set_freq(p_ctl,ch_1_2_table_freq[p_ctl->ch_cntr]);
		if(p_ctl->ver == recv_1_2_ver_1_2){
			recv_1_2_set_freq(p_ctl,ch_1_2_table_freq_[p_ctl->ch_cntr]);
		}
		if(p_ctl->ver == recv_1_2_ver_1_5_){
			recv_1_2_set_freq(p_ctl,ch_1_5_table_freq[p_ctl->ch_cntr]);
		}
		if(p_ctl->ver == recv_1_2_ver_2_4_){
			recv_1_2_set_freq(p_ctl,ch_2_4_table_freq[p_ctl->ch_cntr]);
		}

	}
	if(p_ctl->p_cfg->freq_ch_mode == recv_1_2_freq_mode){
		if((p_ctl->freq + freq) > p_ctl->p_cfg->max_freq){
			recv_1_2_set_freq(p_ctl, p_ctl->p_cfg->min_freq);
		}
		else{
			recv_1_2_set_freq(p_ctl, p_ctl->freq + freq);
		}
	}
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t recv_1_2_set_freq_down(recv_1_2_struct *p_ctl, uint16_t freq){
	if(p_ctl->p_cfg->freq_ch_mode == recv_1_2_ch_mode){
		if(p_ctl->ch_cntr==0){
			p_ctl->ch_cntr = p_ctl->max_num_ch - 1;
		}
		else{
			p_ctl->ch_cntr--;
		}
//		recv_1_2_set_freq(p_ctl,ch_1_2_table_freq[p_ctl->ch_cntr]);
		if(p_ctl->ver == recv_1_2_ver_1_2){
			recv_1_2_set_freq(p_ctl,ch_1_2_table_freq_[p_ctl->ch_cntr]);
		}
		if(p_ctl->ver == recv_1_2_ver_1_5_){
			recv_1_2_set_freq(p_ctl,ch_1_5_table_freq[p_ctl->ch_cntr]);
		}
		if(p_ctl->ver == recv_1_2_ver_2_4_){
			recv_1_2_set_freq(p_ctl,ch_2_4_table_freq[p_ctl->ch_cntr]);
		}

	}
	if(p_ctl->p_cfg->freq_ch_mode == recv_1_2_freq_mode){
		if((p_ctl->freq - freq) < p_ctl->p_cfg->min_freq){
			recv_1_2_set_freq(p_ctl, p_ctl->p_cfg->max_freq);
		}
		else{
			recv_1_2_set_freq(p_ctl, p_ctl->freq - freq);
		}
	}
  return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_dbg_write_reg_func(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ad;
	p_ad = (recv_1_2_struct *)ad32;

//	printf("ad wrt addr = %x, data = %x,\r\n",p_ad->addr, *p8_data);
//
//	rtc6715_write_reg(p_ad,*(uint32_t*)p8_data,p_ad->addr);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_1_2_dbg_read_reg_func(uint32_t ad32, uint32_t num, uint32_t tmp){
	recv_1_2_struct *p_ad;
	p_ad = (recv_1_2_struct *)ad32;
//	rtc6715_read_reg(p_ad,&p_ad->reg_data,p_ad->addr);

	return (uint8_t*)&p_ad->reg_data;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_dbg_write_freq(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ad;
	p_ad = (recv_1_2_struct *)ad32;

//	control.freq_mode = cntrl_freq_mode_manual;
//	control.auto_mode = cntrl_auto_mode_dis;

	recv_1_2_set_freq(p_ad,*(uint16_t*)p8_data);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_1_2_dbg_read_freq(uint32_t ad32, uint32_t num, uint32_t tmp){
	recv_1_2_struct *p_ad;
	p_ad = (recv_1_2_struct *)ad32;
	return (uint8_t*)&p_ad->freq;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* recv_1_2_dbg_get_disable_ch(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
uint8_t pos,cntr,num;
	p_ctl = (recv_1_2_struct *)adf32;
	memset(control.povorot.ack_dbg_tmp,0,sizeof(control.povorot.ack_dbg_tmp));
	cntr=0;
	num = p_ctl->p_cfg->ch_blank[RECV_1_2_MAX_NUM_BLANK_CH];
	for(int i=0;i<num;i++){
//		if(p_ctl->p_cfg->ch_blank[i]){
			pos = strlen(control.povorot.ack_dbg_tmp);
			sprintf((char*)&control.povorot.ack_dbg_tmp[pos],"%d ",p_ctl->p_cfg->ch_blank[i]);
			cntr++;
			if(cntr>8) break;
//		}
	}
	return (uint8_t*)control.povorot.ack_dbg_tmp;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* recv_1_2_dbg_get_dis_en_ch(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)adf32;
	return (uint8_t*)&p_ctl->dis_en_ch;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_dbg_set_dis_en_ch(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)adf32;
	p_ctl->dis_en_ch = *p8_data;
	if(p_ctl->dis_en_ch >= RECV_1_2_MAX_NUM_CH){
		p_ctl->dis_en_ch = RECV_1_2_MAX_NUM_CH - 1;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_dbg_add_to_disable_ch(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
recv_1_2_struct *p_ctl;
int i;
uint8_t num;
	p_ctl = (recv_1_2_struct *)adf32;
	num = p_ctl->p_cfg->ch_blank[RECV_1_2_MAX_NUM_BLANK_CH];
	if(num >= RECV_1_2_MAX_NUM_BLANK_CH) return;
	for(i = 0; i < num;i++){
		if(p_ctl->dis_en_ch == p_ctl->p_cfg->ch_blank[i]) return;
	}
//	for(i = 0; i < RECV_1_2_MAX_NUM_CH; i++){
//		if(p_ctl->dis_en_ch == ch_1_2_table[i]) break;
//	}
//	if(i >= RECV_1_2_MAX_NUM_CH) return;

	p_ctl->p_cfg->ch_blank[num] = p_ctl->dis_en_ch;
	p_ctl->p_cfg->ch_blank[RECV_1_2_MAX_NUM_BLANK_CH] += 1;

//	for(i = 0; i<RECV_1_2_MAX_NUM_BLANK_CH;i++){
//		if(!p_ctl->p_cfg->ch_blank[i]){
//			p_ctl->p_cfg->ch_blank[i] = p_ctl->dis_en_ch;
//			break;
//		}
//	}
	recv_1_2_write_cfg(p_ctl);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_dbg_remove_from_disable_ch(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
recv_1_2_struct *p_ctl;
int i,j;
uint8_t num;
	p_ctl = (recv_1_2_struct *)adf32;
	num = p_ctl->p_cfg->ch_blank[RECV_1_2_MAX_NUM_BLANK_CH];
	for(i = 0; i < num;i++){
		if(p_ctl->dis_en_ch == p_ctl->p_cfg->ch_blank[i]){
			break;
		}
	}
	if(i >= num) return;

	for(j = i; j < (num-1); j++){
		p_ctl->p_cfg->ch_blank[j] = p_ctl->p_cfg->ch_blank[j+1];
	}

	p_ctl->p_cfg->ch_blank[RECV_1_2_MAX_NUM_BLANK_CH] -= 1;

	recv_1_2_write_cfg(p_ctl);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t recv_1_2_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, recv_1_2_struct *p_ctl, const char *p_name){
SettingCell_t 	cell;
uint32_t 		i;
	i=0;

	for(i=0;i<COMPONENT_MAX_TABLE_SIZE;i++){
		if(!p_cell[i].CellNumber) break;
	}
	if( i>= COMPONENT_MAX_TABLE_SIZE) return num;

	cell.CellAttr = RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 2;
	cell.VarPtr = &p_ctl->reg_addr;
	cell.ReadProc = 0;
	cell.WriteProc = 0;
	cell.HighLim = 0;
	cell.DefaultValue = 0;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "recv 1_2 addr";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)recv_1_2_dbg_read_reg_func;
	cell.WriteProc = (void*)(void*)recv_1_2_dbg_write_reg_func;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "recv 1_2 reg";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 2;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)recv_1_2_dbg_read_freq;
	cell.WriteProc = (void*)(void*)recv_1_2_dbg_write_freq;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "recv 1_2 freq";
	p_cell[i++] = cell;

	sw_fdc_add_to_cell_table(p_cell, num++, &p_ctl->sw_pwr_5v, "recv12 sw 5v");
//	sw_fdc_add_to_cell_table(p_cell, num++, &p_ctl->sw_pwr_8v, "recv12 sw 8v");

	sw_fdc_add_to_cell_table(p_cell, num++, &p_ctl->sw_da6_in, "recv12 da6 in");

	return num;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_dbg_set_scan_time(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	p_ctl->p_cfg->time = *p8_data;
	if(!p_ctl->p_cfg->time) p_ctl->p_cfg->time = 1;

	recv_1_2_write_cfg(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_1_2_dbg_get_scan_time(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	return &p_ctl->p_cfg->time;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_dbg_set_min_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	p_ctl->p_cfg->min_freq = *(uint16_t*)p8_data;
	if(p_ctl->p_cfg->min_freq < RECV_1_2_MIN_FREQ) p_ctl->p_cfg->min_freq = RECV_1_2_MIN_FREQ;
	if(p_ctl->p_cfg->min_freq > RECV_1_2_MAX_FREQ) p_ctl->p_cfg->min_freq = RECV_1_2_MIN_FREQ;

	recv_1_2_write_cfg(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_1_2_dbg_get_min_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	return (uint8_t*)&p_ctl->p_cfg->min_freq;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_dbg_set_max_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	p_ctl->p_cfg->max_freq = *(uint16_t*)p8_data;
	if(p_ctl->p_cfg->max_freq < RECV_1_2_MIN_FREQ) p_ctl->p_cfg->max_freq = RECV_1_2_MAX_FREQ;
	if(p_ctl->p_cfg->max_freq > RECV_1_2_MAX_FREQ) p_ctl->p_cfg->max_freq = RECV_1_2_MAX_FREQ;

	recv_1_2_write_cfg(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_1_2_dbg_get_max_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	return (uint8_t*)&p_ctl->p_cfg->max_freq;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_dbg_set_manual_step_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	p_ctl->p_cfg->manual_step_freq = *(uint16_t*)p8_data;
	if(!p_ctl->p_cfg->manual_step_freq) p_ctl->p_cfg->manual_step_freq = 1;

	recv_1_2_write_cfg(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_1_2_dbg_get_manual_step_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	return (uint8_t*)&p_ctl->p_cfg->manual_step_freq;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_dbg_set_auto_step_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	p_ctl->p_cfg->auto_step_freq = *(uint16_t*)p8_data;
	if(!p_ctl->p_cfg->auto_step_freq) p_ctl->p_cfg->auto_step_freq = 1;

	recv_1_2_write_cfg(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_1_2_dbg_get_auto_step_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	return (uint8_t*)&p_ctl->p_cfg->auto_step_freq;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_dbg_set_disable_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	p_ctl->p_cfg->dis_start_freq[num] = *(uint16_t*)p8_data;
	if(p_ctl->p_cfg->dis_start_freq[num] < RECV_1_2_MIN_FREQ) p_ctl->p_cfg->dis_start_freq[num] = RECV_1_2_MIN_FREQ;
	if(p_ctl->p_cfg->dis_start_freq[num] > RECV_1_2_MAX_FREQ) p_ctl->p_cfg->dis_start_freq[num] = RECV_1_2_MAX_FREQ;
	recv_1_2_write_cfg(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_1_2_dbg_get_disable_freq(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	return (uint8_t*)&p_ctl->p_cfg->dis_start_freq[num];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_dbg_set_disable_freq_len(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	p_ctl->p_cfg->dis_len_freq[num] = *(uint16_t*)p8_data;
	recv_1_2_write_cfg(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_1_2_dbg_get_disable_freq_len(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	return (uint8_t*)&p_ctl->p_cfg->dis_len_freq[num];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void recv_1_2_dbg_set_view_mode(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	p_ctl->p_cfg->view_mode = (recv_1_2_view_mode_e)cmd;
	switch (p_ctl->p_cfg->view_mode ){
	case recv_1_2_view_mode_normal:
		recv_1_2_make_view_mode(p_ctl, recv_1_2_view_mode_now_normal);
	break;
	case recv_1_2_view_mode_inv:
		recv_1_2_make_view_mode(p_ctl, recv_1_2_view_mode_now_inv);
	break;
	case recv_1_2_view_mode_inv_norm:
		recv_1_2_make_view_mode(p_ctl, recv_1_2_view_mode_now_normal);
	break;
	}
	recv_1_2_write_cfg(p_ctl);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_1_2_dbg_get_view_mode(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	p_ctl->tmp[0] = 0;
	if(p_ctl->p_cfg->view_mode == cmd){
		p_ctl->tmp[0] = 1;
	}
	return (uint8_t*)&p_ctl->tmp[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* recv_1_2_dbg_get_freq_mode(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	p_ctl->tmp[0] = 0;

	if(p_ctl->p_cfg->freq_ch_mode == recv_1_2_freq_mode){
		p_ctl->tmp[0] = 1;
	}
	return (uint8_t*)&p_ctl->tmp[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void recv_1_2_dbg_set_freq_mode(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)ctl32;
	if(*p8_data){
		p_ctl->p_cfg->freq_ch_mode = recv_1_2_freq_mode;
	}
	else{
		p_ctl->p_cfg->freq_ch_mode = recv_1_2_ch_mode;
	}
	recv_1_2_write_cfg(p_ctl);
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* ch_cntrl_dbg_get_disable_ch(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)adf32;
	return (uint8_t*)&p_ctl->p_cfg->ch_blank[cmd];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void ch_cntrl_dbg_set_disable_ch(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	recv_1_2_struct *p_ctl;
	p_ctl = (recv_1_2_struct *)adf32;
	p_ctl->p_cfg->ch_blank[cmd] = 0;
	if(*p8_data){
		p_ctl->p_cfg->ch_blank[cmd] = 1;
	}
	recv_1_2_write_cfg(p_ctl);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recv_1_2_make_html_cntrl_table(recv_1_2_struct* p_ctl, ctrl_table_list_struct* p_list){
SettingCell_t* p_cell;
uint8_t num=1;
SettingCell_t 	cell;
uint32_t 		i;
	memset(p_ctl->html_table,0,sizeof(p_ctl->html_table));
	if(p_ctl->ver == recv_1_2_ver_1_2){
		p_list->name = "Config_1_2";
	}
	if(p_ctl->ver == recv_1_2_ver_1_5_){
		p_list->name = "Config_1_5";
	}
	if(p_ctl->ver == recv_1_2_ver_2_4_){
		p_list->name = "Config_2_4";
	}


	p_list->mode = div_time_att | div_wr_att | div_rd_att;// | div_collect_mode;//div_collect_mode
	//div_all_wr | div_collect_mode;// | div_collect_mode_wr;//div_all_rd
	p_list->p_start_cell = &p_ctl->html_table[0];
	p_cell = p_list->p_start_cell;

	if(p_ctl->ver == recv_1_2_ver_1_2){
		cmd_h_add_to_cell_table_ext_size(
								p_cell,
								num++,
								p_ctl,
								NULL,
								NULL,
								0,
								FixIntegerCellType,
								"Config_1_2");
	}
	if(p_ctl->ver == recv_1_2_ver_1_5_){
		cmd_h_add_to_cell_table_ext_size(
								p_cell,
								num++,
								p_ctl,
								NULL,
								NULL,
								0,
								FixIntegerCellType,
								"Config_1_5");
	}

	if(p_ctl->ver == recv_1_2_ver_2_4_){
		cmd_h_add_to_cell_table_ext_size(
								p_cell,
								num++,
								p_ctl,
								NULL,
								NULL,
								0,
								FixIntegerCellType,
								"Config_2_4");
	}

	i = 1;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 2;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)recv_1_2_dbg_read_freq;
	cell.WriteProc = (void*)(void*)recv_1_2_dbg_write_freq;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "freq";
	p_cell[i++] = cell;

	cell.CellAttr = RD_Att + Action_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_scan_time;
	cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_scan_time;
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
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_view_mode;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_view_mode;
		cell.LowLim = 1;
		cell.HighLim = recv_1_2_view_mode_normal;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "view normal";
		p_cell[i++] = cell;

		cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
		cell.CellType = FixIntegerCellType + 1,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_view_mode;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_view_mode;
		cell.LowLim = 1;
		cell.HighLim = recv_1_2_view_mode_inv;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "view invert";
		p_cell[i++] = cell;

		cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
		cell.CellType = FixIntegerCellType + 1,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_view_mode;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_view_mode;
		cell.LowLim = 1;
		cell.HighLim = recv_1_2_view_mode_inv_norm;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "view inv/norm";
		p_cell[i++] = cell;

		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
		cell.CellType = FixIntegerCellType + 1,
		cell.CellNumber = num++;
		cell.VarPtr = 0;
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_freq_mode;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_freq_mode;
		cell.LowLim = 1;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.DescriptStr = "Freq mode";
		p_cell[i++] = cell;

	if(p_ctl->p_cfg->freq_ch_mode == recv_1_2_freq_mode){

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_min_freq;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_min_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "min freq";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_max_freq;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_max_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "max freq";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_manual_step_freq;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_manual_step_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "manual step freq";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_auto_step_freq;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_auto_step_freq;
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
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_disable_freq;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_disable_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 0;
		cell.DescriptStr = "dis. freq 1";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_disable_freq_len;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_disable_freq_len;
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
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_disable_freq;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_disable_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 1;
		cell.DescriptStr = "dis. freq 2";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_disable_freq_len;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_disable_freq_len;
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
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_disable_freq;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_disable_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 2;
		cell.DescriptStr = "dis. freq 3";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_disable_freq_len;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_disable_freq_len;
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
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_disable_freq;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_disable_freq;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 3;
		cell.DescriptStr = "dis. freq 3";
		p_cell[i++] = cell;

		cell.CellAttr = RD_Att + Action_Att + WR_Att;
		cell.CellType = FixIntegerCellType + 2;
		cell.VarPtr = NULL;
		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_disable_freq_len;
		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_disable_freq_len;
		cell.HighLim = 0;
		cell.DefaultValue = (uint32_t)p_ctl;
		cell.CellNumber = num++;
		cell.LowLim = 3;
		cell.DescriptStr = "dis. len 3(MHz)";
		p_cell[i++] = cell;
	}

	if(p_ctl->p_cfg->freq_ch_mode == recv_1_2_ch_mode){

		for(int k=0;k<p_ctl->max_num_ch;k++){
			cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
			cell.CellType = FixIntegerCellType + 1,
			cell.CellNumber = num++;
			cell.VarPtr = 0;
			cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
			cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
			cell.LowLim = 1;
			cell.HighLim = k;
			cell.DefaultValue = (uint32_t)p_ctl;
			cell.DescriptStr = ch_1_2_ch_dis_name[k];
			p_cell[i++] = cell;

		}

//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 0;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 0";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 1;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 1";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 2;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 2";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 3;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 3";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 4;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 4";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 5;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 5";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 6;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 6";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 7;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 7";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 8;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 8";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 9;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 9";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 10;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 10";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 11;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 11";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 12;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 12";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 13;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 13";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 14;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 14";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)ch_cntrl_dbg_get_disable_ch;
//		cell.WriteProc = (void*)(void*)ch_cntrl_dbg_set_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 15;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch 15";
//		p_cell[i++] = cell;


//		cell.CellAttr = Action_Att + RD_Att,
//		cell.CellType = VarStringCellType + 31,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_disable_ch;
//		cell.WriteProc = NULL;
//		cell.LowLim = 1;
//		cell.HighLim = 12;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "dis ch all";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = WR_Att + Action_Att + RD_Att,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = (void*)(void*)recv_1_2_dbg_get_dis_en_ch;
//		cell.WriteProc = (void*)(void*)recv_1_2_dbg_set_dis_en_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 11;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "add/remove ch";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = WR_Att + Action_Att + Default_Attr,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = NULL;
//		cell.WriteProc = (void*)(void*)recv_1_2_dbg_add_to_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 11;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "add to dis. ch";
//		p_cell[i++] = cell;
//
//		cell.CellAttr = WR_Att + Action_Att + Default_Attr,
//		cell.CellType = FixIntegerCellType + 1,
//		cell.CellNumber = num++;
//		cell.VarPtr = 0;
//		cell.ReadProc = NULL;
//		cell.WriteProc = (void*)(void*)recv_1_2_dbg_remove_from_disable_ch;
//		cell.LowLim = 1;
//		cell.HighLim = 11;
//		cell.DefaultValue = (uint32_t)p_ctl;
//		cell.DescriptStr = "rem. from dis. ch";
//		p_cell[i++] = cell;
	}
}
//0 - 15076
//1 - 15276
//3  15476
//https://aliexpress.ru/item/1005006067477030.html?spm=a2g2w.orderdetail.0.0.62ee4aa6WCQLvY&sku_id=12000035578088863
//Частота приема: 0-2410 г/1-2430 г/2-2,45OG/3-2,47OG/4-2.49OG/ 5-2510 г/6-2390 г/7-2370 г/8-2350 г/9-2330 г/ч-2310 г/C-2.290G

#endif
