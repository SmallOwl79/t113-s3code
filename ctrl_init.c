/*
 * ctrl_init.c
 *
 *  Created on: 3 февр. 2024 г.
 *      Author: Petr
 */
#include "string.h"
//#include "stm32f4xx_hal.h"
#include "sunxi_dma.h"
#include "ctrl_init.h"

extern uint8_t video_luma_buf_csi[1024*1024];
extern uint8_t video_chroma_buf_csi[1024*1024];

float goertzel_mag(uint16_t* data, int n, int k);

void CSI_DMA_IRQHandler(void);
uint8_t reg_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, control_struct *p_ctl);

const cmd_cell_param_s main_cell_param={
	    0,//uint16_t    x_offset;
	    200,//uint16_t	wide;
	    20,//uint16_t	height;
	    5,//uint16_t	height_space;
	    120,//uint16_t	name_wide;
	    180,//uint16_t	edit_wide;
	    50,//uint16_t	exec_but_wide;
	    50,//uint16_t	sys_but_1_wide;
	    25,//uint16_t	sys_but_2_wide;
};
const cmd_cell_param_s table_cell_param={
	    0,//uint16_t    x_offset;
	    200,//uint16_t	wide;
	    20,//uint16_t	height;
	    5,//uint16_t	height_space;
	    120,//uint16_t	name_wide;
	    80,//uint16_t	edit_wide;
	    50,//uint16_t	exec_but_wide;
	    50,//uint16_t	sys_but_1_wide;
	    25,//uint16_t	sys_but_2_wide;
};

uint8_t gpadc_init(control_struct *p_ctl, uint8_t table_pos);

//#include "lwip/api.h"

