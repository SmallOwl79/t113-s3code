/*
 * w25q64.c
 *
 *  Created on: 8 окт. 2018 г.
 *      Author: krasikov
 */

#include "string.h"
#include "stdint.h"
#include "stdlib.h"
#include "w25q64.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void w25q64_call_back_start_send_data(void *p){
w25q64_struct *p_ctl;
	if(p != NULL){
		p_ctl = (w25q64_struct*)p;
		gpio_set_sun(&p_ctl->cs_pin,GPIO_RESET);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void w25q64_call_back_stop_send_data(void *p){
w25q64_struct *p_ctl;
	if(p != NULL){
		p_ctl = (w25q64_struct*)p;
		gpio_set_sun(&p_ctl->cs_pin,GPIO_SET);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void w25q64_start_init(w25q64_struct *p_ctl){
//GPIO_InitTypeDef GPIO_InitStructure;
//
//	p_ctl->p_spi = p_spi;
//
//	p_ctl->CS_pin = cs_pin;
//	p_ctl->CS_port = cs_port;
//
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStructure.Pin = p_ctl->CS_pin;
//	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
//	HAL_GPIO_Init(p_ctl->CS_port, &GPIO_InitStructure);
//
//	p_ctl->driver_config.bit_order = ARM_SPI_MSB_LSB;
//	p_ctl->driver_config.frame_format = ARM_SPI_CPOL0_CPHA0;
//	p_ctl->driver_config.speed = ARM_SPI_SPEED_NORMAL;
//	p_ctl->driver_config.start_call_back = w25q64_call_back_start_send_data;
//	p_ctl->driver_config.stop_call_back = w25q64_call_back_stop_send_data;
//	p_ctl->driver_config.p_argument = p_ctl;
//
//	p_ctl->driver_config.op_mode_read_block = ARM_SPI_MODE_BLOCK_READ;
//	p_ctl->driver_config.op_mode_write_block = ARM_SPI_MODE_BLOCK_WRITE;
//
//	p_ctl->driver_config.b_need_copy_read = ARM_SPI_MODE_NOT_COPY_READ;
//	p_ctl->driver_config.b_need_copy_write = ARM_SPI_MODE_NOT_COPY_WRITE;

	w25q64_call_back_stop_send_data(p_ctl);

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t	w25q64_read_status_reg1(w25q64_struct *p_ctl){
uint8_t tmp_buf[3];
ARM_SPI_STATUS status;
	tmp_buf[0] = w25q64_cmd_read_status_reg1;
	tmp_buf[1] = 0;
	tmp_buf[2] = 0;

	status = p_ctl->p_spi->Read(2,tmp_buf,&p_ctl->driver_config);
	if(status != ARM_SPI_OK){
		return 0xff;
	}
	return tmp_buf[1];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void w25q64_read_manufacture_device_id(w25q64_struct *p_ctl, uint8_t *p_m_id, uint8_t *p_dev_id){
uint8_t tmp_buf[8];
ARM_SPI_STATUS status;
	tmp_buf[0] = w25q64_cmd_read_manufacture_id;
	tmp_buf[1] = 0;
	tmp_buf[2] = 0;

	*p_m_id = 0x00;
	*p_dev_id = 0x00;

	status = p_ctl->p_spi->Read(6,tmp_buf,&p_ctl->driver_config);
	if(status != ARM_SPI_OK){
		return;
	}
	*p_m_id = tmp_buf[4];
	*p_dev_id = tmp_buf[5];
	return ;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void w25q64_write_enable(w25q64_struct *p_ctl){
uint8_t tmp_buf[3];
ARM_SPI_STATUS status;
	tmp_buf[0] = w25q64_cmd_write_enable;
	tmp_buf[1] = 0;
	tmp_buf[2] = 0;

	status = p_ctl->p_spi->Write(1,tmp_buf,&p_ctl->driver_config);
	osDelay(20);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void w25q64_write_disable(w25q64_struct *p_ctl){
uint8_t tmp_buf[3];
ARM_SPI_STATUS status;
	tmp_buf[0] = w25q64_cmd_write_disable;
	tmp_buf[1] = 0;
	tmp_buf[2] = 0;

	status = p_ctl->p_spi->Write(1,tmp_buf,&p_ctl->driver_config);
	osDelay(20);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t w25q64_sector_erase(w25q64_struct *p_ctl, uint32_t adress, uint8_t b_check_wel){
uint8_t i;
w25q64_status_reg1 status;
uint8_t tmp_buf[8];

	if(b_check_wel){
		status.byte = w25q64_read_status_reg1(p_ctl);
		if(!status.bits.wel){
			w25q64_write_enable(p_ctl);
		}
		status.byte = w25q64_read_status_reg1(p_ctl);
		if(!status.bits.wel){
			return 1;
		}
		if(status.bits.busy){
			return 1;
		}
	}

	tmp_buf[0] = w25q64_cmd_sector_erase_4KB;
	tmp_buf[1] = adress>>16;
	tmp_buf[2] = adress>>8;
	tmp_buf[3] = adress;
	p_ctl->p_spi->Write(4,tmp_buf,&p_ctl->driver_config);
	osDelay(10);
	for(i=0;i<60;i++){
		status.byte = w25q64_read_status_reg1(p_ctl);
		if(!status.bits.busy){
			return 0;
		}
		osDelay(10);
	}
	return 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t w25q64_page_programm(w25q64_struct *p_ctl, uint32_t adress, uint8_t *p_data, uint16_t len){
uint8_t i;
w25q64_status_reg1 status;

//return 0;

	status.byte = w25q64_read_status_reg1(p_ctl);
	if(!status.bits.wel){
		w25q64_write_enable(p_ctl);
	}
	status.byte = w25q64_read_status_reg1(p_ctl);
	if(!status.bits.wel){
		return 1;
	}
	if(status.bits.busy){
		return 2;
	}

	p_ctl->page_buf[0] = w25q64_cmd_page_programm;
	p_ctl->page_buf[1] = adress>>16;
	p_ctl->page_buf[2] = adress>>8;
	p_ctl->page_buf[3] = adress;
	if(len > 256){
		return 1;
	}

	memcpy(&p_ctl->page_buf[4],p_data,len);
//	osDelay(10);
//	return 0;

	p_ctl->p_spi->Write(len + 4,p_ctl->page_buf,&p_ctl->driver_config);
	for(i=0;i<10;i++){
		status.byte = w25q64_read_status_reg1(p_ctl);
		if(!status.bits.busy){
			return 0;
		}
		osDelay(1);
	}
	return 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t w25q64_read_data_to_page_buf(w25q64_struct *p_ctl, uint32_t adress, uint16_t len){
uint8_t i;
w25q64_status_reg1 status;

	status.byte = w25q64_read_status_reg1(p_ctl);
	if(status.bits.busy){
		return 1;
	}
	if(len > 256) return 1;

	p_ctl->page_buf[0] = w25q64_cmd_read_data;
	p_ctl->page_buf[1] = adress>>16;
	p_ctl->page_buf[2] = adress>>8;
	p_ctl->page_buf[3] = adress;

	p_ctl->p_spi->Read(len + 4,p_ctl->page_buf,&p_ctl->driver_config);

	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* w25q64_cmd_h_read_manufacture_device_id(uint32_t reg32, uint32_t tmp1, uint32_t tmp2){
w25q64_struct *p_ctl;
	p_ctl = (w25q64_struct*)reg32;
	w25q64_read_manufacture_device_id(p_ctl,&p_ctl->page_buf[0],&p_ctl->page_buf[1]);
	return &p_ctl->page_buf[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* w25q64_cmd_h_read_status_reg1(uint32_t reg32, uint32_t tmp1, uint32_t tmp2){
	w25q64_struct *p_ctl;
		p_ctl = (w25q64_struct*)reg32;
		p_ctl->page_buf[0] = w25q64_read_status_reg1(p_ctl);
		return &p_ctl->page_buf[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void w25q64_cmd_h_write_enable(uint32_t reg32, uint32_t tmp1, uint32_t tmp2, uint8_t *p_data){
w25q64_struct *p_ctl;
	p_ctl = (w25q64_struct*)reg32;
	w25q64_write_enable(p_ctl);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void w25q64_cmd_h_write_disable(uint32_t reg32, uint32_t tmp1, uint32_t tmp2, uint8_t *p_data){
w25q64_struct *p_ctl;
	p_ctl = (w25q64_struct*)reg32;
	w25q64_write_disable(p_ctl);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* w25q64_cmd_h_read_data_to_page_buf(uint32_t ctl32, uint32_t index, uint32_t all_size, uint32_t *p_num){
w25q64_struct *p_ctl;
uint8_t *p_ret;
uint32_t table_size;
uint32_t num_read;
	//uint32_t data;

	p_ctl = (w25q64_struct*)ctl32;
	if(!index){
		w25q64_read_data_to_page_buf(p_ctl,p_ctl->addr,256);
	}
	else{
		printf("next read \r\n");
	}
	p_ret = &p_ctl->page_buf[4];

	table_size = 256;
	if(table_size > all_size) table_size = all_size;

	p_ret += index * (*p_num);
	if(table_size <= (index * (*p_num))){
		*p_num = 0;
		return p_ret;
	}
	num_read = table_size - index * (*p_num);
	if(num_read < (*p_num)){
		*p_num = num_read;
	}
	return p_ret;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  w25q64_cmd_h_write_data_256(uint32_t ctl32, uint32_t index, uint16_t offset, uint16_t size, uint32_t *p_num){
w25q64_struct *p_ctl;
uint8_t *p_ret;
uint32_t table_size;
uint32_t num_read;
uint8_t i;
w25q64_status_reg1 status;

	p_ctl = (w25q64_struct*)ctl32;
//	printf("index = %d offset = %d size = %d \r\n",index,offset,size);
	if((offset + size)>260){
		return;
	}
	memcpy(&p_ctl->page_buf[4 + offset],p_num,size);
	if((offset + size)==256){
		status.byte = w25q64_read_status_reg1(p_ctl);
		if(!status.bits.wel){
			w25q64_write_enable(p_ctl);
		}
		status.byte = w25q64_read_status_reg1(p_ctl);
		if(!status.bits.wel){
			return;
		}
		if(status.bits.busy){
			return;
		}

		p_ctl->page_buf[0] = w25q64_cmd_page_programm;
		p_ctl->page_buf[1] = p_ctl->addr>>16;
		p_ctl->page_buf[2] = p_ctl->addr>>8;
		p_ctl->page_buf[3] = p_ctl->addr;

		p_ctl->p_spi->Write(260,p_ctl->page_buf,&p_ctl->driver_config);
		for(i=0;i<10;i++){
			status.byte = w25q64_read_status_reg1(p_ctl);
			if(!status.bits.busy){
				return;
			}
			osDelay(1);
		}
	}
	return;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint8_t* w25q64_cmd_h_read_data_to_page_buf(uint32_t data, uint32_t data_low, uint32_t data_hight, uint32_t *p_ack_len){
//w25q64_struct *p_ctl;
//	p_ctl = (w25q64_struct*)data;
//	w25q64_read_data_to_page_buf(p_ctl,p_ctl->addr,256);
//	*p_ack_len = 256;
//	return &p_ctl->page_buf[4];
//
//
//
//	control_struct *p_control;
//	uint8_t *p_ret;
//	uint32_t table_size;
//	uint32_t num_read;
//		//uint32_t data;
//		p_control = (control_struct *)ctl32;
//		table_size = p_control->table_calibrate_ch_collect_common.num_point * p_control->table_calibrate_ch_collect_common.size;
//		if(table_size > all_size) table_size = all_size;
//		p_ret = (uint8_t *)p_control->table_calibrate_ch_collect_common.p_data;
//		p_ret += index * (*p_num);
//		if(table_size <= (index * (*p_num))){
//			*p_num = 0;
//			return p_ret;
//		}
//		num_read = table_size - index * (*p_num);
//		if(num_read < (*p_num)){
//			*p_num = num_read;
//		}
//		return p_ret;
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void w25q64_cmd_h_page4k_erase(uint32_t reg32, uint32_t tmp1, uint32_t tmp2, uint8_t *p_data){
w25q64_struct *p_ctl;
	p_ctl = (w25q64_struct*)reg32;
	w25q64_sector_erase(p_ctl,p_ctl->addr,1);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t w25q64_cmd_h_add_to_cell_table(w25q64_struct *p_ctl, SettingCell_t *p_cell, uint32_t num, const char *p_name){
SettingCell_t 	cell;
uint32_t 		i;
	i=0;

	p_cell = name_add_to_cell_table(p_cell,num++,p_name);

	p_cell++;

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)w25q64_cmd_h_write_disable;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "wrt. disable";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)w25q64_cmd_h_write_enable;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "wrt. enable";
	p_cell[i++] = cell;

	cell.CellAttr =  Action_Att + RD_Att;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)w25q64_cmd_h_read_status_reg1;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "read status 1";
	p_cell[i++] = cell;

	cell.CellAttr =  Action_Att + RD_Att;
	cell.CellType = FixIntegerCellType + 2;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)w25q64_cmd_h_read_manufacture_device_id;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "read id";
	p_cell[i++] = cell;

	cell.CellAttr =  WR_Att + RD_Att;
	cell.CellType = FixIntegerCellType + 3;
	cell.VarPtr = &p_ctl->addr;
	cell.ReadProc = NULL;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "addr";
	p_cell[i++] = cell;

	cell.CellAttr =  WR_Att + RD_Att;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = &p_ctl->num_byte;
	cell.ReadProc = NULL;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "num byte";
	p_cell[i++] = cell;

	cell.CellAttr =  Action_Att + RD_Att + WR_Att;
	cell.CellType = FixExtArrayCellType;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)w25q64_cmd_h_read_data_to_page_buf;
	cell.WriteProc = (void*)(void*)w25q64_cmd_h_write_data_256;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "read 256 from addr";
	p_cell[i++] = cell;

	cell.CellAttr = Action_Att + WR_Att + Default_Attr;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = 0;
	cell.ReadProc = NULL;
	cell.WriteProc = (void*)(void*)w25q64_cmd_h_page4k_erase;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "page 4k erase";
	p_cell[i++] = cell;

	return num;
}
