/*
 * switch.h
 *
 *  Created on: 12.03.2013
 *      Author: krasikov
 */

#ifndef SWITCH_COMMON_H_
#define SWITCH_COMMON_H_

#include "debug_cells_lib.h"
#include "shift_reg.h"


typedef struct bits_2_struct{
	uint8_t A:	1;
	uint8_t B:	1;
}bits_2_struct;

typedef enum{
	port_bit_need_update = 0,		// управление ногами ключа через порт мк
	port_bit_ok_state = 1,			// управление ногами ключа через регистр сдвига
} switch_update_control;

typedef enum{
	now_need_update = 0,
	now_not_need_update = 1,
} switch_update_immediate;

typedef enum{
	port_control = 0,		// управление ногами ключа через порт мк
	shift_reg_control = 1,	// управление ногами ключа через регистр сдвига
} switch_type_control;

#endif /* SWITCH_COMMON_H_ */

