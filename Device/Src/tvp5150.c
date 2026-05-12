/*
 * tvp5150.c
 *
 *  Created on: 17 окт. 2023 г.
 *      Author: Petr
 */


#include "stddef.h"
#include "lwip/arch.h"
//#include "stm32f4xx_hal_gpio.h"
#include "debug_cells_lib.h"
#include "tvp5150_reg.h"

const char *TVP5150_NAME_REG[TVP5150_MAX_NUM_REG] = {
		"TVP addr",
		"TVP data",
		"TVP err"
};

const char *TVP5150_NAME_REG_SLAVE[TVP5150_MAX_NUM_REG] = {
		"TVP slave addr",
		"TVP slave data",
		"TVP slave err"
};


//#include "tvp5150.h"
static uint32_t tvp_write_buffer(struct sunxi_i2c *hi2c, uint8_t I2C_ADDRESS, uint8_t *aTxBuffer, uint8_t TXBUFFERSIZE);
static uint32_t tvp_read_buffer(struct sunxi_i2c *hi2c, uint8_t I2C_ADDRESS, uint8_t *aRxBuffer, uint8_t RXBUFFERSIZE);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tvp_start_reset(tvp5150_struct* p_ctl){
	uint32_t get;
	p_ctl->error = 0;


	gpio_init_pin_sun((struct gpio_t_ *)&p_ctl->reset_pin);
	gpio_init_pin_sun((struct gpio_t_ *)&p_ctl->int_pin);


//
//	get = NVIC_GetPriority(p_ctl->irq_type);
//	if(get > TVP_INT_PRIORITY){
//		NVIC_SetPriority(p_ctl->irq_type, TVP_INT_PRIORITY);
//	}
//	NVIC_GetEnabledIRQ(p_ctl->irq_type);
//
//	HAL_NVIC_DisableIRQ(p_ctl->irq_type);
//
//	GPIO_InitStructure.Pin = p_ctl->int_pin;
//	GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
//	GPIO_InitStructure.Pull = GPIO_PULLUP;
//	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
//
//	HAL_GPIO_ConfigExtiPin(p_ctl->int_port, &GPIO_InitStructure);
//
//	HAL_GPIO_DisableExtiPinIRQ(p_ctl->int_pin);
//	__HAL_GPIO_EXTI_CLEAR_IT(p_ctl->int_pin);
//
//	HAL_NVIC_EnableIRQ(p_ctl->irq_type);


//	HAL_GPIO_WritePin(p_ctl->reset_port, p_ctl->reset_pin, GPIO_PIN_SET);
	gpio_set_sun(&p_ctl->reset_pin,GPIO_SET);
	osDelay(10);
//	HAL_GPIO_WritePin(p_ctl->reset_port, p_ctl->reset_pin, GPIO_PIN_RESET);
	gpio_set_sun(&p_ctl->reset_pin,GPIO_RESET);
	osDelay(100);
//	HAL_GPIO_WritePin(p_ctl->reset_port, p_ctl->reset_pin, GPIO_PIN_SET);
	gpio_set_sun(&p_ctl->reset_pin,GPIO_SET);
	osDelay(100);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TVP_Init(tvp5150_struct* p_ctl){
volatile uint32_t state;


//	TVP_Write(p_ctl,0x02,0x30);
//	TVP_Write(p_ctl,0x03,0x0D);

//	TVP_Write(0x03,0x0D);
//	TVP_Write(0x03,0x2F);
//	TVP_Write(0x03,0x09);
	TVP_Write(p_ctl,0x0D,0x40);
	TVP_Write(p_ctl,0x0F,0x00);
//	TVP_Write(p_ctl,0x28,0x0C);//secam!!!!
//	TVP_Write(p_ctl,0x28,0x00);//all
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t TVP_interrut_event_check(tvp5150_struct* p_ctl){
//uint32_t res=0;
//uint8_t back;
//	back = TVP_Read(p_ctl, 0xC0);
//	if(back==0xC0){
//		res = 1;
//	}
//	return res;
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t TVP_interrut_stop(tvp5150_struct* p_ctl){
////	HAL_GPIO_DisableExtiPinIRQ(p_ctl->int_pin);
//	return 0;
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t TVP_interrut_start(tvp5150_struct* p_ctl){
//
//	uint8_t back;
//	back = TVP_Read(p_ctl, 0xC0);
//	TVP_Write(p_ctl, 0xC0, back & 0x7F); //reset interrupt
//
//	TVP_Write(p_ctl, 0xC1, 0x40); //interrupt lock en
//
////	__HAL_GPIO_EXTI_CLEAR_IT(p_ctl->int_pin);
////	HAL_GPIO_EnableExtiPinIRQ(p_ctl->int_pin);
//
//	return 0;
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TVP_Write(tvp5150_struct* p_ctl, uint8_t addr, uint8_t data){
//	return;

	uint8_t i2cbuf[2];
	i2cbuf[0] = addr;
	i2cbuf[1] = data;
//	I2C1_WriteBuffer(TVP_WRITE_ADDRESS, i2cbuf, 2);
	if (tvp_write_buffer(p_ctl->p_hi2c, p_ctl->i2c_addr, i2cbuf, 2)){
		p_ctl->error |= 1;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TVP_Write_tmp(tvp5150_struct* p_ctl, uint8_t addr, uint8_t data){
//	return;

	uint8_t i2cbuf[3];
	i2cbuf[0] = 0x2;
	i2cbuf[1] = 0xBB;
	i2cbuf[2] = 0xDD;
//	I2C1_WriteBuffer(TVP_WRITE_ADDRESS, i2cbuf, 2);
	if (tvp_write_buffer(p_ctl->p_hi2c, 2, i2cbuf, 3)){
		p_ctl->error |= 1;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t TVP_Read(tvp5150_struct* p_ctl, uint8_t addr){
uint8_t res=0;
//	return 0;
	uint8_t i2cbuf[1];
//	I2C1_WriteBuffer(TVP_READ_ADDRESS, &addr, 1);
//	I2C1_ReadBuffer(TVP_READ_ADDRESS, i2cbuf, 1);
	res |= tvp_write_buffer(p_ctl->p_hi2c, p_ctl->i2c_addr, &addr, 1);
	res |= tvp_read_buffer(p_ctl->p_hi2c, p_ctl->i2c_addr, i2cbuf, 1);
	if (res){
		p_ctl->error |= 1;
	}

	return i2cbuf[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint32_t tvp_write_buffer(struct sunxi_i2c  *hi2c, uint8_t I2C_ADDRESS, uint8_t *aTxBuffer, uint8_t TXBUFFERSIZE){
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint32_t tvp_read_buffer(struct sunxi_i2c  *hi2c, uint8_t I2C_ADDRESS, uint8_t *aRxBuffer, uint8_t RXBUFFERSIZE){
    struct i2c_msg msg;
    int ret;
    int retry = 5;

    // Настройка сообщения на чтение
    msg.addr  = I2C_ADDRESS;   // Опять же: 7-битный адрес, БЕЗ сдвига << 1
    msg.flags = I2C_M_RD;     // Ключевой флаг: ЧТЕНИЕ
    msg.len   = RXBUFFERSIZE;
    msg.buf   = aRxBuffer;

    while (retry--) {
        ret = sunxi_i2c_xfer(hi2c, &msg, 1);

        if (ret > 0) {
            return 0; // Данные в буфере aRxBuffer
        }
        osDelay(10);
        // Если шина занята или NACK — небольшая пауза перед повтором
    }

    return 1; // Таймаут или ошибка шины
//    while(HAL_I2C_Master_Receive(hi2c, (uint16_t)I2C_ADDRESS<<1, aRxBuffer, (uint16_t)RXBUFFERSIZE, (uint32_t)1000) != HAL_OK) {
//        if (HAL_I2C_GetError(hi2c) != HAL_I2C_ERROR_AF){
//            return 1;
////			 break;
//        }
//    }
//    while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY) {
//    }
//    return 0;
}

//void TVP_SetInputSource(TVP_CS_TypeDef ChanelSource)
//{
//	TVP_Write(TVP_Addr_InputSource, ChanelSource);
//}
//
//TVP_CS_TypeDef TVP_GetInputSource()
//{
//	return (TVP_CS_TypeDef) TVP_Read(TVP_Addr_InputSource);
//}
//
//
//void TVP_SetAnalogChannelControls(TVP_ACC_TypeDef OffsetControl, TVP_ACC_TypeDef GainControl)
//{
//	TVP_Write(TVP_Addr_AnalogChannelControls, 0x10 + (OffsetControl << 2) + GainControl);
//}
//
//void TVP_GetAnalogChannelControls(TVP_ACC_StructTypeDef *res)
//{
//	res->VAL    = TVP_Read(TVP_Addr_AnalogChannelControls);
//	res->Offset = (TVP_ACC_TypeDef) (res->VAL & 3);
//	res->Gain   = (TVP_ACC_TypeDef) ((res->VAL >> 2) & 3);
//}
//
//
////Important: PowerDown = 1 - power down; 0 - normal
//void TVP_SetOperatingModeControls(bool PowerDown, bool GLCO)
//{
//	uint8_t tmp = 0;
//
//	tmp = (GLCO * 4) + PowerDown;
//	TVP_Write(TVP_Addr_OperatingModeControls, tmp);
//}
//
//void TVP_PowerDown()
//{
//	TVP_SetOperatingModeControls(1, 0);
//}
//
//void TVP_GetOperatingModeControls(TVP_OMC_StructTypeDef *res)
//{
//	res->VAL 	 		 = TVP_Read(TVP_Addr_OperatingModeControls);
//	res->PowerDown = (bool) (res->VAL & 1);
//	res->GLCO 		 = (bool) ((res->VAL >> 2) & 1);
//}
//
//
//void TVP_SetMiscellaneousControls(TVP_SMC_StructTypeDef *res)
//{
//	uint8_t tmp = 0;
//
//	tmp = res->VBKO * 0x80 + res->GPLC * 0x40 + res->GPLC_Mode * 0x20 + res->HVLK * 0x10
//	    + res->YUV * 8 + res->HSYNK * 4 + res->VBLK * 2 + res->SCLK;
//	TVP_Write(TVP_Addr_MiscellaneousControls, tmp);
//}
//
//void TVP_GetMiscellaneousControls(TVP_SMC_StructTypeDef *res)
//{
//	res->VAL   = TVP_Read(TVP_Addr_MiscellaneousControls);
//	res->VBKO  = (bool) ((res->VAL >> 7) & 1);
//	res->GPLC  = (bool) ((res->VAL >> 6) & 1);
//	res->GPLC_Mode = (bool) ((res->VAL >> 5) & 1);
//	res->HVLK  = (bool) ((res->VAL >> 4) & 1);
//	res->YUV   = (bool) ((res->VAL >> 3) & 1);
//	res->HSYNK = (bool) ((res->VAL >> 2) & 1);
//	res->VBLK  = (bool) ((res->VAL >> 1) & 1);
//	res->SCLK  = (bool) (res->VAL & 1);
//}
//
//
//void TVP_SetAutoswitchMask(bool paln, bool palm, bool ntsc443)
//{
//	uint8_t tmp = 0;
//
//	tmp = ntsc443 * 8 + paln * 4 + palm * 2;
//	TVP_Write(TVP_Addr_AutoswitchMask, tmp);
//}
//
//void TVP_GetAutoswitchMask(TVP_ASM_StructTypeDef *res)
//{
//	res->VAL  = TVP_Read(TVP_Addr_AutoswitchMask);
//	res->n443 = (bool) ((res->VAL >> 4) & 1);
//	res->paln = (bool) ((res->VAL >> 3) & 1);
//	res->palm = (bool) ((res->VAL >> 2) & 1);
//}
//
//
//void TVP_SoftwareReset()
//{
//	TVP_Write(TVP_Addr_SoftwareReset, 1);
//}
//
//
//void TVP_SetColorKillerControl(TVP_CK_TypeDef colorKiller, uint8_t theshold)
//{
//	uint8_t tmp = 0;
//
//	tmp = (((colorKiller << 4) + (theshold & 0x1f)) & 0x7f);
//	TVP_Write(TVP_Addr_ColorKillerControl, tmp);
//}
//
//void TVP_GetColorKillerControl(TVP_CK_StructTypeDef *res)
//{
//	res->VAL         = TVP_Read(TVP_Addr_ColorKillerControl);
//	res->ColorKiller = (TVP_CK_TypeDef) ((res->VAL >> 4) & 3);
//	res->Theshold    = res->VAL  & 0x1f;
//}
//
//
//void TVP_SetLuminanceControl(TVP_LC_StructTypeDef *res)
//{
//	uint8_t tmp = 0;
//
//	tmp = res->BypassMode * 0x80 + res->NoPedetal * 0x40 + res->NoRawHeader * 0x20 + res->VertBlanckBypass * 0x10
//	    + (res->SignalDelay & 0x0f);
//	TVP_Write(TVP_Addr_LuminanceControl1, tmp);
//
//	tmp = res->Filter * 0x40 + res->PeakingGain * 0x2;
//	TVP_Write(TVP_Addr_LuminanceControl2, tmp);
//
//	tmp = res->FilterStopBand & 0x03;
//	TVP_Write(TVP_Addr_LuminanceControl3, tmp);
//}
//
//void TVP_GetLuminanceControl(TVP_LC_StructTypeDef *res)
//{
//	res->VAL1             = TVP_Read(TVP_Addr_LuminanceControl1);
//	res->BypassMode       = (bool) ((res->VAL1 >> 7) & 1);
//	res->NoPedetal        = (bool) ((res->VAL1 >> 6) & 1);
//	res->NoRawHeader      = (bool) ((res->VAL1 >> 5) & 1);
//	res->VertBlanckBypass = (bool) ((res->VAL1 >> 4) & 1);
//	res->SignalDelay      = (TVP_LSD_TypeDef) (res->VAL1 & 0x0f);
//	res->VAL2             = TVP_Read(TVP_Addr_LuminanceControl2);
//	res->Filter           = (bool) ((res->VAL1 >> 6) & 1);
//	res->PeakingGain      = (TVP_LPG_TypeDef) ((res->VAL1 >> 1) & 0x0f);
//	res->VAL3             = TVP_Read(TVP_Addr_LuminanceControl3);
//	res->FilterStopBand   = (TVP_LFS_TypeDef) (res->VAL1 & 0x03);
//}
//
////OutputAndRates
//void TVP_SetOutputAndRates(TVP_OAR_StructTypeDef *res)
//{
//	uint8_t tmp = 0;
//
//	tmp = res->YUV_CodeRange * 0x40 + res->UV_CodeFormat * 0x20
//	    + res->YUV_Data * 0x08 + (res->YUV_OutputFormat & 0x07);
//	TVP_Write(TVP_Addr_OutputAndRatesSelect, tmp);
//}
//
//void TVP_GetOutputAndRates(TVP_OAR_StructTypeDef *res)
//{
//	res->VAL              = TVP_Read(TVP_Addr_OutputAndRatesSelect);
//	res->YUV_CodeRange    = (bool) ((res->VAL >> 6) & 1);
//	res->UV_CodeFormat    = (bool) ((res->VAL >> 5) & 1);
//	res->YUV_Data         = (TVP_YUD_TypeDef) ((res->VAL >> 3) & 3);
//	res->YUV_OutputFormat = (TVP_YOF_TypeDef) (res->VAL  & 7);
//}
//
////ActiveVideoCroping
//void TVP_SetActiveVideoCroping(TVP_AVC_StructTypeDef *res)
//{
//	uint8_t tmp = 0;
//	int16_t tst;
//
//	tst = res->Start;
//	if (tst<0) tst = (-513 - tst);
//	tmp = (res->AVID * 4) + (tst & 3);
//	TVP_Write(TVP_Addr_ActiveVideoCropingStartLSB, tmp);
//
//	tmp = (tst >> 6) & 0x07f + ((tst & 0x8000)>> 8);
//	TVP_Write(TVP_Addr_ActiveVideoCropingStartMSB, tmp);
//
//	tst = res->Stop;
//	if (tst<0) tst = (-513 - tst);
//	tmp = tst & 0x03;
//	TVP_Write(TVP_Addr_ActiveVideoCropingStopLSB, tmp);
//
//	tmp = (tst >> 6) & 0x07f + ((tst & 0x8000) >> 8);
//	TVP_Write(TVP_Addr_ActiveVideoCropingStopMSB, tmp);
//}
//
//void TVP_GetActiveVideoCroping(TVP_AVC_StructTypeDef *res)
//{
//	res->VAL1  = TVP_Read(TVP_Addr_ActiveVideoCropingStartLSB);
//	res->VAL2  = TVP_Read(TVP_Addr_ActiveVideoCropingStartMSB);
//	res->AVID  = (bool) ((res->VAL1 >> 2) & 1);
//	res->Start = res->VAL2;
//	res->Start = (((res->Start & 0x07f) << 2) + (res->VAL1 & 3) | ((res->Start & 0x80) << 8));
//	res->VAL1  = TVP_Read(TVP_Addr_ActiveVideoCropingStopLSB);
//	res->VAL2  = TVP_Read(TVP_Addr_ActiveVideoCropingStopMSB);
//	res->Stop  = res->VAL2;
//	res->Stop  = (((res->Stop & 0x07f) << 2) + (res->VAL1 & 3)) | ((res->Stop & 0x80) << 8) ;
//}
//
////Genlock
//void TVP_SetGenlock(bool CDTO_LSB, bool GLCO_RTC)
//{
//	uint8_t tmp = 0;
//
//	tmp = (CDTO_LSB << 2) + GLCO_RTC;
//	TVP_Write(TVP_Addr_Genlock, tmp);
//}
//
//void TVP_GetGenlock(TVP_SG_StructTypeDef *res)
//{
//	res->VAL      = TVP_Read(TVP_Addr_Genlock);
//	res->CDTO_LSB = (bool) ((res->VAL >> 4) & 1);
//	res->GLCO_RTC = (bool) res->VAL & 0x01;
//}
//
////Horizontal Sync (HSYNC) Start
//void TVP_SetHorizontalSyncStart(uint8_t HSYNC)
//{
//	TVP_Write(TVP_Addr_HorisontalSynkStart, HSYNC);
//}
//
//uint8_t TVP_GetHorizontalSyncStart()
//{
//	uint8_t res;
//	res = TVP_Read(TVP_Addr_HorisontalSynkStart);
//	return res;
//}
//
////VerticalBlanking
//void TVP_SetVerticalBlanking(TVP_VB_StructTypeDef *res)
//{
//	uint8_t tmp = 0;
//
//	tmp = res->start ;
//	TVP_Write(TVP_Addr_VerticalBlankingStart, tmp);
//
//	tmp = res->stop;
//	TVP_Write(TVP_Addr_VerticalBlankingStop, tmp);
//
//}
//
//void TVP_GetVerticalBlanking(TVP_VB_StructTypeDef *res)
//{
//	res->start = TVP_Read(TVP_Addr_VerticalBlankingStart);
//	res->stop  = TVP_Read(TVP_Addr_VerticalBlankingStop);
//}
//
////Chrominance Control
//void TVP_SetChrominanceControl(TVP_CC_StructTypeDef *res)
//{
//	uint8_t tmp = 0;
//
//	tmp = (res->ColorPLL_Reset << 4) + (res->CE << 3) + (res->CE << 2) + res->ACGC;
//	TVP_Write(TVP_Addr_CrominanceControl1, tmp);
//
//	tmp = (res->CombFilterMode << 4) + (res->WCF << 2) + res->FilterSelect;
//	TVP_Write(TVP_Addr_CrominanceControl2, tmp);
//
//}
//
//void TVP_GetChrominanceControl(TVP_CC_StructTypeDef *res)
//{
//	res->VAL1           = TVP_Read(TVP_Addr_CrominanceControl1);
//	res->ColorPLL_Reset = (bool) ((res->VAL1 >> 4) & 1);
//	res->ACE            = (bool) ((res->VAL1 >> 3) & 1);
//	res->CE             = (bool) ((res->VAL1 >> 2) & 1);
//	res->ACGC           = (TVP_ACGC_TypeDef) (res->VAL1 & 0x03);
//
//	res->VAL2           = TVP_Read(TVP_Addr_CrominanceControl2);
//	res->CombFilterMode = (TVP_CCFM_TypeDef) ((res->VAL2 >> 4) & 0x0F);
//	res->WCF            = (bool) ((res->VAL2 >> 2) & 1);
//	res->FilterSelect   = (TVP_LFS_TypeDef) (res->VAL2 & 0x03);
//}
//
//
//void TVP_InitImageConfig(TVP_IC_StructTypeDef *cfg)
//{
//	cfg->Brightness = 128;
//	cfg->Contrast   = 128;
//	cfg->Saturation = 128;
//	cfg->Hue        = 0;
//}
//
////void TVP_SetImageConfig(TVP_IC_StructTypeDef *cfg)
////{
////	TVP_Write(TVP_Addr_BrightnessControl, cfg->Brightness);
////	TVP_Write(TVP_Addr_ContrastControl,   cfg->Contrast);
////	TVP_Write(TVP_Addr_SaturationControl, cfg->Saturation);
////	TVP_Write(TVP_Addr_HueControl,        cfg->Hue);
////}
////
////void TVP_SetImageConfigA(uint8_t Brightness, uint8_t Contrast, uint8_t Saturation, int8_t Hue)
////{
////	TVP_Write(TVP_Addr_BrightnessControl, Brightness);
////	TVP_Write(TVP_Addr_ContrastControl,   Contrast);
////	TVP_Write(TVP_Addr_SaturationControl, Saturation);
////	TVP_Write(TVP_Addr_HueControl,        Hue);
////}
//
////void TVP_GetImageConfig(TVP_IC_StructTypeDef *cfg)
////{
////	cfg->Brightness = TVP_Read(TVP_Addr_BrightnessControl);
////	cfg->Contrast   = TVP_Read(TVP_Addr_ContrastControl);
////	cfg->Saturation = TVP_Read(TVP_Addr_SaturationControl);
////	cfg->Hue        = TVP_Read(TVP_Addr_HueControl);
////}
//
//
//void TVP_InitPinsConfig(TVP_Pins_StructTypeDef *pins)
//{
//	pins->pin23 = TVP_P23_GLCO;
//	pins->pin24 = TVP_P24_VSYNC;
//	pins->pin27 = TVP_P27_INTREQ;
//	pins->pin9  = TVP_P09_SCLK;
//}
//
//void TVP_SetPinsConfig(TVP_Pins_StructTypeDef *pins)
//{
//	uint8_t tmp;
//
//	tmp = ((pins->pin23 & 2)>>1) * 0x40 + ((pins->pin24 & 2)>>1) * 0x20 + ((pins->pin24 & 2)>>1) * 0x10
//	    + (pins->pin23 & 1) * 8 + (pins->pin24 & 1) * 4 + pins->pin27 * 2 + pins->pin9;
//
//	TVP_Write(TVP_Addr_PinsConfig, tmp);
//}
//
//void TVP_GetPinsConfig(TVP_Pins_StructTypeDef *pins)
//{
//	uint8_t tmp;
//	tmp = TVP_Read(TVP_Addr_PinsConfig);
//	pins->VAL = tmp;
//
//	if ((tmp >> 6) & 1)  pins->pin23 = TVP_P23_LOCK;
//	else pins->pin23 = (TVP_P23_TypeDef) ((tmp >> 3) & 1);
//
//	if (((tmp >> 4) & 1) || ((tmp >> 5) & 1)) pins->pin24 = TVP_P24_LOCK;
//	else pins->pin24 = (TVP_P24_TypeDef) ((tmp >> 2) & 1);
//
//	pins->pin27 = (TVP_P27_TypeDef) ((tmp >> 1) & 1);
//	pins->pin9  = (TVP_P09_TypeDef)  (tmp & 1);
//}
//
//
//void TVP_SetVideoStandart(TVP_VS_TypeDef VideoStandart)
//{
//	TVP_Write(TVP_Addr_VideoStandart, VideoStandart);
//}
//
//TVP_VS_TypeDef TVP_GetVideoStandart()
//{
//	return (TVP_VS_TypeDef) TVP_Read(TVP_Addr_VideoStandart);
//}
//
////Interrupt
//void TVP_SetInterruptB_Reset(TVP_INTB_StructTypeDef *res)
//{
//	uint8_t tmp;
//	tmp = res->SoftwareInit * 0x80 + res->MacrovisionDetect * 0x40 + res->CommandReady * 0x20 + res->FieldRate * 0x10
//	    + res->LineAlternation * 8 + res->ColorLock * 4 + res->HV_Lock * 2 + res->TV_VCR;
//	TVP_Write(TVP_Addr_InterruptB_Reset, tmp);
//}

//void TVP_GetInterruptB_Reset(TVP_INTB_StructTypeDef *res)
//{
//	res->VAL                = TVP_Read(TVP_Addr_InterruptB_Reset);
//	res->SoftwareInit       = (bool) ((res->VAL >> 7) & 1);
//	res->MacrovisionDetect  = (bool) ((res->VAL >> 6) & 1);
//	res->CommandReady       = (bool) ((res->VAL >> 5) & 1);
//	res->FieldRate          = (bool) ((res->VAL >> 4) & 1);
//	res->LineAlternation    = (bool) ((res->VAL >> 3) & 1);
//	res->ColorLock          = (bool) ((res->VAL >> 2) & 1);
//	res->HV_Lock            = (bool) ((res->VAL >> 1) & 1);
//	res->TV_VCR             = (bool)  (res->VAL & 1);
//}
//
//void TVP_SetInterruptB_Enable(TVP_INTB_StructTypeDef *res)
//{
//	uint8_t tmp;
//	tmp = res->SoftwareInit * 0x80 + res->MacrovisionDetect * 0x40 + res->CommandReady * 0x20 + res->FieldRate * 0x10
//	    + res->LineAlternation * 8 + res->ColorLock * 4 + res->HV_Lock * 2 + res->TV_VCR;
//	TVP_Write(TVP_Addr_InterruptB_Enable, tmp);
//}
//
//void TVP_GetInterruptB_Enable(TVP_INTB_StructTypeDef *res)
//{
//	res->VAL                = TVP_Read(TVP_Addr_InterruptB_Enable);
//	res->SoftwareInit       = (bool) ((res->VAL >> 7) & 1);
//	res->MacrovisionDetect  = (bool) ((res->VAL >> 6) & 1);
//	res->CommandReady       = (bool) ((res->VAL >> 5) & 1);
//	res->FieldRate          = (bool) ((res->VAL >> 4) & 1);
//	res->LineAlternation    = (bool) ((res->VAL >> 3) & 1);
//	res->ColorLock          = (bool) ((res->VAL >> 2) & 1);
//	res->HV_Lock            = (bool) ((res->VAL >> 1) & 1);
//	res->TV_VCR             = (bool)  (res->VAL & 1);
//}
//
////Interrupt B is: 0 (default) - active low; 1 = active high
//void TVP_SetInterruptB_Polarity(bool res)
//{
//	TVP_Write(TVP_Addr_InterruptB_Config, res & 1);
//}
//
//bool TVP_GetInterruptB_Polarity()
//{
//	return (bool) (TVP_Read(TVP_Addr_InterruptB_Config) & 1);
//}
//
//void TVP_GetInterruptB_Status(TVP_INTB_StructTypeDef *res)
//{
//	res->VAL                = TVP_Read(TVP_Addr_InterruptB_Status);
//	res->SoftwareInit       = (bool) ((res->VAL >> 7) & 1);
//	res->MacrovisionDetect  = (bool) ((res->VAL >> 6) & 1);
//	res->CommandReady       = (bool) ((res->VAL >> 5) & 1);
//	res->FieldRate          = (bool) ((res->VAL >> 4) & 1);
//	res->LineAlternation    = (bool) ((res->VAL >> 3) & 1);
//	res->ColorLock          = (bool) ((res->VAL >> 2) & 1);
//	res->HV_Lock            = (bool) ((res->VAL >> 1) & 1);
//	res->TV_VCR             = (bool)  (res->VAL & 1);
//}
//
//bool TVP_GetInterruptB_Active()
//{
//	return (bool) (TVP_Read(TVP_Addr_InterruptB_Active) & 1);
//}
//
//
//void TVP_GetInfo(TVP_Info_StructTypeDef *res)
//{
//	res->DeviceId   = TVP_Read(TVP_Addr_DeviceMSB);
//	res->DeviceId   = (res->DeviceId << 8) + TVP_Read(TVP_Addr_DeviceLSB);
//	res->RAM        =  TVP_Read(TVP_Addr_RAMVersion);
//	res->ROM        =  TVP_Read(TVP_Addr_ROMVersion);
//	res->VerticalLineCount  = TVP_Read(TVP_Addr_VerticalLineCountMSB);
//	res->VerticalLineCount  = (res->VerticalLineCount << 8) + TVP_Read(TVP_Addr_VerticalLineCountLSB);
//}
//
//void TVP_GetStatus(TVP_Status_StructTypeDef *res)
//{
//	res->VAL1          =  TVP_Read(TVP_Addr_StatusRegister1);
//	res->PeakWhiteDetect     = (bool) ((res->VAL1 >> 7) & 1);
//	res->LineAlternating     = (bool) ((res->VAL1 >> 6) & 1);
//	res->FieldRate           = (bool) ((res->VAL1 >> 5) & 1);
//	res->LostLock            = (bool) ((res->VAL1 >> 4) & 1);
//	res->ColorSubcarrierLock = (bool) ((res->VAL1 >> 3) & 1);
//	res->VerticalSyncLock    = (bool) ((res->VAL1 >> 2) & 1);
//	res->HorizontalSyncLock  = (bool) ((res->VAL1 >> 1) & 1);
//	res->TV_VCR              = (bool) (res->VAL1 & 1);
//	res->VAL2          =  TVP_Read(TVP_Addr_StatusRegister2);
//	res->WeakSignalDetection = (bool) ((res->VAL2 >> 6) & 1);
//	res->PAL_SwitchPolarity  = (bool) ((res->VAL2 >> 5) & 1);
//	res->FieldSequence       = (bool) ((res->VAL2 >> 4) & 1);
//	res->AGC_AndOffsetFrozen = (bool) ((res->VAL2 >> 3) & 1);
//	res->MacrovisionDetection = (TVP_MD_TypeDef) (res->VAL2 & 3);
//	res->AGC           =  TVP_Read(TVP_Addr_StatusRegister3);
//	res->SCH           =  TVP_Read(TVP_Addr_StatusRegister4);
//	res->VAL3          =  TVP_Read(TVP_Addr_StatusRegister5);
//	res->PeakWhiteDetect     = (bool) ((res->VAL3 >> 7) & 1);
//	res->VideoStandard       = (TVP_VS_TypeDef) ((res->VAL3  & 0x0F) + 1);
//	res->SamplingRate        = (bool) (res->VAL3 & 1);
//}
//
//void TVP_SetFullFieldEnabled(bool res)
//{
//	TVP_Write(TVP_Addr_FullFieldEnable, res & 1);
//}
//
//bool TVP_GetFullFieldEnabled()
//{
//	return (bool) (TVP_Read(TVP_Addr_FullFieldEnable) & 1);
//}
//
//
//uint8_t TVP_GetFIFO_WordCount()
//{
//	uint8_t res;
//	res = TVP_Read(TVP_Addr_FIFO_WordCount);
//	return res;
//}
//
//void TVP_SetFIFO_InterruptThreshold(uint8_t res)
//{
//	TVP_Write(TVP_Addr_FIFO_InterruptThreshold, res);
//}
//
//uint8_t TVP_GetFIFO_InterruptThreshold()
//{
//	uint8_t res;
//	res = TVP_Read(TVP_Addr_FIFO_InterruptThreshold);
//	return res;
//}
//
//void TVP_FIFO_Reset(uint8_t res)
//{
//	TVP_Write(TVP_Addr_FIFO_InterruptThreshold, res);
//}
//
//void TVP_SetFIFO_OutputControl(bool res)
//{
//	TVP_Write(TVP_Addr_FIFO_OutputControl, res & 1);
//}
//
//bool TVP_GetFIFO_OutputControl()
//{
//	return (bool) (TVP_Read(TVP_Addr_FIFO_OutputControl) & 1);
//}
//
////Automatic Initialization
//void TVP_SetAutomaticInit(bool auto_init, bool auto_clock)
//{
//	uint8_t tmp = 0;
//
//	tmp = (auto_init * 4) +  (auto_clock * 2);
//	TVP_Write(TVP_Addr_AutomaticInitialization, tmp);
//}
//
//void TVP_GetAutomaticInit(TVP_AI_StructTypeDef *res)
//{
//	res->VAL 	 		 = TVP_Read(TVP_Addr_AutomaticInitialization);
//	res->AutoInit  = (bool) ((res->VAL >> 2) & 1);
//	res->AutoClock = (bool) ((res->VAL >> 1) & 1);
//}
//
////Pixel Alignment
//void TVP_SetPixelAlignment(uint16_t addr)
//{
//	uint8_t tmp = 0;
//
//	tmp = addr & 0xFF ;
//	TVP_Write(TVP_Addr_PixelAlignmentLSB, tmp);
//
//	tmp = (addr >> 8) & 3;
//	TVP_Write(TVP_Addr_PixelAlignmentMSB, tmp);
//
//}
//
//uint16_t TVP_GetPixelAlignment()
//{
//	uint16_t res = 0;
//	res =       TVP_Read(TVP_Addr_PixelAlignmentMSB) * 0x0FF;
//	res = res + TVP_Read(TVP_Addr_PixelAlignmentLSB);
//  return res;
//}
//
////Line Number Interrupt
//void TVP_SetLineNumberInterrupt(bool field1, bool field2, uint8_t line_num)
//{
//	uint8_t tmp = 0;
//
//	tmp = (field1 * 0x80) +  (field2 * 0x40) + line_num & 0x3F;
//	TVP_Write(TVP_Addr_LineNumberInterrupt, tmp);
//}
//
//void TVP_GetLineNumberInterrupt(TVP_LNI_StructTypeDef *res)
//{
//	res->VAL 	 		  = TVP_Read(TVP_Addr_LineNumberInterrupt);
//	res->Field1     = (bool) ((res->VAL >> 7) & 1);
//	res->Field2 		= (bool) ((res->VAL >> 6) & 1);
//	res->LineNumber =          res->VAL  & 0x3F;
//}
//
////VDP Status
//void TVP_SetVDPStatus(TVP_VDP_StructTypeDef *res)
//{
//	uint8_t tmp = 0;
//	tmp = res->FIFO_FullError * 0x80 + res->FIFO_Empty * 0x40 + res->Teletext * 0x20 + res->CC_Field1 * 0x10
//	    + res->CC_Field2 * 8 + res->WSS * 4 + res->WPS * 2 + res->VITC;
//	TVP_Write(TVP_Addr_VDP_Status, tmp);
//}
//
//void TVP_GetVDPStatus(TVP_VDP_StructTypeDef *res)
//{
//	res->VAL            = TVP_Read(TVP_Addr_VDP_Status);
//	res->FIFO_FullError = (bool) ((res->VAL >> 7) & 1);
//	res->FIFO_Empty     = (bool) ((res->VAL >> 6) & 1);
//	res->Teletext       = (bool) ((res->VAL >> 5) & 1);
//	res->CC_Field1      = (bool) ((res->VAL >> 4) & 1);
//	res->CC_Field2      = (bool) ((res->VAL >> 3) & 1);
//	res->WSS            = (bool) ((res->VAL >> 2) & 1);
//	res->WSS            = (bool) ((res->VAL >> 1) & 1);
//	res->VITC           = (bool)  (res->VAL & 1);
//}
//
//void TVP_SetVDP_Conf(uint16_t addr, uint8_t data)
//{
//	uint8_t tmp = 0;
//	tmp =  addr & 0xFF;
//	TVP_Write(TVP_Addr_VDP_Conf_RAM_AddressLSB, tmp);
//	tmp = (addr >> 8) & 0x1;
//	TVP_Write(TVP_Addr_VDP_Conf_RAM_AddressMSB, tmp);
//	TVP_Write(TVP_Addr_VDP_Conf_RAM_Data, data);
//}
//
//uint8_t TVP_GetVDP_Conf(uint16_t addr)
//{
//	uint8_t tmp = 0, res;
//	tmp =  addr & 0xFF;
//	TVP_Write(TVP_Addr_VDP_Conf_RAM_AddressLSB, tmp);
//	tmp = (addr >> 8) & 0x1;
//	TVP_Write(TVP_Addr_VDP_Conf_RAM_AddressMSB, tmp);
//	res = TVP_Read(TVP_Addr_VDP_Conf_RAM_Data);
//	return res;
//}
//
////Interrupt Configuration Register A
//void TVP_SetInterruptA_Config(TVP_INTA_Conf_StructTypeDef *res)
//{
//	uint8_t tmp = 0;
//	tmp = res->YUV * 4 + res->InterruptA * 2 + res->Polaryty;
//	TVP_Write(TVP_Addr_InterruptA_Config, tmp);
//}
//
//void TVP_GetInterruptA_Config(TVP_INTA_Conf_StructTypeDef *res)
//{
//	res->VAL         = TVP_Read(TVP_Addr_InterruptA_Config);
//	res->YUV         = (bool) ((res->VAL >> 2) & 1);
//	res->InterruptA  = (bool) ((res->VAL >> 1) & 1);
//	res->Polaryty    = (bool) (res->VAL & 1);
//}
//
//void TVP_SetInterruptA_Enable(TVP_INTA_StructTypeDef *res)
//{
//	uint8_t tmp;
//	tmp = res->Lock * 0x40 + res->CycleComplete * 0x20 + res->BusError * 0x10
//	    + res->FIFO_Threshold * 4 + res->Line * 2 + res->Data;
//	TVP_Write(TVP_Addr_InterruptA_Enable, tmp);
//}

//void TVP_GetInterruptA_Enable(TVP_INTA_StructTypeDef *res)
//{
//	res->VAL            = TVP_Read(TVP_Addr_InterruptA_Enable);
//	res->Lock           = (bool) ((res->VAL >> 6) & 1);
//	res->CycleComplete  = (bool) ((res->VAL >> 5) & 1);
//	res->BusError       = (bool) ((res->VAL >> 4) & 1);
//	res->FIFO_Threshold = (bool) ((res->VAL >> 2) & 1);
//	res->Line           = (bool) ((res->VAL >> 1) & 1);
//	res->Data           = (bool)  (res->VAL & 1);
//}
//
//void TVP_SetInterruptA_Status(TVP_INTA_StructTypeDef *res)
//{
//	uint8_t tmp;
//	tmp = res->LockState * 0x80 + res->Lock * 0x40 + res->CycleComplete * 0x20 + res->BusError * 0x10
//	    + res->FIFO_Threshold * 4 + res->Line * 2 + res->Data;
//	TVP_Write(TVP_Addr_InterruptA_Enable, tmp);
//}
//
//void TVP_GetInterruptA_Status(TVP_INTA_StructTypeDef *res)
//{
//	res->VAL            = TVP_Read(TVP_Addr_InterruptA_Enable);
//	res->LockState      = (bool) ((res->VAL >> 7) & 1);
//	res->Lock           = (bool) ((res->VAL >> 6) & 1);
//	res->CycleComplete  = (bool) ((res->VAL >> 5) & 1);
//	res->BusError       = (bool) ((res->VAL >> 4) & 1);
//	res->FIFO_Threshold = (bool) ((res->VAL >> 2) & 1);
//	res->Line           = (bool) ((res->VAL >> 1) & 1);
//	res->Data           = (bool)  (res->VAL & 1);
//}
//
//void TVP_SetLineMode(uint8_t line_num, TVP_LFM_StructTypeDef *res)
//{
//	uint8_t tmp = 0;
//
//	tmp = res->NullFiltering * 0x80 + res->SendVBI * 0x40 + res->ErrorVBI * 0x20 + res->ErroCorrection * 0x10
//	    + (res->Mode & 0x0f);
//	TVP_Write(TVP_Addr_LineModeStart + line_num, tmp);
//
//}
//
//void TVP_GetLineMode(uint8_t line_num, TVP_LFM_StructTypeDef *res)
//{
//	res->VAL            = TVP_Read(TVP_Addr_LineModeStart + line_num);
//	res->NullFiltering  = (bool) ((res->VAL >> 7) & 1);
//	res->SendVBI        = (bool) ((res->VAL >> 6) & 1);
//	res->ErrorVBI       = (bool) ((res->VAL >> 5) & 1);
//	res->ErroCorrection = (bool) ((res->VAL >> 4) & 1);
//	res->Mode           = (TVP_LFM_TypeDef) (res->VAL & 0x0f);
//}
//
//void TVP_SetFieldMode(TVP_LFM_StructTypeDef *res)
//{
//	uint8_t tmp = 0;
//
//	tmp = res->NullFiltering * 0x80 + res->SendVBI * 0x40 + res->ErrorVBI * 0x20 + res->ErroCorrection * 0x10
//	    + (res->Mode & 0x0f);
//	TVP_Write(TVP_Addr_FullFieldMode, tmp);
//
//}
//
//void TVP_GetFieldMode(TVP_LFM_StructTypeDef *res)
//{
//	res->VAL            = TVP_Read(TVP_Addr_FullFieldMode);
//	res->NullFiltering  = (bool) ((res->VAL >> 7) & 1);
//	res->SendVBI        = (bool) ((res->VAL >> 6) & 1);
//	res->ErrorVBI       = (bool) ((res->VAL >> 5) & 1);
//	res->ErroCorrection = (bool) ((res->VAL >> 4) & 1);
//	res->Mode           = (TVP_LFM_TypeDef) (res->VAL & 0x0f);
//}
//
//uint8_t TVP_GetWSS_Data(uint8_t num)
//{
//	uint8_t res;
//	if (num > 5) return 0;
//	res            = TVP_Read(TVP_Addr_WSS_Data1 + num);
//	return res;
//}
//
//uint8_t TVP_GetVPS_Data(uint8_t num)
//{
//	uint8_t res;
//	if (num > 11) return 0;
//	res            = TVP_Read(TVP_Addr_VPS_Data01 + num);
//	return res;
//}
//
//uint8_t TVP_GetVITC_Data(uint8_t num)
//{
//	uint8_t res;
//	if (num > 9) return 0;
//	res            = TVP_Read(TVP_Addr_VITC_Data01 + num);
//	return res;
//}
//
//uint8_t TVP_GetClosedCapture_Data(uint8_t num)
//{
//	uint8_t res;
//	if (num > 3) return 0;
//	res            = TVP_Read(TVP_Addr_ClosedCaptionData1 + num);
//	return res;
//}
//
//uint8_t TVP_GetVBI_FIFO_Data()
//{
//	uint8_t res;
//	res            = TVP_Read(TVP_Addr_VBI_FIFO);
//	return res;
//}
//
//void TVP_SetTeletextFilter(uint8_t num, uint8_t res)
//{
//	if (num > 9) return;
//
//	TVP_Write(TVP_Addr_LineModeStart + num, res);
//
//}
//
//uint8_t TVP_GetTeletextFilter(uint8_t num)
//{
//	uint8_t res;
//	if (num > 9) return 0;
//	res            = TVP_Read(TVP_Addr_LineModeStart + num);
//	return res;
//}
//
//void TVP_SetTeletextFilterControl(TVP_TTC_StructTypeDef *res)
//{
//	uint8_t tmp = 0;
//
//	tmp = res->FilterLogic * 8 + res->Mode * 4 + res->TeletextFilter2 * 2 + res->TeletextFilter1;
//	TVP_Write(TVP_Addr_TeletextFilterControl, tmp);
//
//}
//
//void TVP_GetTeletextFilterControl(TVP_TTC_StructTypeDef *res)
//{
//	res->VAL             = TVP_Read(TVP_Addr_TeletextFilterControl);
//	res->FilterLogic     = (TVP_TTC_TypeDef) ((res->VAL >> 3) & 3);
//	res->Mode            = (bool) ((res->VAL >> 2) & 1);
//	res->TeletextFilter2 = (bool) ((res->VAL >> 1) & 1);
//	res->TeletextFilter1 = (bool)  (res->VAL & 1);
//}
//#include "videodev2.h"
//
//#define TVP5150_H_MAX		720U
//#define TVP5150_V_MAX_525_60	480U
//#define TVP5150_V_MAX_OTHERS	576U
//#define TVP5150_MAX_CROP_LEFT	511
//#define TVP5150_MAX_CROP_TOP	127
//#define TVP5150_CROP_SHIFT	2
//#define TVP5150_MBUS_FMT	MEDIA_BUS_FMT_UYVY8_2X8
//#define TVP5150_FIELD		V4L2_FIELD_ALTERNATE
//#define TVP5150_COLORSPACE	V4L2_COLORSPACE_SMPTE170M
//#define TVP5150_STD_MASK	(V4L2_STD_NTSC     | \
//				 V4L2_STD_NTSC_443 | \
//				 V4L2_STD_PAL      | \
//				 V4L2_STD_PAL_M    | \
//				 V4L2_STD_PAL_N    | \
//				 V4L2_STD_PAL_Nc   | \
//				 V4L2_STD_SECAM)
//
//#define TVP5150_MAX_CONNECTORS	3 /* Check dt-bindings for more information */
////#define __must_check                    __attribute__((__warn_unused_result__))
//
//static inline void * ERR_PTR(long error)
//{
//	return (void *) error;
//}
//
////MODULE_DESCRIPTION("Texas Instruments TVP5150A/TVP5150AM1/TVP5151 video decoder driver");
////MODULE_AUTHOR("Mauro Carvalho Chehab");
////MODULE_LICENSE("GPL v2");
//
//
//static int debug;
////module_param(debug, int, 0644);
////MODULE_PARM_DESC(debug, "Debug level (0-2)");
//
//#define dprintk0(__dev, __arg...) dev_dbg_lvl(__dev, 0, 0, __arg)
//
//enum tvp5150_pads {
//	TVP5150_PAD_AIP1A,
//	TVP5150_PAD_AIP1B,
//	TVP5150_PAD_VID_OUT,
//	TVP5150_NUM_PADS
//};
//
//struct tvp5150_connector {
//	struct v4l2_fwnode_connector base;
//	struct media_entity ent;
//	struct media_pad pad;
//};
//struct v4l2_subdev {
////#if defined(CONFIG_MEDIA_CONTROLLER)
////	struct media_entity entity;
////#endif
////	struct list_head list;
////	struct module *owner;
////	bool owner_v4l2_dev;
////	u32 flags;
////	struct v4l2_device *v4l2_dev;
////	const struct v4l2_subdev_ops *ops;
////	const struct v4l2_subdev_internal_ops *internal_ops;
////	struct v4l2_ctrl_handler *ctrl_handler;
////	char name[V4L2_SUBDEV_NAME_SIZE];
////	u32 grp_id;
////	void *dev_priv;
////	void *host_priv;
////	struct video_device *devnode;
//	struct device *dev;
////	struct fwnode_handle *fwnode;
////	struct list_head async_list;
////	struct v4l2_async_subdev *asd;
////	struct v4l2_async_notifier *notifier;
////	struct v4l2_async_notifier *subdev_notifier;
////	struct v4l2_subdev_platform_data *pdata;
////	struct mutex *state_lock;
////
////	/*
////	 * The fields below are private, and should only be accessed via
////	 * appropriate functions.
////	 */
////
////	struct led_classdev *privacy_led;
////
////	/*
////	 * TODO: active_state should most likely be changed from a pointer to an
////	 * embedded field. For the time being it's kept as a pointer to more
////	 * easily catch uses of active_state in the cases where the driver
////	 * doesn't support it.
////	 */
////	struct v4l2_subdev_state *active_state;
//	u64 enabled_streams;
//};
//
//struct tvp5150 {
//	struct v4l2_subdev sd;
//
//	struct media_pad pads[TVP5150_NUM_PADS];
//	struct tvp5150_connector connectors[TVP5150_MAX_CONNECTORS];
//	struct tvp5150_connector *cur_connector;
//	unsigned int connectors_num;
//
//	struct v4l2_ctrl_handler hdl;
//	struct v4l2_rect rect;
//	struct regmap *regmap;
//	int irq;
//
//	v4l2_std_id norm;	/* Current set standard */
//	v4l2_std_id detected_norm;
//	u32 input;
//	u32 output;
//	u32 oe;
//	int enable;
//	bool lock;
//
//	u16 dev_id;
//	u16 rom_ver;
//
//	enum v4l2_mbus_type mbus_type;
//};
//
//static inline struct tvp5150 *to_tvp5150(struct v4l2_subdev *sd)
//{
//	return container_of(sd, struct tvp5150, sd);
//}
//
//static inline struct v4l2_subdev *to_sd(struct v4l2_ctrl *ctrl)
//{
//	return &container_of(ctrl->handler, struct tvp5150, hdl)->sd;
//}
//
//static int tvp5150_read(struct v4l2_subdev *sd, unsigned char addr)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	int ret, val;
//
//	ret = regmap_read(decoder->regmap, addr, &val);
//	if (ret < 0)
//		return ret;
//
//	return val;
//}
//
//static void dump_reg_range(struct v4l2_subdev *sd, char *s, u8 init,
//				const u8 end, int max_line)
//{
//	u8 buf[16];
//	int i = 0, j, len;
//
//	if (max_line > 16) {
//		dprintk0(sd->dev, "too much data to dump\n");
//		return;
//	}
//
//	for (i = init; i < end; i += max_line) {
//		len = (end - i > max_line) ? max_line : end - i;
//
//		for (j = 0; j < len; j++)
//			buf[j] = tvp5150_read(sd, i + j);
//
//		dprintk0(sd->dev, "%s reg %02x = %*ph\n", s, i, len, buf);
//	}
//}
//
//static int tvp5150_log_status(struct v4l2_subdev *sd)
//{
//	dprintk0(sd->dev, "tvp5150: Video input source selection #1 = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_VD_IN_SRC_SEL_1));
//	dprintk0(sd->dev, "tvp5150: Analog channel controls = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_ANAL_CHL_CTL));
//	dprintk0(sd->dev, "tvp5150: Operation mode controls = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_OP_MODE_CTL));
//	dprintk0(sd->dev, "tvp5150: Miscellaneous controls = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_MISC_CTL));
//	dprintk0(sd->dev, "tvp5150: Autoswitch mask= 0x%02x\n",
//		tvp5150_read(sd, TVP5150_AUTOSW_MSK));
//	dprintk0(sd->dev, "tvp5150: Color killer threshold control = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_COLOR_KIL_THSH_CTL));
//	dprintk0(sd->dev, "tvp5150: Luminance processing controls #1 #2 and #3 = %02x %02x %02x\n",
//		tvp5150_read(sd, TVP5150_LUMA_PROC_CTL_1),
//		tvp5150_read(sd, TVP5150_LUMA_PROC_CTL_2),
//		tvp5150_read(sd, TVP5150_LUMA_PROC_CTL_3));
//	dprintk0(sd->dev, "tvp5150: Brightness control = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_BRIGHT_CTL));
//	dprintk0(sd->dev, "tvp5150: Color saturation control = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_SATURATION_CTL));
//	dprintk0(sd->dev, "tvp5150: Hue control = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_HUE_CTL));
//	dprintk0(sd->dev, "tvp5150: Contrast control = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_CONTRAST_CTL));
//	dprintk0(sd->dev, "tvp5150: Outputs and data rates select = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_DATA_RATE_SEL));
//	dprintk0(sd->dev, "tvp5150: Configuration shared pins = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_CONF_SHARED_PIN));
//	dprintk0(sd->dev, "tvp5150: Active video cropping start = 0x%02x%02x\n",
//		tvp5150_read(sd, TVP5150_ACT_VD_CROP_ST_MSB),
//		tvp5150_read(sd, TVP5150_ACT_VD_CROP_ST_LSB));
//	dprintk0(sd->dev, "tvp5150: Active video cropping stop  = 0x%02x%02x\n",
//		tvp5150_read(sd, TVP5150_ACT_VD_CROP_STP_MSB),
//		tvp5150_read(sd, TVP5150_ACT_VD_CROP_STP_LSB));
//	dprintk0(sd->dev, "tvp5150: Genlock/RTC = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_GENLOCK));
//	dprintk0(sd->dev, "tvp5150: Horizontal sync start = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_HORIZ_SYNC_START));
//	dprintk0(sd->dev, "tvp5150: Vertical blanking start = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_VERT_BLANKING_START));
//	dprintk0(sd->dev, "tvp5150: Vertical blanking stop = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_VERT_BLANKING_STOP));
//	dprintk0(sd->dev, "tvp5150: Chrominance processing control #1 and #2 = %02x %02x\n",
//		tvp5150_read(sd, TVP5150_CHROMA_PROC_CTL_1),
//		tvp5150_read(sd, TVP5150_CHROMA_PROC_CTL_2));
//	dprintk0(sd->dev, "tvp5150: Interrupt reset register B = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_INT_RESET_REG_B));
//	dprintk0(sd->dev, "tvp5150: Interrupt enable register B = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_INT_ENABLE_REG_B));
//	dprintk0(sd->dev, "tvp5150: Interrupt configuration register B = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_INTT_CONFIG_REG_B));
//	dprintk0(sd->dev, "tvp5150: Video standard = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_VIDEO_STD));
//	dprintk0(sd->dev, "tvp5150: Chroma gain factor: Cb=0x%02x Cr=0x%02x\n",
//		tvp5150_read(sd, TVP5150_CB_GAIN_FACT),
//		tvp5150_read(sd, TVP5150_CR_GAIN_FACTOR));
//	dprintk0(sd->dev, "tvp5150: Macrovision on counter = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_MACROVISION_ON_CTR));
//	dprintk0(sd->dev, "tvp5150: Macrovision off counter = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_MACROVISION_OFF_CTR));
//	dprintk0(sd->dev, "tvp5150: ITU-R BT.656.%d timing(TVP5150AM1 only)\n",
//		(tvp5150_read(sd, TVP5150_REV_SELECT) & 1) ? 3 : 4);
//	dprintk0(sd->dev, "tvp5150: Device ID = %02x%02x\n",
//		tvp5150_read(sd, TVP5150_MSB_DEV_ID),
//		tvp5150_read(sd, TVP5150_LSB_DEV_ID));
//	dprintk0(sd->dev, "tvp5150: ROM version = (hex) %02x.%02x\n",
//		tvp5150_read(sd, TVP5150_ROM_MAJOR_VER),
//		tvp5150_read(sd, TVP5150_ROM_MINOR_VER));
//	dprintk0(sd->dev, "tvp5150: Vertical line count = 0x%02x%02x\n",
//		tvp5150_read(sd, TVP5150_VERT_LN_COUNT_MSB),
//		tvp5150_read(sd, TVP5150_VERT_LN_COUNT_LSB));
//	dprintk0(sd->dev, "tvp5150: Interrupt status register B = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_INT_STATUS_REG_B));
//	dprintk0(sd->dev, "tvp5150: Interrupt active register B = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_INT_ACTIVE_REG_B));
//	dprintk0(sd->dev, "tvp5150: Status regs #1 to #5 = %02x %02x %02x %02x %02x\n",
//		tvp5150_read(sd, TVP5150_STATUS_REG_1),
//		tvp5150_read(sd, TVP5150_STATUS_REG_2),
//		tvp5150_read(sd, TVP5150_STATUS_REG_3),
//		tvp5150_read(sd, TVP5150_STATUS_REG_4),
//		tvp5150_read(sd, TVP5150_STATUS_REG_5));
//
//	dump_reg_range(sd, "Teletext filter 1",   TVP5150_TELETEXT_FIL1_INI,
//			TVP5150_TELETEXT_FIL1_END, 8);
//	dump_reg_range(sd, "Teletext filter 2",   TVP5150_TELETEXT_FIL2_INI,
//			TVP5150_TELETEXT_FIL2_END, 8);
//
//	dprintk0(sd->dev, "tvp5150: Teletext filter enable = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_TELETEXT_FIL_ENA));
//	dprintk0(sd->dev, "tvp5150: Interrupt status register A = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_INT_STATUS_REG_A));
//	dprintk0(sd->dev, "tvp5150: Interrupt enable register A = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_INT_ENABLE_REG_A));
//	dprintk0(sd->dev, "tvp5150: Interrupt configuration = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_INT_CONF));
//	dprintk0(sd->dev, "tvp5150: VDP status register = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_VDP_STATUS_REG));
//	dprintk0(sd->dev, "tvp5150: FIFO word count = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_FIFO_WORD_COUNT));
//	dprintk0(sd->dev, "tvp5150: FIFO interrupt threshold = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_FIFO_INT_THRESHOLD));
//	dprintk0(sd->dev, "tvp5150: FIFO reset = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_FIFO_RESET));
//	dprintk0(sd->dev, "tvp5150: Line number interrupt = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_LINE_NUMBER_INT));
//	dprintk0(sd->dev, "tvp5150: Pixel alignment register = 0x%02x%02x\n",
//		tvp5150_read(sd, TVP5150_PIX_ALIGN_REG_HIGH),
//		tvp5150_read(sd, TVP5150_PIX_ALIGN_REG_LOW));
//	dprintk0(sd->dev, "tvp5150: FIFO output control = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_FIFO_OUT_CTRL));
//	dprintk0(sd->dev, "tvp5150: Full field enable = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_FULL_FIELD_ENA));
//	dprintk0(sd->dev, "tvp5150: Full field mode register = 0x%02x\n",
//		tvp5150_read(sd, TVP5150_FULL_FIELD_MODE_REG));
//
//	dump_reg_range(sd, "CC   data",   TVP5150_CC_DATA_INI,
//			TVP5150_CC_DATA_END, 8);
//
//	dump_reg_range(sd, "WSS  data",   TVP5150_WSS_DATA_INI,
//			TVP5150_WSS_DATA_END, 8);
//
//	dump_reg_range(sd, "VPS  data",   TVP5150_VPS_DATA_INI,
//			TVP5150_VPS_DATA_END, 8);
//
//	dump_reg_range(sd, "VITC data",   TVP5150_VITC_DATA_INI,
//			TVP5150_VITC_DATA_END, 10);
//
//	dump_reg_range(sd, "Line mode",   TVP5150_LINE_MODE_INI,
//			TVP5150_LINE_MODE_END, 8);
//	return 0;
//}
//
///****************************************************************************
//			Basic functions
// ****************************************************************************/
//
//static void tvp5150_selmux(struct v4l2_subdev *sd)
//{
//	int opmode = 0;
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	unsigned int mask, val;
//	int input = 0;
//
//	/* Only tvp5150am1 and tvp5151 have signal generator support */
//	if ((decoder->dev_id == 0x5150 && decoder->rom_ver == 0x0400) ||
//	    (decoder->dev_id == 0x5151 && decoder->rom_ver == 0x0100)) {
//		if (!decoder->enable)
//			input = 8;
//	}
//
//	switch (decoder->input) {
//	case TVP5150_COMPOSITE1:
//		input |= 2;
//		fallthrough;
//	case TVP5150_COMPOSITE0:
//		break;
//	case TVP5150_SVIDEO:
//	default:
//		input |= 1;
//		break;
//	}
//
//	dev_dbg_lvl(sd->dev, 1, debug,
//		    "Selecting video route: route input=%s, output=%s => tvp5150 input=0x%02x, opmode=0x%02x\n",
//		    decoder->input == 0 ? "aip1a" :
//		    decoder->input == 2 ? "aip1b" : "svideo",
//		    decoder->output == 0 ? "normal" : "black-frame-gen",
//		    input, opmode);
//
//	regmap_write(decoder->regmap, TVP5150_OP_MODE_CTL, opmode);
//	regmap_write(decoder->regmap, TVP5150_VD_IN_SRC_SEL_1, input);
//
//	/*
//	 * Setup the FID/GLCO/VLK/HVLK and INTREQ/GPCL/VBLK output signals. For
//	 * S-Video we output the vertical lock (VLK) signal on FID/GLCO/VLK/HVLK
//	 * and set INTREQ/GPCL/VBLK to logic 0. For composite we output the
//	 * field indicator (FID) signal on FID/GLCO/VLK/HVLK and set
//	 * INTREQ/GPCL/VBLK to logic 1.
//	 */
//	mask = TVP5150_MISC_CTL_GPCL | TVP5150_MISC_CTL_HVLK;
//	if (decoder->input == TVP5150_SVIDEO)
//		val = TVP5150_MISC_CTL_HVLK;
//	else
//		val = TVP5150_MISC_CTL_GPCL;
//	regmap_update_bits(decoder->regmap, TVP5150_MISC_CTL, mask, val);
//};
//
//struct i2c_reg_value {
//	unsigned char reg;
//	unsigned char value;
//};
//
///* Default values as sugested at TVP5150AM1 datasheet */
//static const struct i2c_reg_value tvp5150_init_default[] = {
//	{ /* 0x00 */
//		TVP5150_VD_IN_SRC_SEL_1, 0x00
//	},
//	{ /* 0x01 */
//		TVP5150_ANAL_CHL_CTL, 0x15
//	},
//	{ /* 0x02 */
//		TVP5150_OP_MODE_CTL, 0x00
//	},
//	{ /* 0x03 */
//		TVP5150_MISC_CTL, 0x01
//	},
//	{ /* 0x06 */
//		TVP5150_COLOR_KIL_THSH_CTL, 0x10
//	},
//	{ /* 0x07 */
//		TVP5150_LUMA_PROC_CTL_1, 0x60
//	},
//	{ /* 0x08 */
//		TVP5150_LUMA_PROC_CTL_2, 0x00
//	},
//	{ /* 0x09 */
//		TVP5150_BRIGHT_CTL, 0x80
//	},
//	{ /* 0x0a */
//		TVP5150_SATURATION_CTL, 0x80
//	},
//	{ /* 0x0b */
//		TVP5150_HUE_CTL, 0x00
//	},
//	{ /* 0x0c */
//		TVP5150_CONTRAST_CTL, 0x80
//	},
//	{ /* 0x0d */
//		TVP5150_DATA_RATE_SEL, 0x47
//	},
//	{ /* 0x0e */
//		TVP5150_LUMA_PROC_CTL_3, 0x00
//	},
//	{ /* 0x0f */
//		TVP5150_CONF_SHARED_PIN, 0x08
//	},
//	{ /* 0x11 */
//		TVP5150_ACT_VD_CROP_ST_MSB, 0x00
//	},
//	{ /* 0x12 */
//		TVP5150_ACT_VD_CROP_ST_LSB, 0x00
//	},
//	{ /* 0x13 */
//		TVP5150_ACT_VD_CROP_STP_MSB, 0x00
//	},
//	{ /* 0x14 */
//		TVP5150_ACT_VD_CROP_STP_LSB, 0x00
//	},
//	{ /* 0x15 */
//		TVP5150_GENLOCK, 0x01
//	},
//	{ /* 0x16 */
//		TVP5150_HORIZ_SYNC_START, 0x80
//	},
//	{ /* 0x18 */
//		TVP5150_VERT_BLANKING_START, 0x00
//	},
//	{ /* 0x19 */
//		TVP5150_VERT_BLANKING_STOP, 0x00
//	},
//	{ /* 0x1a */
//		TVP5150_CHROMA_PROC_CTL_1, 0x0c
//	},
//	{ /* 0x1b */
//		TVP5150_CHROMA_PROC_CTL_2, 0x14
//	},
//	{ /* 0x1c */
//		TVP5150_INT_RESET_REG_B, 0x00
//	},
//	{ /* 0x1d */
//		TVP5150_INT_ENABLE_REG_B, 0x00
//	},
//	{ /* 0x1e */
//		TVP5150_INTT_CONFIG_REG_B, 0x00
//	},
//	{ /* 0x28 */
//		TVP5150_VIDEO_STD, 0x00
//	},
//	{ /* 0x2e */
//		TVP5150_MACROVISION_ON_CTR, 0x0f
//	},
//	{ /* 0x2f */
//		TVP5150_MACROVISION_OFF_CTR, 0x01
//	},
//	{ /* 0xbb */
//		TVP5150_TELETEXT_FIL_ENA, 0x00
//	},
//	{ /* 0xc0 */
//		TVP5150_INT_STATUS_REG_A, 0x00
//	},
//	{ /* 0xc1 */
//		TVP5150_INT_ENABLE_REG_A, 0x00
//	},
//	{ /* 0xc2 */
//		TVP5150_INT_CONF, 0x04
//	},
//	{ /* 0xc8 */
//		TVP5150_FIFO_INT_THRESHOLD, 0x80
//	},
//	{ /* 0xc9 */
//		TVP5150_FIFO_RESET, 0x00
//	},
//	{ /* 0xca */
//		TVP5150_LINE_NUMBER_INT, 0x00
//	},
//	{ /* 0xcb */
//		TVP5150_PIX_ALIGN_REG_LOW, 0x4e
//	},
//	{ /* 0xcc */
//		TVP5150_PIX_ALIGN_REG_HIGH, 0x00
//	},
//	{ /* 0xcd */
//		TVP5150_FIFO_OUT_CTRL, 0x01
//	},
//	{ /* 0xcf */
//		TVP5150_FULL_FIELD_ENA, 0x00
//	},
//	{ /* 0xd0 */
//		TVP5150_LINE_MODE_INI, 0x00
//	},
//	{ /* 0xfc */
//		TVP5150_FULL_FIELD_MODE_REG, 0x7f
//	},
//	{ /* end of data */
//		0xff, 0xff
//	}
//};
//
///* Default values as sugested at TVP5150AM1 datasheet */
//static const struct i2c_reg_value tvp5150_init_enable[] = {
//	{	/* Automatic offset and AGC enabled */
//		TVP5150_ANAL_CHL_CTL, 0x15
//	}, {	/* Activate YCrCb output 0x9 or 0xd ? */
//		TVP5150_MISC_CTL, TVP5150_MISC_CTL_GPCL |
//				  TVP5150_MISC_CTL_INTREQ_OE |
//				  TVP5150_MISC_CTL_YCBCR_OE |
//				  TVP5150_MISC_CTL_SYNC_OE |
//				  TVP5150_MISC_CTL_VBLANK |
//				  TVP5150_MISC_CTL_CLOCK_OE,
//	}, {	/* Activates video std autodetection for all standards */
//		TVP5150_AUTOSW_MSK, 0x0
//	}, {	/* Default format: 0x47. For 4:2:2: 0x40 */
//		TVP5150_DATA_RATE_SEL, 0x47
//	}, {
//		TVP5150_CHROMA_PROC_CTL_1, 0x0c
//	}, {
//		TVP5150_CHROMA_PROC_CTL_2, 0x54
//	}, {	/* Non documented, but initialized on WinTV USB2 */
//		0x27, 0x20
//	}, {
//		0xff, 0xff
//	}
//};
//
//struct tvp5150_vbi_type {
//	unsigned int vbi_type;
//	unsigned int ini_line;
//	unsigned int end_line;
//	unsigned int by_field :1;
//};
//
//struct i2c_vbi_ram_value {
//	u16 reg;
//	struct tvp5150_vbi_type type;
//	unsigned char values[16];
//};
//
///* This struct have the values for each supported VBI Standard
// * by
// tvp5150_vbi_types should follow the same order as vbi_ram_default
// * value 0 means rom position 0x10, value 1 means rom position 0x30
// * and so on. There are 16 possible locations from 0 to 15.
// */
//
//static struct i2c_vbi_ram_value vbi_ram_default[] = {
//
//	/*
//	 * FIXME: Current api doesn't handle all VBI types, those not
//	 * yet supported are placed under #if 0
//	 */
//#if 0
//	[0] = {0x010, /* Teletext, SECAM, WST System A */
//		{V4L2_SLICED_TELETEXT_SECAM, 6, 23, 1},
//		{ 0xaa, 0xaa, 0xff, 0xff, 0xe7, 0x2e, 0x20, 0x26,
//		  0xe6, 0xb4, 0x0e, 0x00, 0x00, 0x00, 0x10, 0x00 }
//	},
//#endif
//	[1] = {0x030, /* Teletext, PAL, WST System B */
//		{V4L2_SLICED_TELETEXT_B, 6, 22, 1},
//		{ 0xaa, 0xaa, 0xff, 0xff, 0x27, 0x2e, 0x20, 0x2b,
//		  0xa6, 0x72, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00 }
//	},
//#if 0
//	[2] = {0x050, /* Teletext, PAL, WST System C */
//		{V4L2_SLICED_TELETEXT_PAL_C, 6, 22, 1},
//		{ 0xaa, 0xaa, 0xff, 0xff, 0xe7, 0x2e, 0x20, 0x22,
//		  0xa6, 0x98, 0x0d, 0x00, 0x00, 0x00, 0x10, 0x00 }
//	},
//	[3] = {0x070, /* Teletext, NTSC, WST System B */
//		{V4L2_SLICED_TELETEXT_NTSC_B, 10, 21, 1},
//		{ 0xaa, 0xaa, 0xff, 0xff, 0x27, 0x2e, 0x20, 0x23,
//		  0x69, 0x93, 0x0d, 0x00, 0x00, 0x00, 0x10, 0x00 }
//	},
//	[4] = {0x090, /* Tetetext, NTSC NABTS System C */
//		{V4L2_SLICED_TELETEXT_NTSC_C, 10, 21, 1},
//		{ 0xaa, 0xaa, 0xff, 0xff, 0xe7, 0x2e, 0x20, 0x22,
//		  0x69, 0x93, 0x0d, 0x00, 0x00, 0x00, 0x15, 0x00 }
//	},
//	[5] = {0x0b0, /* Teletext, NTSC-J, NABTS System D */
//		{V4L2_SLICED_TELETEXT_NTSC_D, 10, 21, 1},
//		{ 0xaa, 0xaa, 0xff, 0xff, 0xa7, 0x2e, 0x20, 0x23,
//		  0x69, 0x93, 0x0d, 0x00, 0x00, 0x00, 0x10, 0x00 }
//	},
//	[6] = {0x0d0, /* Closed Caption, PAL/SECAM */
//		{V4L2_SLICED_CAPTION_625, 22, 22, 1},
//		{ 0xaa, 0x2a, 0xff, 0x3f, 0x04, 0x51, 0x6e, 0x02,
//		  0xa6, 0x7b, 0x09, 0x00, 0x00, 0x00, 0x27, 0x00 }
//	},
//#endif
//	[7] = {0x0f0, /* Closed Caption, NTSC */
//		{V4L2_SLICED_CAPTION_525, 21, 21, 1},
//		{ 0xaa, 0x2a, 0xff, 0x3f, 0x04, 0x51, 0x6e, 0x02,
//		  0x69, 0x8c, 0x09, 0x00, 0x00, 0x00, 0x27, 0x00 }
//	},
//	[8] = {0x110, /* Wide Screen Signal, PAL/SECAM */
//		{V4L2_SLICED_WSS_625, 23, 23, 1},
//		{ 0x5b, 0x55, 0xc5, 0xff, 0x00, 0x71, 0x6e, 0x42,
//		  0xa6, 0xcd, 0x0f, 0x00, 0x00, 0x00, 0x3a, 0x00 }
//	},
//#if 0
//	[9] = {0x130, /* Wide Screen Signal, NTSC C */
//		{V4L2_SLICED_WSS_525, 20, 20, 1},
//		{ 0x38, 0x00, 0x3f, 0x00, 0x00, 0x71, 0x6e, 0x43,
//		  0x69, 0x7c, 0x08, 0x00, 0x00, 0x00, 0x39, 0x00 }
//	},
//	[10] = {0x150, /* Vertical Interval Timecode (VITC), PAL/SECAM */
//		{V4l2_SLICED_VITC_625, 6, 22, 0},
//		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x8f, 0x6d, 0x49,
//		  0xa6, 0x85, 0x08, 0x00, 0x00, 0x00, 0x4c, 0x00 }
//	},
//	[11] = {0x170, /* Vertical Interval Timecode (VITC), NTSC */
//		{V4l2_SLICED_VITC_525, 10, 20, 0},
//		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x8f, 0x6d, 0x49,
//		  0x69, 0x94, 0x08, 0x00, 0x00, 0x00, 0x4c, 0x00 }
//	},
//#endif
//	[12] = {0x190, /* Video Program System (VPS), PAL */
//		{V4L2_SLICED_VPS, 16, 16, 0},
//		{ 0xaa, 0xaa, 0xff, 0xff, 0xba, 0xce, 0x2b, 0x0d,
//		  0xa6, 0xda, 0x0b, 0x00, 0x00, 0x00, 0x60, 0x00 }
//	},
//	/* 0x1d0 User programmable */
//};
//
//static int tvp5150_write_inittab(struct v4l2_subdev *sd,
//				const struct i2c_reg_value *regs)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//
//	while (regs->reg != 0xff) {
//		regmap_write(decoder->regmap, regs->reg, regs->value);
//		regs++;
//	}
//	return 0;
//}
//
//static int tvp5150_vdp_init(struct v4l2_subdev *sd)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	struct regmap *map = decoder->regmap;
//	unsigned int i;
//	int j;
//
//	/* Disable Full Field */
//	regmap_write(map, TVP5150_FULL_FIELD_ENA, 0);
//
//	/* Before programming, Line mode should be at 0xff */
//	for (i = TVP5150_LINE_MODE_INI; i <= TVP5150_LINE_MODE_END; i++)
//		regmap_write(map, i, 0xff);
//
//	/* Load Ram Table */
//	for (j = 0; j < ARRAY_SIZE(vbi_ram_default); j++) {
//		const struct i2c_vbi_ram_value *regs = &vbi_ram_default[j];
//
//		if (!regs->type.vbi_type)
//			continue;
//
//		regmap_write(map, TVP5150_CONF_RAM_ADDR_HIGH, regs->reg >> 8);
//		regmap_write(map, TVP5150_CONF_RAM_ADDR_LOW, regs->reg);
//
//		for (i = 0; i < 16; i++)
//			regmap_write(map, TVP5150_VDP_CONF_RAM_DATA,
//				     regs->values[i]);
//	}
//	return 0;
//}
//
///* Fills VBI capabilities based on i2c_vbi_ram_value struct */
//static int tvp5150_g_sliced_vbi_cap(struct v4l2_subdev *sd,
//				struct v4l2_sliced_vbi_cap *cap)
//{
//	int line, i;
//
//	dev_dbg_lvl(sd->dev, 1, debug, "g_sliced_vbi_cap\n");
//	memset(cap, 0, sizeof(*cap));
//
//	for (i = 0; i < ARRAY_SIZE(vbi_ram_default); i++) {
//		const struct i2c_vbi_ram_value *regs = &vbi_ram_default[i];
//
//		if (!regs->type.vbi_type)
//			continue;
//
//		for (line = regs->type.ini_line;
//		     line <= regs->type.end_line;
//		     line++) {
//			cap->service_lines[0][line] |= regs->type.vbi_type;
//		}
//		cap->service_set |= regs->type.vbi_type;
//	}
//	return 0;
//}
//
///* Set vbi processing
// * type - one of tvp5150_vbi_types
// * line - line to gather data
// * fields: bit 0 field1, bit 1, field2
// * flags (default=0xf0) is a bitmask, were set means:
// *	bit 7: enable filtering null bytes on CC
// *	bit 6: send data also to FIFO
// *	bit 5: don't allow data with errors on FIFO
// *	bit 4: enable ECC when possible
// * pix_align = pix alignment:
// *	LSB = field1
// *	MSB = field2
// */
//static int tvp5150_set_vbi(struct v4l2_subdev *sd,
//			unsigned int type, u8 flags, int line,
//			const int fields)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	v4l2_std_id std = decoder->norm;
//	u8 reg;
//	int i, pos = 0;
//
//	if (std == V4L2_STD_ALL) {
//		dev_err(sd->dev, "VBI can't be configured without knowing number of lines\n");
//		return 0;
//	} else if (std & V4L2_STD_625_50) {
//		/* Don't follow NTSC Line number convension */
//		line += 3;
//	}
//
//	if (line < 6 || line > 27)
//		return 0;
//
//	for (i = 0; i < ARRAY_SIZE(vbi_ram_default); i++) {
//		const struct i2c_vbi_ram_value *regs =  &vbi_ram_default[i];
//
//		if (!regs->type.vbi_type)
//			continue;
//
//		if ((type & regs->type.vbi_type) &&
//		    (line >= regs->type.ini_line) &&
//		    (line <= regs->type.end_line))
//			break;
//		pos++;
//	}
//
//	type = pos | (flags & 0xf0);
//	reg = ((line - 6) << 1) + TVP5150_LINE_MODE_INI;
//
//	if (fields & 1)
//		regmap_write(decoder->regmap, reg, type);
//
//	if (fields & 2)
//		regmap_write(decoder->regmap, reg + 1, type);
//
//	return type;
//}
//
//static int tvp5150_get_vbi(struct v4l2_subdev *sd, int line)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	v4l2_std_id std = decoder->norm;
//	u8 reg;
//	int pos, type = 0;
//	int i, ret = 0;
//
//	if (std == V4L2_STD_ALL) {
//		dev_err(sd->dev, "VBI can't be configured without knowing number of lines\n");
//		return 0;
//	} else if (std & V4L2_STD_625_50) {
//		/* Don't follow NTSC Line number convension */
//		line += 3;
//	}
//
//	if (line < 6 || line > 27)
//		return 0;
//
//	reg = ((line - 6) << 1) + TVP5150_LINE_MODE_INI;
//
//	for (i = 0; i <= 1; i++) {
//		ret = tvp5150_read(sd, reg + i);
//		if (ret < 0) {
//			dev_err(sd->dev, "%s: failed with error = %d\n",
//				 __func__, ret);
//			return 0;
//		}
//		pos = ret & 0x0f;
//		if (pos < ARRAY_SIZE(vbi_ram_default))
//			type |= vbi_ram_default[pos].type.vbi_type;
//	}
//
//	return type;
//}
//
//static int tvp5150_set_std(struct v4l2_subdev *sd, v4l2_std_id std)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	int fmt = 0;
//
//	/* First tests should be against specific std */
//
//	if (std == V4L2_STD_NTSC_443) {
//		fmt = VIDEO_STD_NTSC_4_43_BIT;
//	} else if (std == V4L2_STD_PAL_M) {
//		fmt = VIDEO_STD_PAL_M_BIT;
//	} else if (std == V4L2_STD_PAL_N || std == V4L2_STD_PAL_Nc) {
//		fmt = VIDEO_STD_PAL_COMBINATION_N_BIT;
//	} else {
//		/* Then, test against generic ones */
//		if (std & V4L2_STD_NTSC)
//			fmt = VIDEO_STD_NTSC_MJ_BIT;
//		else if (std & V4L2_STD_PAL)
//			fmt = VIDEO_STD_PAL_BDGHIN_BIT;
//		else if (std & V4L2_STD_SECAM)
//			fmt = VIDEO_STD_SECAM_BIT;
//	}
//
//	dev_dbg_lvl(sd->dev, 1, debug, "Set video std register to %d.\n", fmt);
//	regmap_write(decoder->regmap, TVP5150_VIDEO_STD, fmt);
//	return 0;
//}
//
//static int tvp5150_g_std(struct v4l2_subdev *sd, v4l2_std_id *std)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//
//	*std = decoder->norm;
//
//	return 0;
//}
//
//static int tvp5150_s_std(struct v4l2_subdev *sd, v4l2_std_id std)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	struct tvp5150_connector *cur_con = decoder->cur_connector;
//	v4l2_std_id supported_stds;
//
//	if (decoder->norm == std)
//		return 0;
//
//	/* In case of no of-connectors are available no limitations are made */
//	if (!decoder->connectors_num)
//		supported_stds = V4L2_STD_ALL;
//	else
//		supported_stds = cur_con->base.connector.analog.sdtv_stds;
//
//	/*
//	 * Check if requested std or group of std's is/are supported by the
//	 * connector.
//	 */
//	if ((supported_stds & std) == 0)
//		return -EINVAL;
//
//	/* Change cropping height limits */
//	if (std & V4L2_STD_525_60)
//		decoder->rect.height = TVP5150_V_MAX_525_60;
//	else
//		decoder->rect.height = TVP5150_V_MAX_OTHERS;
//
//	/* Set only the specific supported std in case of group of std's. */
//	decoder->norm = supported_stds & std;
//
//	return tvp5150_set_std(sd, std);
//}
//
//static v4l2_std_id tvp5150_read_std(struct v4l2_subdev *sd)
//{
//	int val = tvp5150_read(sd, TVP5150_STATUS_REG_5);
//
//	switch (val & 0x0F) {
//	case 0x01:
//		return V4L2_STD_NTSC;
//	case 0x03:
//		return V4L2_STD_PAL;
//	case 0x05:
//		return V4L2_STD_PAL_M;
//	case 0x07:
//		return V4L2_STD_PAL_N | V4L2_STD_PAL_Nc;
//	case 0x09:
//		return V4L2_STD_NTSC_443;
//	case 0xb:
//		return V4L2_STD_SECAM;
//	default:
//		return V4L2_STD_UNKNOWN;
//	}
//}
//
//static int query_lock(struct v4l2_subdev *sd)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	int status;
//
//	if (decoder->irq)
//		return decoder->lock;
//
//	regmap_read(decoder->regmap, TVP5150_STATUS_REG_1, &status);
//
//	/* For standard detection, we need the 3 locks */
//	return (status & 0x0e) == 0x0e;
//}
//
//static int tvp5150_querystd(struct v4l2_subdev *sd, v4l2_std_id *std_id)
//{
//	*std_id = query_lock(sd) ? tvp5150_read_std(sd) : V4L2_STD_UNKNOWN;
//
//	return 0;
//}
//
//static const struct v4l2_event tvp5150_ev_fmt = {
//	.type = V4L2_EVENT_SOURCE_CHANGE,
//	.u.src_change.changes = V4L2_EVENT_SRC_CH_RESOLUTION,
//};
//
//static irqreturn_t tvp5150_isr(int irq, void *dev_id)
//{
//	struct tvp5150 *decoder = dev_id;
//	struct regmap *map = decoder->regmap;
//	unsigned int mask, active = 0, status = 0;
//
//	mask = TVP5150_MISC_CTL_YCBCR_OE | TVP5150_MISC_CTL_SYNC_OE |
//	       TVP5150_MISC_CTL_CLOCK_OE;
//
//	regmap_read(map, TVP5150_INT_STATUS_REG_A, &status);
//	if (status) {
//		regmap_write(map, TVP5150_INT_STATUS_REG_A, status);
//
//		if (status & TVP5150_INT_A_LOCK) {
//			decoder->lock = !!(status & TVP5150_INT_A_LOCK_STATUS);
//			dev_dbg_lvl(decoder->sd.dev, 1, debug,
//				    "sync lo%s signal\n",
//				    decoder->lock ? "ck" : "ss");
//			v4l2_subdev_notify_event(&decoder->sd, &tvp5150_ev_fmt);
//			regmap_update_bits(map, TVP5150_MISC_CTL, mask,
//					   decoder->lock ? decoder->oe : 0);
//		}
//
//		return IRQ_HANDLED;
//	}
//
//	regmap_read(map, TVP5150_INT_ACTIVE_REG_B, &active);
//	if (active) {
//		status = 0;
//		regmap_read(map, TVP5150_INT_STATUS_REG_B, &status);
//		if (status)
//			regmap_write(map, TVP5150_INT_RESET_REG_B, status);
//	}
//
//	return IRQ_HANDLED;
//}
//
//static int tvp5150_reset(struct v4l2_subdev *sd, u32 val)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	struct regmap *map = decoder->regmap;
//
//	/* Initializes TVP5150 to its default values */
//	tvp5150_write_inittab(sd, tvp5150_init_default);
//
//	if (decoder->irq) {
//		/* Configure pins: FID, VSYNC, INTREQ, SCLK */
//		regmap_write(map, TVP5150_CONF_SHARED_PIN, 0x0);
//		/* Set interrupt polarity to active high */
//		regmap_write(map, TVP5150_INT_CONF, TVP5150_VDPOE | 0x1);
//		regmap_write(map, TVP5150_INTT_CONFIG_REG_B, 0x1);
//	} else {
//		/* Configure pins: FID, VSYNC, GPCL/VBLK, SCLK */
//		regmap_write(map, TVP5150_CONF_SHARED_PIN, 0x2);
//		/* Keep interrupt polarity active low */
//		regmap_write(map, TVP5150_INT_CONF, TVP5150_VDPOE);
//		regmap_write(map, TVP5150_INTT_CONFIG_REG_B, 0x0);
//	}
//
//	/* Initializes VDP registers */
//	tvp5150_vdp_init(sd);
//
//	/* Selects decoder input */
//	tvp5150_selmux(sd);
//
//	/* Initialize image preferences */
//	v4l2_ctrl_handler_setup(&decoder->hdl);
//
//	return 0;
//}
//
//static int tvp5150_enable(struct v4l2_subdev *sd)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	v4l2_std_id std;
//
//	/* Initializes TVP5150 to stream enabled values */
//	tvp5150_write_inittab(sd, tvp5150_init_enable);
//
//	if (decoder->norm == V4L2_STD_ALL)
//		std = tvp5150_read_std(sd);
//	else
//		std = decoder->norm;
//
//	/* Disable autoswitch mode */
//	tvp5150_set_std(sd, std);
//
//	/*
//	 * Enable the YCbCr and clock outputs. In discrete sync mode
//	 * (non-BT.656) additionally enable the sync outputs.
//	 */
//	switch (decoder->mbus_type) {
//	case V4L2_MBUS_PARALLEL:
//		/* 8-bit 4:2:2 YUV with discrete sync output */
//		regmap_update_bits(decoder->regmap, TVP5150_DATA_RATE_SEL,
//				   0x7, 0x0);
//		decoder->oe = TVP5150_MISC_CTL_YCBCR_OE |
//			      TVP5150_MISC_CTL_CLOCK_OE |
//			      TVP5150_MISC_CTL_SYNC_OE;
//		break;
//	case V4L2_MBUS_BT656:
//		decoder->oe = TVP5150_MISC_CTL_YCBCR_OE |
//			      TVP5150_MISC_CTL_CLOCK_OE;
//		break;
//	default:
//		return -EINVAL;
//	}
//
//	return 0;
//};
//
//static int tvp5150_s_ctrl(struct v4l2_ctrl *ctrl)
//{
//	struct v4l2_subdev *sd = to_sd(ctrl);
//	struct tvp5150 *decoder = to_tvp5150(sd);
//
//	switch (ctrl->id) {
//	case V4L2_CID_BRIGHTNESS:
//		regmap_write(decoder->regmap, TVP5150_BRIGHT_CTL, ctrl->val);
//		return 0;
//	case V4L2_CID_CONTRAST:
//		regmap_write(decoder->regmap, TVP5150_CONTRAST_CTL, ctrl->val);
//		return 0;
//	case V4L2_CID_SATURATION:
//		regmap_write(decoder->regmap, TVP5150_SATURATION_CTL,
//			     ctrl->val);
//		return 0;
//	case V4L2_CID_HUE:
//		regmap_write(decoder->regmap, TVP5150_HUE_CTL, ctrl->val);
//		return 0;
//	case V4L2_CID_TEST_PATTERN:
//		decoder->enable = ctrl->val ? false : true;
//		tvp5150_selmux(sd);
//		return 0;
//	}
//	return -EINVAL;
//}
//
//static void tvp5150_set_default(v4l2_std_id std, struct v4l2_rect *crop)
//{
//	/* Default is no cropping */
//	crop->top = 0;
//	crop->left = 0;
//	crop->width = TVP5150_H_MAX;
//	if (std & V4L2_STD_525_60)
//		crop->height = TVP5150_V_MAX_525_60;
//	else
//		crop->height = TVP5150_V_MAX_OTHERS;
//}
//
//static struct v4l2_rect *
//tvp5150_get_pad_crop(struct tvp5150 *decoder,
//		     struct v4l2_subdev_state *sd_state, unsigned int pad,
//		     enum v4l2_subdev_format_whence which)
//{
//	switch (which) {
//	case V4L2_SUBDEV_FORMAT_ACTIVE:
//		return &decoder->rect;
//	case V4L2_SUBDEV_FORMAT_TRY:
//#if defined(CONFIG_VIDEO_V4L2_SUBDEV_API)
//		return v4l2_subdev_get_try_crop(&decoder->sd, sd_state, pad);
//#else
//		return ERR_PTR(-EINVAL);
//#endif
//	default:
//		return ERR_PTR(-EINVAL);
//	}
//}
//
//static int tvp5150_fill_fmt(struct v4l2_subdev *sd,
//			    struct v4l2_subdev_state *sd_state,
//			    struct v4l2_subdev_format *format)
//{
//	struct v4l2_mbus_framefmt *f;
//	struct tvp5150 *decoder = to_tvp5150(sd);
//
//	if (!format || (format->pad != TVP5150_PAD_VID_OUT))
//		return -EINVAL;
//
//	f = &format->format;
//
//	f->width = decoder->rect.width;
//	f->height = decoder->rect.height / 2;
//
//	f->code = TVP5150_MBUS_FMT;
//	f->field = TVP5150_FIELD;
//	f->colorspace = TVP5150_COLORSPACE;
//
//	dev_dbg_lvl(sd->dev, 1, debug, "width = %d, height = %d\n", f->width,
//		    f->height);
//	return 0;
//}
//
//static unsigned int tvp5150_get_hmax(struct v4l2_subdev *sd)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	v4l2_std_id std;
//
//	/* Calculate height based on current standard */
//	if (decoder->norm == V4L2_STD_ALL)
//		std = tvp5150_read_std(sd);
//	else
//		std = decoder->norm;
//
//	return (std & V4L2_STD_525_60) ?
//		TVP5150_V_MAX_525_60 : TVP5150_V_MAX_OTHERS;
//}
//
//static void tvp5150_set_hw_selection(struct v4l2_subdev *sd,
//				     struct v4l2_rect *rect)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	unsigned int hmax = tvp5150_get_hmax(sd);
//
//	regmap_write(decoder->regmap, TVP5150_VERT_BLANKING_START, rect->top);
//	regmap_write(decoder->regmap, TVP5150_VERT_BLANKING_STOP,
//		     rect->top + rect->height - hmax);
//	regmap_write(decoder->regmap, TVP5150_ACT_VD_CROP_ST_MSB,
//		     rect->left >> TVP5150_CROP_SHIFT);
//	regmap_write(decoder->regmap, TVP5150_ACT_VD_CROP_ST_LSB,
//		     rect->left | (1 << TVP5150_CROP_SHIFT));
//	regmap_write(decoder->regmap, TVP5150_ACT_VD_CROP_STP_MSB,
//		     (rect->left + rect->width - TVP5150_MAX_CROP_LEFT) >>
//		     TVP5150_CROP_SHIFT);
//	regmap_write(decoder->regmap, TVP5150_ACT_VD_CROP_STP_LSB,
//		     rect->left + rect->width - TVP5150_MAX_CROP_LEFT);
//}
//
//static int tvp5150_set_selection(struct v4l2_subdev *sd,
//				 struct v4l2_subdev_state *sd_state,
//				 struct v4l2_subdev_selection *sel)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	struct v4l2_rect *rect = &sel->r;
//	struct v4l2_rect *crop;
//	unsigned int hmax;
//
//	if (sel->target != V4L2_SEL_TGT_CROP)
//		return -EINVAL;
//
//	dev_dbg_lvl(sd->dev, 1, debug, "%s left=%d, top=%d, width=%d, height=%d\n",
//		__func__, rect->left, rect->top, rect->width, rect->height);
//
//	/* tvp5150 has some special limits */
//	rect->left = clamp(rect->left, 0, TVP5150_MAX_CROP_LEFT);
//	rect->top = clamp(rect->top, 0, TVP5150_MAX_CROP_TOP);
//	hmax = tvp5150_get_hmax(sd);
//
//	/*
//	 * alignments:
//	 *  - width = 2 due to UYVY colorspace
//	 *  - height, image = no special alignment
//	 */
//	v4l_bound_align_image(&rect->width,
//			      TVP5150_H_MAX - TVP5150_MAX_CROP_LEFT - rect->left,
//			      TVP5150_H_MAX - rect->left, 1, &rect->height,
//			      hmax - TVP5150_MAX_CROP_TOP - rect->top,
//			      hmax - rect->top, 0, 0);
//
//	if (!IS_ENABLED(CONFIG_VIDEO_V4L2_SUBDEV_API) &&
//	    sel->which == V4L2_SUBDEV_FORMAT_TRY)
//		return 0;
//
//	crop = tvp5150_get_pad_crop(decoder, sd_state, sel->pad, sel->which);
//	if (IS_ERR(crop))
//		return PTR_ERR(crop);
//
//	/*
//	 * Update output image size if the selection (crop) rectangle size or
//	 * position has been modified.
//	 */
//	if (sel->which == V4L2_SUBDEV_FORMAT_ACTIVE &&
//	    !v4l2_rect_equal(rect, crop))
//		tvp5150_set_hw_selection(sd, rect);
//
//	*crop = *rect;
//
//	return 0;
//}
//
//static int tvp5150_get_selection(struct v4l2_subdev *sd,
//				 struct v4l2_subdev_state *sd_state,
//				 struct v4l2_subdev_selection *sel)
//{
//	struct tvp5150 *decoder = container_of(sd, struct tvp5150, sd);
//	struct v4l2_rect *crop;
//	v4l2_std_id std;
//
//	switch (sel->target) {
//	case V4L2_SEL_TGT_CROP_BOUNDS:
//		sel->r.left = 0;
//		sel->r.top = 0;
//		sel->r.width = TVP5150_H_MAX;
//
//		/* Calculate height based on current standard */
//		if (decoder->norm == V4L2_STD_ALL)
//			std = tvp5150_read_std(sd);
//		else
//			std = decoder->norm;
//		if (std & V4L2_STD_525_60)
//			sel->r.height = TVP5150_V_MAX_525_60;
//		else
//			sel->r.height = TVP5150_V_MAX_OTHERS;
//		return 0;
//	case V4L2_SEL_TGT_CROP:
//		crop = tvp5150_get_pad_crop(decoder, sd_state, sel->pad,
//					    sel->which);
//		if (IS_ERR(crop))
//			return PTR_ERR(crop);
//		sel->r = *crop;
//		return 0;
//	default:
//		return -EINVAL;
//	}
//}
//
//static int tvp5150_get_mbus_config(struct v4l2_subdev *sd,
//				   unsigned int pad,
//				   struct v4l2_mbus_config *cfg)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//
//	cfg->type = decoder->mbus_type;
//	cfg->bus.parallel.flags = V4L2_MBUS_MASTER
//				| V4L2_MBUS_PCLK_SAMPLE_RISING
//				| V4L2_MBUS_FIELD_EVEN_LOW
//				| V4L2_MBUS_DATA_ACTIVE_HIGH;
//
//	return 0;
//}
//
///****************************************************************************
//			V4L2 subdev pad ops
// ****************************************************************************/
//static int tvp5150_init_cfg(struct v4l2_subdev *sd,
//			    struct v4l2_subdev_state *sd_state)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	v4l2_std_id std;
//
//	/*
//	 * Reset selection to maximum on subdev_open() if autodetection is on
//	 * and a standard change is detected.
//	 */
//	if (decoder->norm == V4L2_STD_ALL) {
//		std = tvp5150_read_std(sd);
//		if (std != decoder->detected_norm) {
//			decoder->detected_norm = std;
//			tvp5150_set_default(std, &decoder->rect);
//		}
//	}
//
//	return 0;
//}
//
//static int tvp5150_enum_mbus_code(struct v4l2_subdev *sd,
//		struct v4l2_subdev_state *sd_state,
//		struct v4l2_subdev_mbus_code_enum *code)
//{
//	if (code->pad || code->index)
//		return -EINVAL;
//
//	code->code = TVP5150_MBUS_FMT;
//	return 0;
//}
//
//static int tvp5150_enum_frame_size(struct v4l2_subdev *sd,
//				   struct v4l2_subdev_state *sd_state,
//				   struct v4l2_subdev_frame_size_enum *fse)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//
//	if (fse->index >= 8 || fse->code != TVP5150_MBUS_FMT)
//		return -EINVAL;
//
//	fse->code = TVP5150_MBUS_FMT;
//	fse->min_width = decoder->rect.width;
//	fse->max_width = decoder->rect.width;
//	fse->min_height = decoder->rect.height / 2;
//	fse->max_height = decoder->rect.height / 2;
//
//	return 0;
//}
//
///****************************************************************************
// *			Media entity ops
// ****************************************************************************/
//#if defined(CONFIG_MEDIA_CONTROLLER)
//static int tvp5150_set_link(struct media_pad *connector_pad,
//			    struct media_pad *tvp5150_pad, u32 flags)
//{
//	struct media_link *link;
//
//	link = media_entity_find_link(connector_pad, tvp5150_pad);
//	if (!link)
//		return -EINVAL;
//
//	link->flags = flags;
//	link->reverse->flags = link->flags;
//
//	return 0;
//}
//
//static int tvp5150_disable_all_input_links(struct tvp5150 *decoder)
//{
//	struct media_pad *connector_pad;
//	unsigned int i;
//	int err;
//
//	for (i = 0; i < TVP5150_NUM_PADS - 1; i++) {
//		connector_pad = media_pad_remote_pad_first(&decoder->pads[i]);
//		if (!connector_pad)
//			continue;
//
//		err = tvp5150_set_link(connector_pad, &decoder->pads[i], 0);
//		if (err)
//			return err;
//	}
//
//	return 0;
//}
//
//static int tvp5150_s_routing(struct v4l2_subdev *sd, u32 input, u32 output,
//			     u32 config);
//
//static int tvp5150_link_setup(struct media_entity *entity,
//			      const struct media_pad *tvp5150_pad,
//			      const struct media_pad *remote, u32 flags)
//{
//	struct v4l2_subdev *sd = media_entity_to_v4l2_subdev(entity);
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	struct media_pad *other_tvp5150_pad =
//		&decoder->pads[tvp5150_pad->index ^ 1];
//	struct v4l2_fwnode_connector *v4l2c;
//	bool is_svideo = false;
//	unsigned int i;
//	int err;
//
//	/*
//	 * The TVP5150 state is determined by the enabled sink pad link(s).
//	 * Enabling or disabling the source pad link has no effect.
//	 */
//	if (tvp5150_pad->flags & MEDIA_PAD_FL_SOURCE)
//		return 0;
//
//	/* Check if the svideo connector should be enabled */
//	for (i = 0; i < decoder->connectors_num; i++) {
//		if (remote->entity == &decoder->connectors[i].ent) {
//			v4l2c = &decoder->connectors[i].base;
//			is_svideo = v4l2c->type == V4L2_CONN_SVIDEO;
//			break;
//		}
//	}
//
//	dev_dbg_lvl(sd->dev, 1, debug, "link setup '%s':%d->'%s':%d[%d]",
//		    remote->entity->name, remote->index,
//		    tvp5150_pad->entity->name, tvp5150_pad->index,
//		    flags & MEDIA_LNK_FL_ENABLED);
//	if (is_svideo)
//		dev_dbg_lvl(sd->dev, 1, debug,
//			    "link setup '%s':%d->'%s':%d[%d]",
//			    remote->entity->name, remote->index,
//			    other_tvp5150_pad->entity->name,
//			    other_tvp5150_pad->index,
//			    flags & MEDIA_LNK_FL_ENABLED);
//
//	/*
//	 * The TVP5150 has an internal mux which allows the following setup:
//	 *
//	 * comp-connector1  --\
//	 *		       |---> AIP1A
//	 *		      /
//	 * svideo-connector -|
//	 *		      \
//	 *		       |---> AIP1B
//	 * comp-connector2  --/
//	 *
//	 * We can't rely on user space that the current connector gets disabled
//	 * first before enabling the new connector. Disable all active
//	 * connector links to be on the safe side.
//	 */
//	err = tvp5150_disable_all_input_links(decoder);
//	if (err)
//		return err;
//
//	tvp5150_s_routing(sd, is_svideo ? TVP5150_SVIDEO : tvp5150_pad->index,
//			  flags & MEDIA_LNK_FL_ENABLED ? TVP5150_NORMAL :
//			  TVP5150_BLACK_SCREEN, 0);
//
//	if (flags & MEDIA_LNK_FL_ENABLED) {
//		struct v4l2_fwnode_connector_analog *v4l2ca;
//		u32 new_norm;
//
//		/*
//		 * S-Video connector is conneted to both ports AIP1A and AIP1B.
//		 * Both links must be enabled in one-shot regardless which link
//		 * the user requests.
//		 */
//		if (is_svideo) {
//			err = tvp5150_set_link((struct media_pad *)remote,
//					       other_tvp5150_pad, flags);
//			if (err)
//				return err;
//		}
//
//		if (!decoder->connectors_num)
//			return 0;
//
//		/* Update the current connector */
//		decoder->cur_connector =
//			container_of(remote, struct tvp5150_connector, pad);
//
//		/*
//		 * Do nothing if the new connector supports the same tv-norms as
//		 * the old one.
//		 */
//		v4l2ca = &decoder->cur_connector->base.connector.analog;
//		new_norm = decoder->norm & v4l2ca->sdtv_stds;
//		if (decoder->norm == new_norm)
//			return 0;
//
//		/*
//		 * Fallback to the new connector tv-norms if we can't find any
//		 * common between the current tv-norm and the new one.
//		 */
//		tvp5150_s_std(sd, new_norm ? new_norm : v4l2ca->sdtv_stds);
//	}
//
//	return 0;
//}
//
//static const struct media_entity_operations tvp5150_sd_media_ops = {
//	.link_setup = tvp5150_link_setup,
//};
//#endif
///****************************************************************************
//			I2C Command
// ****************************************************************************/
//static int __maybe_unused tvp5150_runtime_suspend(struct device *dev)
//{
//	struct v4l2_subdev *sd = dev_get_drvdata(dev);
//	struct tvp5150 *decoder = to_tvp5150(sd);
//
//	if (decoder->irq)
//		/* Disable lock interrupt */
//		return regmap_update_bits(decoder->regmap,
//					  TVP5150_INT_ENABLE_REG_A,
//					  TVP5150_INT_A_LOCK, 0);
//	return 0;
//}
//
//static int __maybe_unused tvp5150_runtime_resume(struct device *dev)
//{
//	struct v4l2_subdev *sd = dev_get_drvdata(dev);
//	struct tvp5150 *decoder = to_tvp5150(sd);
//
//	if (decoder->irq)
//		/* Enable lock interrupt */
//		return regmap_update_bits(decoder->regmap,
//					  TVP5150_INT_ENABLE_REG_A,
//					  TVP5150_INT_A_LOCK,
//					  TVP5150_INT_A_LOCK);
//	return 0;
//}
//
//static int tvp5150_s_stream(struct v4l2_subdev *sd, int enable)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	unsigned int mask, val = 0;
//	int ret;
//
//	mask = TVP5150_MISC_CTL_YCBCR_OE | TVP5150_MISC_CTL_SYNC_OE |
//	       TVP5150_MISC_CTL_CLOCK_OE;
//
//	if (enable) {
//		ret = pm_runtime_resume_and_get(sd->dev);
//		if (ret < 0)
//			return ret;
//
//		tvp5150_enable(sd);
//
//		/* Enable outputs if decoder is locked */
//		if (decoder->irq)
//			val = decoder->lock ? decoder->oe : 0;
//		else
//			val = decoder->oe;
//
//		v4l2_subdev_notify_event(&decoder->sd, &tvp5150_ev_fmt);
//	} else {
//		pm_runtime_put(sd->dev);
//	}
//
//	regmap_update_bits(decoder->regmap, TVP5150_MISC_CTL, mask, val);
//
//	return 0;
//}
//
//static int tvp5150_s_routing(struct v4l2_subdev *sd,
//			     u32 input, u32 output, u32 config)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//
//	decoder->input = input;
//	decoder->output = output;
//
//	if (output == TVP5150_BLACK_SCREEN)
//		decoder->enable = false;
//	else
//		decoder->enable = true;
//
//	tvp5150_selmux(sd);
//	return 0;
//}
//
//static int tvp5150_s_raw_fmt(struct v4l2_subdev *sd, struct v4l2_vbi_format *fmt)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//
//	/*
//	 * this is for capturing 36 raw vbi lines
//	 * if there's a way to cut off the beginning 2 vbi lines
//	 * with the tvp5150 then the vbi line count could be lowered
//	 * to 17 lines/field again, although I couldn't find a register
//	 * which could do that cropping
//	 */
//
//	if (fmt->sample_format == V4L2_PIX_FMT_GREY)
//		regmap_write(decoder->regmap, TVP5150_LUMA_PROC_CTL_1, 0x70);
//	if (fmt->count[0] == 18 && fmt->count[1] == 18) {
//		regmap_write(decoder->regmap, TVP5150_VERT_BLANKING_START,
//			     0x00);
//		regmap_write(decoder->regmap, TVP5150_VERT_BLANKING_STOP, 0x01);
//	}
//	return 0;
//}
//
//static int tvp5150_s_sliced_fmt(struct v4l2_subdev *sd, struct v4l2_sliced_vbi_format *svbi)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	int i;
//
//	if (svbi->service_set != 0) {
//		for (i = 0; i <= 23; i++) {
//			svbi->service_lines[1][i] = 0;
//			svbi->service_lines[0][i] =
//				tvp5150_set_vbi(sd, svbi->service_lines[0][i],
//						0xf0, i, 3);
//		}
//		/* Enables FIFO */
//		regmap_write(decoder->regmap, TVP5150_FIFO_OUT_CTRL, 1);
//	} else {
//		/* Disables FIFO*/
//		regmap_write(decoder->regmap, TVP5150_FIFO_OUT_CTRL, 0);
//
//		/* Disable Full Field */
//		regmap_write(decoder->regmap, TVP5150_FULL_FIELD_ENA, 0);
//
//		/* Disable Line modes */
//		for (i = TVP5150_LINE_MODE_INI; i <= TVP5150_LINE_MODE_END; i++)
//			regmap_write(decoder->regmap, i, 0xff);
//	}
//	return 0;
//}
//
//static int tvp5150_g_sliced_fmt(struct v4l2_subdev *sd, struct v4l2_sliced_vbi_format *svbi)
//{
//	int i, mask = 0;
//
//	memset(svbi->service_lines, 0, sizeof(svbi->service_lines));
//
//	for (i = 0; i <= 23; i++) {
//		svbi->service_lines[0][i] =
//			tvp5150_get_vbi(sd, i);
//		mask |= svbi->service_lines[0][i];
//	}
//	svbi->service_set = mask;
//	return 0;
//}
//
//#ifdef CONFIG_VIDEO_ADV_DEBUG
//static int tvp5150_g_register(struct v4l2_subdev *sd, struct v4l2_dbg_register *reg)
//{
//	int res;
//
//	res = tvp5150_read(sd, reg->reg & 0xff);
//	if (res < 0) {
//		dev_err(sd->dev, "%s: failed with error = %d\n", __func__, res);
//		return res;
//	}
//
//	reg->val = res;
//	reg->size = 1;
//	return 0;
//}
//
//static int tvp5150_s_register(struct v4l2_subdev *sd, const struct v4l2_dbg_register *reg)
//{
//	struct tvp5150 *decoder = to_tvp5150(sd);
//
//	return regmap_write(decoder->regmap, reg->reg & 0xff, reg->val & 0xff);
//}
//#endif
//
//static int tvp5150_subscribe_event(struct v4l2_subdev *sd, struct v4l2_fh *fh,
//				   struct v4l2_event_subscription *sub)
//{
//	switch (sub->type) {
//	case V4L2_EVENT_SOURCE_CHANGE:
//		return v4l2_src_change_event_subdev_subscribe(sd, fh, sub);
//	case V4L2_EVENT_CTRL:
//		return v4l2_ctrl_subdev_subscribe_event(sd, fh, sub);
//	default:
//		return -EINVAL;
//	}
//}
//
//static int tvp5150_g_tuner(struct v4l2_subdev *sd, struct v4l2_tuner *vt)
//{
//	int status = tvp5150_read(sd, 0x88);
//
//	vt->signal = ((status & 0x04) && (status & 0x02)) ? 0xffff : 0x0;
//	return 0;
//}
//
//static int tvp5150_registered(struct v4l2_subdev *sd)
//{
//#if defined(CONFIG_MEDIA_CONTROLLER)
//	struct tvp5150 *decoder = to_tvp5150(sd);
//	unsigned int i;
//	int ret;
//
//	/*
//	 * Setup connector pads and links. Enable the link to the first
//	 * available connector per default.
//	 */
//	for (i = 0; i < decoder->connectors_num; i++) {
//		struct media_entity *con = &decoder->connectors[i].ent;
//		struct media_pad *pad = &decoder->connectors[i].pad;
//		struct v4l2_fwnode_connector *v4l2c =
//			&decoder->connectors[i].base;
//		struct v4l2_connector_link *link =
//			v4l2_connector_first_link(v4l2c);
//		unsigned int port = link->fwnode_link.remote_port;
//		unsigned int flags = i ? 0 : MEDIA_LNK_FL_ENABLED;
//		bool is_svideo = v4l2c->type == V4L2_CONN_SVIDEO;
//
//		pad->flags = MEDIA_PAD_FL_SOURCE;
//		ret = media_entity_pads_init(con, 1, pad);
//		if (ret < 0)
//			goto err;
//
//		ret = media_device_register_entity(sd->v4l2_dev->mdev, con);
//		if (ret < 0)
//			goto err;
//
//		ret = media_create_pad_link(con, 0, &sd->entity, port, flags);
//		if (ret < 0)
//			goto err;
//
//		if (is_svideo) {
//			/*
//			 * Check tvp5150_link_setup() comments for more
//			 * information.
//			 */
//			link = v4l2_connector_last_link(v4l2c);
//			port = link->fwnode_link.remote_port;
//			ret = media_create_pad_link(con, 0, &sd->entity, port,
//						    flags);
//			if (ret < 0)
//				goto err;
//		}
//
//		/* Enable default input. */
//		if (flags == MEDIA_LNK_FL_ENABLED) {
//			decoder->input =
//				is_svideo ? TVP5150_SVIDEO :
//				port == 0 ? TVP5150_COMPOSITE0 :
//				TVP5150_COMPOSITE1;
//
//			tvp5150_selmux(sd);
//			decoder->cur_connector = &decoder->connectors[i];
//			tvp5150_s_std(sd, v4l2c->connector.analog.sdtv_stds);
//		}
//	}
//
//	return 0;
//
//err:
//	for (i = 0; i < decoder->connectors_num; i++) {
//		media_device_unregister_entity(&decoder->connectors[i].ent);
//		media_entity_cleanup(&decoder->connectors[i].ent);
//	}
//	return ret;
//#endif
//
//	return 0;
//}
//
//static int tvp5150_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
//{
//	return pm_runtime_resume_and_get(sd->dev);
//}
//
//static int tvp5150_close(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
//{
//	pm_runtime_put(sd->dev);
//
//	return 0;
//}
//
///* ----------------------------------------------------------------------- */
//
//static const struct v4l2_ctrl_ops tvp5150_ctrl_ops = {
//	.s_ctrl = tvp5150_s_ctrl,
//};
//
//static const struct v4l2_subdev_core_ops tvp5150_core_ops = {
//	.log_status = tvp5150_log_status,
//	.reset = tvp5150_reset,
//#ifdef CONFIG_VIDEO_ADV_DEBUG
//	.g_register = tvp5150_g_register,
//	.s_register = tvp5150_s_register,
//#endif
//	.subscribe_event = tvp5150_subscribe_event,
//	.unsubscribe_event = v4l2_event_subdev_unsubscribe,
//};
//
//static const struct v4l2_subdev_tuner_ops tvp5150_tuner_ops = {
//	.g_tuner = tvp5150_g_tuner,
//};
//
//static const struct v4l2_subdev_video_ops tvp5150_video_ops = {
//	.s_std = tvp5150_s_std,
//	.g_std = tvp5150_g_std,
//	.querystd = tvp5150_querystd,
//	.s_stream = tvp5150_s_stream,
//	.s_routing = tvp5150_s_routing,
//};
//
//static const struct v4l2_subdev_vbi_ops tvp5150_vbi_ops = {
//	.g_sliced_vbi_cap = tvp5150_g_sliced_vbi_cap,
//	.g_sliced_fmt = tvp5150_g_sliced_fmt,
//	.s_sliced_fmt = tvp5150_s_sliced_fmt,
//	.s_raw_fmt = tvp5150_s_raw_fmt,
//};
//
//static const struct v4l2_subdev_pad_ops tvp5150_pad_ops = {
//	.init_cfg = tvp5150_init_cfg,
//	.enum_mbus_code = tvp5150_enum_mbus_code,
//	.enum_frame_size = tvp5150_enum_frame_size,
//	.set_fmt = tvp5150_fill_fmt,
//	.get_fmt = tvp5150_fill_fmt,
//	.get_selection = tvp5150_get_selection,
//	.set_selection = tvp5150_set_selection,
//	.get_mbus_config = tvp5150_get_mbus_config,
//};
//
//static const struct v4l2_subdev_ops tvp5150_ops = {
//	.core = &tvp5150_core_ops,
//	.tuner = &tvp5150_tuner_ops,
//	.video = &tvp5150_video_ops,
//	.vbi = &tvp5150_vbi_ops,
//	.pad = &tvp5150_pad_ops,
//};
//
//static const struct v4l2_subdev_internal_ops tvp5150_internal_ops = {
//	.registered = tvp5150_registered,
//	.open = tvp5150_open,
//	.close = tvp5150_close,
//};
//

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tvp_write_reg_func(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
tvp5150_struct *p_ctl;
	p_ctl = (tvp5150_struct *)ad32;
	TVP_Write(p_ctl,p_ctl->addr,*p8_data);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void tvp_write_reg_func_tmp_off(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
//tvp5150_struct *p_ctl;
//	p_ctl = (tvp5150_struct *)ad32;
//	uint8_t i2cbuf[3];
//	i2cbuf[0] = 1;
//	i2cbuf[1] = video_cmd_off;
//	if (tvp_write_buffer(p_ctl->p_hi2c, 2, i2cbuf, 2)){
//		p_ctl->error |= 1;
//	}
//
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void tvp_write_reg_func_tmp_sw(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
//tvp5150_struct *p_ctl;
//	p_ctl = (tvp5150_struct *)ad32;
//	uint8_t i2cbuf[3];
//	i2cbuf[0] = 2;
//	i2cbuf[1] = num;
//	i2cbuf[2] = * p8_data;
//	if (tvp_write_buffer(p_ctl->p_hi2c, 2, i2cbuf, 3)){
//		p_ctl->error |= 1;
//	}
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void tvp_write_reg_func_tmp_freq(uint32_t ad32, uint32_t num, uint32_t tmp, uint8_t* p8_data){
//tvp5150_struct *p_ctl;
//	p_ctl = (tvp5150_struct *)ad32;
//	uint8_t i2cbuf[4];
//	i2cbuf[0] = 3;
//	i2cbuf[1] = video_cmd_set_freq;
//	i2cbuf[2] = p8_data[0];
//	i2cbuf[3] = p8_data[1];
//	if (tvp_write_buffer(p_ctl->p_hi2c, 2, i2cbuf, 4)){
//		p_ctl->error |= 1;
//	}
//}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* tvp_read_reg_func(uint32_t ad32, uint32_t num, uint32_t tmp){
	tvp5150_struct *p_ctl;
	p_ctl = (tvp5150_struct *)ad32;
	p_ctl->reg = TVP_Read(p_ctl,p_ctl->addr);
	return (uint8_t*)&p_ctl->reg;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t tvp_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, tvp5150_struct* p_ctl, const char *p_name[TVP5150_MAX_NUM_REG]){
SettingCell_t 	cell;
uint32_t 		i;
	i=0;

	for(i=0;i<COMPONENT_MAX_TABLE_SIZE;i++){
		if(!p_cell[i].CellNumber) break;
	}
	if( i>= COMPONENT_MAX_TABLE_SIZE) return num;

	cell.CellAttr = RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 2;
	cell.VarPtr = &p_ctl->addr;
	cell.ReadProc = 0;
	cell.WriteProc = 0;
	cell.HighLim = 0;
	cell.DefaultValue = 0;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = p_name[0];//"tvp addr";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)tvp_read_reg_func;
	cell.WriteProc = (void*)(void*)tvp_write_reg_func;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = p_name[1];//"tvp reg";
	p_cell[i++] = cell;

	cell.CellAttr = RD_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = &p_ctl->error;
	cell.ReadProc = 0;
	cell.WriteProc = 0;
	cell.HighLim = 0;
	cell.DefaultValue = 0;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = p_name[2];//"tvp err";
	p_cell[i++] = cell;

//	cell.CellAttr = Action_Att + RD_Att + WR_Att;
//	cell.CellType = FixIntegerCellType + 1;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)tvp_read_reg_func;
//	cell.WriteProc = (void*)(void*)tvp_write_reg_func_tmp_off;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.CellNumber = num++;
//	cell.LowLim = 0;
//	cell.DescriptStr = "pwr off";//"tvp reg";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = Action_Att + WR_Att;
//	cell.CellType = FixIntegerCellType + 1;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)tvp_read_reg_func;
//	cell.WriteProc = (void*)(void*)tvp_write_reg_func_tmp_sw;
//	cell.HighLim = video_cmd_set_sw_0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.CellNumber = num++;
//	cell.LowLim = video_cmd_set_sw_0;
//	cell.DescriptStr = "sw0";//"tvp reg";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = Action_Att + WR_Att;
//	cell.CellType = FixIntegerCellType + 1;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)tvp_read_reg_func;
//	cell.WriteProc = (void*)(void*)tvp_write_reg_func_tmp_sw;
//	cell.HighLim = video_cmd_set_sw_1;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.CellNumber = num++;
//	cell.LowLim = video_cmd_set_sw_1;
//	cell.DescriptStr = "sw1";//"tvp reg";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = Action_Att + WR_Att;
//	cell.CellType = FixIntegerCellType + 1;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)tvp_read_reg_func;
//	cell.WriteProc = (void*)(void*)tvp_write_reg_func_tmp_sw;
//	cell.HighLim = video_cmd_set_sw_2;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.CellNumber = num++;
//	cell.LowLim = video_cmd_set_sw_2;
//	cell.DescriptStr = "sw2";//"tvp reg";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = Action_Att + RD_Att + WR_Att;
//	cell.CellType = FixIntegerCellType + 2;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)tvp_read_reg_func;
//	cell.WriteProc = (void*)(void*)tvp_write_reg_func_tmp_freq;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.CellNumber = num++;
//	cell.LowLim = 0;
//	cell.DescriptStr = "freq";//"tvp reg";
//	p_cell[i++] = cell;
	return num;

}
