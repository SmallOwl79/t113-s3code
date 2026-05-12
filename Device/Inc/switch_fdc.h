/*
 * switch.h
 *
 *  Created on: 12.03.2013
 *      Author: krasikov
 */

#ifndef SWITCH_FDC_H_
#define SWITCH_FDC_H_

#include "switch_common.h"
#include "gpio_sun.h"
#include "debug_cells_lib.h"


typedef enum{
	fdc_on = 0,
	fdc_off = 1,
	fdc_not_control = 2,		// �� ���������� - ��������� ���������
} fdc_switch_state;

typedef struct switch_fdc_struct{
														//�������� � ����� ����� ��� ������ ���������� ������. �� ��
	struct gpio_t_ 			ON_pin;

	switch_type_control		cntrl_mode;					//����� ������ ������
	uint8_t 				b_inveretd_ON		:1;		//�������� ����������????
	uint8_t 				ON					:1;
	switch_update_control 	b_update 			:1;		//���� ��������� - ��������� ����� � ����
	fdc_switch_state		state				:2;		//����� ����� �������


	shift_reg_struct		*p_shift_reg;				//��-�� �� ������� ������, ���� ������ ��������� ����� ����
	uint32_t				ON_mask;					//������� ����� - ��� �������� ������!!!! - ��� CTRL

	uint8_t					tmp;
}switch_fdc_struct;


void switch_fdc_start_init(switch_fdc_struct *p_sw);
void switch_fdc_pin_init(switch_fdc_struct *p_sw);
void switch_fdc_on(switch_fdc_struct *p_sw, fdc_switch_state state);
uint32_t switch_fdc_set_data(switch_fdc_struct *p_sw);
void switch_fdc_on_update(switch_fdc_struct *p_sw, fdc_switch_state state,switch_update_immediate update);
//void switch_fdc_shift_init(switch_fdc_struct *p_fdc, shift_reg_struct *p_shift, uint32_t mask );
void switch_fdc_off_ptr(void *p_void);
void switch_fdc_on_ptr(void *p_void);
uint8_t *switch_fdc_read_state_ptr(void *p_void);
void switch_fdc_set_func(uint32_t fdc32, uint32_t pos, uint32_t tmp);
void switch_fdc_set_func_from_ptr(uint32_t fdc32, uint32_t tmp1, uint32_t tmp2, uint8_t *ptr);
uint8_t* switch_fdc_get_func(uint32_t fdc32, uint32_t tmp1, uint32_t tmp2);
uint8_t* switch_fdc_get_func_state(uint32_t fdc32, uint32_t tmp1, uint32_t tmp2);

void switch_fdc_shift_init(switch_fdc_struct *p_fdc, shift_reg_struct *p_shift,uint32_t mask );
void switch_fdc_start_pin_init(switch_fdc_struct *p_fdc);

void sw_fdc_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, switch_fdc_struct *p_sw, const char *p_name);

#endif /* SWITCH_FDC_H_ */