////tvp_master_init
////extern I2C_HandleTypeDef hi2c1;
//
//I2C_HandleTypeDef hi2c1;
//I2C_HandleTypeDef hi2c3;
//DMA_HandleTypeDef hdma_i2c1_rx;
//DMA_HandleTypeDef hdma_i2c1_tx;
//DMA_HandleTypeDef hdma_i2c3_rx;
//DMA_HandleTypeDef hdma_i2c3_tx;
//
//ADC_HandleTypeDef    AdcHandle,AdcHandle2;
//
//TIM_HandleTypeDef    htim4;
//#ifdef	MULTI_EMIT
//TIM_HandleTypeDef    htim1;
//#endif
//TIM_HandleTypeDef    TimHandle;
////DMA_HandleTypeDef hdma_tim4_up;
//
//static DMA_HandleTypeDef  hdma_adc1;
//static DMA_HandleTypeDef  hdma_adc2;
//
//#define TIMER_FREQUENCY                ((uint32_t) 500)    /* Timer frequency (unit: Hz). With a timer 16 bits and time base freq min 1Hz, range is min=1Hz, max=32kHz. */
//#define TIMER_FREQUENCY_RANGE_MIN      ((uint32_t)    1)    /* Timer minimum frequency (unit: Hz). With a timer 16 bits, maximum frequency will be 32000 times this value. */
//#define TIMER_PRESCALER_MAX_VALUE      (0xFFFF-1)           /* Timer prescaler maximum value (0xFFFF for a timer 16 bits) */
//
//#ifndef NOT_VIDEO_RECEIVER
//#pragma location = ".ccram"
//uint8_t grab_image_dst[GRABBER_DEF_WIDTH*(96+6)];
//#define image_buffer grab_image_dst
//#endif
//
//#ifdef EXT_FLASH_SUPPORT
//#pragma location = ".ccram"
//uint8_t ff_buf[4096];
//FATFS fs;
//#endif
//
//void MX_TIM4_Init(uint32_t period, uint32_t imp);
//void MX_TIM1_Init(uint32_t period, uint32_t imp);
//
////#ifdef UDP_SENDER
//
//#ifndef NOT_VIDEO_RECEIVER
//uint8_t grab_image_dst_ext[GRABBER_DEF_WIDTH*24];
////#endif
//
////#ifdef UDP_SENDER
////uint32_t	grab_image[GRABBER_DEF_WIDTH*8];
//uint32_t	grab_image[GRABBER_DEF_WIDTH*4];
////#endif
//
//#endif
//
//osThreadDef (uni_prot_thread, osPriorityBelowNormal, UNI_PROT_THREAD_NUM, UNI_PROT_THREAD_SIZE);
//
//
//extern const osThreadDef_t os_thread_def_short_time_thread;
//
//osMessageQDef(short_time_msg_que, SHORT_TIME_QUE_MSG_POOL_SIZE, short_msg_s); // Define message queue
//osPoolDef(short_time_msg_que_pool, SHORT_TIME_QUE_MSG_POOL_SIZE, short_msg_s); // Define memory pool
//
//osMessageQDef(control_sys_msg_que, CONTROL_QUE_MSG_POOL_SIZE, control_sys_msg_que_type_s); // Define message queue
//osPoolDef(control_sys_msg_que_pool, CONTROL_QUE_MSG_POOL_SIZE, control_sys_msg_que_type_s); // Define memory pool
//
//void grabber_half_dma_get(struct __DMA_HandleTypeDef* p_ctl);
//void grabber_all_dma_get(struct __DMA_HandleTypeDef* p_ctl);
///* I2C1 init function */
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void Error_Handler(void){
//
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void MX_I2C1_Init(void){
//
//  hi2c1.Instance = I2C1;
//  hi2c1.Init.ClockSpeed = 100000;
//  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
//  hi2c1.Init.OwnAddress1 = 0;
//  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
//  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
//  hi2c1.Init.OwnAddress2 = 0;
//  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_ENABLED;
//  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_ENABLED;
//  HAL_I2C_Init(&hi2c1);
//
//}
//#if defined(RECV_1_2) || defined(RECV_2_4) || defined(RECEIVER_EXT_MODE)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void MX_I2C3_Init(void){
//
//  hi2c3.Instance = I2C3;
//  hi2c3.Init.ClockSpeed = 400000;
//  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
//  hi2c3.Init.OwnAddress1 = 0;
//  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
//  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
//  hi2c3.Init.OwnAddress2 = 0;
//  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_ENABLED;
//  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_ENABLED;
//  HAL_I2C_Init(&hi2c3);
//
//}
//#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c){
//
//  GPIO_InitTypeDef GPIO_InitStruct;
//  if(hi2c->Instance==I2C1){
//    GPIO_InitStruct.Pin = VID3_SCL_PIN | VID3_SDA_PIN;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
//    HAL_GPIO_Init(VID3_SCL_PORT, &GPIO_InitStruct);
//
//    /* Peripheral clock enable */
//    __I2C1_CLK_ENABLE();
//
//    hdma_i2c1_rx.Instance = DMA1_Stream0;
//    hdma_i2c1_rx.Init.Channel = DMA_CHANNEL_1;
//    hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
//    hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
//    hdma_i2c1_rx.Init.MemInc = DMA_MINC_DISABLE;
//    hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//    hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//    hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
//    hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_LOW;
//    hdma_i2c1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//    hdma_i2c1_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
//    hdma_i2c1_rx.Init.MemBurst = DMA_MBURST_SINGLE;
//    hdma_i2c1_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
//    HAL_DMA_Init(&hdma_i2c1_rx);
//
//    __HAL_LINKDMA(hi2c,hdmarx,hdma_i2c1_rx);
//
//    hdma_i2c1_tx.Instance = DMA1_Stream6;
//    hdma_i2c1_tx.Init.Channel = DMA_CHANNEL_1;
//    hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
//    hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
//    hdma_i2c1_tx.Init.MemInc = DMA_MINC_DISABLE;
//    hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//    hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//    hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
//    hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_LOW;
//    hdma_i2c1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//    hdma_i2c1_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
//    hdma_i2c1_tx.Init.MemBurst = DMA_MBURST_SINGLE;
//    hdma_i2c1_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
//    HAL_DMA_Init(&hdma_i2c1_tx);
//
//    __HAL_LINKDMA(hi2c,hdmatx,hdma_i2c1_tx);
//
//    /* Peripheral interrupt init*/
//    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
//    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
//  }
//  if(hi2c->Instance==I2C3){
//    GPIO_InitStruct.Pin = VID3_SDA5V_PIN;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
//    HAL_GPIO_Init(VID3_SDA5V_PORT, &GPIO_InitStruct);
//
//    GPIO_InitStruct.Pin = VID3_SCL5V_PIN;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
//    HAL_GPIO_Init(VID3_SCL5V_PORT, &GPIO_InitStruct);
//
//
//    /* Peripheral clock enable */
//    __I2C3_CLK_ENABLE();
//
//    hdma_i2c3_rx.Instance = DMA1_Stream2;
//    hdma_i2c3_rx.Init.Channel = DMA_CHANNEL_3;
//    hdma_i2c3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
//    hdma_i2c3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
//    hdma_i2c3_rx.Init.MemInc = DMA_MINC_DISABLE;
//    hdma_i2c3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//    hdma_i2c3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//    hdma_i2c3_rx.Init.Mode = DMA_NORMAL;
//    hdma_i2c3_rx.Init.Priority = DMA_PRIORITY_LOW;
//    hdma_i2c3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//    hdma_i2c3_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
//    hdma_i2c3_rx.Init.MemBurst = DMA_MBURST_SINGLE;
//    hdma_i2c3_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
//    HAL_DMA_Init(&hdma_i2c3_rx);
//
//    __HAL_LINKDMA(hi2c,hdmarx,hdma_i2c3_rx);
//
//    hdma_i2c3_tx.Instance = DMA1_Stream4;
//    hdma_i2c3_tx.Init.Channel = DMA_CHANNEL_3;
//    hdma_i2c3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
//    hdma_i2c3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
//    hdma_i2c3_tx.Init.MemInc = DMA_MINC_DISABLE;
//    hdma_i2c3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//    hdma_i2c3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//    hdma_i2c3_tx.Init.Mode = DMA_NORMAL;
//    hdma_i2c3_tx.Init.Priority = DMA_PRIORITY_LOW;
//    hdma_i2c3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//    hdma_i2c3_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
//    hdma_i2c3_tx.Init.MemBurst = DMA_MBURST_SINGLE;
//    hdma_i2c3_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
//    HAL_DMA_Init(&hdma_i2c3_tx);
//
//    __HAL_LINKDMA(hi2c,hdmatx,hdma_i2c3_tx);
//
//    /* Peripheral interrupt init*/
//    HAL_NVIC_SetPriority(I2C3_EV_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
//    HAL_NVIC_SetPriority(I2C3_ER_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);
//  }
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c){
//
//  if(hi2c->Instance==I2C1){
//    __I2C1_CLK_DISABLE();
//    HAL_GPIO_DeInit(VID3_SCL_PORT, VID3_SCL_PIN | VID3_SDA_PIN);
//
//    HAL_DMA_DeInit(hi2c->hdmarx);
//    HAL_DMA_DeInit(hi2c->hdmatx);
//
//    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
//    HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
//  }
//  if(hi2c->Instance==I2C3){
//    __I2C3_CLK_DISABLE();
//    HAL_GPIO_DeInit(VID3_SDA5V_PORT, VID3_SDA5V_PIN);
//    HAL_GPIO_DeInit(VID3_SCL5V_PORT, VID3_SCL5V_PIN);
//
//    HAL_DMA_DeInit(hi2c->hdmarx);
//    HAL_DMA_DeInit(hi2c->hdmatx);
//
//    HAL_NVIC_DisableIRQ(I2C3_EV_IRQn);
//    HAL_NVIC_DisableIRQ(I2C3_ER_IRQn);
//  }
//}
//
//#ifndef NOT_VIDEO_RECEIVER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void cntr_tvp_master_init_dbg(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
//	control_struct *p_ctl;
//	p_ctl = (control_struct *)ad32;
//	cntr_tvp_master_init(p_ctl);
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t  cntr_tvp_master_start_init(control_struct *p_ctl, uint8_t table_pos){
//uint32_t pos;
//
//#ifdef	TVP_INT_EN
//	p_ctl->tvp_5150.cntrl_thread = p_ctl->thread_id;
//	p_ctl->tvp_5150.cntrl_int_event = main_cntrl_thread_event_tvp;
//
//	p_ctl->tvp_5150.int_port = VIDEO_CNTRL_TVP_IRQ_PORT;
//	p_ctl->tvp_5150.int_pin = VIDEO_CNTRL_TVP_IRQ_PIN;
//#endif
//
//	p_ctl->tvp_5150_master.cntrl_thread = p_ctl->main_thread_id;
//	p_ctl->tvp_5150_master.cntrl_int_event = cntrl_thread_event_tvp_master;
//
//	p_ctl->tvp_5150_master.irq_type = EXTI4_IRQn;
//
//	p_ctl->tvp_5150_master.int_port = VID3_INTREQ_0_PORT;
//	p_ctl->tvp_5150_master.int_pin = VID3_INTREQ_0_PIN;
//
//	p_ctl->tvp_5150_master.reset_port = VID3_RESET_B_PORT;
//	p_ctl->tvp_5150_master.reset_pin = VID3_RESET_B_PIN;
//
//	p_ctl->tvp_5150_master.pdn_port = VID3_PDN_0_PORT;
//	p_ctl->tvp_5150_master.pdn_pin = VID3_PDN_0_PIN;
//
//	p_ctl->tvp_5150_master.i2c_addr = TVP_I2C_ADDRESS_LO;
//	p_ctl->tvp_5150_master.p_hi2c = &hi2c1;
//
//	tvp_start_reset(&p_ctl->tvp_5150_master);
//
////	pos = tvp_add_to_cell_table(&p_ctl->comp_table[0],table_pos,&p_ctl->tvp_5150_master,TVP5150_NAME_REG);
//	pos = tvp_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->tvp_5150_master,TVP5150_NAME_REG);
//
//	cmd_h_add_to_cell_table_ext_size(
//							p_ctl->p_comp_table,
//							pos++,
//							p_ctl,
//							NULL,
//							(void*)&cntr_tvp_master_init_dbg,
//							WR_Att + Default_Attr + Action_Att,
//							FixIntegerCellType + 1,
//							"Master init");
//
//
//	return pos;
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t  cntr_tvp_master_init(control_struct *p_ctl){
//uint8_t back;
//	TVP_Write(&p_ctl->tvp_5150_master,0x02,0x30);
//	TVP_Write(&p_ctl->tvp_5150_master,0x03,0x0D | 0x20 );
//
//	TVP_Init(&p_ctl->tvp_5150_master);
//
////	TVP_Write(p_ctl, 0x1D, 0x10); //interrupt field change en!!!!!
//
//	back = TVP_Read(&p_ctl->tvp_5150_master, 0x03);
//	if(back != 0x2D){
//		p_ctl->error |= 1;
//	}
//	return 0;
//}
//#endif
//#ifdef RTC6705
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t  cntr_tvp_slave_disable_start_init(control_struct *p_ctl, uint8_t table_pos){
//uint32_t pos;
//
//	p_ctl->tvp_5150_slave.reset_port = VID3_RESET_B1_PORT;
//	p_ctl->tvp_5150_slave.reset_pin = VID3_RESET_B1_PIN;
//
//	p_ctl->tvp_5150_slave.pdn_port = VID3_PDN_1_PORT;
//	p_ctl->tvp_5150_slave.pdn_pin = VID3_PDN_1_PIN;
//
//
//	GPIO_InitTypeDef GPIO_InitStructure;
//	uint32_t get;
//	p_ctl->error = 0;
//
//	GPIO_InitStructure.Pin = p_ctl->tvp_5150_slave.reset_pin;
//	GPIO_InitStructure.Pull = GPIO_PULLUP;
//	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	HAL_GPIO_Init(p_ctl->tvp_5150_slave.reset_port, &GPIO_InitStructure);
//
//	GPIO_InitStructure.Pin = p_ctl->tvp_5150_slave.pdn_pin;
//	GPIO_InitStructure.Pull = GPIO_PULLUP;
//	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	HAL_GPIO_Init(p_ctl->tvp_5150_slave.pdn_port, &GPIO_InitStructure);
//
//	HAL_GPIO_WritePin(p_ctl->tvp_5150_slave.reset_port, p_ctl->tvp_5150_slave.reset_pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(p_ctl->tvp_5150_slave.pdn_port, p_ctl->tvp_5150_slave.pdn_pin, GPIO_PIN_SET);
//
//	osDelay(10);
//
//	HAL_GPIO_WritePin(p_ctl->tvp_5150_slave.reset_port, p_ctl->tvp_5150_slave.reset_pin, GPIO_PIN_RESET);
//	osDelay(100);
//	HAL_GPIO_WritePin(p_ctl->tvp_5150_slave.reset_port, p_ctl->tvp_5150_slave.reset_pin, GPIO_PIN_SET);
//	osDelay(100);
//
////	tvp_start_reset(&p_ctl->tvp_5150_slave);
//	return pos;
//}
//#endif
//
////#ifndef RTC6705
//#ifndef NO_SEC_CH
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t  cntr_tvp_slave_start_init(control_struct *p_ctl, uint8_t table_pos){
//uint32_t pos;
//#ifdef	TVP_INT_EN
//	p_ctl->tvp_5150.cntrl_thread = p_ctl->thread_id;
//	p_ctl->tvp_5150.cntrl_int_event = main_cntrl_thread_event_tvp;
//
//	p_ctl->tvp_5150.int_port = VIDEO_CNTRL_TVP_IRQ_PORT;
//	p_ctl->tvp_5150.int_pin = VIDEO_CNTRL_TVP_IRQ_PIN;
//#endif
//
//	p_ctl->tvp_5150_slave.cntrl_thread = p_ctl->main_thread_id;
//	p_ctl->tvp_5150_slave.cntrl_int_event = cntrl_thread_event_tvp_slave;
//
//	p_ctl->tvp_5150_slave.irq_type = EXTI15_10_IRQn;
//
//	p_ctl->tvp_5150_slave.int_port = VID3_INTREQ_1_PORT;
//	p_ctl->tvp_5150_slave.int_pin = VID3_INTREQ_1_PIN;
//
//	p_ctl->tvp_5150_slave.reset_port = VID3_RESET_B1_PORT;
//	p_ctl->tvp_5150_slave.reset_pin = VID3_RESET_B1_PIN;
//
//	p_ctl->tvp_5150_slave.pdn_port = VID3_PDN_1_PORT;
//	p_ctl->tvp_5150_slave.pdn_pin = VID3_PDN_1_PIN;
//
//	p_ctl->tvp_5150_slave.i2c_addr = TVP_I2C_ADDRESS_HI;
//	p_ctl->tvp_5150_slave.p_hi2c = &hi2c1;
//
//	tvp_start_reset(&p_ctl->tvp_5150_slave);
//
////	pos = tvp_add_to_cell_table(&p_ctl->comp_table[0],table_pos,&p_ctl->tvp_5150_slave,TVP5150_NAME_REG_SLAVE);
//	pos = tvp_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->tvp_5150_slave,TVP5150_NAME_REG_SLAVE);
//	return pos;
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t  cntr_tvp_slave_init(control_struct *p_ctl){
//uint8_t back;
//	TVP_Write(&p_ctl->tvp_5150_slave,0x02,0x30);
//	TVP_Write(&p_ctl->tvp_5150_slave,0x03,0x20);//onli int req
//
//	TVP_Init(&p_ctl->tvp_5150_slave);
//
//	back = TVP_Read(&p_ctl->tvp_5150_slave, 0x03);
//	if(back != 0x20){
//		p_ctl->error |= 1;
//	}
//
//	return 0;
//}
//#endif
//
//
//#ifdef SX1280
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t  cntr_sx1280_init(control_struct *p_ctl, uint8_t table_pos){
//
////	GPIO_InitTypeDef GPIO_InitStructure;
////
////		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
////		GPIO_InitStructure.Pull = GPIO_PULLUP;
////		GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
////
////		GPIO_InitStructure.Pin = VID3_PWR_EN_5_8_PIN;
////		HAL_GPIO_Init(VID3_PWR_EN_5_8_PORT, &GPIO_InitStructure);
//////убрать?
////	HAL_GPIO_WritePin(VID3_PWR_EN_5_8_PORT, VID3_PWR_EN_5_8_PIN, GPIO_PIN_RESET);
////	osDelay(50);
//
//	memcpy(&p_ctl->sx1280.short_time, &p_ctl->short_time, sizeof(p_ctl->short_time));
//
//	p_ctl->sx1280.CS_pin = GPIO_PIN_1;
//	p_ctl->sx1280.CS_port = GPIOB;
//
//	p_ctl->sx1280.MOSI_port = GPIOA;
//	p_ctl->sx1280.MOSI_pin = GPIO_PIN_3;
//
//	p_ctl->sx1280.CLK_port = GPIOC;
//	p_ctl->sx1280.CLK_pin = GPIO_PIN_10;
//
//	p_ctl->sx1280.MISO_port = GPIOC;
//	p_ctl->sx1280.MISO_pin = GPIO_PIN_11;
//
//	p_ctl->sx1280.RST_port = GPIOE;
//	p_ctl->sx1280.RST_pin = GPIO_PIN_10;
//
//
//	sx1280_init(&p_ctl->sx1280);
//	return sx1280_add_to_cell_table(p_ctl->p_comp_table,table_pos, &p_ctl->sx1280, NULL);
//}
//#endif
//
//#ifdef AD9910
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifndef NOT_VIDEO_RECEIVER
//uint32_t  cntr_ad9910_init(control_struct *p_ctl, uint8_t table_pos){
//
////	return 0;
//
//		GPIO_InitTypeDef GPIO_InitStructure;
//
//			GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//			GPIO_InitStructure.Pull = GPIO_PULLUP;
//			GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
//
//			GPIO_InitStructure.Pin = GPIO_PIN_1;
//			HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
//	//убрать?
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
//		osDelay(50);
//
//
//	p_ctl->ad9910.CS_port = GPIOD;
//	p_ctl->ad9910.CS_pin = GPIO_PIN_1;
//
//	p_ctl->ad9910.DATA_port = GPIOA;
//	p_ctl->ad9910.DATA_pin = GPIO_PIN_3;
//
//	p_ctl->ad9910.CLK_port = GPIOC;
//	p_ctl->ad9910.CLK_pin = GPIO_PIN_10;
//
//	p_ctl->ad9910.MASTER_reset_port = GPIOD;
//	p_ctl->ad9910.MASTER_reset_pin = GPIO_PIN_15;
//
//	p_ctl->ad9910.IO_UPDATE_port = NULL;
//	p_ctl->ad9910.IO_UPDATE_pin = VID3_TR_DATA_PIN;
//
//	p_ctl->ad9910.EXT_PWR_DWN_port =  GPIOD;
//	p_ctl->ad9910.EXT_PWR_DWN_pin =  GPIO_PIN_13;
//
//	p_ctl->ad9910.OSK_port = NULL;
//	p_ctl->ad9910.OSK_pin = VID3_TR_PWR_EN_PIN;
//
//	p_ctl->ad9910.P0_port = NULL;
//	p_ctl->ad9910.P0_pin  = GPIO_PIN_10;
//
//	p_ctl->ad9910.P1_port = NULL;
//	p_ctl->ad9910.P1_pin = GPIO_PIN_11;
//
//	p_ctl->ad9910.P2_port= NULL;
//	p_ctl->ad9910.P2_pin = GPIO_PIN_12;
//
//	p_ctl->ad9910.DRCTL_port= NULL;
//	p_ctl->ad9910.DRCTL_pin = VID3_PWR_EN_1_2_PIN;
//
//	p_ctl->ad9910.TXen_port= GPIOD;
//	p_ctl->ad9910.TXen_pin = GPIO_PIN_12;
//
//	p_ctl->ad9910.Ref_Clk = 25000000;
//	p_ctl->ad9910.DDS_Core_Clock = 1000000000;//480000000;
//								  //480000000
//
//	p_ctl->ad9910.sweep_start_freq = 100000000;
//	p_ctl->ad9910.sweep_stop_freq =  200000000;
//	p_ctl->ad9910.sweep_time = 50;
//	p_ctl->ad9910.sweep_time_format = 2;//usec
//
//	p_ctl->ad9910.fm_carrier = 100000000;
//	p_ctl->ad9910.fm_fmod = 1000;
//	p_ctl->ad9910.fm_fdev = 500000;
//
//	ad9910_init(&p_ctl->ad9910, 1, 0, 40000000);
//
//	osDelay(100);
//
//	SingleProfileFreqOut(&p_ctl->ad9910, 52000000, 0);
//
////	osDelay(1000);
//
//	ad9910_power_down(&p_ctl->ad9910);
//
//	return ad9910_add_to_cell_table(p_ctl->p_comp_table,table_pos, &p_ctl->ad9910, NULL);
//}
//#endif
//
//#ifdef NOT_VIDEO_RECEIVER
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//uint32_t  cntr_rffc5071_init(control_struct *p_ctl, uint8_t table_pos){
////GPIO_InitTypeDef GPIO_InitStructure;
//
//	p_ctl->rffc5071.ref_freq = REF_FREQ_25;
//
//	p_ctl->rffc5071.reset_pin = GPIO_PIN_10;
//	p_ctl->rffc5071.reset_port = GPIOE;
//
//	p_ctl->rffc5071.cs_pin = GPIO_PIN_15;
//	p_ctl->rffc5071.cs_port = GPIOB;
//
//	p_ctl->rffc5071.mosi_port = GPIOE;
//	p_ctl->rffc5071.mosi_pin = GPIO_PIN_11;
//
//	p_ctl->rffc5071.clk_port = GPIOB;
//	p_ctl->rffc5071.clk_pin = GPIO_PIN_14;
//
//	switch_fdc_start_init(&p_ctl->rffc5071.sw_en);
//	p_ctl->rffc5071.sw_en.b_inveretd_ON = 0;
//	p_ctl->rffc5071.sw_en.ON_pin = GPIO_PIN_15;
//	p_ctl->rffc5071.sw_en.ON_port = GPIOA;
//	switch_fdc_on(&p_ctl->rffc5071.sw_en,fdc_off);
//	switch_fdc_set_data(&p_ctl->rffc5071.sw_en);
//	switch_fdc_pin_init(&p_ctl->rffc5071.sw_en);
//
//	p_ctl->rffc5071.sw0.ON_port = NULL;
//	p_ctl->rffc5071.sw1.ON_port = NULL;
//	p_ctl->rffc5071.sw2.ON_port = NULL;
//
////	switch_fdc_start_init(&p_ctl->rffc5071.sw0);
////	p_ctl->rffc5071.sw0.b_inveretd_ON = 0;
////	p_ctl->rffc5071.sw0.ON_pin = GPIO_PIN_13;
////	p_ctl->rffc5071.sw0.ON_port = GPIOE;
////	switch_fdc_on(&p_ctl->rffc5071.sw0,fdc_off);
////	switch_fdc_set_data(&p_ctl->rffc5071.sw0);
////	switch_fdc_pin_init(&p_ctl->rffc5071.sw0);
////
////	switch_fdc_start_init(&p_ctl->rffc5071.sw1);
////	p_ctl->rffc5071.sw1.b_inveretd_ON = 0;
////	p_ctl->rffc5071.sw1.ON_pin = GPIO_PIN_14;
////	p_ctl->rffc5071.sw1.ON_port = GPIOE;
////	switch_fdc_on(&p_ctl->rffc5071.sw1,fdc_off);
////	switch_fdc_set_data(&p_ctl->rffc5071.sw1);
////	switch_fdc_pin_init(&p_ctl->rffc5071.sw1);
////
////	switch_fdc_start_init(&p_ctl->rffc5071.sw2);
////	p_ctl->rffc5071.sw2.b_inveretd_ON = 0;
////	p_ctl->rffc5071.sw2.ON_pin = GPIO_PIN_8;
////	p_ctl->rffc5071.sw2.ON_port = GPIOC;
////	switch_fdc_on(&p_ctl->rffc5071.sw2,fdc_off);
////	switch_fdc_set_data(&p_ctl->rffc5071.sw2);
////	switch_fdc_pin_init(&p_ctl->rffc5071.sw2);
//
//
//
//
////
////
////	switch_fdc_start_init(&p_ctl->rtc6715.sw_da1_en);
////	p_ctl->rtc6715.sw_da1_en.b_inveretd_ON = 1;
////	p_ctl->rtc6715.sw_da1_en.ON_pin = VID3_SW_5_8_EN_PIN;
////	p_ctl->rtc6715.sw_da1_en.ON_port = VID3_SW_5_8_EN_PORT;
////	switch_fdc_on(&p_ctl->rtc6715.sw_da1_en,fdc_on);
////	switch_fdc_set_data(&p_ctl->rtc6715.sw_da1_en);
////	switch_fdc_pin_init(&p_ctl->rtc6715.sw_da1_en);
////
////	switch_fdc_start_init(&p_ctl->rtc6715.sw_da1_in);
////	p_ctl->rtc6715.sw_da1_in.b_inveretd_ON = 0;
////	p_ctl->rtc6715.sw_da1_in.ON_pin = VID3_SW_5_8_IN_PIN;
////	p_ctl->rtc6715.sw_da1_in.ON_port = VID3_SW_5_8_IN_PORT;
////	switch_fdc_on(&p_ctl->rtc6715.sw_da1_in,fdc_off);
////	switch_fdc_set_data(&p_ctl->rtc6715.sw_da1_in);
////	switch_fdc_pin_init(&p_ctl->rtc6715.sw_da1_in);
////
////	GPIO_InitStructure.Pin = p_ctl->rtc6715.sw_pwr_5v.ON_pin;
////	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
////	GPIO_InitStructure.Pull = GPIO_NOPULL;
////	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
////	HAL_GPIO_Init(p_ctl->rtc6715.sw_pwr_5v.ON_port, &GPIO_InitStructure);
//
//	memcpy(&p_ctl->rffc5071.short_time,&p_ctl->short_time,sizeof(p_ctl->short_time));
//
////	rtc6715_start_pin_init(&p_ctl->rtc6715);
//
////	p_ctl->rtc6715.p_cfg = &p_ctl->dev_config.rec.recv_5_8_cfg;
//
//	rffc5071_start_init(&p_ctl->rffc5071);
//
//
//
////	return rtc6715_add_to_cell_table(&p_ctl->comp_table[0],table_pos,&p_ctl->rtc6715,NULL);
//	return rffc5071_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->rffc5071,NULL);
//}
//
//
//uint32_t  cntr_ad9910_init(control_struct *p_ctl, uint8_t table_pos){
//
////	return 0;
//
////		GPIO_InitTypeDef GPIO_InitStructure;
////
////			GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
////			GPIO_InitStructure.Pull = GPIO_PULLUP;
////			GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
////
////			GPIO_InitStructure.Pin = GPIO_PIN_1;
////			HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
////	//убрать?
////		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
//		osDelay(50);
//
//
//	p_ctl->ad9910.CS_port = GPIOD;
//	p_ctl->ad9910.CS_pin = GPIO_PIN_1;
//
//	p_ctl->ad9910.DATA_port = GPIOC;
//	p_ctl->ad9910.DATA_pin = GPIO_PIN_12;
//
//	p_ctl->ad9910.CLK_port = GPIOC;
//	p_ctl->ad9910.CLK_pin = GPIO_PIN_10;
//
//	p_ctl->ad9910.MASTER_reset_port = GPIOC;
//	p_ctl->ad9910.MASTER_reset_pin = GPIO_PIN_15;
//
//	p_ctl->ad9910.IO_UPDATE_port = NULL;
//	p_ctl->ad9910.IO_UPDATE_pin = VID3_TR_DATA_PIN;
//
//	p_ctl->ad9910.EXT_PWR_DWN_port =  GPIOC;
//	p_ctl->ad9910.EXT_PWR_DWN_pin =  GPIO_PIN_14;
//
//	p_ctl->ad9910.OSK_port = NULL;
//	p_ctl->ad9910.OSK_pin = VID3_TR_PWR_EN_PIN;
//
//	p_ctl->ad9910.P0_port = NULL;
//	p_ctl->ad9910.P0_pin  = GPIO_PIN_10;
//
//	p_ctl->ad9910.P1_port = NULL;
//	p_ctl->ad9910.P1_pin = GPIO_PIN_11;
//
//	p_ctl->ad9910.P2_port= NULL;
//	p_ctl->ad9910.P2_pin = GPIO_PIN_12;
//
//	p_ctl->ad9910.DRCTL_port= NULL;
//	p_ctl->ad9910.DRCTL_pin = VID3_PWR_EN_1_2_PIN;
//
//	p_ctl->ad9910.TXen_port= GPIOD;
//	p_ctl->ad9910.TXen_pin = GPIO_PIN_12;
//
//	p_ctl->ad9910.Ref_Clk = 25000000;
//	p_ctl->ad9910.DDS_Core_Clock = 1000000000;//480000000;
//								  //480000000
//
//	p_ctl->ad9910.sweep_start_freq = 100000000;
//	p_ctl->ad9910.sweep_stop_freq =  200000000;
//	p_ctl->ad9910.sweep_time = 50;
//	p_ctl->ad9910.sweep_time_format = 2;//usec
//
//	p_ctl->ad9910.fm_carrier = 230000000;
//	p_ctl->ad9910.fm_fmod = 1000;
//	p_ctl->ad9910.fm_fdev = 5000000;
//
//
//	memcpy(&p_ctl->ad9910_1,&p_ctl->ad9910,sizeof(p_ctl->ad9910));
//	p_ctl->ad9910_1.CS_port = GPIOE;
//	p_ctl->ad9910_1.CS_pin = GPIO_PIN_7;
//
//
//
//	ad9910_init(&p_ctl->ad9910, 0);
//
//	ad9910_init(&p_ctl->ad9910_1, 1);
//
//	osDelay(100);
//
//	SingleProfileFreqOut(&p_ctl->ad9910,   230000000, 0);
//	SingleProfileFreqOut(&p_ctl->ad9910_1, 260000000, 0);
//
////	osDelay(1000);
//
////	ad9910_power_down(&p_ctl->ad9910);
//	table_pos = ad9910_add_to_cell_table(p_ctl->p_comp_table,table_pos, &p_ctl->ad9910, AD9910_NAME_REG);
//
//	return ad9910_add_to_cell_table(p_ctl->p_comp_table,table_pos, &p_ctl->ad9910_1, AD9910_NAME_REG_1);
//}
//
//
//#endif
//#endif
//
//#ifdef RECV_1_2
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t  cntr_recv_1_2_init(control_struct *p_ctl, uint8_t table_pos){
//uint8_t addr;
//
//	switch_fdc_start_init(&p_ctl->recv_1_2.sw_da6_en);
//	p_ctl->recv_1_2.sw_da6_en.b_inveretd_ON = 1;
//	p_ctl->recv_1_2.sw_da6_en.ON_pin = VID3_SW_1_2_EN_PIN;
//	p_ctl->recv_1_2.sw_da6_en.ON_port = VID3_SW_1_2_EN_PORT;
//	switch_fdc_on(&p_ctl->recv_1_2.sw_da6_en,fdc_on);
//	switch_fdc_set_data(&p_ctl->recv_1_2.sw_da6_en);
//	switch_fdc_pin_init(&p_ctl->recv_1_2.sw_da6_en);
//
//	switch_fdc_start_init(&p_ctl->recv_1_2.sw_da6_in);
//	p_ctl->recv_1_2.sw_da6_in.b_inveretd_ON = 0;
//	p_ctl->recv_1_2.sw_da6_in.ON_pin = VID3_SW_1_2_IN_PIN;
//	p_ctl->recv_1_2.sw_da6_in.ON_port = VID3_SW_1_2_IN_PORT;
//	switch_fdc_on(&p_ctl->recv_1_2.sw_da6_in,fdc_off);
//	switch_fdc_set_data(&p_ctl->recv_1_2.sw_da6_in);
//	switch_fdc_pin_init(&p_ctl->recv_1_2.sw_da6_in);
//
//
////	GPIO_InitTypeDef GPIO_InitStructure;
////
////	GPIO_InitStructure.Pin = GPIO_PIN_10;
////	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
////	GPIO_InitStructure.Pull = GPIO_PULLUP;
////	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
////	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
////
////
////	GPIO_InitStructure.Pin = GPIO_PIN_11;
////	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
////	GPIO_InitStructure.Pull = GPIO_PULLUP;
////	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
////	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
////
////
////	osDelay(10);
//
//	recv_1_2_ver_e ver;
//
//	ver = recv_1_2_ver_1_2;
//	addr = 0x60;
//#ifdef	RECV_1_2_EXT
////	addr = 0x63;
//#endif
//
////	if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10) == GPIO_PIN_RESET){
//#ifdef	RECV_1_2_SUB_1_5
//		ver = recv_1_2_ver_1_5_;
//#endif
//#ifdef	RECV_1_2_SUB_2_4
//		ver = recv_1_2_ver_2_4_;
//		addr = 0x63;
//#endif
////	}
//
//		addr = 0x63;
//
//
////	recv_1_2_start_init(&p_ctl->recv_1_2, ver, 0x61, &hi2c3, VID3_PWR_EN_1_2_PORT, VID3_PWR_EN_1_2_PIN, VID3_PWR_EN_8_PORT, VID3_PWR_EN_8_PIN, VID3_RSII_1_2_PORT, VID3_RSII_1_2_PIN, &p_ctl->dev_config.rec.recv_1_2_cfg);
////	recv_1_2_start_init(&p_ctl->recv_1_2, ver, 0x61, &hi2c3, VID3_PWR_EN_1_2_PORT, VID3_PWR_EN_1_2_PIN, VID3_PWR_EN_8_PORT, VID3_PWR_EN_8_PIN, VID3_RSII_1_2_PORT, VID3_RSII_1_2_PIN, &p_ctl->dev_config.rec.recv_1_2_cfg);
//	recv_1_2_start_init(&p_ctl->recv_1_2, ver, addr, &hi2c3, VID3_PWR_EN_1_2_PORT, VID3_PWR_EN_1_2_PIN, VID3_PWR_EN_8_PORT, VID3_PWR_EN_8_PIN, VID3_RSII_1_2_PORT, VID3_RSII_1_2_PIN, &p_ctl->dev_config.rec.recv_1_2_cfg);
//	osDelay(10);
//	MX_I2C3_Init();
////	return recv_1_2_add_to_cell_table(&p_ctl->comp_table[0],table_pos,&p_ctl->recv_1_2,NULL);
//	return recv_1_2_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->recv_1_2,NULL);
//}
//#endif
////#ifdef RECV_1_2
//#ifdef RECV_2_4
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t  cntr_recv_2_4_init(control_struct *p_ctl, uint8_t table_pos){
//
//
//	switch_fdc_start_init(&p_ctl->recv_2_4.sw_da6_en);
//	p_ctl->recv_2_4.sw_da6_en.b_inveretd_ON = 1;
//	p_ctl->recv_2_4.sw_da6_en.ON_pin = VID3_SW_1_2_EN_PIN;// VID3_SW_5_8_EN_PIN;
//	p_ctl->recv_2_4.sw_da6_en.ON_port = VID3_SW_1_2_EN_PORT;//VID3_SW_5_8_EN_PORT;
//	switch_fdc_on(&p_ctl->recv_2_4.sw_da6_en,fdc_on);
//	switch_fdc_set_data(&p_ctl->recv_2_4.sw_da6_en);
//	switch_fdc_pin_init(&p_ctl->recv_2_4.sw_da6_en);
//
//	switch_fdc_start_init(&p_ctl->recv_2_4.sw_da6_in);
//	p_ctl->recv_2_4.sw_da6_in.b_inveretd_ON = 0;
//	p_ctl->recv_2_4.sw_da6_in.ON_pin = VID3_SW_1_2_IN_PIN;//VID3_SW_5_8_IN_PIN;
//	p_ctl->recv_2_4.sw_da6_in.ON_port = VID3_SW_1_2_IN_PORT;//VID3_SW_5_8_IN_PORT;
//	switch_fdc_on(&p_ctl->recv_2_4.sw_da6_in,fdc_off);
//	switch_fdc_set_data(&p_ctl->recv_2_4.sw_da6_in);
//	switch_fdc_pin_init(&p_ctl->recv_2_4.sw_da6_in);
//
//
//	GPIO_InitTypeDef GPIO_InitStructure;
//
////	GPIO_InitStructure.Pin = GPIO_PIN_10;
////	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
////	GPIO_InitStructure.Pull = GPIO_PULLUP;
////	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
////	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
//
//
//	GPIO_InitStructure.Pin = GPIO_PIN_11;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStructure.Pull = GPIO_PULLUP;
//	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
//	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
//
//	osDelay(10);
//
//	recv_1_2_ver_e ver;
//
//	ver = recv_1_2_ver_2_4_;
//
////	if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10) == GPIO_PIN_RESET){
////		ver = recv_1_2_ver_1_5;
////	}
//
////	recv_1_2_start_init(&p_ctl->recv_1_2, ver, 0x61, &hi2c3, VID3_PWR_EN_1_2_PORT, VID3_PWR_EN_1_2_PIN, VID3_PWR_EN_8_PORT, VID3_PWR_EN_8_PIN, VID3_RSII_1_2_PORT, VID3_RSII_1_2_PIN, &p_ctl->dev_config.rec.recv_1_2_cfg);
//	recv_1_2_start_init(&p_ctl->recv_2_4, ver, 0x63, &hi2c3, VID3_PWR_EN_1_2_PORT, VID3_PWR_EN_1_2_PIN, VID3_PWR_EN_8_PORT, VID3_PWR_EN_8_PIN, VID3_RSII_1_2_PORT, VID3_RSII_1_2_PIN, &p_ctl->dev_config.rec.recv_2_4_cfg);
////	recv_1_2_start_init(&p_ctl->recv_1_2, ver, 0x60, &hi2c3, VID3_PWR_EN_1_2_PORT, VID3_PWR_EN_1_2_PIN, VID3_PWR_EN_8_PORT, VID3_PWR_EN_8_PIN, VID3_RSII_1_2_PORT, VID3_RSII_1_2_PIN, &p_ctl->dev_config.rec.recv_1_2_cfg);
////	osDelay(10);
////	MX_I2C3_Init();
////	return recv_1_2_add_to_cell_table(&p_ctl->comp_table[0],table_pos,&p_ctl->recv_1_2,NULL);
////	return recv_1_2_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->recv_1_2,NULL);
//	return table_pos;
//}
//#endif
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef RTC6711
//uint32_t  cntr_recv_2_4_init(control_struct *p_ctl, uint8_t table_pos){
//GPIO_InitTypeDef GPIO_InitStructure;
//	p_ctl->rtc6711.cs_pin = VID3_CS_RX5808_PIN;
//	p_ctl->rtc6711.cs_port = VID3_CS_RX5808_PORT;
//
//
//	p_ctl->rtc6711.mosi_port = VID3_MOSI_PORT;
//	p_ctl->rtc6711.mosi_pin = VID3_MOSI_PIN;
//
//	p_ctl->rtc6711.clk_port = VID3_SCK_PORT;
//	p_ctl->rtc6711.clk_pin = VID3_SCK_PIN;
//
//	p_ctl->rtc6711.rssi_port  = VID3_RSII_5_8_PORT;
//	p_ctl->rtc6711.rssi_pin = VID3_RSII_5_8_PIN;
//
//	switch_fdc_start_init(&p_ctl->rtc6711.sw_pwr_5v);
//	p_ctl->rtc6711.sw_pwr_5v.b_inveretd_ON = 1;
//	p_ctl->rtc6711.sw_pwr_5v.ON_pin = VID3_PWR_EN_5_8_PIN;
//	p_ctl->rtc6711.sw_pwr_5v.ON_port = VID3_PWR_EN_5_8_PORT;
//	switch_fdc_on(&p_ctl->rtc6711.sw_pwr_5v,fdc_on);
//	switch_fdc_set_data(&p_ctl->rtc6711.sw_pwr_5v);
//
//
//	switch_fdc_start_init(&p_ctl->rtc6711.sw_da1_en);
//	p_ctl->rtc6711.sw_da1_en.b_inveretd_ON = 1;
//	p_ctl->rtc6711.sw_da1_en.ON_pin = VID3_SW_5_8_EN_PIN;
//	p_ctl->rtc6711.sw_da1_en.ON_port = VID3_SW_5_8_EN_PORT;
//	switch_fdc_on(&p_ctl->rtc6711.sw_da1_en,fdc_on);
//	switch_fdc_set_data(&p_ctl->rtc6711.sw_da1_en);
//	switch_fdc_pin_init(&p_ctl->rtc6711.sw_da1_en);
//
//	switch_fdc_start_init(&p_ctl->rtc6711.sw_da1_in);
//	p_ctl->rtc6711.sw_da1_in.b_inveretd_ON = 0;
//	p_ctl->rtc6711.sw_da1_in.ON_pin = VID3_SW_5_8_IN_PIN;
//	p_ctl->rtc6711.sw_da1_in.ON_port = VID3_SW_5_8_IN_PORT;
//	switch_fdc_on(&p_ctl->rtc6711.sw_da1_in,fdc_off);
//	switch_fdc_set_data(&p_ctl->rtc6711.sw_da1_in);
//	switch_fdc_pin_init(&p_ctl->rtc6711.sw_da1_in);
//
//	GPIO_InitStructure.Pin = p_ctl->rtc6711.sw_pwr_5v.ON_pin;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
//	HAL_GPIO_Init(p_ctl->rtc6711.sw_pwr_5v.ON_port, &GPIO_InitStructure);
//
//	memcpy(&p_ctl->rtc6711.short_time,&p_ctl->short_time,sizeof(p_ctl->short_time));
//
//	rtc6711_start_pin_init(&p_ctl->rtc6711);
//
////	p_ctl->rtc6715.p_cfg = &p_ctl->dev_config.rec.recv_5_8_cfg;
//
//	rtc6711_start_init(&p_ctl->rtc6711);
//
////#ifdef RECV_5_8_RSSI
////	recv_5_8_rssi_init(&p_ctl->rtc6715,p_ctl->main_thread_id,cntrl_thread_event_recv_5_8_rssi);
////	p_ctl->rtc6715.rssi.p_adc = &AdcHandle;
////	p_ctl->rtc6715.rssi.adc_buf_num_point = 16;
////	p_ctl->rtc6715.rssi.p_adc_buf = &p_ctl->adc_ring_buf[0];
//////	p_ctl->rtc6715.rssi.p_uni = &p_ctl->u_slave_udp;
////#endif
//
////	return rtc6715_add_to_cell_table(&p_ctl->comp_table[0],table_pos,&p_ctl->rtc6715,NULL);
//	return rtc6711_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->rtc6711,NULL);
//}
//#endif
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef RECV_3_4_I2C
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void i2_recv_3_4_default_init(i2c_recv_cfg_struct *p_cfg){
//
//	memset(p_cfg,0,sizeof(i2c_recv_cfg_struct));
//	p_cfg->min_freq = 3000;
//	p_cfg->max_freq = 4000;
//	p_cfg->auto_step_freq =  10;
//	p_cfg->manual_step_freq =  4;
//
//	p_cfg->time = 2;
//	p_cfg->en_lit = 0xFFFF;
//	p_cfg->b_en_fast_scan = 0;
//	p_cfg->view_mode = i2c_recv_view_mode_normal;
//
//	p_cfg->lvl = 0x30;
//
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//uint32_t  cntr_i2_recv_init_3_4(control_struct *p_ctl, uint8_t table_pos){
//GPIO_InitTypeDef GPIO_InitStructure;
//
//	memset(&p_ctl->i2c_recv_3_4,0,sizeof(p_ctl->i2c_recv_3_4));
//
//	p_ctl->i2c_recv_3_4.type = i2c_recv_type_3_4;
//
//	p_ctl->i2c_recv_3_4.p_hi2c = &hi2c3;
//	p_ctl->i2c_recv_3_4.addr = 4;
//
//	p_ctl->i2c_recv_3_4.p_cfg = &p_ctl->dev_config.rec.i2c_recv_3_4_cfg;
//	p_ctl->i2c_recv_3_4.p_cfg->b_en_fast_scan = 0;
//
//	p_ctl->i2c_recv_3_4.max_num_ch = RECV_3_4_MAX_NUM_CH;
////	p_ctl->i2c_recv_3_4.ch_table_ptr = (uint8_t*)ch_3_4_table[0];
////	p_ctl->i2c_recv_3_4.lit_table_ptr = (uint8_t*)ch_3_4_lit_table[0];
//	p_ctl->i2c_recv_3_4.table_freq_ptr = (uint16_t*)ch_3_4_freq;
//
//	p_ctl->i2c_recv_3_4.reset_pin =VID3_SW_I2RECV_3_4_RST_PIN;
//	p_ctl->i2c_recv_3_4.reset_port =VID3_SW_I2RECV_3_4_RST_PORT;
//
//	switch_fdc_start_init(&p_ctl->i2c_recv_3_4.sw_pwr_5v);
//	p_ctl->i2c_recv_3_4.sw_pwr_5v.b_inveretd_ON = 1;
//	p_ctl->i2c_recv_3_4.sw_pwr_5v.ON_pin = VID3_PWR_EN_I2RECV_3_4_PIN;
//	p_ctl->i2c_recv_3_4.sw_pwr_5v.ON_port = VID3_PWR_EN_I2RECV_3_4_PORT;
//	switch_fdc_on(&p_ctl->i2c_recv_3_4.sw_pwr_5v,fdc_on);
//	switch_fdc_set_data(&p_ctl->i2c_recv_3_4.sw_pwr_5v);
//
//
//	switch_fdc_start_init(&p_ctl->i2c_recv_3_4.sw_en);
//	p_ctl->i2c_recv_3_4.sw_en.b_inveretd_ON = 1;
//	p_ctl->i2c_recv_3_4.sw_en.ON_pin = VID3_SW_I2RECV_3_4_EN_PIN;
//	p_ctl->i2c_recv_3_4.sw_en.ON_port = VID3_SW_I2RECV_3_4_EN_PORT;
//	switch_fdc_on(&p_ctl->i2c_recv_3_4.sw_en,fdc_on);
//	switch_fdc_set_data(&p_ctl->i2c_recv_3_4.sw_en);
//	switch_fdc_pin_init(&p_ctl->i2c_recv_3_4.sw_en);
//
//	switch_fdc_start_init(&p_ctl->i2c_recv_3_4.sw_in);
//	p_ctl->i2c_recv_3_4.sw_in.b_inveretd_ON = 0;
//	p_ctl->i2c_recv_3_4.sw_in.ON_pin = VID3_SW_I2RECV_3_4_IN_PIN;
//	p_ctl->i2c_recv_3_4.sw_in.ON_port = VID3_SW_I2RECV_3_4_IN_PORT;
//	switch_fdc_on(&p_ctl->i2c_recv_3_4.sw_in,fdc_off);
//	switch_fdc_set_data(&p_ctl->i2c_recv_3_4.sw_in);
//	switch_fdc_pin_init(&p_ctl->i2c_recv_3_4.sw_in);
//
//	GPIO_InitStructure.Pin = p_ctl->i2c_recv_3_4.sw_pwr_5v.ON_pin;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
//	HAL_GPIO_Init(p_ctl->i2c_recv_3_4.sw_pwr_5v.ON_port, &GPIO_InitStructure);
//
//	MX_I2C3_Init();
//
//	i2c_recv_start_init(&p_ctl->i2c_recv_3_4);
//
//	return i2c_recv_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->i2c_recv_3_4,NULL);
//}
//#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef RECV_4_5_I2C
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void i2_recv_4_5_default_init(i2c_recv_cfg_struct *p_cfg){
//
//	memset(p_cfg,0,sizeof(i2c_recv_cfg_struct));
//	p_cfg->min_freq = 4000;
//	p_cfg->max_freq = 5000;
//	p_cfg->auto_step_freq =  10;
//	p_cfg->manual_step_freq =  4;
//
//	p_cfg->time = 2;
//	p_cfg->en_lit = 0xFFFF;
//	p_cfg->b_en_fast_scan = 0;
//	p_cfg->view_mode = i2c_recv_view_mode_normal;
//
//	p_cfg->lvl = 0x30;
//
//}
//
//uint32_t  cntr_i2_recv_init_4_5(control_struct *p_ctl, uint8_t table_pos){
//GPIO_InitTypeDef GPIO_InitStructure;
//
//	memset(&p_ctl->i2c_recv_4_5,0,sizeof(p_ctl->i2c_recv_4_5));
//
//	p_ctl->i2c_recv_4_5.type = i2c_recv_type_4_5;
//
//	p_ctl->i2c_recv_4_5.p_hi2c = &hi2c3;
//	p_ctl->i2c_recv_4_5.addr = 2;
//
//	p_ctl->i2c_recv_4_5.p_cfg = &p_ctl->dev_config.rec.i2c_recv_4_5_cfg;
//	p_ctl->i2c_recv_4_5.p_cfg->b_en_fast_scan = 0;
//
//	p_ctl->i2c_recv_4_5.max_num_ch = RECV_4_5_MAX_NUM_CH;
////	p_ctl->i2c_recv_4_5.ch_table_ptr = (uint8_t*)ch_4_5_table[0];
////	p_ctl->i2c_recv_4_5.lit_table_ptr = (uint8_t*)ch_4_5_lit_table[0];
//	p_ctl->i2c_recv_4_5.table_freq_ptr = (uint16_t*)ch_4_5_freq;
//
//	p_ctl->i2c_recv_4_5.reset_pin =VID3_SW_I2RECV_4_5_RST_PIN;
//	p_ctl->i2c_recv_4_5.reset_port =VID3_SW_I2RECV_4_5_RST_PORT;
//
//	switch_fdc_start_init(&p_ctl->i2c_recv_4_5.sw_pwr_5v);
//	p_ctl->i2c_recv_4_5.sw_pwr_5v.b_inveretd_ON = 1;
//	p_ctl->i2c_recv_4_5.sw_pwr_5v.ON_pin = VID3_PWR_EN_I2RECV_4_5_PIN;
//	p_ctl->i2c_recv_4_5.sw_pwr_5v.ON_port = VID3_PWR_EN_I2RECV_4_5_PORT;
//	switch_fdc_on(&p_ctl->i2c_recv_4_5.sw_pwr_5v,fdc_on);
//	switch_fdc_set_data(&p_ctl->i2c_recv_4_5.sw_pwr_5v);
//
//	switch_fdc_start_init(&p_ctl->i2c_recv_4_5.sw_en);
//	p_ctl->i2c_recv_4_5.sw_en.b_inveretd_ON = 1;
//	p_ctl->i2c_recv_4_5.sw_en.ON_pin = VID3_SW_I2RECV_4_5_EN_PIN;
//	p_ctl->i2c_recv_4_5.sw_en.ON_port = VID3_SW_I2RECV_4_5_EN_PORT;
//	switch_fdc_on(&p_ctl->i2c_recv_4_5.sw_en,fdc_on);
//	switch_fdc_set_data(&p_ctl->i2c_recv_4_5.sw_en);
//	switch_fdc_pin_init(&p_ctl->i2c_recv_4_5.sw_en);
//
//	switch_fdc_start_init(&p_ctl->i2c_recv_4_5.sw_in);
//	p_ctl->i2c_recv_4_5.sw_in.b_inveretd_ON = 0;
//	p_ctl->i2c_recv_4_5.sw_in.ON_pin = VID3_SW_I2RECV_4_5_IN_PIN;
//	p_ctl->i2c_recv_4_5.sw_in.ON_port = VID3_SW_I2RECV_4_5_IN_PORT;
//	switch_fdc_on(&p_ctl->i2c_recv_4_5.sw_in,fdc_off);
//	switch_fdc_set_data(&p_ctl->i2c_recv_4_5.sw_in);
//	switch_fdc_pin_init(&p_ctl->i2c_recv_4_5.sw_in);
//
//	GPIO_InitStructure.Pin = p_ctl->i2c_recv_4_5.sw_pwr_5v.ON_pin;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
//	HAL_GPIO_Init(p_ctl->i2c_recv_4_5.sw_pwr_5v.ON_port, &GPIO_InitStructure);
//
//	i2c_recv_start_init(&p_ctl->i2c_recv_4_5);
//
//	return i2c_recv_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->i2c_recv_4_5,NULL);
//}
//#endif
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef RFFC5071
//uint32_t  cntr_rffc5071_init(control_struct *p_ctl, uint8_t table_pos){
////GPIO_InitTypeDef GPIO_InitStructure;
//
//	p_ctl->rffc5071.ref_freq = REF_FREQ_12;
//
//	p_ctl->reset_port = NULL;
//	p_ctl->rffc5071.cs_pin = VID3_CS_RX5808_PIN;
//	p_ctl->rffc5071.cs_port = VID3_CS_RX5808_PORT;
//
//
//	p_ctl->rffc5071.mosi_port = VID3_MOSI_PORT;
//	p_ctl->rffc5071.mosi_pin = VID3_MOSI_PIN;
//
//	p_ctl->rffc5071.clk_port = VID3_SCK_PORT;
//	p_ctl->rffc5071.clk_pin = VID3_SCK_PIN;
//
//	switch_fdc_start_init(&p_ctl->rffc5071.sw_en);
//	p_ctl->rffc5071.sw_en.b_inveretd_ON = 0;
//	p_ctl->rffc5071.sw_en.ON_pin = GPIO_PIN_12;
//	p_ctl->rffc5071.sw_en.ON_port = GPIOE;
//	switch_fdc_on(&p_ctl->rffc5071.sw_en,fdc_off);
//	switch_fdc_set_data(&p_ctl->rffc5071.sw_en);
//	switch_fdc_pin_init(&p_ctl->rffc5071.sw_en);
//
//	switch_fdc_start_init(&p_ctl->rffc5071.sw0);
//	p_ctl->rffc5071.sw0.b_inveretd_ON = 0;
//	p_ctl->rffc5071.sw0.ON_pin = GPIO_PIN_13;
//	p_ctl->rffc5071.sw0.ON_port = GPIOE;
//	switch_fdc_on(&p_ctl->rffc5071.sw0,fdc_off);
//	switch_fdc_set_data(&p_ctl->rffc5071.sw0);
//	switch_fdc_pin_init(&p_ctl->rffc5071.sw0);
//
//	switch_fdc_start_init(&p_ctl->rffc5071.sw1);
//	p_ctl->rffc5071.sw1.b_inveretd_ON = 0;
//	p_ctl->rffc5071.sw1.ON_pin = GPIO_PIN_14;
//	p_ctl->rffc5071.sw1.ON_port = GPIOE;
//	switch_fdc_on(&p_ctl->rffc5071.sw1,fdc_off);
//	switch_fdc_set_data(&p_ctl->rffc5071.sw1);
//	switch_fdc_pin_init(&p_ctl->rffc5071.sw1);
//
//	switch_fdc_start_init(&p_ctl->rffc5071.sw2);
//	p_ctl->rffc5071.sw2.b_inveretd_ON = 0;
//	p_ctl->rffc5071.sw2.ON_pin = GPIO_PIN_8;
//	p_ctl->rffc5071.sw2.ON_port = GPIOC;
//	switch_fdc_on(&p_ctl->rffc5071.sw2,fdc_off);
//	switch_fdc_set_data(&p_ctl->rffc5071.sw2);
//	switch_fdc_pin_init(&p_ctl->rffc5071.sw2);
//
//
//
//
////
////
////	switch_fdc_start_init(&p_ctl->rtc6715.sw_da1_en);
////	p_ctl->rtc6715.sw_da1_en.b_inveretd_ON = 1;
////	p_ctl->rtc6715.sw_da1_en.ON_pin = VID3_SW_5_8_EN_PIN;
////	p_ctl->rtc6715.sw_da1_en.ON_port = VID3_SW_5_8_EN_PORT;
////	switch_fdc_on(&p_ctl->rtc6715.sw_da1_en,fdc_on);
////	switch_fdc_set_data(&p_ctl->rtc6715.sw_da1_en);
////	switch_fdc_pin_init(&p_ctl->rtc6715.sw_da1_en);
////
////	switch_fdc_start_init(&p_ctl->rtc6715.sw_da1_in);
////	p_ctl->rtc6715.sw_da1_in.b_inveretd_ON = 0;
////	p_ctl->rtc6715.sw_da1_in.ON_pin = VID3_SW_5_8_IN_PIN;
////	p_ctl->rtc6715.sw_da1_in.ON_port = VID3_SW_5_8_IN_PORT;
////	switch_fdc_on(&p_ctl->rtc6715.sw_da1_in,fdc_off);
////	switch_fdc_set_data(&p_ctl->rtc6715.sw_da1_in);
////	switch_fdc_pin_init(&p_ctl->rtc6715.sw_da1_in);
////
////	GPIO_InitStructure.Pin = p_ctl->rtc6715.sw_pwr_5v.ON_pin;
////	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
////	GPIO_InitStructure.Pull = GPIO_NOPULL;
////	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
////	HAL_GPIO_Init(p_ctl->rtc6715.sw_pwr_5v.ON_port, &GPIO_InitStructure);
//
//	memcpy(&p_ctl->rffc5071.short_time,&p_ctl->short_time,sizeof(p_ctl->short_time));
//
////	rtc6715_start_pin_init(&p_ctl->rtc6715);
//
////	p_ctl->rtc6715.p_cfg = &p_ctl->dev_config.rec.recv_5_8_cfg;
//
//	rffc5071_start_init(&p_ctl->rffc5071);
//
//
//
////	return rtc6715_add_to_cell_table(&p_ctl->comp_table[0],table_pos,&p_ctl->rtc6715,NULL);
//	return rffc5071_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->rffc5071,NULL);
//}
//#endif
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef RTC6715
//#ifdef LORA
//uint32_t  cntr_recv_5_8_init(control_struct *p_ctl, uint8_t table_pos){
//GPIO_InitTypeDef GPIO_InitStructure;
//	p_ctl->rtc6715.cs_pin = VID3_CS_RX5808_PIN;
//	p_ctl->rtc6715.cs_port = VID3_CS_RX5808_PORT;
//
//
//	p_ctl->rtc6715.mosi_port = VID3_MOSI_PORT;
//	p_ctl->rtc6715.mosi_pin = VID3_MOSI_PIN;
//
//	p_ctl->rtc6715.clk_port = VID3_SCK_PORT;
//	p_ctl->rtc6715.clk_pin = VID3_SCK_PIN;
//
//	p_ctl->rtc6715.rssi_port  = VID3_RSII_5_8_PORT;
//	p_ctl->rtc6715.rssi_pin = VID3_RSII_5_8_PIN;
//
//	switch_fdc_start_init(&p_ctl->rtc6715.sw_pwr_5v);
//	p_ctl->rtc6715.sw_pwr_5v.b_inveretd_ON = 1;
//	p_ctl->rtc6715.sw_pwr_5v.ON_pin = VID3_PWR_EN_5_8_PIN;
//	p_ctl->rtc6715.sw_pwr_5v.ON_port = VID3_PWR_EN_5_8_PORT;
//	switch_fdc_on(&p_ctl->rtc6715.sw_pwr_5v,fdc_on);
//	switch_fdc_set_data(&p_ctl->rtc6715.sw_pwr_5v);
//
//
//	switch_fdc_start_init(&p_ctl->rtc6715.sw_da1_en);
//	p_ctl->rtc6715.sw_da1_en.b_inveretd_ON = 1;
//	p_ctl->rtc6715.sw_da1_en.ON_pin = VID3_SW_5_8_EN_PIN;
//	p_ctl->rtc6715.sw_da1_en.ON_port = VID3_SW_5_8_EN_PORT;
//	switch_fdc_on(&p_ctl->rtc6715.sw_da1_en,fdc_on);
//	switch_fdc_set_data(&p_ctl->rtc6715.sw_da1_en);
//	switch_fdc_pin_init(&p_ctl->rtc6715.sw_da1_en);
//
//	switch_fdc_start_init(&p_ctl->rtc6715.sw_da1_in);
//	p_ctl->rtc6715.sw_da1_in.b_inveretd_ON = 0;
//	p_ctl->rtc6715.sw_da1_in.ON_pin = VID3_SW_5_8_IN_PIN;
//	p_ctl->rtc6715.sw_da1_in.ON_port = VID3_SW_5_8_IN_PORT;
//	switch_fdc_on(&p_ctl->rtc6715.sw_da1_in,fdc_off);
//	switch_fdc_set_data(&p_ctl->rtc6715.sw_da1_in);
//	switch_fdc_pin_init(&p_ctl->rtc6715.sw_da1_in);
//
//	GPIO_InitStructure.Pin = p_ctl->rtc6715.sw_pwr_5v.ON_pin;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
//	HAL_GPIO_Init(p_ctl->rtc6715.sw_pwr_5v.ON_port, &GPIO_InitStructure);
//
//	memcpy(&p_ctl->rtc6715.short_time,&p_ctl->short_time,sizeof(p_ctl->short_time));
//
//	rtc6715_start_pin_init(&p_ctl->rtc6715);
//
//	p_ctl->rtc6715.p_cfg = &p_ctl->dev_config.rec.recv_5_8_cfg;
//
//	rtc6715_start_init(&p_ctl->rtc6715);
//
//#ifdef RECV_5_8_RSSI
//	recv_5_8_rssi_init(&p_ctl->rtc6715,p_ctl->main_thread_id,cntrl_thread_event_recv_5_8_rssi);
//	p_ctl->rtc6715.rssi.p_adc = &AdcHandle;
//	p_ctl->rtc6715.rssi.adc_buf_num_point = 16;
//	p_ctl->rtc6715.rssi.p_adc_buf = &p_ctl->adc_ring_buf[0];
//	p_ctl->rtc6715.rssi.p_tcp_cmd = &p_ctl->tcp_cmd;
//#endif
//
//#ifdef RECV_5_8_SUB_3_3
//	p_ctl->rtc6715.p_rff5071 = &p_ctl->rffc5071;
//#endif
//
//
////	return rtc6715_add_to_cell_table(&p_ctl->comp_table[0],table_pos,&p_ctl->rtc6715,NULL);
//	return rtc6715_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->rtc6715,NULL);
//}
//#endif
//#ifndef LORA
//uint32_t  cntr_recv_5_8_init(control_struct *p_ctl, uint8_t table_pos){
//GPIO_InitTypeDef GPIO_InitStructure;
//	p_ctl->rtc6715.cs_pin = VID3_CS_RX5808_PIN;
//	p_ctl->rtc6715.cs_port = VID3_CS_RX5808_PORT;
//
//
//	p_ctl->rtc6715.mosi_port = VID3_MOSI_PORT;
//	p_ctl->rtc6715.mosi_pin = VID3_MOSI_PIN;
//
//	p_ctl->rtc6715.clk_port = VID3_SCK_PORT;
//	p_ctl->rtc6715.clk_pin = VID3_SCK_PIN;
//
//	p_ctl->rtc6715.rssi_port  = VID3_RSII_5_8_PORT;
//	p_ctl->rtc6715.rssi_pin = VID3_RSII_5_8_PIN;
//
//	switch_fdc_start_init(&p_ctl->rtc6715.sw_pwr_5v);
//	p_ctl->rtc6715.sw_pwr_5v.b_inveretd_ON = 1;
//	p_ctl->rtc6715.sw_pwr_5v.ON_pin = VID3_PWR_EN_5_8_PIN;
//	p_ctl->rtc6715.sw_pwr_5v.ON_port = VID3_PWR_EN_5_8_PORT;
//	switch_fdc_on(&p_ctl->rtc6715.sw_pwr_5v,fdc_on);
//	switch_fdc_set_data(&p_ctl->rtc6715.sw_pwr_5v);
//
//
//	switch_fdc_start_init(&p_ctl->rtc6715.sw_da1_en);
//	p_ctl->rtc6715.sw_da1_en.b_inveretd_ON = 1;
//	p_ctl->rtc6715.sw_da1_en.ON_pin = VID3_SW_5_8_EN_PIN;
//	p_ctl->rtc6715.sw_da1_en.ON_port = VID3_SW_5_8_EN_PORT;
//	switch_fdc_on(&p_ctl->rtc6715.sw_da1_en,fdc_on);
//	switch_fdc_set_data(&p_ctl->rtc6715.sw_da1_en);
//	switch_fdc_pin_init(&p_ctl->rtc6715.sw_da1_en);
//
//	switch_fdc_start_init(&p_ctl->rtc6715.sw_da1_in);
//	p_ctl->rtc6715.sw_da1_in.b_inveretd_ON = 0;
//	p_ctl->rtc6715.sw_da1_in.ON_pin = VID3_SW_5_8_IN_PIN;
//	p_ctl->rtc6715.sw_da1_in.ON_port = VID3_SW_5_8_IN_PORT;
//	switch_fdc_on(&p_ctl->rtc6715.sw_da1_in,fdc_off);
//	switch_fdc_set_data(&p_ctl->rtc6715.sw_da1_in);
//	switch_fdc_pin_init(&p_ctl->rtc6715.sw_da1_in);
//
//	GPIO_InitStructure.Pin = p_ctl->rtc6715.sw_pwr_5v.ON_pin;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
//	HAL_GPIO_Init(p_ctl->rtc6715.sw_pwr_5v.ON_port, &GPIO_InitStructure);
//
//	memcpy(&p_ctl->rtc6715.short_time,&p_ctl->short_time,sizeof(p_ctl->short_time));
//
//	rtc6715_start_pin_init(&p_ctl->rtc6715);
//
//	p_ctl->rtc6715.p_cfg = &p_ctl->dev_config.rec.recv_5_8_cfg;
//
//	rtc6715_start_init(&p_ctl->rtc6715);
//
//#ifdef RECV_5_8_RSSI
//	recv_5_8_rssi_init(&p_ctl->rtc6715,p_ctl->main_thread_id,cntrl_thread_event_recv_5_8_rssi);
//	p_ctl->rtc6715.rssi.p_adc = &AdcHandle;
//	p_ctl->rtc6715.rssi.adc_buf_num_point = 16;
//	p_ctl->rtc6715.rssi.p_adc_buf = &p_ctl->adc_ring_buf[0];
//	p_ctl->rtc6715.rssi.p_tcp_cmd = &p_ctl->tcp_cmd;
//#endif
//
//#ifdef RECV_5_8_SUB_3_3
//	p_ctl->rtc6715.p_rff5071 = &p_ctl->rffc5071;
//#endif
//
//
////	return rtc6715_add_to_cell_table(&p_ctl->comp_table[0],table_pos,&p_ctl->rtc6715,NULL);
//	return rtc6715_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->rtc6715,NULL);
//}
//#endif
//#endif
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef MULTI_EMIT
//uint32_t  cntr_rtc6705_multi_1_init(control_struct *p_ctl, uint8_t table_pos){
//
//	GPIO_InitTypeDef GPIO_InitStructure;
//
//	GPIO_InitStructure.Pin = GPIO_PIN_10;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
//	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);
//
//
//	p_ctl->mutli_e1.p_cfg = &p_ctl->dev_config.rec.rtc6705_cfg;
//
////edit!!!!
//	MX_TIM4_Init(p_ctl->mutli_e1.p_cfg->period,p_ctl->mutli_e1.p_cfg->imp);
//
//	p_ctl->mutli_e1.p_htim = &htim4;
//	p_ctl->mutli_e1.tim_ch = TIM_CHANNEL_4;
//
//	p_ctl->mutli_e1.rtc[0].cs_port = GPIOD;
//	p_ctl->mutli_e1.rtc[0].cs_pin = GPIO_PIN_13;
//
//	p_ctl->mutli_e1.rtc[1].cs_port = GPIOD;
//	p_ctl->mutli_e1.rtc[1].cs_pin = GPIO_PIN_12;
//
//	p_ctl->mutli_e1.rtc[2].cs_port = GPIOE;
//	p_ctl->mutli_e1.rtc[2].cs_pin = GPIO_PIN_8;
//
//	p_ctl->mutli_e1.rtc[3].cs_port = GPIOE;
//	p_ctl->mutli_e1.rtc[3].cs_pin = GPIO_PIN_9;
//
//	p_ctl->mutli_e1.mosi_port = GPIOD;
//	p_ctl->mutli_e1.mosi_pin = GPIO_PIN_11;
//
//	p_ctl->mutli_e1.clk_port = GPIOE;
//	p_ctl->mutli_e1.clk_pin = GPIO_PIN_7;
//
//	rtc6705_multi_start_init(&p_ctl->mutli_e1);
//
//
//	p_ctl->mutli_e1.rtc[0].freq= control.dev_config.rec.freq[0];
//	p_ctl->mutli_e1.rtc[1].freq= control.dev_config.rec.freq[1];
//	p_ctl->mutli_e1.rtc[2].freq= control.dev_config.rec.freq[2];
//	p_ctl->mutli_e1.rtc[3].freq= control.dev_config.rec.freq[3];
//
//	p_ctl->mutli_e1.reg.reg4.bits.BC_SEL = 0;
//
//	switch_fdc_start_init(&p_ctl->mutli_e1.sw_pwr);
//	p_ctl->mutli_e1.sw_pwr.b_inveretd_ON = 1;
//	p_ctl->mutli_e1.sw_pwr.ON_pin = GPIO_PIN_13;
//	p_ctl->mutli_e1.sw_pwr.ON_port = GPIOC;
//	switch_fdc_on(&p_ctl->mutli_e1.sw_pwr,fdc_off);
//	switch_fdc_set_data(&p_ctl->mutli_e1.sw_pwr);
//	switch_fdc_pin_init(&p_ctl->mutli_e1.sw_pwr);
//
//	switch_fdc_start_init(&p_ctl->mutli_e1.sw_amp);
//	p_ctl->mutli_e1.sw_amp.b_inveretd_ON = 1;
//	p_ctl->mutli_e1.sw_amp.ON_pin = GPIO_PIN_15;
//	p_ctl->mutli_e1.sw_amp.ON_port = GPIOC;
//	switch_fdc_on(&p_ctl->mutli_e1.sw_amp,fdc_off);
//	switch_fdc_set_data(&p_ctl->mutli_e1.sw_amp);
//	switch_fdc_pin_init(&p_ctl->mutli_e1.sw_amp);
//
//	switch_fdc_start_init(&p_ctl->mutli_e1.sw_rel);
//	p_ctl->mutli_e1.sw_rel.b_inveretd_ON = 0;
//	p_ctl->mutli_e1.sw_rel.ON_pin = GPIO_PIN_10;
//	p_ctl->mutli_e1.sw_rel.ON_port = GPIOD;
//	switch_fdc_on(&p_ctl->mutli_e1.sw_rel,fdc_off);
//	switch_fdc_set_data(&p_ctl->mutli_e1.sw_rel);
//	switch_fdc_pin_init(&p_ctl->mutli_e1.sw_rel);
//
//
//	return rtc6705_multi_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->mutli_e1,RTC6705_MULTI_NAME_REG);
//}
//
//uint32_t  cntr_rtc6705_multi_2_init(control_struct *p_ctl, uint8_t table_pos){
//
//	p_ctl->mutli_e2.p_cfg = &p_ctl->dev_config.rec.rtc6705_cfg_add;
//
////edit!!!!
//	MX_TIM1_Init(p_ctl->mutli_e2.p_cfg->period,p_ctl->mutli_e2.p_cfg->imp);
//
//	htim1.Instance->CCER |= TIM_CCER_CC2NE;
//	htim1.Instance->PSC = 1;
//
//	p_ctl->mutli_e2.p_htim = &htim1;
//	p_ctl->mutli_e2.tim_ch = TIM_CHANNEL_2;
//
//	p_ctl->mutli_e2.rtc[0].cs_port = GPIOA;
//	p_ctl->mutli_e2.rtc[0].cs_pin = GPIO_PIN_15;
//
//	p_ctl->mutli_e2.rtc[1].cs_port = GPIOB;
//	p_ctl->mutli_e2.rtc[1].cs_pin = GPIO_PIN_15;
//
//	p_ctl->mutli_e2.rtc[2].cs_port = GPIOA;
//	p_ctl->mutli_e2.rtc[2].cs_pin = GPIO_PIN_3;
//
//	p_ctl->mutli_e2.rtc[3].cs_port = GPIOE;
//	p_ctl->mutli_e2.rtc[3].cs_pin = GPIO_PIN_10;
//
//	p_ctl->mutli_e2.mosi_port = GPIOB;
//	p_ctl->mutli_e2.mosi_pin = GPIO_PIN_0;
//
//	p_ctl->mutli_e2.clk_port = GPIOB;
//	p_ctl->mutli_e2.clk_pin = GPIO_PIN_1;
//
//	rtc6705_multi_start_init(&p_ctl->mutli_e2);
//
//	p_ctl->mutli_e2.rtc[0].freq= control.dev_config.rec.freq[4];
//	p_ctl->mutli_e2.rtc[1].freq= control.dev_config.rec.freq[5];
//	p_ctl->mutli_e2.rtc[2].freq= control.dev_config.rec.freq[6];
//	p_ctl->mutli_e2.rtc[3].freq= control.dev_config.rec.freq[7];
//
//	p_ctl->mutli_e2.reg.reg4.bits.BC_SEL = 0;
//
//	switch_fdc_start_init(&p_ctl->mutli_e2.sw_pwr);
//	p_ctl->mutli_e2.sw_pwr.b_inveretd_ON = 1;
//	p_ctl->mutli_e2.sw_pwr.ON_pin = GPIO_PIN_0;
//	p_ctl->mutli_e2.sw_pwr.ON_port = GPIOA;
//	switch_fdc_on(&p_ctl->mutli_e2.sw_pwr,fdc_off);
//	switch_fdc_set_data(&p_ctl->mutli_e2.sw_pwr);
//	switch_fdc_pin_init(&p_ctl->mutli_e2.sw_pwr);
//
//	switch_fdc_start_init(&p_ctl->mutli_e2.sw_amp);
//	p_ctl->mutli_e2.sw_amp.b_inveretd_ON = 1;
//	p_ctl->mutli_e2.sw_amp.ON_pin = GPIO_PIN_12;
//	p_ctl->mutli_e2.sw_amp.ON_port = GPIOC;
//	switch_fdc_on(&p_ctl->mutli_e2.sw_amp,fdc_off);
//	switch_fdc_set_data(&p_ctl->mutli_e2.sw_amp);
//	switch_fdc_pin_init(&p_ctl->mutli_e2.sw_amp);
//
//	switch_fdc_start_init(&p_ctl->mutli_e2.sw_rel);
//	p_ctl->mutli_e2.sw_rel.b_inveretd_ON = 0;
//	p_ctl->mutli_e2.sw_rel.ON_pin = GPIO_PIN_11;
//	p_ctl->mutli_e2.sw_rel.ON_port = GPIOC;
//	switch_fdc_on(&p_ctl->mutli_e2.sw_rel,fdc_off);
//	switch_fdc_set_data(&p_ctl->mutli_e2.sw_rel);
//	switch_fdc_pin_init(&p_ctl->mutli_e2.sw_rel);
//
////	return table_pos;
//	return rtc6705_multi_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->mutli_e2,RTC6705_MULTI_NAME_REG);
//}
//#endif
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef RTC6705
//uint32_t  cntr_rtc6705_init(control_struct *p_ctl, uint8_t table_pos){
//
//	p_ctl->rtc6705.p_cfg = &p_ctl->dev_config.rec.rtc6705_cfg;
//
//	MX_TIM4_Init(p_ctl->rtc6705.p_cfg->period,p_ctl->rtc6705.p_cfg->imp);
//
//	p_ctl->rtc6705.p_htim = &htim4;
//
////	p_ctl->rtc6705.cs_pin = VID3_EXT_CS_PIN;
////	p_ctl->rtc6705.cs_port = VID3_EXT_CS_PORT;
//
//	p_ctl->rtc6705.cs_port = GPIOC;
//	p_ctl->rtc6705.cs_pin = GPIO_PIN_8;
//
//
//
////	p_ctl->rtc6705.mosi_port = VID3_MOSI_PORT;
////	p_ctl->rtc6705.mosi_pin = VID3_MOSI_PIN;
//
//	p_ctl->rtc6705.mosi_port = GPIOE;
//	p_ctl->rtc6705.mosi_pin = GPIO_PIN_13;
//
//
////	p_ctl->rtc6705.clk_port = VID3_SCK_PORT;
////	p_ctl->rtc6705.clk_pin = VID3_SCK_PIN;
//
//	p_ctl->rtc6705.clk_port = GPIOE;
//	p_ctl->rtc6705.clk_pin = GPIO_PIN_14;
//
//
//	memcpy(&p_ctl->rtc6705.short_time,&p_ctl->short_time,sizeof(p_ctl->short_time));
//
////	rtc6705_start_pin_init(&p_ctl->rtc6705);
//
//	rtc6705_start_init(&p_ctl->rtc6705);
//
////	p_ctl->rtc6705.reg.reg7.word = 0;
//
//	p_ctl->rtc6705.reg.reg4.bits.BC_SEL = 0;
//
//	switch_fdc_start_init(&p_ctl->rtc6705.sw_24v);
//	p_ctl->rtc6705.sw_24v.b_inveretd_ON = 0;
////	p_ctl->rtc6705.sw_24v.ON_pin = VID3_24V_EN_PIN;
////	p_ctl->rtc6705.sw_24v.ON_port = VID3_24V_EN_PORT;
//
//	p_ctl->rtc6705.sw_24v.ON_pin = GPIO_PIN_1;
//	p_ctl->rtc6705.sw_24v.ON_port = GPIOD;
//
//
//	switch_fdc_on(&p_ctl->rtc6705.sw_24v,fdc_off);
//	switch_fdc_set_data(&p_ctl->rtc6705.sw_24v);
//	switch_fdc_pin_init(&p_ctl->rtc6705.sw_24v);
//
//	switch_fdc_start_init(&p_ctl->rtc6705.sw_3v);
//	p_ctl->rtc6705.sw_3v.b_inveretd_ON = 1;
//
////	p_ctl->rtc6705.sw_3v.ON_pin = VID3_3V_EN_PIN;
////	p_ctl->rtc6705.sw_3v.ON_port = VID3_3V_EN_PORT;
//
//	p_ctl->rtc6705.sw_3v.ON_pin = GPIO_PIN_12;
//	p_ctl->rtc6705.sw_3v.ON_port = GPIOE;
//
//	switch_fdc_on(&p_ctl->rtc6705.sw_3v,fdc_off);
//	switch_fdc_set_data(&p_ctl->rtc6705.sw_3v);
//	switch_fdc_pin_init(&p_ctl->rtc6705.sw_3v);
//
////	return rtc6705_add_to_cell_table(&p_ctl->comp_table[0],table_pos,&p_ctl->rtc6705,NULL);
//
//	return rtc6705_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->rtc6705,RTC6705_NAME_REG);
//
//
//
//}
//#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////void grabber_init (grabber_struct*	p_ctl, uint8_t* p_in_buf, uint8_t* p_out_buf, uint32_t in_size, uint32_t out_size) {
//#ifndef NOT_VIDEO_RECEIVER
//void grabber_init (grabber_struct*	p_ctl, uint8_t* p_in_buf, uint8_t* p_out_buf, uint32_t in_size, uint32_t out_size, uint8_t *p_out1, uint32_t out1_size) {
//
//	GPIO_InitTypeDef GPIO_InitStructure;
//
//	memset(grab_image_dst_ext,0,sizeof(grab_image_dst_ext));
//
//	GPIO_InitStructure.Pin = p_ctl->fi_id_pin;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;//GPIO_PULLUP;
//	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
//	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
//	HAL_GPIO_Init(p_ctl->p_fi_id_port, &GPIO_InitStructure);
//
//	MX_I2C1_Init();
//
//	p_ctl->b_start = 0;
//	p_ctl->b_en = 0;
//	p_ctl->b_in_progress = 0;
//
//	p_ctl->waited_type_frame = grabber_frame_1;
//
//
//	p_ctl->wigth = GRABBER_DEF_WIDTH;
//	p_ctl->height = GRABBER_DEF_HEIGHT;
//
//	p_ctl->p_input_buff = p_in_buf;
//	p_ctl->input_buff_size = in_size;
//
//	p_ctl->p_output_buff = p_out_buf;
//	p_ctl->output_buff_size = out_size;;
//
//	p_ctl->line_wr_max_pos_buf_0 = p_ctl->output_buff_size  / p_ctl->wigth;
//
//	p_ctl->p_output_buff_1 = p_out1;
//	p_ctl->output_buff_size_1 = out1_size;
//	p_ctl->line_wr_max_pos_buf_1 = p_ctl->output_buff_size_1  / p_ctl->wigth;
//
//	p_ctl->io.num_wr = 0;
//	p_ctl->io.num_rd = 0;
//
////	void MX_DCMI_Init(void)
////	{
//
//	p_ctl->hdcmi.Instance = DCMI;
//	p_ctl->hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
//	p_ctl->hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_FALLING;//DCMI_PCKPOLARITY_RISING;//DCMI_PCKPOLARITY_FALLING;
//	p_ctl->hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_HIGH;
//	p_ctl->hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_HIGH;
//	p_ctl->hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
//	p_ctl->hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
//	p_ctl->hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
//	  HAL_DCMI_Init(&p_ctl->hdcmi);
//		  // DMA
//	  p_ctl->hdma_dcmi.Init.Channel = DMA_CHANNEL_1;
//	  p_ctl->hdma_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;
//	  p_ctl->hdma_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;
//	  p_ctl->hdma_dcmi.Init.MemInc = DMA_MINC_ENABLE;
//	  p_ctl->hdma_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
//	  p_ctl->hdma_dcmi.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
//	  p_ctl->hdma_dcmi.Init.Mode = DMA_CIRCULAR;//DMA_PFCTRL;//DMA_NORMAL;
//	  p_ctl->hdma_dcmi.Init.Priority = DMA_PRIORITY_MEDIUM;//DMA_PRIORITY_VERY_HIGH;
//	  p_ctl->hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_ENABLE;//DMA_FIFOMODE_DISABLE;//DMA_FIFOMODE_ENABLE;// DMA_FIFOMODE_DISABLE;//DMA_FIFOMODE_ENABLE;
//	  p_ctl->hdma_dcmi.Init.FIFOThreshold =DMA_FIFO_THRESHOLD_FULL;// DMA_FIFO_THRESHOLD_HALFFULL;
//	  p_ctl->hdma_dcmi.Init.MemBurst = DMA_MBURST_SINGLE;
//	  p_ctl->hdma_dcmi.Init.PeriphBurst = DMA_PBURST_SINGLE;//DMA_PBURST_INC16;//DMA_PBURST_SINGLE;
//	  p_ctl->hdma_dcmi.Instance = DMA2_Stream7;
////	  p_ctl->hdma_dcmi.Instance = DMA2_Stream1;
//
//	  p_ctl->hdma_dcmi.XferHalfCpltCallback = grabber_half_dma_get;
//	  p_ctl->hdma_dcmi.XferCpltCallback = grabber_all_dma_get;
//
//	  p_ctl->line_wait_start = 0;
//	  p_ctl->line_wait_stop = p_ctl->height;///2;
//	 // p_ctl->io.line_wr_pos_buf = 0;
//	  p_ctl->line_wr_pos_buf = 0;
//	  p_ctl->line_wr_max_pos_buf = p_ctl->output_buff_size/p_ctl->wigth;
//	  p_ctl->io.line_free_cntr = p_ctl->line_wr_max_pos_buf;
//
//		  HAL_DMA_Init(&p_ctl->hdma_dcmi);
//
//		  // Связываем DMA с DCMI
//		  __HAL_LINKDMA(&p_ctl->hdcmi, DMA_Handle, p_ctl->hdma_dcmi);
//
//		  uint16_t w,h;
//		  w = *(uint16_t*)&control.dev_config.rec.tmp[2];
//		  h = *(uint16_t*)&control.dev_config.rec.tmp[4];
//
//			w = w - w%2;
//			if(w>320) w = 320;
//
//			if(!h) h = 2;
//			if(h>72) h = 72;
//
////		  HAL_DCMI_ConfigCrop(&p_ctl->hdcmi,60+60+60, (300-p_ctl->height)/2, p_ctl->wigth*2-1, p_ctl->height);
//		  HAL_DCMI_ConfigCrop(&p_ctl->hdcmi,w, h, p_ctl->wigth*2-1, p_ctl->height);
//
//		  HAL_DCMI_EnableCrop(&p_ctl->hdcmi);
//
//		  // Прерывание по завершении передачи (864-p_ctl->wigth)/2+
//		  HAL_NVIC_SetPriority(DCMI_IRQn, 5, 0);
//		  HAL_NVIC_EnableIRQ(DCMI_IRQn);
//	//
//	//	  // Прерывание при полном приеме
//		  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
//		  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
//
//		  // Инициализация DMA с DCMI
//
//		  // Включение DMA с DCMI
//	//	  __HAL_DCMI_ENABLE(&hdcmi);
//	//	  __HAL_DMA_ENABLE(hdcmi.DMA_Handle);
////	}
//
//}
//#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifndef NOT_VIDEO_RECEIVER
//uint32_t  cntr_grabber_init(control_struct *p_ctl){
//	p_ctl->grabber.p_fi_id_port = VID3_FID_PORT;
//	p_ctl->grabber.fi_id_pin = VID3_FID_PIN;
//	grabber_init (&p_ctl->grabber,(uint8_t*)grab_image,(uint8_t*)grab_image_dst,sizeof(grab_image),sizeof(grab_image_dst),grab_image_dst_ext,sizeof(grab_image_dst_ext));
//	return 0;
//}
//#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void SystemClock_Config(void){
//  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//
//  /** Configure the main internal regulator output voltage
//  */
//  __HAL_RCC_PWR_CLK_ENABLE();
//  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
//
//  /** Initializes the RCC Oscillators according to the specified parameters
//  * in the RCC_OscInitTypeDef structure.
//  */
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
//  RCC_OscInitStruct.PLL.PLLM = 25;
//  RCC_OscInitStruct.PLL.PLLN = 336;
//  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
//  RCC_OscInitStruct.PLL.PLLQ = 4;
//  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Initializes the CPU, AHB and APB buses clocks
//  */
//  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
//
//  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
//  {
//    Error_Handler();
//  }
//}
//////////////////////////////////////////////
////slave config
//////////////////////////////////////////////
//#ifdef CNTRL_SEL_TCP_MSG
//void cntrl_tcp_cmd_start_init(control_struct *p_main,tcp_cmd_eth_parser* p_tcp_cmd, uint16_t port, uint8_t type){
//
//tcp_cmd_int_s *p_ctl;
//
//err_t res;
//
////	p_ctl = &eth_res;
//	p_ctl= &p_tcp_cmd->internal;
//
//	p_ctl->os_message_def.queue_sz = TCP_CMD_INT_MESSAGE_QUE_TYPE_NUM;
//	p_ctl->os_message_def.pool = p_ctl->os_message;
//	p_ctl->os_message_write_id = osMessageCreate(&p_ctl->os_message_def, NULL); // create msg queue
//
//	p_ctl->os_pool_def.item_sz = sizeof(tcp_cmd_ethernet_int_msg_type);
//	p_ctl->os_pool_def.pool_sz = TCP_CMD_INT_MESSAGE_QUE_TYPE_NUM;
//	p_ctl->os_pool_def.pool = p_ctl->os_pool;
//	p_ctl->msg_pool_id = osPoolCreate(&p_ctl->os_pool_def); // create memory pool
//
//
//
//
//
//	memset(&p_ctl->tcp_cmd,0,sizeof(p_ctl->tcp_cmd));
//	memset(&p_ctl->tcp_ack_cmd,0,sizeof(p_ctl->tcp_ack_cmd));
//
//
//	p_ctl->tcp_cmd.os_message_def.queue_sz = TCP_CMD_MESSAGE_QUE_TYPE_NUM;
//	p_ctl->tcp_cmd.os_message_def.pool = p_ctl->tcp_cmd.os_message;
//	p_ctl->tcp_cmd.os_message_write_id = osMessageCreate(&p_ctl->tcp_cmd.os_message_def, NULL); // create msg queue
//
//
//	p_ctl->tcp_cmd.os_pool_def.item_sz = sizeof(tcp_cmd_msg_s);
//	p_ctl->tcp_cmd.os_pool_def.pool_sz = TCP_CMD_MESSAGE_QUE_TYPE_NUM;
//	p_ctl->tcp_cmd.os_pool_def.pool = p_ctl->tcp_cmd.os_pool;
//	p_ctl->tcp_cmd.msg_pool_id = osPoolCreate(&p_ctl->tcp_cmd.os_pool_def); // create memory pool
//
//	p_ctl->tcp_ack_cmd.os_message_def.queue_sz = TCP_CMD_MESSAGE_QUE_TYPE_NUM;
//	p_ctl->tcp_ack_cmd.os_message_def.pool = p_ctl->tcp_ack_cmd.os_message;
//	p_ctl->tcp_ack_cmd.os_message_write_id = osMessageCreate(&p_ctl->tcp_ack_cmd.os_message_def, NULL); // create msg queue
//
//	memset(p_ctl->tcp_ack_cmd.os_pool,0,sizeof(p_ctl->tcp_ack_cmd.os_pool));
//	p_ctl->tcp_ack_cmd.os_pool_def.item_sz = sizeof(tcp_cmd_msg_s);
//	p_ctl->tcp_ack_cmd.os_pool_def.pool_sz = TCP_CMD_MESSAGE_QUE_TYPE_NUM;
//	p_ctl->tcp_ack_cmd.os_pool_def.pool = p_ctl->tcp_ack_cmd.os_pool;
//	p_ctl->tcp_ack_cmd.msg_pool_id = osPoolCreate(&p_ctl->tcp_ack_cmd.os_pool_def); // create memory pool
//
//	p_ctl->tcp_cmd.thread_id = control.thread_id;
//	p_ctl->tcp_cmd.signals = main_cntrl_thread_event_tcp_msg;
//
//
//	p_tcp_cmd->type = type;
//	p_tcp_cmd->get_msg.msg_pool_id = p_ctl->tcp_cmd.msg_pool_id;
//	p_tcp_cmd->get_msg.os_message_write_id = p_ctl->tcp_cmd.os_message_write_id;
//	p_tcp_cmd->get_msg.thread_id = control.thread_id;
//	p_tcp_cmd->get_msg.signals = main_cntrl_thread_event_tcp_cmd_parser;
//
//	p_tcp_cmd->ack_msg.msg_pool_id = p_ctl->tcp_ack_cmd.msg_pool_id;
//	p_tcp_cmd->ack_msg.os_message_write_id = p_ctl->tcp_ack_cmd.os_message_write_id;
//	p_tcp_cmd->ack_msg.thread_id = control.thread_id;// _ctl->own_thread_id;
//	p_tcp_cmd->ack_msg.signals = main_cntrl_thread_event_tcp_ack_msg;
//
////	p_tcp_cmd->p_l_conn = netconn_new_with_callback(NETCONN_TCP,tcp_cmd_eth_parser_callback);
//	p_tcp_cmd->p_l_conn = netconn_new_with_callback(p_tcp_cmd->type,tcp_cmd_eth_parser_callback);
//	if(p_tcp_cmd->type == NETCONN_UDP){
//		p_tcp_cmd->p_conn = p_tcp_cmd->p_l_conn;//udp
//#ifndef LWIP_OLD
//		p_tcp_cmd->p_conn->callback_arg.socket = (int)p_tcp_cmd;//udp
//#else
//		p_tcp_cmd->p_conn->socket = (int)p_tcp_cmd;//udp
//#endif
//	}
//
////	netconn_set_nonblocking(p_tcp_cmd->p_l_conn,1);
//
//	res = netconn_bind(p_tcp_cmd->p_l_conn, NULL,port);
//	if(res!=ERR_OK){
////		printf("err tcp\r\n");
//	}
//	else{
////		printf("tcp s=%x\r\n",(uint32_t)p_tcp_cmd->p_l_conn);
//	}
//#ifndef LWIP_OLD
//		p_tcp_cmd->p_l_conn->callback_arg.socket = (int)p_tcp_cmd;//udp
//#else
//	p_tcp_cmd->p_l_conn->socket = (int)p_tcp_cmd;
//#endif
//	if(p_tcp_cmd->type == NETCONN_TCP){
//		netconn_listen(p_tcp_cmd->p_l_conn);
//	}
//}
//#endif
//////////////////////////////////////////////
////slave config
//////////////////////////////////////////////
//#ifdef CNTRL_SEL_UDP
////void cntrl_parser_udp_start_init(control_struct *p_ctl){
////uni_prot_cmd_callback_param_s callback;
////
////	uni_prot_start_init_ext(&p_ctl->u_slave_udp, uni_prot_type_udp, NULL, NULL);
////	p_ctl->u_slave_udp.master_slave = uni_prot_slave;
////	p_ctl->u_slave_udp.slave.signal.os_parser_thread_id = osThreadGetId();
////	p_ctl->u_slave_udp.slave.signal.os_cmd_signal = main_cntrl_thread_event_udp_parser;
////	p_ctl->u_slave_udp.p_thread_name = "UDP parser";
////
////	osThreadCreate (osThread(uni_prot_thread), &p_ctl->u_slave_udp);
////
////	osDelay(10);
////
////	struct uni_prot_connect_udp_s udps;
////	udps.local_port = 10002;
////	udps.dest_port = 10008;
////	udps.p_conn = NULL;
////	IP4_ADDR(&udps.dest_ip, 192, 168, 1, 1);
////
////	uni_prot_addr_param_s adr;
////	adr.dst_addr_lit = 127;
////	adr.dst_ch = 0x33;
////	adr.src_addr_lit = 0x80;
////	adr.src_ch = 0x81;
////	adr.version =  uni_prot_header_version_1;
////
////	uni_prot_thread_config_udp_connection_cmd(&p_ctl->u_slave_udp, &udps);
////	uni_prot_thread_config_addr_cmd(&p_ctl->u_slave_udp, &adr);
////
////	uni_prot_thread_config_addr_cmd(&p_ctl->u_slave_udp, &adr);
////	uni_prot_thread_start_receive_cmd(&p_ctl->u_slave_udp);
////
////	callback.p_func = NULL;
////	callback.p_param = NULL;
////
////	uni_prot_thread_config_slave_call_back(&p_ctl->u_slave_udp,&callback);
////
////	osDelay(10);
////}
//#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
////
//////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef CNTRL_SEL_UART
//void cntrl_parser_uart_start_init(control_struct *p_ctl){
//uni_prot_addr_param_s adr;
//uni_prot_cmd_callback_param_s callback;
//	uni_prot_start_init_ext(&p_ctl->u_slave_uart, uni_prot_type_uart, NULL, NULL);
//	p_ctl->u_slave_uart.master_slave = uni_prot_slave;
//	p_ctl->u_slave_uart.slave.signal.os_parser_thread_id = osThreadGetId();
//	p_ctl->u_slave_uart.slave.signal.os_cmd_signal = main_cntrl_thread_event_uart_parser;
//	p_ctl->u_slave_uart.conn.uart.p_uart = p_ctl->p_uart1;
//	p_ctl->u_slave_uart.conn.uart.baudrate = 115200;
//	p_ctl->u_slave_uart.conn.uart.data_bits = 8;
//	p_ctl->u_slave_uart.conn.uart.parity = USART_PARITY_NONE;
//	p_ctl->u_slave_uart.conn.uart.stop_bits = USART_STOPBITS_1;
//	p_ctl->u_slave_uart.conn.uart.flow_control = UART_HWCONTROL_NONE;
//	p_ctl->u_slave_uart.max_wait_alloc_send_buf_ms = 100;
//
//	p_ctl->u_slave_uart.conn.uart.os_thread_is = control.tid_uart_driver_fone_thread;
//
//	p_ctl->u_slave_uart.p_thread_name = "Uart parser";
//	osThreadCreate (osThread(uni_prot_thread), &p_ctl->u_slave_uart);
//
//	osDelay(10);
//
//	adr.dst_addr_lit = 127;
//	adr.dst_ch = 0x33;
//	adr.src_addr_lit = 0x80;
//	adr.src_ch = 0x81;
//	adr.version =  uni_prot_header_version_2;
//
//	uni_prot_thread_config_addr_cmd(&p_ctl->u_slave_uart, &adr);
//	callback.p_func = NULL;
//	callback.p_param = NULL;
//
//	uni_prot_thread_config_slave_call_back(&p_ctl->u_slave_uart,&callback);
//
//	uni_prot_thread_config_addr_cmd(&p_ctl->u_slave_uart, &adr);
//	uni_prot_thread_start_receive_cmd(&p_ctl->u_slave_uart);
//	osDelay(10);
//}
//#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
////
//////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef UDP_SENDER
//void cntrl_udp_sender_start_init(control_struct *p_ctl){
//err_t res;
//
//	p_ctl->udp_sender_data_ptr = grab_image_dst_ext;
//
//	p_ctl->udp_sender_send_port = 27015;
////12563400 680e 4001 f000 0400 8282828
////				320
////	typedef struct im_header{
////		uint32_t header;
////		uint16_t num;
////		uint16_t width;
////		uint16_t height;
////		uint16_t lines;
////	//	uint8_t	odd_even;
////
////	}im_header;
//
//	p_ctl->header.header = 0x345612;
//	p_ctl->header.height = GRABBER_DEF_HEIGHT;
//	p_ctl->header.width = 320;//GRABBER_DEF_WIDTH;
//	p_ctl->header.num = 0;
//	p_ctl->header.lines = 0;
//
//	p_ctl->udp_sender_net = netbuf_new ();
//
//	IP4_ADDR(&p_ctl->udp_sender_dest_ip, 192, 168, 1, 1);
//
//	p_ctl->p_udp_sender = netconn_new_with_callback(NETCONN_UDP,NULL);
//	if(p_ctl->p_udp_sender == NULL) {
//
//		return;
//	}
//	netconn_set_nonblocking(p_ctl->p_udp_sender,1);
//	res = netconn_bind(p_ctl->p_udp_sender, NULL,1234);
//	if(res != ERR_OK) {
//		netconn_delete(p_ctl->p_udp_sender);
//		p_ctl->p_udp_sender = NULL;//!!!!!
//		return;
//	}
//#ifndef LWIP_OLD
//	p_ctl->p_udp_sender->callback_arg.socket = (int)p_ctl;
//#else
//	p_ctl->p_udp_sender->socket = (int)p_ctl;
//#endif
//}
//#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
////
//////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef CNTRL_SEL_TCP
//void cntrl_parser_tcp_start_init(control_struct *p_ctl){
//uni_prot_addr_param_s adr;
//uni_prot_cmd_callback_param_s callback;
//struct uni_prot_connect_tcp_s tcp_s;
//
//	uni_prot_start_init_ext(&p_ctl->u_slave_tcp, uni_prot_type_tcp, NULL, NULL);
//	p_ctl->u_slave_tcp.master_slave = uni_prot_slave;
//	p_ctl->u_slave_tcp.slave.signal.os_parser_thread_id = osThreadGetId();
//	p_ctl->u_slave_tcp.slave.signal.os_cmd_signal = main_cntrl_thread_event_tcp_parser;
//
//	p_ctl->u_slave_tcp.p_thread_name = "TCP parser";
//
//	osThreadCreate (osThread(uni_prot_thread), &p_ctl->u_slave_tcp);
//
//	osDelay(100);
//
//	tcp_s.local_port = 10000;
//	tcp_s.dest_port = 10001;
//	tcp_s.p_conn = NULL;
//	tcp_s.p_l_conn = NULL;
//	tcp_s.connect_e = uni_prot_connected_not;
//	IP4_ADDR(&tcp_s.dest_ip, 192, 168, 1, 111);
//
//// uni_prot_addr_param_s adr;
//	adr.dst_addr_lit = 127;
//	adr.dst_ch = 0x33;
//	adr.src_addr_lit = 0x80;
//	adr.src_ch = 0x81;
//	adr.version =  uni_prot_header_version_2;
//#ifdef UNI_PROT_TCP_DEF
//	uni_prot_thread_config_tcp_connection_cmd(&p_ctl->u_slave_tcp, &tcp_s);
//#endif
//	uni_prot_thread_config_addr_cmd(&p_ctl->u_slave_tcp, &adr);
//
//	callback.p_func = NULL;
//	callback.p_param = NULL;
//	uni_prot_thread_config_slave_call_back(&p_ctl->u_slave_tcp,&callback);
//
//	uni_prot_thread_config_addr_cmd(&p_ctl->u_slave_tcp, &adr);
//	uni_prot_thread_start_receive_cmd(&p_ctl->u_slave_tcp);
//
//	osDelay(100);
//}
//#endif
//
//////////////////////////////////////////////////////////////////////////////////////////////////
////
//////////////////////////////////////////////////////////////////////////////////////////////////
///*
//void cntrl_short_start_init(control_struct *p_ctl){
//	p_ctl->short_time.msg_id = osMessageCreate(osMessageQ(short_time_msg_que), NULL);
//	if (p_ctl->short_time.msg_id == NULL){
//		p_ctl->error |= control_err_soft;
//	};
//	p_ctl->short_time.pool_id = osPoolCreate(osPool(control_sys_msg_que_pool)); // create memory pool
//	if (p_ctl->short_time.pool_id == NULL){
//		p_ctl->error |= control_err_soft;
//	};
//
//	p_ctl->short_time.event = 1;
//	p_ctl->short_time.thread_id = osThreadCreate (osThread(short_time_thread), &control);
//
////	p_control->cmd_ack_msg_que_id = osMessageCreate(osMessageQ(control_sys_msg_que), NULL);
////	if (p_control->cmd_ack_msg_que_id == NULL){
////		p_control->error |= control_err_soft;
////	};
////	p_control->cmd_ack_msg_pool_id = osPoolCreate(osPool(control_sys_msg_que_pool)); // create memory pool
////	if (p_control->cmd_ack_msg_pool_id == NULL){
////		p_control->error |= control_err_soft;
////	};
//}
//*/
//////////////////////////////////////////////////////////////////////////////////////////////////
////
//////////////////////////////////////////////////////////////////////////////////////////////////
//static void TIM_Config(void){
//  TIM_MasterConfigTypeDef master_timer_config;
//  RCC_ClkInitTypeDef clk_init_struct = {0};       /* Temporary variable to retrieve RCC clock configuration */
//  uint32_t latency;                               /* Temporary variable to retrieve Flash Latency */
//
//  uint32_t timer_clock_frequency = 0;             /* Timer clock frequency */
//  uint32_t timer_prescaler = 0;                   /* Time base prescaler to have timebase aligned on minimum frequency possible */
//
//
//  /* Configuration of timer as time base:                                     */
//  /* Caution: Computation of frequency is done for a timer instance on APB1   */
//  /*          (clocked by PCLK1)                                              */
//  /* Timer period can be adjusted by modifying the following constants:       */
//  /* - TIMER_FREQUENCY: timer frequency (unit: Hz).                           */
//  /* - TIMER_FREQUENCY_RANGE_MIN: timer minimum frequency (unit: Hz).         */
//
//  /* Retrieve timer clock source frequency */
//  HAL_RCC_GetClockConfig(&clk_init_struct, &latency);
//  /* If APB1 prescaler is different of 1, timers have a factor x2 on their    */
//  /* clock source.                                                            */
//  if (clk_init_struct.APB1CLKDivider == RCC_HCLK_DIV1)
//  {
//    timer_clock_frequency = HAL_RCC_GetPCLK1Freq();
//  }
//  else
//  {
//    timer_clock_frequency = HAL_RCC_GetPCLK1Freq() *2;
//  }
//
//  /* Timer prescaler calculation */
//  /* (computation for timer 16 bits, additional + 1 to round the prescaler up) */
//  timer_prescaler = (timer_clock_frequency / (TIMER_PRESCALER_MAX_VALUE * TIMER_FREQUENCY_RANGE_MIN)) +1;
//
//  /* Set timer instance */
//  TimHandle.Instance = TIM3;
//  HAL_TIM_Base_DeInit(&TimHandle);
//
//  /* Configure timer parameters */
//  TimHandle.Init.Period            = ((timer_clock_frequency / (timer_prescaler * TIMER_FREQUENCY)) - 1);
//  TimHandle.Init.Prescaler         = (timer_prescaler - 1);
//  TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
//  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
//  TimHandle.Init.RepetitionCounter = 0x0;
//
//  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
//  {
//    /* Timer initialization Error */
// //   printf("timer cfg error \r\n");
//  }
//
//  /* Timer TRGO selection */
//  master_timer_config.MasterOutputTrigger = TIM_TRGO_UPDATE;
//  master_timer_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//
//  if (HAL_TIMEx_MasterConfigSynchronization(&TimHandle, &master_timer_config) != HAL_OK){
//    /* Timer TRGO selection Error */
//	//    printf("timer cfg error \r\n");
//  }
//
//}
//////////////////////////////////////////////////////////////////////////////////////////////////
////
//////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef ADC_BASE_CFG
//static void ADC_Config(void){
//ADC_ChannelConfTypeDef   sConfig;
//
//  AdcHandle.Instance = ADC1;
//  AdcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
//  AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
//  AdcHandle.Init.ScanConvMode = ENABLE;
//  AdcHandle.Init.ContinuousConvMode = DISABLE;
//  AdcHandle.Init.DiscontinuousConvMode = DISABLE;
//  AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
//  AdcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;//ADC_EXTERNALTRIGCONV_T3_CC1;
//  AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//  AdcHandle.Init.NbrOfConversion = 3;
//  AdcHandle.Init.DMAContinuousRequests = ENABLE;//DISABLE;
//  AdcHandle.Init.EOCSelection = ADC_EOC_SEQ_CONV;//ADC_EOC_SINGLE_CONV;
//
//  if (HAL_ADC_Init(&AdcHandle) != HAL_OK){
//    printf("error adc \r\n");
//  }
//
//  sConfig.Channel      = ADC_CHANNEL_9;
//  sConfig.Rank         = 1;
//  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;//ADC_SAMPLETIME_15CYCLES
//
//  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK) {
//	  printf("error adc \r\n");
//  }
//
//  sConfig.Channel      = ADC_CHANNEL_3;
//  sConfig.Rank         = 2;
//  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
//
//  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)  {
//	  printf("error adc \r\n");
//  }
//
//  sConfig.Channel      = ADC_CHANNEL_0;
//  sConfig.Rank         = 3;
//  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
//
//  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)  {
//	  printf("error adc \r\n");
//  }
//}
//#endif
//
//#ifndef ADC_BASE_CFG
//static void ADC_Config(void){
//ADC_ChannelConfTypeDef   sConfig;
//
//	AdcHandle.Instance = ADC1;
//	AdcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
//	AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
//	AdcHandle.Init.ScanConvMode = ENABLE;
//	AdcHandle.Init.ContinuousConvMode = DISABLE;
//	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
//	AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
//	AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;//ADC_EXTERNALTRIGCONV_T3_CC1;
//	AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//	AdcHandle.Init.NbrOfConversion = 16;
//	AdcHandle.Init.DMAContinuousRequests = ENABLE;//DISABLE;
//	AdcHandle.Init.EOCSelection = ADC_EOC_SEQ_CONV;//ADC_EOC_SINGLE_CONV;
//
//	if (HAL_ADC_Init(&AdcHandle) != HAL_OK){
//
//	}
//	for(int i=0;i<16;i++){
//		sConfig.Channel      = ADC_CHANNEL_8;
//		sConfig.Rank         = i+1;
//		sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;//ADC_SAMPLETIME_15CYCLES
//
//		if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK) {
//		}
//	}
//#ifndef LORA
//	  AdcHandle2.Instance = ADC2;
//	  AdcHandle2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
//	  AdcHandle2.Init.Resolution = ADC_RESOLUTION_12B;
//	  AdcHandle2.Init.ScanConvMode = ENABLE;
//	  AdcHandle2.Init.ContinuousConvMode = DISABLE;
//	  AdcHandle2.Init.DiscontinuousConvMode = DISABLE;
//	  AdcHandle2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
//	  AdcHandle2.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;//ADC_EXTERNALTRIGCONV_T3_CC1;
//	  AdcHandle2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//	  AdcHandle2.Init.NbrOfConversion = 3;
//	  AdcHandle2.Init.DMAContinuousRequests = ENABLE;//DISABLE;
//	  AdcHandle2.Init.EOCSelection = ADC_EOC_SEQ_CONV;//ADC_EOC_SINGLE_CONV;
//
//	  if (HAL_ADC_Init(&AdcHandle2) != HAL_OK){
//
//	  }
//
//	  sConfig.Channel      = ADC_CHANNEL_9;
//	  sConfig.Rank         = 1;
//	  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;//ADC_SAMPLETIME_15CYCLES
//
//	  if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig) != HAL_OK) {
//
//	  }
//
//	  sConfig.Channel      = ADC_CHANNEL_3;
//	  sConfig.Rank         = 2;
//	  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
//
//	  if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig) != HAL_OK)  {
//
//	  }
//
//	  sConfig.Channel      = ADC_CHANNEL_0;
//	  sConfig.Rank         = 3;
//	  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
//
//	  if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig) != HAL_OK)  {
//
//	  }
//#endif
//
//}
//#endif
//
////
////  sConfig.Channel      = ADC_CHANNEL_8;
////  sConfig.Rank         = 2;
////  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;//ADC_SAMPLETIME_15CYCLES
////
////  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK) {
////	  printf("error adc \r\n");
////  }
//
////
////  sConfig.Channel      = ADC_CHANNEL_8;
////  sConfig.Rank         = 2;
////  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;//ADC_SAMPLETIME_15CYCLES
////
////  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK) {
////	  printf("error adc \r\n");
////  }
//
//
////  sConfig.Channel      = ADC_CHANNEL_3;
////  sConfig.Rank         = 2;
////  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
////
////  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)  {
////	  printf("error adc \r\n");
////  }
////
////  sConfig.Channel      = ADC_CHANNEL_0;
////  sConfig.Rank         = 3;
////  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
////
////  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)  {
////	  printf("error adc \r\n");
////  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void voltage_start_init(voltage_cntrl_struct *p_v, uint32_t ref, uint32_t max_code, float f_gain, float ref_gain){
//GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.Pin = p_v->pin;
//	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(p_v->port, &GPIO_InitStructure);
//
//	f_gain = f_gain * ref;
//	f_gain = f_gain / max_code;
//	p_v->gain_16_16 = float_to_fract16_16(f_gain);
//	p_v->ref_gain_16_16 = float_to_fract16_16(ref_gain);
//
//	p_v->p16_ext_adc = 0;
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t voltage_callback_meashure(voltage_cntrl_struct *p_v, uint16_t code){
////uint8_t cntr;
//uint32_t res=0;
//unsigned long long v;
//unsigned int input_v;
//uint16_t vol;
//voltage_cntrl_struct *p_ref;
//	if(!p_v->p16_ext_adc){
//		res = code;
//	}
//	else{
//		res = *p_v->p16_ext_adc;
//	}
//
//	res = res << 16;
//	v = (unsigned long long )res * (unsigned long long )p_v->gain_16_16;
//	p_v->voltage = v >> 32;
// 	if(p_v->p_ref){
//		p_ref = p_v->p_ref;
//		input_v = p_ref->voltage << 16;
//		v = (unsigned long long )input_v * (unsigned long long )p_v->ref_gain_16_16;
//		vol = (v >> 32);
//		p_v->voltage = vol - p_v->voltage;
//	}
//    return 0;
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint8_t* voltage_get_func_state(uint32_t lt32, uint32_t tmp1, uint32_t tmp2){
//voltage_cntrl_struct *p_lt;
//	p_lt = (voltage_cntrl_struct*) lt32;
//	if(p_lt->p16_ext_adc)
//		voltage_callback_meashure(p_lt,0);
//	return (uint8_t*)&p_lt->voltage;
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void voltage_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, voltage_cntrl_struct *p_lt, const char *p_name){
//SettingCell_t 	cell;
//uint32_t 		i;
//	for(i=0;i<COMPONENT_MAX_TABLE_SIZE;i++){
//		if(!p_cell[i].CellNumber) break;
//	}
//	if( i>= COMPONENT_MAX_TABLE_SIZE) return;
//
//	cell.CellAttr = Action_Att + RD_Att;
//	cell.CellType = FixIntegerCellType + 2;
//	cell.CellNumber = num;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)voltage_get_func_state;
//	cell.WriteProc = 0;
//	cell.LowLim = 0;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_lt;
//	cell.DescriptStr = p_name;
//
//	p_cell[i] = cell;
//}
//////////////////////////////////////////////////////////////////////////////////////////////////
////
//////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef ADC_BASE_CFG
//void adc_config_start(void){
//
//	control.vol_24v_in.pin = VID3_24V_IN_PIN;
//	control.vol_24v_in.port = VID3_24V_IN_PORT;
//	voltage_start_init(&control.vol_24v_in,3150,4096,VID3_24V_IN_DIV,0);
//	control.vol_24v_in.p16_ext_adc = &control.adc_ring_buf[0];
//
//	control.vol_24v_out.pin = VID3_24V_OUT_PIN;
//	control.vol_24v_out.port = VID3_24V_OUT_PORT;
//	voltage_start_init(&control.vol_24v_out,3150,4096,VID3_24V_OUT_DIV,0);
//	control.vol_24v_out.p16_ext_adc = &control.adc_ring_buf[1];
//
//	control.cur_24v.pin = VID3_24_CUR_PIN;
//	control.cur_24v.port = VID3_24_CUR_PORT;
//	voltage_start_init(&control.cur_24v,3150,4096,VID3_24_CUR_DIV,0);
//	control.cur_24v.p16_ext_adc = &control.adc_ring_buf[2];
//
//
//	ADC_Config();
//	TIM_Config();
//	if (HAL_TIM_Base_Start(&TimHandle) != HAL_OK){
//		printf("timer error \r\n");
//	}
//	if (HAL_ADC_Start_DMA_No_DMA_It(&AdcHandle,
//					(uint32_t *)&control.adc_ring_buf[0],
//					3) != HAL_OK){
//	}
//}
//#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
////
//////////////////////////////////////////////////////////////////////////////////////////////////
//#ifndef ADC_BASE_CFG
//void adc_config_start(void){
//
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.Pin = GPIO_PIN_0;
//	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(GPIOB 	, &GPIO_InitStructure);
//
//#ifndef LORA
//	control.vol_24v_in.pin = VID3_24V_IN_PIN;
//	control.vol_24v_in.port = VID3_24V_IN_PORT;
//	voltage_start_init(&control.vol_24v_in,3150,4096,VID3_24V_IN_DIV,0);
//	control.vol_24v_in.p16_ext_adc = &control.adc_ring_buf_2[0];
//
//	control.vol_24v_out.pin = VID3_24V_OUT_PIN;
//	control.vol_24v_out.port = VID3_24V_OUT_PORT;
//	voltage_start_init(&control.vol_24v_out,3150,4096,VID3_24V_OUT_DIV,0);
//	control.vol_24v_out.p16_ext_adc = &control.adc_ring_buf_2[1];
//
//	control.cur_24v.pin = VID3_24_CUR_PIN;
//	control.cur_24v.port = VID3_24_CUR_PORT;
//	voltage_start_init(&control.cur_24v,3150,4096,VID3_24_CUR_DIV,0);
//	control.cur_24v.p16_ext_adc = &control.adc_ring_buf_2[2];
//#endif
//
//	ADC_Config();
//
////	memset(&control.adc_ring_buf[0],0,sizeof(control.adc_ring_buf));
//
//
//	ADC_Config();
//	TIM_Config();
//	if (HAL_TIM_Base_Start(&TimHandle) != HAL_OK){
//		printf("timer error \r\n");
//	}
//#ifndef LORA
//	if (HAL_ADC_Start_DMA_No_DMA_It(&AdcHandle2,
//					(uint32_t *)&control.adc_ring_buf_2[0],
//					3) != HAL_OK){
//	}
//#endif
//
////	osDelay(1000);
////volatile uint16_t av;
////	av = recv_5_8_get_rssi(&control.adc_ring_buf[0],16);
////	printf("av=%d\r\n",av);
////
////	if (HAL_ADC_Start_DMA_No_DMA_It(&AdcHandle,
////					(uint32_t *)&control.adc_ring_buf[0],
////					16) != HAL_OK){
////	}
////	osDelay(1000);
////	av = recv_5_8_get_rssi(&control.adc_ring_buf[0],16);
////	printf("av=%d\r\n",av);
////
////	if (HAL_ADC_Start_DMA_No_DMA_It(&AdcHandle,
////					(uint32_t *)&control.adc_ring_buf[0],
////					16) != HAL_OK){
////	}
////	osDelay(1000);
////	av = recv_5_8_get_rssi(&control.adc_ring_buf[0],16);
////	printf("av=%d\r\n",av);
//
//}
//#endif
//
//void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim){
//	 if(htim->Instance==TIM3){
//		  __HAL_RCC_TIM3_CLK_ENABLE();
//	 }
//	  if(htim->Instance==TIM1){
//	    __HAL_RCC_TIM1_CLK_ENABLE();
//	  }
//	  if(htim->Instance==TIM4){
//	    __HAL_RCC_TIM4_CLK_ENABLE();
//
//
///*
// * Убрал ???? зачем и что это было???
//	    hdma_tim4_up.Instance = DMA1_Stream6;
//	    hdma_tim4_up.Init.Channel = DMA_CHANNEL_2;
//	    hdma_tim4_up.Init.Direction = DMA_MEMORY_TO_PERIPH;
//	    hdma_tim4_up.Init.PeriphInc = DMA_PINC_DISABLE;
//	    hdma_tim4_up.Init.MemInc = DMA_MINC_ENABLE;
//	    hdma_tim4_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
//	    hdma_tim4_up.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
//	    hdma_tim4_up.Init.Mode = DMA_CIRCULAR;
//	    hdma_tim4_up.Init.Priority = DMA_PRIORITY_LOW;
//	    hdma_tim4_up.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
//	    hdma_tim4_up.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
//	    hdma_tim4_up.Init.MemBurst = DMA_MBURST_SINGLE;
//	    hdma_tim4_up.Init.PeriphBurst = DMA_PBURST_SINGLE;
//	    if (HAL_DMA_Init(&hdma_tim4_up) != HAL_OK){
//	      Error_Handler();
//	    }
//
//	    __HAL_LINKDMA(htim,hdma[TIM_DMA_ID_UPDATE],hdma_tim4_up);
//	    HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
//	    HAL_NVIC_EnableIRQ(TIM4_IRQn);
//*/
//	  }
//}
//void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim){
//	  if(htim->Instance==TIM3){
//		__HAL_RCC_TIM3_FORCE_RESET();
//		__HAL_RCC_TIM3_RELEASE_RESET();
//	  }
//	  if(htim->Instance==TIM4){
//	    __HAL_RCC_TIM4_CLK_DISABLE();
//	    HAL_NVIC_DisableIRQ(TIM4_IRQn);
//	  }
//	  if(htim->Instance==TIM1){
//	    __HAL_RCC_TIM1_CLK_DISABLE();
//	  }
//
//}
//
// void MX_TIM4_Init(uint32_t period, uint32_t imp){
//
//  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
//  TIM_MasterConfigTypeDef sMasterConfig = {0};
//  TIM_OC_InitTypeDef sConfigOC = {0};
//
//  htim4.Instance = TIM4;
//  htim4.Init.Prescaler = 0;
//  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim4.Init.Period = period;
//  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
//  if (HAL_TIM_Base_Init(&htim4) != HAL_OK){
//    Error_Handler();
//  }
//  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK){
//    Error_Handler();
//  }
//  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK){
//    Error_Handler();
//  }
//  sConfigOC.OCMode = TIM_OCMODE_PWM1;
//  sConfigOC.Pulse = imp;
//  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK){
//    Error_Handler();
//  }
//
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//  GPIO_InitStruct.Pin = GPIO_PIN_15;
//  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//  GPIO_InitStruct.Pull = GPIO_PULLUP;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
//  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
//
//}
//#ifdef	MULTI_EMIT
// void MX_TIM1_Init(uint32_t period, uint32_t imp){
//
//   TIM_ClockConfigTypeDef sClockSourceConfig = {0};
//   TIM_MasterConfigTypeDef sMasterConfig = {0};
//   TIM_OC_InitTypeDef sConfigOC = {0};
//
//   htim1.Instance = TIM1;
//   htim1.Init.Prescaler = 0;
//   htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
//   htim1.Init.Period = period;
//   htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//   htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
//   if (HAL_TIM_Base_Init(&htim1) != HAL_OK){
//     Error_Handler();
//   }
//   sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//   if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK){
//     Error_Handler();
//   }
//   if (HAL_TIM_PWM_Init(&htim1) != HAL_OK){
//     Error_Handler();
//   }
//   sConfigOC.OCMode = TIM_OCMODE_PWM1;
//   sConfigOC.Pulse = imp;
//   if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK){
//     Error_Handler();
//   }
//
//   GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//   GPIO_InitStruct.Pin = GPIO_PIN_14;
//   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//   GPIO_InitStruct.Pull = GPIO_PULLUP;
//   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//   GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
//   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
// }
//#endif
//
//#ifndef ADC_BASE_CFG
//void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc){
//
//	if(hadc->Instance == ADC1){
//	  __HAL_RCC_ADC1_CLK_ENABLE();
//	  hdma_adc1.Instance = DMA2_Stream0;
//	  hdma_adc1.Init.Channel = DMA_CHANNEL_0;
//	  hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
//	  hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
//	  hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
//	  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
//	  hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
//	  hdma_adc1.Init.Mode = DMA_CIRCULAR;
//	  hdma_adc1.Init.Priority = DMA_PRIORITY_MEDIUM;
//	  hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//	  if (HAL_DMA_Init(&hdma_adc1) != HAL_OK){
//		Error_Handler();
//	  }
//	  __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);
//	}
//	if(hadc->Instance == ADC2){
//
//		  __HAL_RCC_ADC2_CLK_ENABLE();
//
//		  hdma_adc2.Instance = DMA2_Stream3;
//		  hdma_adc2.Init.Channel = DMA_CHANNEL_1;
//		  hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
//		  hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
//		  hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
//		  hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
//		  hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
//		  hdma_adc2.Init.Mode = DMA_CIRCULAR;
//		  hdma_adc2.Init.Priority = DMA_PRIORITY_MEDIUM;
//		  hdma_adc2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//		  if (HAL_DMA_Init(&hdma_adc2) != HAL_OK){
//		    Error_Handler();
//		  }
//		  __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc2);
//	}
//
//
//}
//#endif
//#ifdef ADC_BASE_CFG
//void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc){
//  static DMA_HandleTypeDef  hdma_adc1;
//  RCC_PeriphCLKInitTypeDef  PeriphClkInit;
//  /*##-1- Enable peripherals and GPIO Clocks #################################*/
//  /* Enable clock of GPIO associated to the peripheral channels */
//  //ADCx_CHANNELa_GPIO_CLK_ENABLE();
//
//  /* Enable clock of ADCx peripheral */
//  __HAL_RCC_ADC1_CLK_ENABLE();
//
//
//
//  hdma_adc1.Instance = DMA2_Stream0;
//  hdma_adc1.Init.Channel = DMA_CHANNEL_0;
//  hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
//  hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
//  hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
//  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
//  hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
//  hdma_adc1.Init.Mode = DMA_CIRCULAR;
//  hdma_adc1.Init.Priority = DMA_PRIORITY_MEDIUM;
//  hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//  if (HAL_DMA_Init(&hdma_adc1) != HAL_OK){
//    Error_Handler();
//  }
//
//  __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);
//}
//#endif

#ifdef TVP_SLAVE_HAVE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t  cntr_tvp_slave_start_init(control_struct *p_ctl, uint8_t table_pos){
uint32_t pos;
	p_ctl->tvp_5150_slave.cmd_evt_id = NULL;//p_ctl->thread_id;
	p_ctl->tvp_5150_slave.cntrl_int_event = cntrl_thread_event_tvp_slave;

	p_ctl->tvp_5150_slave.reset_pin.gpio = GPIOD;
	p_ctl->tvp_5150_slave.reset_pin.pin = (15);
	p_ctl->tvp_5150_slave.reset_pin.mode = GPIO_MODE_OUTPUT;
	p_ctl->tvp_5150_slave.reset_pin.drv = GPIO_DRV_3;
	p_ctl->tvp_5150_slave.reset_pin.pupd = GPIO_PUPD_UP;

	p_ctl->tvp_5150_slave.int_pin.gpio = GPIOD;
	p_ctl->tvp_5150_slave.int_pin.pin = (16);
	p_ctl->tvp_5150_slave.int_pin.mode = GPIO_MODE_INPUT;
	p_ctl->tvp_5150_slave.int_pin.drv = GPIO_DRV_3;
	p_ctl->tvp_5150_slave.int_pin.pupd = GPIO_PUPD_UP;

//	p_ctl->tvp_5150_slave.int_port = VIDEO_CNTRL_TVP_IRQ_PORT;
//	p_ctl->tvp_5150_slave.int_pin = VIDEO_CNTRL_TVP_IRQ_PIN;
//
//	p_ctl->tvp_5150_slave.cntrl_thread = p_ctl->main_thread_id;
//	p_ctl->tvp_5150_slave.cntrl_int_event = cntrl_thread_event_tvp_slave;
//
//	p_ctl->tvp_5150_slave.irq_type = EXTI15_10_IRQn;
//
//	p_ctl->tvp_5150_slave.int_port = VID3_INTREQ_1_PORT;
//	p_ctl->tvp_5150_slave.int_pin = VID3_INTREQ_1_PIN;
//
//	p_ctl->tvp_5150_slave.reset_port = VID3_RESET_B1_PORT;
//	p_ctl->tvp_5150_slave.reset_pin = VID3_RESET_B1_PIN;
//
//	p_ctl->tvp_5150_slave.pdn_port = VID3_PDN_1_PORT;
//	p_ctl->tvp_5150_slave.pdn_pin = VID3_PDN_1_PIN;

	p_ctl->tvp_5150_slave.i2c_addr = TVP_I2C_ADDRESS_HI;
	p_ctl->tvp_5150_slave.p_hi2c = p_ctl->p_hi2c_tvp;

	tvp_start_reset(&p_ctl->tvp_5150_slave);

//	pos = tvp_add_to_cell_table(&p_ctl->comp_table[0],table_pos,&p_ctl->tvp_5150_slave,TVP5150_NAME_REG_SLAVE);
	pos = tvp_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->tvp_5150_slave,TVP5150_NAME_REG_SLAVE);
	return pos;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t  cntr_tvp_slave_init(control_struct *p_ctl){
uint8_t back;
	TVP_Write(&p_ctl->tvp_5150_slave,0x02,0x30);
	TVP_Write(&p_ctl->tvp_5150_slave,0x03,0x20);//onli int req

	TVP_Init(&p_ctl->tvp_5150_slave);

	back = TVP_Read(&p_ctl->tvp_5150_slave, 0x03);
	if(back != 0x20){
		p_ctl->error |= 1;
	}

	return 0;
}
#endif
#ifdef TVP_MASTER_HAVE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cntr_tvp_master_init_dbg(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	control_struct *p_ctl;
	p_ctl = (control_struct *)ad32;
	cntr_tvp_master_init(p_ctl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t  cntr_tvp_master_start_init(control_struct *p_ctl, uint8_t table_pos){
uint32_t pos;

	p_ctl->tvp_5150_master.cmd_evt_id = NULL;//p_ctl->main_thread_id;
	p_ctl->tvp_5150_master.cntrl_int_event = cntrl_thread_event_tvp_master;

//	p_ctl->tvp_5150_master.irq_type = EXTI4_IRQn;


	p_ctl->tvp_5150_master.reset_pin.gpio = GPIOD;
	p_ctl->tvp_5150_master.reset_pin.pin = (7);
	p_ctl->tvp_5150_master.reset_pin.mode = GPIO_MODE_OUTPUT;
	p_ctl->tvp_5150_master.reset_pin.drv = GPIO_DRV_3;
	p_ctl->tvp_5150_master.reset_pin.pupd = GPIO_PUPD_UP;

	p_ctl->tvp_5150_master.int_pin.gpio = GPIOE;
	p_ctl->tvp_5150_master.int_pin.pin = (3);
	p_ctl->tvp_5150_master.int_pin.mode = GPIO_MODE_INPUT;
	p_ctl->tvp_5150_master.int_pin.drv = GPIO_DRV_3;
	p_ctl->tvp_5150_master.int_pin.pupd = GPIO_PUPD_UP;

	p_ctl->tvp_5150_master.i2c_addr = TVP_I2C_ADDRESS_LO;
	p_ctl->tvp_5150_master.p_hi2c = p_ctl->p_hi2c_tvp;

	tvp_start_reset(&p_ctl->tvp_5150_master);

	pos = tvp_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->tvp_5150_master,TVP5150_NAME_REG);

	cmd_h_add_to_cell_table_ext_size(
							p_ctl->p_comp_table,
							pos++,
							p_ctl,
							NULL,
							(void*)&cntr_tvp_master_init_dbg,
							WR_Att + Default_Attr + Action_Att,
							FixIntegerCellType + 1,
							"Master init");

	return pos;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t  cntr_tvp_master_init(control_struct *p_ctl){
uint8_t back;
	TVP_Write(&p_ctl->tvp_5150_master,0x02,0x30);
	TVP_Write(&p_ctl->tvp_5150_master,0x03,0x0D | 0x20 );

	TVP_Init(&p_ctl->tvp_5150_master);

//	TVP_Write(p_ctl, 0x1D, 0x10); //interrupt field change en!!!!!

	back = TVP_Read(&p_ctl->tvp_5150_master, 0x03);
	if(back != 0x2D){
		p_ctl->error |= cntrl_error_tvp_master;
	}
	return 0;
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void csi_scaner_internal_clk_init(void) {
uint32_t reg_val = 0;

struct gpio_t_ gpio_pin;

	gpio_pin.gpio = GPIOE;
	gpio_pin.mode = GPIO_MODE_FNC2;
	gpio_pin.drv = GPIO_DRV_3;
	gpio_pin.pupd = GPIO_PUPD_UP;

	gpio_pin.pin = 0;//HSYNC
	gpio_init_pin_sun(&gpio_pin);

	gpio_pin.pin = 1;//VSYNC
	gpio_init_pin_sun(&gpio_pin);

	gpio_pin.pin = 2;//PCLK
	gpio_init_pin_sun(&gpio_pin);

	gpio_pin.pin = 3;//MCLK
	gpio_init_pin_sun(&gpio_pin);

	gpio_pin.pin = 4;//D0
	gpio_init_pin_sun(&gpio_pin);

	gpio_pin.pin = 5;//D1
	gpio_init_pin_sun(&gpio_pin);

	gpio_pin.pin = 6;//D2
	gpio_init_pin_sun(&gpio_pin);

	gpio_pin.pin = 7;//D3
	gpio_init_pin_sun(&gpio_pin);

	gpio_pin.pin = 8;//D4
	gpio_init_pin_sun(&gpio_pin);

	gpio_pin.pin = 9;//D5
	gpio_init_pin_sun(&gpio_pin);

	gpio_pin.pin = 10;//D6
	gpio_init_pin_sun(&gpio_pin);

	gpio_pin.pin = 11;//D7
	gpio_init_pin_sun(&gpio_pin);

	gpio_pin.pin = 12;//FID
	gpio_pin.mode = GPIO_MODE_FNC3;//!!!!!
	gpio_init_pin_sun(&gpio_pin);

	udelay(100);

	writel(0x80000000, 0x05800000 + 0);//en clk
	udelay(100);
	writel(0x01, 0x05800000 + 0x04);//en clk parser 0
	udelay(100);
	writel(0x00010001, 0x05800000 + 0x0C);//включить Post0 и Bank 0
	udelay(100);

	writel(0, 0x05801000 + 0x00);//parser 0 en
	udelay(100);
	writel(0, 0x05801000 + 0x0C);// ch0 en
	udelay(100);
	writel(0x70000000, 0x05809000 + 0x00);//

	writel(0, 0x05809000 + 0x04);//
	udelay(100);
	writel(0, 0x05800800 + 0x00);//csi top en
	udelay(100);

//	writel(0x01, 0x05800800 + 0x00);//csi top en


	writel(0x02D00000, 0x05801000 + 0x28);// обрезка кадра - 720 на 0
	writel(0x02400016, 0x05801000 + 0x2C);//


	reg_val = 0;

	reg_val |= 0x08<<24;//delay frame test
	reg_val |= 1<<20;//interlace
//		reg_val |= 1<<13;//ddr
		reg_val |= 0x01<<14;//f sync
//		reg_val |= 1<<19;//F
//		reg_val |= 1<<18;
//		reg_val |= 1<<17;

	reg_val |= 2<<6;
	reg_val |= 1<<19;

//		Биты 23:20 = 0x1 (Interlaced)
//		Бит 19 = 0x0 (Field polarity)
//		Бит 18 = 0x1 (VRef Positive)
//		Бит 17 = 0x1 (HRef Positive)
//		Бит 16 = 0x0 (PCLK Rising)
//		Биты 7:6 = 0x2 (UYVY)
//		Биты 4:0 = 0x0 (YUV Separate Sync)

	writel(reg_val, 0x05801000 + 0x04);//

	udelay(100);
	udelay(100);
	udelay(100);


//		reg_val = 0;
//
//		reg_val |= 0x08<<24;//delay frame test
//		reg_val |= 1<<20;//interlace
////		reg_val |= 1<<13;//ddr
////		reg_val |= 0x02<<14;//v sync
////		reg_val |= 1<<19;
////		reg_val |= 1<<18;
////		reg_val |= 1<<17;
//
//		reg_val |= 2<<6;
//
////		Биты 23:20 = 0x1 (Interlaced)
////		Бит 19 = 0x0 (Field polarity)
////		Бит 18 = 0x1 (VRef Positive)
////		Бит 17 = 0x1 (HRef Positive)
////		Бит 16 = 0x0 (PCLK Rising)
////		Биты 7:6 = 0x2 (UYVY)
////		Биты 4:0 = 0x0 (YUV Separate Sync)
//
//		writel(reg_val, 0x05801000 + 0x04);//
//
//		udelay(100);

//		0x0024 Parser Channel_0 Input Format Register (Default Value:0x0000_0003) default yuv422


//		writel(1, 0x05801000 + 0x3C);//interlace

	writel(0x02400000, 0x05809000 + 0x14);//вертикальная обрезка
	writel(0x02D00000, 0x05809000 + 0x10);// горизонтальная обрезка кадра - 720 на 0

	reg_val = 0;
	reg_val |= 2<<10;
	reg_val |= 7<<16;//0111: frame planar YCbCr 422 UV combined (UV sequence)
	writel(reg_val, 0x05809000 + 0x04);//здесь оба типа полей

	udelay(100);

		writel(0x02, 0x05809000 + 0x0054);// irq clear
		IRQ_SetHandler(CSI_DMA0_IRQn, CSI_DMA_IRQHandler);
		IRQ_Enable(CSI_DMA0_IRQn);
		writel(0x02, 0x05809000 + 0x0050);// irq enable

	writel(0x02D002D0, 0x05809000 + 0x0038);// buflen
//		writel(0x70000017, 0x05809000 + 0x00);// start
	writel(0x70000087, 0x05809000 + 0x00);// start



//	asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r" (0xFFFFFFFF));

//		memset((void*)0x40000000,0,64);

//		memset(video_luma_buf,0,sizeof(video_luma_buf));
//		memset(video_chroma_buf,0,sizeof(video_chroma_buf));


uint32_t tmp;
		tmp = (uint32_t)video_luma_buf_csi;
		tmp &= 0x0FFFFFFF;
		tmp = tmp >> 2;

		udelay(100);
		writel(tmp, 0x05809000 + 0x20);//data addr luma
		udelay(100);
		tmp = (uint32_t)video_chroma_buf_csi;
		tmp &= 0x0FFFFFFF;
		tmp = tmp >> 2;

		writel(tmp, 0x05809000 + 0x28);//data addr hroma
		udelay(100);
		udelay(100);

		writel(0x70000017, 0x05809000 + 0x00);// start

		reg_val = 0;
		reg_val |= 0x10000;// (NCSIC_EN)
		reg_val |= 0x08000;// (PCLK_EN)
	//	reg_val |= 0x00002;// (PRS_MODE = MCSI)
		reg_val |= 0x00001;// (PRS_EN)

		udelay(100);
		writel(0x2, 0x05801000 + 0x0C);// ch0 en
		udelay(100);
		writel(reg_val, 0x05801000 + 0x00);//parser 0 en
		udelay(100);
		writel(0x01, 0x05800800 + 0x00);//csi top en
//		writel(0x70000017, 0x05809000 + 0x00);// start

		udelay(100);

//		0x70000031
//		0x70000000 (Default Software modes)
//		0x00000020 (Frame Count En)
//		0x00000010 (DMA En)
//		0x00000001 (BK Top En)


}
void csi_scaner_clk_init(void) {
uint32_t reg_val = 0;
	reg_val |= (1U << 31);    // TVD_CLK_GATING = ON
	reg_val |= (1 << 24);     // CLK_SRC_SEL = PLL_VIDEO0
	reg_val |= (1 << 0);      // FACTOR_M = 1 (делитель на 2)


	CCU->CSI_CLK_REG = reg_val;
	udelay(100);

	CCU->CSI_BGR_REG |= (1 << 0) | (1 << 16);
	udelay(100);

	reg_val = (1U << 31);    // TVD_CLK_GATING = ON 24 Mhz!!!!!!!
	CCU->CSI_MASTER_CLK_REG = reg_val;
	udelay(100);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t  cntrl_scaner_init(control_struct *p_ctl){
uint16_t pos = p_ctl->comp_table_pos;
	csi_scaner_clk_init();
	csi_scaner_internal_clk_init();

	p_ctl->comp_table_pos = pos;
	return 0;
}
#ifdef MAIN_CNTRL
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t  cntrl_main_cmd_init(control_struct *p_ctl, uint8_t table_pos){

	main_cntrl_init_que(p_ctl);

	p_ctl->auto_mode = cntrl_auto_mode_dis;
	p_ctl->alarm_mode = cntrl_alarm_dis;
//	control.b_skip_but = 0;
//	osThreadCreate (osThread(main_control_thread), &control);
//	osDelay(5001);
//
//	osDelay(20000);

	p_ctl->ch_time = 0;
	p_ctl->ch_max_time = control.dev_config.rec.ch_scan_time;
	p_ctl->ch_max_time = control.ch_max_time * 1000;
	p_ctl->freq_mode = cntrl_freq_mode_manual;
	p_ctl->auto_mode = cntrl_auto_mode_dis;
//	control.ch_ack_time = 0;
	p_ctl->recv_mode = cntr_recv_mode_1_2;
	p_ctl->b_udp_send = 0;
	p_ctl->b_http_file_open = 0;
//	p_ctl->b_collect = 0;
	p_ctl->b_wait_udp_send = 0;

	p_ctl->list_param.height = 100;
	p_ctl->list_param.width = 100;
	p_ctl->list_param.top_x = 1;
	p_ctl->list_param.top_y = 1;
	p_ctl->list_param.flag = 1;

	for(int i=0;i<MAX_NUM_FUNC_TABLE;i++){
		memset(&p_ctl->table_list[i],0,sizeof(p_ctl->table_list[i]));
		p_ctl->table_list[i].p_param = &p_ctl->dev_config.rec.list_param[i];
		if (!i) p_ctl->table_list[i].p_cell_param = (cmd_cell_param_s*) &main_cell_param;
		else p_ctl->table_list[i].p_cell_param = (cmd_cell_param_s*) &table_cell_param;
	}

uint8_t list_pos=0;

	main_cntrl_make_html_cntrl_table(p_ctl,&p_ctl->table_list[list_pos++]);
#ifdef RECV_1_2
	recv_1_2_make_html_cntrl_table(&p_ctl->recv_1_2,&p_ctl->table_list[list_pos++]);
#endif
#ifdef	RTC6705
	rtc6705_make_html_cntrl_table(&p_ctl->rtc6705,&p_ctl->table_list[list_pos++]);
#endif

#ifdef RTC6715
	recv_5_8_make_html_cntrl_table(&p_ctl->rtc6715,&p_ctl->table_list[list_pos++]);
#endif
#ifdef RECV_2_4
	recv_1_2_make_html_cntrl_table(&p_ctl->recv_2_4,&p_ctl->table_list[list_pos++]);
#endif

#ifdef RECV_3_4_I2C
	i2c_recv_make_html_cntrl_table(&p_ctl->i2c_recv_3_4,&p_ctl->table_list[list_pos++],"cfg_3_3");
#endif

#ifdef RECV_4_5_I2C
	i2c_recv_make_html_cntrl_table(&p_ctl->i2c_recv_4_5,&p_ctl->table_list[list_pos++],"cfg_4_2");
#endif
#ifdef MULTI_EMIT
	main_cntrl_make_html_emit_cfg_table(p_ctl,&p_ctl->table_list[list_pos++]);
#endif

	povorot_cmd_make_html_cntrl_table(&p_ctl->povorot,&p_ctl->table_list[list_pos++]);
#ifdef LORA
	lora_cmd_make_html_table(&p_ctl->lora,&p_ctl->table_list[list_pos++]);
#endif

	p_ctl->main_ctrl_thread_ev = osEventFlagsNew(NULL);
    osThreadNew(app_cmd,p_ctl,NULL);    // Create application main thread

	return table_pos;
}
#endif

#ifdef POVOROT
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t  cntrl_povorot_init(control_struct *p_ctl, uint8_t table_pos){
	p_ctl->povorot.rs485_pin.gpio = GPIOG;
	p_ctl->povorot.rs485_pin.pin = (10);
	p_ctl->povorot.rs485_pin.mode = GPIO_MODE_OUTPUT;
	p_ctl->povorot.rs485_pin.drv = GPIO_DRV_3;
	p_ctl->povorot.rs485_pin.pupd = GPIO_PUPD_UP;


//	p_ctl->povorot.rs485_pin.gpio = GPIOD;
//	p_ctl->povorot.rs485_pin.pin = (13);
//	p_ctl->povorot.rs485_pin.mode = GPIO_MODE_FNC5;
//	p_ctl->povorot.rs485_pin.drv = GPIO_DRV_3;
//	p_ctl->povorot.rs485_pin.pupd = GPIO_PUPD_UP;


//	GPIO_MODE_FNC5

	povorot_start_init(&p_ctl->povorot, &Driver_USART3, 0, p_ctl->conf.event_flag);
#ifdef	POVOROT_DEBUG
	return povorot_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->povorot,NULL);
#else
	return table_pos;
#endif
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RECV_1_2
uint32_t  cntr_recv_1_2_init(control_struct *p_ctl, uint8_t table_pos){

	p_ctl->recv_1_2.sw_pwr_5v.ON_pin.gpio = GPIOD;
	p_ctl->recv_1_2.sw_pwr_5v.ON_pin.pin = (9);
	p_ctl->recv_1_2.sw_pwr_5v.ON_pin.mode = GPIO_MODE_OUTPUT;
	p_ctl->recv_1_2.sw_pwr_5v.ON_pin.drv = GPIO_DRV_3;
	p_ctl->recv_1_2.sw_pwr_5v.ON_pin.pupd = GPIO_PUPD_DOWN;
	switch_fdc_start_pin_init(&p_ctl->recv_1_2.sw_pwr_5v);

	switch_fdc_on(&p_ctl->recv_1_2.sw_pwr_5v,fdc_on);
	switch_fdc_set_data(&p_ctl->recv_1_2.sw_pwr_5v);

//	p_ctl->recv_1_2.sw_pwr_8v.ON_pin.gpio = GPIOE;
//	p_ctl->recv_1_2.sw_pwr_8v.ON_pin.pin = (13);
//	p_ctl->recv_1_2.sw_pwr_8v.ON_pin.mode = GPIO_MODE_OUTPUT;
//	p_ctl->recv_1_2.sw_pwr_8v.ON_pin.drv = GPIO_DRV_3;
//	p_ctl->recv_1_2.sw_pwr_8v.ON_pin.pupd = GPIO_PUPD_DOWN;
//	switch_fdc_start_pin_init(&p_ctl->recv_1_2.sw_pwr_8v);
//
//	switch_fdc_on(&p_ctl->recv_1_2.sw_pwr_8v,fdc_on);
//	switch_fdc_set_data(&p_ctl->recv_1_2.sw_pwr_8v);


	p_ctl->recv_1_2.sw_da6_in.ON_pin.gpio = GPIOD;
	p_ctl->recv_1_2.sw_da6_in.ON_pin.pin = (8);
	p_ctl->recv_1_2.sw_da6_in.ON_pin.mode = GPIO_MODE_OUTPUT;
	p_ctl->recv_1_2.sw_da6_in.ON_pin.drv = GPIO_DRV_3;
	p_ctl->recv_1_2.sw_da6_in.ON_pin.pupd = GPIO_PUPD_DOWN;
	switch_fdc_start_pin_init(&p_ctl->recv_1_2.sw_da6_in);

	switch_fdc_on(&p_ctl->recv_1_2.sw_da6_in,fdc_on);
	switch_fdc_set_data(&p_ctl->recv_1_2.sw_da6_in);

	p_ctl->recv_1_2.p_cfg = &p_ctl->recv_1_2.cfg;

	p_ctl->recv_1_2.ver = recv_1_2_ver_1_2;
	p_ctl->recv_1_2.i2c_addr = 0x63;
	p_ctl->recv_1_2.p_hi2c = p_ctl->p_hi2c_1_2;

	recv_1_2_start_init(&p_ctl->recv_1_2);

	return recv_1_2_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->recv_1_2,NULL);
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t  cntr_adc_init(control_struct *p_ctl, uint8_t table_pos){

	p_ctl->sw_adc_in.ON_pin.gpio = GPIOB;
	p_ctl->sw_adc_in.ON_pin.pin = (6);
	p_ctl->sw_adc_in.ON_pin.mode = GPIO_MODE_OUTPUT;
	p_ctl->sw_adc_in.ON_pin.drv = GPIO_DRV_3;
	p_ctl->sw_adc_in.ON_pin.pupd = GPIO_PUPD_DOWN;
	switch_fdc_start_pin_init(&p_ctl->sw_adc_in);

	switch_fdc_on(&p_ctl->sw_adc_in,fdc_on);
	switch_fdc_set_data(&p_ctl->sw_adc_in);
	return adc_add_to_cell_table(p_ctl->p_comp_table,table_pos,p_ctl,NULL);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RTC6715
uint32_t  cntr_recv_5_8_init(control_struct *p_ctl, uint8_t table_pos){

	p_ctl->rtc6715.cs_pin.gpio = GPIOD;
	p_ctl->rtc6715.cs_pin.pin = (10);
	p_ctl->rtc6715.cs_pin.mode = GPIO_MODE_OUTPUT;
	p_ctl->rtc6715.cs_pin.drv = GPIO_DRV_3;
	p_ctl->rtc6715.cs_pin.pupd = GPIO_PUPD_UP;

	p_ctl->rtc6715.mosi_pin.gpio = GPIOD;
	p_ctl->rtc6715.mosi_pin.pin = (12);
	p_ctl->rtc6715.mosi_pin.mode = GPIO_MODE_OUTPUT;
	p_ctl->rtc6715.mosi_pin.drv = GPIO_DRV_3;
	p_ctl->rtc6715.mosi_pin.pupd = GPIO_PUPD_UP;

	p_ctl->rtc6715.clk_pin.gpio = GPIOD;
	p_ctl->rtc6715.clk_pin.pin = (11);
	p_ctl->rtc6715.clk_pin.mode = GPIO_MODE_OUTPUT;
	p_ctl->rtc6715.clk_pin.drv = GPIO_DRV_3;
	p_ctl->rtc6715.clk_pin.pupd = GPIO_PUPD_UP;

	p_ctl->rtc6715.sw_pwr_5v.ON_pin.gpio = GPIOE;
	p_ctl->rtc6715.sw_pwr_5v.ON_pin.pin = (13);
	p_ctl->rtc6715.sw_pwr_5v.ON_pin.mode = GPIO_MODE_OUTPUT;
	p_ctl->rtc6715.sw_pwr_5v.ON_pin.drv = GPIO_DRV_3;
	p_ctl->rtc6715.sw_pwr_5v.ON_pin.pupd = GPIO_PUPD_DOWN;

	switch_fdc_start_pin_init(&p_ctl->rtc6715.sw_pwr_5v);

	switch_fdc_on(&p_ctl->rtc6715.sw_pwr_5v,fdc_on);
	switch_fdc_set_data(&p_ctl->rtc6715.sw_pwr_5v);

	p_ctl->rtc6715.sw_da1_in.ON_pin.gpio = GPIOD;
	p_ctl->rtc6715.sw_da1_in.ON_pin.pin = (6);
	p_ctl->rtc6715.sw_da1_in.ON_pin.mode = GPIO_MODE_OUTPUT;
	p_ctl->rtc6715.sw_da1_in.ON_pin.drv = GPIO_DRV_3;
	p_ctl->rtc6715.sw_da1_in.ON_pin.pupd = GPIO_PUPD_DOWN;
	switch_fdc_start_pin_init(&p_ctl->rtc6715.sw_da1_in);

	switch_fdc_on(&p_ctl->rtc6715.sw_da1_in,fdc_on);
	switch_fdc_set_data(&p_ctl->rtc6715.sw_da1_in);

//	switch_fdc_pin_init(&p_ctl->rtc6715.sw_da1_in);

	rtc6715_start_pin_init(&p_ctl->rtc6715);

//	p_ctl->rtc6715.p_cfg = &p_ctl->dev_config.rec.recv_5_8_cfg;
	p_ctl->rtc6715.p_cfg = &p_ctl->rtc6715.cfg;

	rtc6715_start_init(&p_ctl->rtc6715);

#ifdef RECV_5_8_RSSI
	recv_5_8_rssi_init(&p_ctl->rtc6715,p_ctl->main_thread_id,cntrl_thread_event_recv_5_8_rssi);
	p_ctl->rtc6715.rssi.p_adc = &AdcHandle;
	p_ctl->rtc6715.rssi.adc_buf_num_point = 16;
	p_ctl->rtc6715.rssi.p_adc_buf = &p_ctl->adc_ring_buf[0];
	p_ctl->rtc6715.rssi.p_tcp_cmd = &p_ctl->tcp_cmd;
#endif

#ifdef RECV_5_8_SUB_3_3
	p_ctl->rtc6715.p_rff5071 = &p_ctl->rffc5071;
#endif
	return rtc6715_add_to_cell_table(p_ctl->p_comp_table,table_pos,&p_ctl->rtc6715,NULL);
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t  cntrl_init(control_struct *p_ctl){
uint16_t pos = p_ctl->comp_table_pos;

	pos = reg_add_to_cell_table(p_ctl->p_comp_table, pos, p_ctl);

	cntrl_init_i2c_murka_board(p_ctl);

#ifdef TVP_MASTER_HAVE
	pos =  cntr_tvp_master_start_init(p_ctl, pos);
#endif

#ifdef TVP_SLAVE_HAVE
	pos =  cntr_tvp_slave_start_init(p_ctl, pos);
#endif

#ifdef TVP_MASTER_HAVE
	cntr_tvp_master_init(p_ctl);
#endif

#ifdef TVP_SLAVE_HAVE
	cntr_tvp_slave_init(p_ctl);
#endif


volatile uint8_t err_cntr=0;
volatile uint8_t err=0;

#ifdef TVP_MASTER_HAVE
		err += p_ctl->tvp_5150_master.error;
#endif
#ifdef TVP_SLAVE_HAVE
		err += p_ctl->tvp_5150_slave.error;
#endif

	while(err){
		err = 0;
#ifdef TVP_MASTER_HAVE
			gpio_set_sun(&p_ctl->tvp_5150_master.reset_pin, GPIO_RESET);
#endif
#ifdef TVP_SLAVE_HAVE
			gpio_set_sun(&p_ctl->tvp_5150_slave.reset_pin, GPIO_RESET);
#endif

			osDelay(500);
#ifdef TVP_MASTER_HAVE
			gpio_set_sun(&p_ctl->tvp_5150_master.reset_pin,GPIO_SET);
#endif
#ifdef TVP_SLAVE_HAVE
			gpio_set_sun(&p_ctl->tvp_5150_slave.reset_pin, GPIO_SET);
#endif
			osDelay(500);

#ifdef TVP_MASTER_HAVE
		p_ctl->tvp_5150_master.error = 0;
		cntr_tvp_master_init(p_ctl);
#endif
#ifdef TVP_SLAVE_HAVE
		p_ctl->tvp_5150_slave.error = 0;
		cntr_tvp_slave_init(p_ctl);
#endif


#ifdef TVP_MASTER_HAVE
		err += p_ctl->tvp_5150_master.error;
#endif
#ifdef TVP_SLAVE_HAVE
		err += p_ctl->tvp_5150_slave.error;
#endif
		if(err){
			err_cntr++;
			if(err_cntr>=5) {
				break;
			}
		}
	}



#ifdef RECV_1_2
	pos = cntr_recv_1_2_init(p_ctl, pos);
#endif
#ifdef RTC6715
	pos = cntr_recv_5_8_init(p_ctl, pos);
#endif
#ifdef POVOROT
	pos = cntrl_povorot_init(p_ctl,pos);
#endif
#ifdef MAIN_CNTRL
	pos = cntrl_main_cmd_init(p_ctl,pos);
#endif

//	iommu_init();
	csi_clk_init();
	csi_internal_clk_init();

	pos = gpadc_init(p_ctl,pos);

	p_ctl->comp_table_pos = pos;

	return 0;
}
#ifdef TEST_BOARD
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t  cntrl_init_i2c_test_board(control_struct *p_ctl){
struct i2c_msg msgs[2];

uint8_t i=0;
uint8_t write_data[8];
uint8_t read_data[2];

	i2c_3.bus_freq     = 100000;

	i2c_3.sda.gpio = GPIOB;
	i2c_3.sda.pin = 7;
	i2c_3.sda.mode = GPIO_MODE_FNC4;
	i2c_3.sda.drv = GPIO_DRV_3;
	i2c_3.sda.pupd = GPIO_PUPD_UP;

	i2c_3.scl.gpio = GPIOB;
	i2c_3.scl.pin = 6;
	i2c_3.scl.mode = GPIO_MODE_FNC4;
	i2c_3.scl.drv = GPIO_DRV_3;
	i2c_3.scl.pupd = GPIO_PUPD_UP;

	sunxi_i2c_init(&i2c_3, 3);

	//output all
	write_data[0] = 6;//addr
	write_data[1] = 0x00;
	write_data[2] = 0x00;
	msgs[0].addr  = 0x24;
	msgs[0].flags = 0;       // Запись
	msgs[0].len   = 3;
	msgs[0].buf   = &write_data[0];
	sunxi_i2c_xfer(&i2c_3,msgs,1);

	osDelay(10);

	//output all zero
	write_data[0] = 2;//addr
	write_data[1] = 0x00;
	write_data[2] = 0x00;
	msgs[0].addr  = 0x24;
	msgs[0].flags = 0;       // Запись
	msgs[0].len   = 3;
	msgs[0].buf   = &write_data[0];
	sunxi_i2c_xfer(&i2c_3,msgs,1);

	osDelay(100);

	//output all one
	write_data[0] = 2;//addr
	write_data[1] = 0xFF;
	write_data[2] = 0xFF;
	msgs[0].addr  = 0x24;
	msgs[0].flags = 0;       // Запись
	msgs[0].len   = 3;
	msgs[0].buf   = &write_data[0];
	sunxi_i2c_xfer(&i2c_3,msgs,1);

	osDelay(100);

	return 0;
}
#endif
#ifdef MURKA_BOARD
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t  cntrl_init_i2c_murka_board(control_struct *p_ctl){
struct i2c_msg msgs[2];

	i2c_1.bus_freq     = 100000;

	i2c_1.sda.gpio = GPIOB;
	i2c_1.sda.pin = 5;
	i2c_1.sda.mode = GPIO_MODE_FNC4;
	i2c_1.sda.drv = GPIO_DRV_3;
	i2c_1.sda.pupd = GPIO_PUPD_UP;

	i2c_1.scl.gpio = GPIOB;
	i2c_1.scl.pin = 4;
	i2c_1.scl.mode = GPIO_MODE_FNC4;
	i2c_1.scl.drv = GPIO_DRV_3;
	i2c_1.scl.pupd = GPIO_PUPD_UP;

	sunxi_i2c_init(&i2c_1, 1);

	p_ctl->p_hi2c_tvp = &i2c_1;

	i2c_0.bus_freq     = 100000;

	i2c_0.sda.gpio = GPIOB;
	i2c_0.sda.pin = 2;
	i2c_0.sda.mode = GPIO_MODE_FNC4;
	i2c_0.sda.drv = GPIO_DRV_3;
	i2c_0.sda.pupd = GPIO_PUPD_UP;

	i2c_0.scl.gpio = GPIOB;
	i2c_0.scl.pin = 3;
	i2c_0.scl.mode = GPIO_MODE_FNC4;
	i2c_0.scl.drv = GPIO_DRV_3;
	i2c_0.scl.pupd = GPIO_PUPD_UP;

	sunxi_i2c_init(&i2c_0, 0);

	p_ctl->p_hi2c_1_2 = &i2c_0;


	return 0;
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ctrl_write_cpu_data_func(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	control_struct *p_ctl;
	p_ctl = (control_struct *)ctl32;
	uint32_t tmp32;
	memcpy(&tmp32,p8_data,4);
	writel(tmp32, p_ctl->addr);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* ctrl_read_cpu_data_func(uint32_t ctl32, uint32_t num, uint32_t tmp){
	control_struct *p_ctl;
	p_ctl = (control_struct *)ctl32;
	p_ctl->data = readl(p_ctl->addr);
	return (uint8_t*)&p_ctl->data;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t reg_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, control_struct *p_ctl){
SettingCell_t 	cell;
uint32_t 		i;
	i=0;

	p_ctl->addr = 0x05801000;

	for(i=0;i<COMPONENT_MAX_TABLE_SIZE;i++){
		if(!p_cell[i].CellNumber) break;
	}
	if( i>= COMPONENT_MAX_TABLE_SIZE) return num;

	cell.CellAttr = RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 4;
	cell.VarPtr = &p_ctl->addr;
	cell.ReadProc = 0;
	cell.WriteProc = 0;
	cell.HighLim = 0;
	cell.DefaultValue = 0;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "cpu addr";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 4;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)ctrl_read_cpu_data_func;
	cell.WriteProc = (void*)(void*)ctrl_write_cpu_data_func;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "cpu data";
	p_cell[i++] = cell;

	return num;
}

#define TCP_MSG_COUNT 10
#define TCP_MSG_SIZE  sizeof(tcp_cmd_ethernet_int_msg_type) // Теперь размер — это размер структуры

// 2. Рассчитываем память (с учетом заголовка RTX5)
#define TCP_MSG_FULL_BLOCK_SIZE (((TCP_MSG_SIZE + 3U) & ~3UL) + 12)
#pragma location=".ddr_data"
static uint32_t tcp_msg_myQueue_mem[(TCP_MSG_COUNT * TCP_MSG_FULL_BLOCK_SIZE) / 4];
#pragma location=".ddr_data"
static uint32_t tcp_msg_myQueue_cb[osRtxMessageQueueCbSize / 4];


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tcp_8080_callback(struct netconn *p_n, enum netconn_evt evt, u16_t len){
	tcp_cmd_8080_t *p_ctl;
uint8_t send_cmd=0;
tcp_cmd_ethernet_int_msg_type msg;
//printf("get pn=%x len=%x evt= %x \r\n", p_n, len, evt);
	if(p_n->callback_arg.socket==-1){
		return;
	}
	p_ctl = (tcp_cmd_8080_t *)p_n->callback_arg.socket;

	if((evt==NETCONN_EVT_SENDPLUS)&&(!len)){
		send_cmd = tcp_cmd_send_plus_zero;
	}
	if((evt==NETCONN_EVT_RCVPLUS)&&(!len)){
		send_cmd = tcp_cmd_recv_plus_zero;
	}
	if((evt==NETCONN_EVT_RCVPLUS)&&(len)){
		send_cmd = tcp_cmd_recv_plus_non_zero;
	}
	if(send_cmd){
		msg.cmd = send_cmd;
		msg.p_v = p_n;
		if (osMessageQueuePut(p_ctl->msg_que_id,&msg,0, 0) == osOK){
			osEventFlagsSet(p_ctl->event_flag_id,p_ctl->event);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cntrl_start_8080( tcp_cmd_8080_t *p_ctl){

	const osMessageQueueAttr_t queue_attr = {
	    .name = NULL,
	    .cb_mem = tcp_msg_myQueue_cb,
	    .cb_size = sizeof(tcp_msg_myQueue_cb),
	    .mq_mem = tcp_msg_myQueue_mem,
	    .mq_size = sizeof(tcp_msg_myQueue_mem)
	};

	memset(tcp_msg_myQueue_cb,0,sizeof(tcp_msg_myQueue_cb));
	memset(tcp_msg_myQueue_mem,0,sizeof(tcp_msg_myQueue_mem));

	p_ctl->p_a_conn_1 = NULL;
	p_ctl->p_a_conn_2 = NULL;
	p_ctl->msg_que_id = osMessageQueueNew(TCP_MSG_COUNT, TCP_MSG_SIZE, &queue_attr);

	p_ctl->p_l_conn = netconn_new_with_callback(NETCONN_TCP,tcp_8080_callback);

	netconn_bind(p_ctl->p_l_conn, NULL,8080);
	p_ctl->p_l_conn->callback_arg.socket = (int)p_ctl;//udp
	netconn_listen(p_ctl->p_l_conn);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void send_mjpeg_header(struct netconn *p_n) {
    const char *header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: multipart/x-mixed-replace; boundary=frame_boundary\r\n"
        "Connection: close\r\n\r\n";

    // Сразу посылаем и заголовок, и начало первого кадра (но без данных)
//    const char *first_boundary = "--frame_boundary\r\nContent-Type: image/jpeg\r\n\r\n";

    netconn_write(p_n, header, strlen(header), NETCONN_COPY);
//    netconn_write(p_n, first_boundary, strlen(first_boundary), NETCONN_COPY);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void handle_8080_request(tcp_cmd_8080_t *p_ctl,tcp_cmd_ethernet_int_msg_type* p_msg) {
    struct netbuf *inbuf;
    char *data;
    u16_t len;

    struct netconn *p_n = (struct netconn *)p_msg->p_v;

    // Читаем данные из сокета
    if (netconn_recv(p_n, &inbuf) == ERR_OK) {
        netbuf_data(inbuf, (void**)&data, &len);

        // Проверяем, что это HTTP GET
        if (len > 10 && strncmp(data, "GET ", 4) == 0) {

            if (strstr(data, "GET /cam1")) {
                // Браузер хочет Камеру 1
                // 1. Сохраняем указатель, чтобы знать кому слать кадры
                p_ctl->p_a_conn_1 = p_n;
                printf("cam1 \r\n");
                // 2. Отправляем начальный HTTP заголовок потока
                send_mjpeg_header(p_n);
            }
            else if (strstr(data, "GET /cam2")) {
                // Браузер хочет Камеру 2
                p_ctl->p_a_conn_2 = p_n;
                printf("cam2 \r\n");
                send_mjpeg_header(p_n);
            }
        }
        netbuf_delete(inbuf); // Обязательно удаляем буфер LwIP
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void handle_8080_socket(tcp_cmd_8080_t* p_ctl,	tcp_cmd_ethernet_int_msg_type *p_msg){
    struct netconn *p_n = (struct netconn *)p_msg->p_v;
    if (p_n == p_ctl->p_l_conn) {
        // А. Это новый клиент!
        struct netconn *new_conn;
        if (netconn_accept(p_ctl->p_l_conn, &new_conn) == ERR_OK) {
            new_conn->callback_arg.socket = (int)p_ctl;
            printf("accept \r\n");
            // Сокет создан, ждем от него запроса (придет через non_zero)
        }
    } else {
        // Б. Это закрытие сокета браузером
        // Нужно почистить p_a_conn_1 или p_a_conn_2, если это был один из них
        if (p_n == p_ctl->p_a_conn_1) {
            netconn_delete(p_n);
        	p_ctl->p_a_conn_1 = NULL;
        }
        if (p_n == p_ctl->p_a_conn_2) {
            netconn_delete(p_n);
        	p_ctl->p_a_conn_2 = NULL;
        }
        printf("close \r\n");

//        netconn_close(p_n);

    }
	return;
}
uint8_t gpadc_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, control_struct *p_ctl);

#define SAMPLES_COUNT 2048

#pragma location=".ddr_data"
static uint16_t gpadc_dma_test[SAMPLES_COUNT];

#define FFT_SIZE 1024
#define PI 3.14159265358979323846
// Структура для комплексных чисел
typedef struct { float re, im; } complex;

// Простейшее БПФ (инверсия битов + бабочка)
void fft(complex *v, int n, complex *tmp) {
    if (n > 1) {
        int k, m = n / 2;
        for (k = 0; k < m; k++) {
            tmp[k] = v[2 * k];
            tmp[k + m] = v[2 * k + 1];
        }
        for (k = 0; k < n; k++) v[k] = tmp[k];
        fft(v, m, tmp);
        fft(v + m, m, tmp);
        for (k = 0; k < m; k++) {
            complex e = v[k];
            complex o = v[k + m];
            double a = -2.0 * PI * k / n;
            complex w = { cos(a), sin(a) };
            v[k].re = e.re + w.re * o.re - w.im * o.im;
            v[k].im = e.im + w.re * o.im + w.im * o.re;
            v[k + m].re = e.re - (w.re * o.re - w.im * o.im);
            v[k + m].im = e.im - (w.re * o.im + w.im * o.re);
        }
    }
}
complex fft_input[FFT_SIZE];
complex fft_tmp[FFT_SIZE];
float magnitude[FFT_SIZE / 2];

void process_video_data(uint16_t *adc_data) {
    // 1. Подготовка данных
    for (int i = 0; i < FFT_SIZE; i++) {
        fft_input[i].re = (float)adc_data[i];
        fft_input[i].im = 0;
    }

    // 2. Расчет БПФ
    fft(fft_input, FFT_SIZE, fft_tmp);

    // 3. Расчет амплитуд (спектра)
    for (int i = 0; i < FFT_SIZE / 2; i++) {
        magnitude[i] = sqrt(fft_input[i].re * fft_input[i].re +
                            fft_input[i].im * fft_input[i].im);
    }

    // 4. Ищем пик строчной развертки (15.625 кГц)
    // На 1 МГц и 1024 точках шаг ~976 Гц. 15625 / 976 = 16.
    // Проверяем диапазон индексов 15, 16, 17
    float sync_level = magnitude[16];
    for(int i=0;i<40;i++){
    	printf("i= %d %f \r\n",i,magnitude[i]);
    }

    float best_score = 0;
    int best_freq_idx = 0;

    // Ищем в диапазоне от 13 до 19 бина (район строчной синхры)
    for (int i = 13; i <= 19; i++) {
        int h1 = i;      // Первая гармоника
        int h2 = i * 2;  // Вторая гармоника

        // Твоя формула накопления:
        // Берем текущий бин + его соседей + вторую гармонику с её соседями
        float current_score = magnitude[h1] + (magnitude[h1-1] + magnitude[h1+1]) * 0.5f;
        current_score += magnitude[h2] + (magnitude[h2-1] + magnitude[h2+1]) * 0.5f;

        if (current_score > best_score) {
            best_score = current_score;
            best_freq_idx = i;
        }
        printf("cur= %d %f\r\n",i,current_score );

    }
    printf("best= %d %f \r\n",best_freq_idx,best_score );


    float noise_floor = 0;
    for (int j = 5; j <= 10; j++) {
        noise_floor += magnitude[j];
    }
    noise_floor /= 6.0f; // Средний шум в "холодной" зоне

    // Твой SNR (на основе накопленной энергии)
    float snr = best_score / (noise_floor * 4.0f + 1.0f);

    printf("snr= %f \r\n",snr);
float k6,k7,k8,k9,k_ref;
float k[4];

	k6 = goertzel_mag(adc_data, 1024, 6);
	k7 = goertzel_mag(adc_data, 1024, 7);
	k8 = goertzel_mag(adc_data, 1024, 8);
	k9 = goertzel_mag(adc_data, 1024, 9);

	k_ref = (k6+k7+k8+k9)/4.0f;

	k[0] = goertzel_mag(adc_data, 1024, 15);
	k[1] = goertzel_mag(adc_data, 1024, 16);
	k[2] = goertzel_mag(adc_data, 1024, 17);
	k[3] = goertzel_mag(adc_data, 1024, 18);

	float max_k = 0;
	int max_idx = 0;

	// 1. Ищем самый мощный бин
	for (int i = 0; i < 4; i++) {
	    if (k[i] > max_k) {
	        max_k = k[i];
	        max_idx = i;
	    }
	}

	float s_video = max_k;
	int bins_count = 1; // Мы уже взяли один (max_k)

	if (max_idx > 0 && k[max_idx-1] > max_k * 0.5f) {
	    s_video += k[max_idx-1];
	    bins_count++;
	}
	if (max_idx < 3 && k[max_idx+1] > max_k * 0.5f) {
	    s_video += k[max_idx+1];
	    bins_count++;
	}

	// Теперь считаем среднюю энергию на один бин сигнала
	float avg_s_video = s_video / (float)bins_count;

	// Итоговый SNR
	float snr1 = avg_s_video / (k_ref + 1.0f);

	printf("k6=%f k7=%f k8=%f k9=%f \r\n",k6,k7,k8,k9);
	printf("k15=%f k16=%f k17=%f k18=%f \r\n",k[0],k[1],k[2],k[3]);
	printf("snr=%f max_id=%d max_k=%f\r\n",snr1,max_idx+15,max_k);



//
//	float max_vals[4] = {0}; // для хранения MAX каждого бина
//
//	for (int step = 0; step < 10; step++) {
//	    capture_dma();
//	    for (int j = 0; j < 4; j++) {
//	        float val = goertzel(15 + j);
//	        sum_k[j] += val;
//	        if (val > max_vals[j]) max_vals[j] = val; // запоминаем выброс
//	    }
//	}
//
//	// Убираем самый жирный выброс из каждого бина
//	for (int j = 0; j < 4; j++) {
//	    sum_k[j] = (sum_k[j] - max_vals[j]) / 9.0f;
//	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t gpadc_init(control_struct *p_ctl, uint8_t table_pos) {
#define GPADC_BASE    0x02009000
#define GP_SR_CON     (GPADC_BASE + 0x0000)
#define GP_CTRL          (*(volatile uint32_t*)(GPADC_BASE + 0x0004)) // Проверьте смещение
#define GP_CS_EN         (*(volatile uint32_t*)(GPADC_BASE + 0x0008))
//#define GP_DATA_INTS     (*(volatile uint32_t*)(GPADC_BASE + 0x000C))
#define GP_CH0_DATA      (*(volatile uint32_t*)(GPADC_BASE + 0x0080))
#define GP_FIFO_INTC    (*(volatile uint32_t*)(GPADC_BASE + 0x000C))
#define GP_FIFO_INTS    (*(volatile uint32_t*)(0x02009010))
#define GP_FIFO_DATA    (*(volatile uint32_t*)(GPADC_BASE + 0x0014))
#define GP_FIFO_DATA_ALIVE    (*(volatile uint32_t*)(GPADC_BASE + 0x0080))

	control.adc_time = 200;

    // 1. Снимаем Reset (16-й бит) и включаем Gating (0-й бит)
		CCU->GPADC_BGR_REG |= (1 << 16) | (1 << 0);
		udelay(10);
		writel((0x17 << 16) | (0x09),GP_SR_CON);//speed
//		writel((0xEF << 16) | 0x09, GP_SR_CON);


		// 1. Включаем АЦП и запускаем калибровку (бит 16 и 17)
		writel((1 << 17) | (1 << 16), GPADC_BASE + 0x0004);

		// 2. ЖДЕМ завершения (бит 17 должен стать 0)
		// Это критично! Если начать захват во время калибровки,
		// вы получите те самые "мусорные" калибровочные данные.
		while (readl(GPADC_BASE + 0x0004) & (1 << 17)) {
		    // можно добавить небольшой таймаут
			osDelay(10);
		}


		udelay(10);
	    // Step 5: Continuous conversion mode (bit[19:18] = 0x2)
	    GP_CTRL &= ~(0x3 << 18);
	    GP_CTRL |= (0x2 << 18);
	    udelay(10);

	    // 2. Очистка FIFO и включение DRQ для DMA
	    // 0x1F << 8 (порог 32) | (1 << 18) (DRQ EN) | (1 << 4) (Flush)
	    GP_FIFO_INTC = (0x10 << 8) | (1 << 18) | (1 << 4);
//	    GP_FIFO_INTC = (0x1F << 8) |  (1 << 4);

//
//	    // Step 6: Enable Channel 0
//	    GP_CS_EN |= (0x1 << 0);
//
//	    // Step 7: Enable ADC Function
//	    GP_CTRL |= (1 << 16);


		p_ctl->sw_adc_in.ON_pin.gpio = GPIOB;
		p_ctl->sw_adc_in.ON_pin.pin = (6);
		p_ctl->sw_adc_in.ON_pin.mode = GPIO_MODE_OUTPUT;
		p_ctl->sw_adc_in.ON_pin.drv = GPIO_DRV_3;
		p_ctl->sw_adc_in.ON_pin.pupd = GPIO_PUPD_DOWN;
		switch_fdc_start_pin_init(&p_ctl->sw_adc_in);

		switch_fdc_on(&p_ctl->sw_adc_in,fdc_off);
		switch_fdc_set_data(&p_ctl->sw_adc_in);

	    table_pos = gpadc_add_to_cell_table(p_ctl->p_comp_table, table_pos, p_ctl);

	    dma_init();

		return table_pos;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ctrl_gpadc_test(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	gpadc_test();
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ctrl_gpadc_set_time(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	control.adc_time = *p8_data;
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* ctrl_gpadc_get_time(uint32_t ctl32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
	return &control.adc_time;
}

float goertzel_mag(uint16_t* data, int n, int k) {
    float coeff = 2.0 * cos(2.0 * PI * k / n);
    float s_prev = 0, s_prev2 = 0;

    for (int i = 0; i < n; i++) {
        float s = (float)data[i] + coeff * s_prev - s_prev2;
        s_prev2 = s_prev;
        s_prev = s;
    }

    return sqrt(s_prev2 * s_prev2 + s_prev * s_prev - coeff * s_prev2 * s_prev);
}

int gpadc_test(){
//	dma_set_t dma_set;
	u32		*dst_addr = (u32 *)(uintptr_t)gpadc_dma_test;//board_kernel_load_addr();
	u32		  len	   = SAMPLES_COUNT*2;
	u32		  hdma, st = 0;
//	u32		  timeout;
	u32		  i, valid;


    GP_CTRL &= ~(1 << 16);

//	memset(gpadc_dma_test,0,sizeof(gpadc_dma_test));

	rtc6715_set_freq(&control.rtc6715,5825);

	osDelay(100);

	rtc6715_set_freq(&control.rtc6715,5865);

	osDelay(control.adc_time);

//	len = ALIGN(len, 4);

	/* dma */
//	dma_set.loop_mode		= 0;
//	dma_set.wait_cyc		= 0;
//	dma_set.data_block_size = 4;//1 * 32 / 8;
//	/* channel config (from dram to dram)*/
//	dma_set.channel_cfg.src_drq_type	 = DMAC_CFG_TYPE_GPADC; // dram
//	dma_set.channel_cfg.src_addr_mode	 = DMAC_CFG_DEST_ADDR_TYPE_IO_MODE;
//	dma_set.channel_cfg.src_burst_length = DMAC_CFG_SRC_16_BURST;
//	dma_set.channel_cfg.src_data_width	 = DMAC_CFG_SRC_DATA_WIDTH_16BIT;
//	dma_set.channel_cfg.reserved0		 = 0;
//
//	dma_set.channel_cfg.dst_drq_type	 = DMAC_DMATYPE_NORMAL; // dram
//	dma_set.channel_cfg.dst_addr_mode	 = DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE;
//	dma_set.channel_cfg.dst_burst_length = DMAC_CFG_SRC_16_BURST;
//	dma_set.channel_cfg.dst_data_width	 = DMAC_CFG_DEST_DATA_WIDTH_16BIT;
//	dma_set.channel_cfg.reserved1		 = 0;

	hdma = dma_request(0);
	if (!hdma) {
		error("DMA: can't request dma\r\n");
		return -1;
	}
	dma_hw_cfg_t cfg = {0};
	dma_hw_mode_t hw_mode = {0};

	cfg.bit.src_drq_type   = 12;
	cfg.bit.src_block_size = 1; //
	cfg.bit.src_addr_mode  = 1; // Fixed
	cfg.bit.src_data_width = 1; // 16-bit

	cfg.bit.dst_drq_type   = 0; // DRAM
	cfg.bit.dst_addr_mode  = 0; // Linear
	cfg.bit.dst_block_size  = 1; //
	cfg.bit.dst_data_width  = 1; // 16-bit (должно совпадать с SRC)

//	dma_setting(hdma, &dma_set);
	dma_set_param(hdma, cfg, hw_mode,  127);

	/* timeout : 100 ms */
//	timeout = time_ms();
	udelay(10);

	dma_start(hdma, (u32)(GPADC_BASE + 0x14), (u32)V2P((uintptr_t)dst_addr), len);
//	st = dma_querystatus(hdma);

	udelay(10);

    GP_FIFO_INTC = (0x0F << 8) | (1 << 18) | (1 << 4);

    GP_FIFO_INTS = 0xFFFFFFFF;

    // Step 6: Enable Channel 0
    GP_CS_EN |= (0x1 << 0);
    udelay(10);
    // Step 7: Enable ADC Function
//    GP_CTRL = (0x2 << 18) | (1 << 16) | (3 << 20);
    GP_CTRL = (0x3 << 18) | (1 << 16) | (3 << 20);
//    ctrl = (0x2 << 18) | (1 << 16);
    udelay(10);

    GP_FIFO_INTC = (0x7 << 8) | (1 << 18) | (1 << 4);
//    GP_FIFO_INTC = (0x7 << 8) | (1 << 4);
    udelay(100);
    readl(GPADC_BASE + 0x14);

    uint32_t timeout=0;

//    while (i < SAMPLES_COUNT) {
//        // Проверяем RXA_CNT (биты 13:8) — сколько слов в FIFO
//        // На 1 МГц тут почти всегда будет 1 или более
//        //if (((GP_FIFO_INTS >> 8) & 0x3F) > 0) {
//           if (GP_FIFO_INTS & (1<<16)){
//        	gpadc_dma_test[i++] = (uint16_t)(GP_FIFO_DATA_ALIVE);
//        	GP_FIFO_INTS = (1 << 16);
//            timeout = 0;
//        } else {
//            // Защита от вечного цикла, если АЦП встал
//            if (++timeout > 10000000) break;
//        }
//
//        // Опционально: проверка Overrun (бит 17)
//        if (GP_FIFO_INTS & (1 << 17)) {
//            GP_FIFO_INTS = (1 << 17); // Сбрасываем, если проц не успевает
//        }
//    }



//	while ((time_ms() - timeout < 100) && st) {
//		st = dma_querystatus(hdma);
//	}
	uint8_t cntr_t=0;
//	while(1){
//		osDelay(20);
//		st = dma_querystatus(hdma);
//		if(!st) break;
//		cntr_t++;
//		if(cntr_t>100) break;
//	}

//	if (st) {
//		error("DMA: test timeout!\r\n");
//		dma_stop(hdma);
//		dma_release(hdma);
//
//		return -2;
//	} else {
//		valid = 1;
//	}
//
//	dma_stop(hdma);
//	dma_release(hdma);

	while(1){
		if (dma_querystatus(hdma) <= 0) break;
	}

//	osDelay(100);
	dma_stop(hdma);
	dma_release(hdma);

	process_video_data((uint16_t*)dst_addr);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t gpadc_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, control_struct *p_ctl){
SettingCell_t 	cell;
uint32_t 		i;
	i=0;

//	p_ctl->addr = 0x05801000;

	sw_fdc_add_to_cell_table(p_cell,num++,&p_ctl->sw_adc_in, "adc sw");

	for(i=0;i<COMPONENT_MAX_TABLE_SIZE;i++){
		if(!p_cell[i].CellNumber) break;
	}
	if( i>= COMPONENT_MAX_TABLE_SIZE) return num;

	cell.CellAttr = Action_Att + Default_Attr + WR_Att;
	cell.CellType = FixIntegerCellType + 4;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)ctrl_gpadc_test;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "adc test";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)ctrl_gpadc_get_time;
	cell.WriteProc = (void*)(void*)ctrl_gpadc_set_time;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "adc time";
	p_cell[i++] = cell;



	return num;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tcp_cmd_ethernet_get_msg(tcp_cmd_8080_t *p_ctl){
tcp_cmd_ethernet_int_msg_type msg;
struct netconn *p_n;
//	printf("get msg!!!\r\n");
osStatus_t status;
	while(1){
		status = osMessageQueueGet(p_ctl->msg_que_id, &msg, NULL, 0);
		if(status != osOK) break;
		switch (msg.cmd) {
		    case tcp_cmd_recv_plus_non_zero:
		    	handle_8080_request(p_ctl, &msg);
		        break;
		    case tcp_cmd_recv_plus_zero:
		    	handle_8080_socket(p_ctl,&msg);
		    	handle_8080_request(p_ctl, &msg);
//		        struct netconn *p_n = (struct netconn *)msg.p_v;
//		        if (p_n == p_ctl->p_l_conn) {
//		            // А. Это новый клиент!
//		            struct netconn *new_conn;
//		            if (netconn_accept(p_ctl->p_l_conn, &new_conn) == ERR_OK) {
//		                new_conn->callback_arg.socket = (int)p_ctl;
//		                // Сокет создан, ждем от него запроса (придет через non_zero)
//		            }
//		        } else {
//		            // Б. Это закрытие сокета браузером
//		            // Нужно почистить p_a_conn_1 или p_a_conn_2, если это был один из них
//		            if (p_n == p_ctl->p_a_conn_1) p_ctl->p_a_conn_1 = NULL;
//		            if (p_n == p_ctl->p_a_conn_2) p_ctl->p_a_conn_2 = NULL;
//
//		            netconn_close(p_n);
//		            netconn_delete(p_n);
//		        }
		        break;
		}
	}


//	evt = osMessageGet(eth_res.os_message_write_id,0);
//
////	printf("get msg=%x\r\n",evt.status);
//
//		if (evt.status == osEventMessage) {
//			p_msg = (tcp_cmd_ethernet_int_msg_type*)evt.value.v;
//
////			printf("tcp msg=%d \r\n",p_msg->cmd);
//
//			p_n = (struct netconn *)p_msg->p_v;
////			if(p_n->socket) continue;
//			p_ctl= (tcp_cmd_eth_parser*)p_n->socket;
//			if(p_ctl != &control.tcp_cmd)
//			{
//				printf("pizdetc\r\n");
//			}
//			else{
//
//			switch(p_msg->cmd){
//
//			case tcp_cmd_recv_plus_non_zero:
////				printf("plus\r\n");
//				tcp_cmd_process_recv_data(p_ctl,p_n);
//			break;
//			case tcp_cmd_recv_plus_zero:
////				printf("plus zero\r\n");
//				tcp_cmd_process_socket(p_ctl,p_n);
//				tcp_cmd_process_recv_data(p_ctl,p_n);
//
//			break;
//			}
//			}
//			osPoolFree(eth_res.msg_pool_id,p_msg);
//		}
//		else{
//			break;
//		}
//	}
}
//uint32_t  cntrl_init_spi0(control_struct *p_ctl){
////struct i2c_msg msgs[2];
//
//	if (sunxi_spi_init(&spi_0) != 0) {
//	//		printf("SPI: init failed\r\n");
//	}
//
////
////	i2c_1.bus_freq     = 100000;
////
////	i2c_1.sda.gpio = GPIOB;
////	i2c_1.sda.pin = 5;
////	i2c_1.sda.mode = GPIO_MODE_FNC4;
////	i2c_1.sda.drv = GPIO_DRV_3;
////	i2c_1.sda.pupd = GPIO_PUPD_UP;
////
////	i2c_1.scl.gpio = GPIOB;
////	i2c_1.scl.pin = 4;
////	i2c_1.scl.mode = GPIO_MODE_FNC4;
////	i2c_1.scl.drv = GPIO_DRV_3;
////	i2c_1.scl.pupd = GPIO_PUPD_UP;
////
////	sunxi_i2c_init(&i2c_1, 1);
////
////	p_ctl->p_hi2c_tvp = &i2c_1;
////
////	i2c_0.bus_freq     = 100000;
////
////	i2c_0.sda.gpio = GPIOB;
////	i2c_0.sda.pin = 2;
////	i2c_0.sda.mode = GPIO_MODE_FNC4;
////	i2c_0.sda.drv = GPIO_DRV_3;
////	i2c_0.sda.pupd = GPIO_PUPD_UP;
////
////	i2c_0.scl.gpio = GPIOB;
////	i2c_0.scl.pin = 3;
////	i2c_0.scl.mode = GPIO_MODE_FNC4;
////	i2c_0.scl.drv = GPIO_DRV_3;
////	i2c_0.scl.pupd = GPIO_PUPD_UP;
////
////	sunxi_i2c_init(&i2c_0, 0);
////
////	p_ctl->p_hi2c_1_2 = &i2c_0;
//
//
//	return 0;
//}
