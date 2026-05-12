/*
 * w25q64.h
 *
 *  Created on: 8 окт. 2018 г.
 *      Author: krasikov
 */

#ifndef SRC_INC_W25Q64_H_
#define SRC_INC_W25Q64_H_

#include "stdint.h"
#include "gpio_sun.h"
#include "cmsis_os2.h"
//#include "Driver_SPI.h"

#include "debug_cells_lib.h"

#define W25Q64_MANUFACTURE_ID		0xEF
#define W25Q64_DEVICE_ID			0x16

typedef enum{
	w25q64_cmd_write_status_reg1 	= 0x01,
	w25q64_cmd_page_programm 		= 0x02,
	w25q64_cmd_read_data 			= 0x03,
	w25q64_cmd_write_disable		= 0x04,
	w25q64_cmd_read_status_reg1 	= 0x05,
	w25q64_cmd_write_enable			= 0x06,
	w25q64_cmd_sector_erase_4KB		= 0x20,
	w25q64_cmd_block_erase_32KB		= 0x52,
	w25q64_cmd_block_erase_64KB		= 0xD8,
	w25q64_cmd_chip_erase			= 0xC7,
	w25q64_cmd_read_manufacture_id	= 0x90,
	w25q64_cmd_read_JEDEC_id		= 0x9F,
	w25q64_cmd_read_unique_id		= 0x4B,
}w25q64_cmd;

/////////////////////////////////////////////
///
/////////////////////////////////////////////
typedef union w25q64_status_reg1{
	struct{
		uint8_t						busy:		1;
		uint8_t						wel:		1;
		uint8_t						bp0:		1;
		uint8_t						bp1:		1;
		uint8_t						bp2:		1;
		uint8_t						tb:			1;
		uint8_t						sec:		1;
		uint8_t						srp:		1;
	} bits;
	uint8_t byte;
}w25q64_status_reg1;

typedef struct w25q64_struct{

	struct gpio_t_ 						cs_pin;
//	ARM_DRIVER_SPI 						*p_spi;
//	ARM_SPI_DRIVER_DEVICE_STRUCT		driver_config;
	uint8_t								page_buf[260];
	uint8_t								num_byte;
	uint32_t							addr;
}w25q64_struct;

void w25q64_start_init(w25q64_struct *p_ctl);
uint8_t	w25q64_read_status_reg1(w25q64_struct *p_ctl);
void w25q64_read_manufacture_device_id(w25q64_struct *p_ctl, uint8_t *p_m_id, uint8_t *p_dev_id);
void w25q64_write_enable(w25q64_struct *p_ctl);
uint32_t w25q64_sector_erase(w25q64_struct *p_ctl, uint32_t adress, uint8_t b_check_wel);
uint32_t w25q64_page_programm(w25q64_struct *p_ctl, uint32_t adress, uint8_t *p_data, uint16_t len);
uint32_t w25q64_read_data_to_page_buf(w25q64_struct *p_ctl, uint32_t adress, uint16_t len);

uint32_t w25q64_cmd_h_add_to_cell_table(w25q64_struct *p_ctl, SettingCell_t *p_cell, uint32_t num, const char *p_name);

#endif /* SRC_INC_W25Q64_H_ */
