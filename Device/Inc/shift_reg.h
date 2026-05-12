/*
 * shift_reg.h
 *
 *  Created on: 06.03.2013
 *      Author: krasikov
 */

#ifndef SHIFT_REG_H_
#define SHIFT_REG_H_
//#include "Driver_SPI.h"
#include "gpio_sun.h"
#include "debug_cells_lib.h"



#define SHIFT_REG_DATA_OUT_OK			1
#define SHIFT_REG_DATA_OUT_EMPTY		0

typedef enum{
	shift_reg_update_ok = 0,
	shift_reg_need_update = 1,
} shift_reg_update;

typedef enum{
	shift_reg_cs_active_low = 0,
	shift_reg_cs_active_high = 1,
} shift_reg_enum_cs_active_state;


typedef struct shift_reg_struct{
//	GPIO_TypeDef 		*OE_port;
//	uint16_t			OE_pin;
//	GPIO_TypeDef 		*CS_port;
//	uint16_t			CS_pin;
	struct gpio_t_ 			CS_pin;
	struct gpio_t_ 			OE_pin;

	uint8_t				len;	//����� �������� ������ � ������
//	ARM_DRIVER_SPI		*p_spi;
//	ARM_SPI_DRIVER_DEVICE_STRUCT		driver_config;

	uint32_t 			*p_data;
	uint32_t 			data;
	uint32_t 			back_up_data;
	volatile uint8_t	status			:1;//���� ��������� ������ ������ � ����
	uint8_t				b_update		:1;//���� ������������� ������ ������ - ��������� ������ � �������
	shift_reg_enum_cs_active_state				cs_active		:1;//
	uint8_t				b_lock_oe_bit	:1;//
	uint8_t				b_wait_out		:1;//�������� ������ � ���� - ����������

}shift_reg_struct;

void shift_reg_call_back_start_send_data(void *p);
void shift_reg_call_back_stop_send_data(void *p);

uint32_t shift_reg_send_data(shift_reg_struct *p_reg);
void shift_reg_enable_oe_data(shift_reg_struct *p_reg);
void shift_reg_disable_oe_data(shift_reg_struct *p_reg);
void shift_reg_start_pin_init(shift_reg_struct *p_reg, shift_reg_enum_cs_active_state cs_active);
void shift_reg_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, shift_reg_struct *p_reg, const char *p_name);

#endif /* SHIFT_REG_H_ */
