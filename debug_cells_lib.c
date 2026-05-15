/*! @file ***************************************************************************************
 *
 * @brief
 * 			������� ���������� �����
 *
 * $Revision: 1 $
 * $Date: 2012-06-21 $
 *
 * @par 	������ �������� ������:
 * 			...
 *
 * @par 	�������� ������� ����������� �������� ������:
 *			...
 *
 * @par		�������� ������ � ��������:
 *			...
 *
 *************************************************************************************************/


//*-----------------------------------------------------------------------------------------------
//*		������������ ������
//*-----------------------------------------------------------------------------------------------
//#include "includes.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "debug_cells_lib.h"
#include "control.h"

//*-----------------------------------------------------------------------------------------------
//*		��������� ����������
//*-----------------------------------------------------------------------------------------------
/// ��������� ���������� �����
#pragma location=".ddr_data"
uint8_t temp_cell_buff[256];
uint32_t dbg_ext_index;
uint32_t dbg_ext_offset;
//uint32_t index_tst[10];


/// ��������� ���������� ���������� �������
cmd_result_t debug_cmd_result;

//*-----------------------------------------------------------------------------------------------
//*		������������������ ���������
//*-----------------------------------------------------------------------------------------------

#define SRAM_BASE		0
#define SRAM_SIZE		0x10000
#define SRAM_START		SRAM_BASE
#define ROM_READ_START	FLASH_SETTINGS_BASE
#define ROM_READ_SIZE	FLASH_SETTINGS_SIZE


//*-----------------------------------------------------------------------------------------------
//*		���������� ����� �������-�������
//*-----------------------------------------------------------------------------------------------
/// ��� �������-������ ������:
typedef void * (*RD_Funct_Ptr_t)(void);
typedef void * (*RD_Funct_Ptr_t_void)(void *, uint32_t tmp);
typedef void * (*RD_Funct_Ptr_t_3)(uint32_t data, uint32_t data_low, uint32_t data_hight);
typedef void * (*RD_Funct_Ptr_t_4)(uint32_t data, uint32_t data_low, uint32_t data_hight, uint32_t *p_ack_len);


/// ��� �������-������ ������:
typedef void (*WR_Funct_Ptr_t)(void * ptr);
typedef void (*WR_Funct_Ptr_t_2)(void * ptr,uint32_t data_low, uint32_t data_hight, uint8_t *p_data);
typedef void (*WR_Funct_Ptr_t_3)(uint32_t data, uint32_t data_low, uint32_t data_hight, uint8_t *p_data);
typedef void (*WR_Funct_Ptr_t_4)(uint32_t data, uint32_t data_low, uint32_t data_hight, uint32_t data_num, uint8_t *p_data);



//*-----------------------------------------------------------------------------------------------
/**		������ ���
 *
 * @param	out_buff - ���������, ���� ����������� ������ �������
 * @param	read_data - ��������� �������� ������
 * @param	data_size - ������ ��������� �����
 * @return	true, ���� �������� ���������											 			*/
//*-----------------------------------------------------------------------------------------------
uint8_t debug_read_ram(uint8_t * out_buff, uint8_t * read_data, uint8_t data_size)
{
	if (((uint32_t)read_data < SRAM_START) ||
		(((uint32_t)read_data + data_size) > (SRAM_START+SRAM_SIZE)))
		return false;

	if (data_size == 0)
		return false;

	memcpy(out_buff, read_data, data_size);
	return true;
}


//*-----------------------------------------------------------------------------------------------
/**		������ ���
 *
 * @param	write_data - ��������� �� ����� RAM, ���� ������������ ������
 * @param	data_buff - ��������� �� ������������ ������
 * @param	data_size - ���������� ������������ ����
 * @return	false, ���� ������� �� ���������													*/
//*-----------------------------------------------------------------------------------------------
uint8_t debug_write_ram(uint8_t * write_data, uint8_t * data_buff, uint8_t data_size)
{
	if (((uint32_t)write_data < SRAM_START) ||
		(((uint32_t)write_data + data_size) > (SRAM_START+SRAM_SIZE)))
		return false;

	if (data_size == 0)
		return false;

	memcpy(write_data, data_buff, data_size);
	return true;
}


// -----------------------------------------------------------------------------------------------
/**		����� ������ � ��������� �������
 *
 * @param 	CellNumb - ����� ������� ������
 * @param	descript_ptr - ��������� �� ������� �������� �����
 * @return	��������� �� ������������ �������� ������ � ������,
 * 			���� ������ �� ������� - ������������ nil=0											*/
//*-----------------------------------------------------------------------------------------------
SettingCell_t * FindCell(uint8_t CellNumb, SettingCell_t * descript_ptr)
{
	uint8_t temp = descript_ptr->CellNumber;
	while ((temp != CellNumb) && (temp != 0))
	{
		descript_ptr++;
		temp = descript_ptr->CellNumber;
	}

	if (temp == 0)
		descript_ptr = nil;

	return descript_ptr;
}


//*-----------------------------------------------------------------------------------------------
/**		������ ������ �������������� �����
 *
 * @param	skip_cells - ���������� �����, ������� ���������� ���������� ��� ������ ������
 * @param	outbuff - ��������� �� ����� ��� �������� ������; outbuff[0] - ����� ����� ������
 * 			������ ������������� ������� � outbuff[1]
 * @param	descript_ptr - ��������� �� ���������� �����
 * @return	��������� ���������� ������� ���� cell_cmd_result_t									*/
//*-----------------------------------------------------------------------------------------------
cell_cmd_result_t debug_cell_read_list(uint8_t skip_cells, uint8_t * outbuff, SettingCell_t * descript_ptr)
{
	uint8_t temp, cnt = 0;
	uint8_t	buff_ptr;

	// ���������� ��������� ���������� �����
	temp = descript_ptr->CellNumber;
	while ((skip_cells != 0) && (temp != 0))
	{
		descript_ptr++;
		skip_cells--;
		temp = descript_ptr->CellNumber;
	}

	// ������ ������
	buff_ptr = 1; cnt = 0;
	do
	{
		temp = descript_ptr->CellNumber;
		if (temp == 0) break;
		outbuff[buff_ptr++] = temp;
		cnt++;
		descript_ptr++;
	}
	while ((cnt < 255) && (temp != 0));

	// ���������� ������
	outbuff[buff_ptr] = 0;

	// ����� ������
	outbuff[0] = buff_ptr-1;

	if (cnt > 0)
		return cell_data_ready;
	else
		return cell_param_error;
}


//*-----------------------------------------------------------------------------------------------
/**		������ �������� ���� ������
 *
 * @param	cell_num - ����� ������������� ������
 * @param	outbuff - ��������� �� �����, � ������� ������� ��������� ��������
 * 			outbuff[0] - ����� ���������, �������� ���������� ������� � ������ outbuff[1]
 * @param	descript_ptr - ��������� �� ������� � ���������
 * @return	��������� ���������� ������� ���� cell_cmd_result_t									*/
//*-----------------------------------------------------------------------------------------------
cell_cmd_result_t debug_cell_read_type(uint8_t cell_numb, uint8_t * outbuff, SettingCell_t * descript_ptr)
{
	// ����� ����������� ������ ������� ��������
	SettingCell_t * ptr = FindCell(cell_numb, descript_ptr);

	// ���� ������ �� �������, ������
	if (ptr == nil)
		return cell_not_found;

	SettingCell_t Cell = *ptr;
/*
	if ((Cell.CellAttr & ROM_Att) == 0)
	{
		if ((Cell.ReadProc != nil) || (Cell.WriteProc != nil))
				Cell.CellAttr |= Action_Att;
	}
*/
	/*
	else
	{
		if ((Cell.ReadProc_LowLim != nil) || (Cell.WriteProc_HiLim != nil))
				Cell.CellAttr |= Range_Attr;
	}
	*/
	//if (Cell.DefaultValue != nil) Cell.CellAttr |= Default_Attr;

	outbuff[1] = (uint8_t)Cell.CellNumber;
	outbuff[2] = Cell.CellType;
	outbuff[3] = Cell.CellAttr;

	// ��������� ��������
	uint8_t i = 0;
	while ((Cell.DescriptStr[i] != 0) && (i < DescriptNameLenght ))
	{
		outbuff[4 + i] = Cell.DescriptStr[i];
		i++;
	}
	outbuff[4 + i] = 0x00;

	// �������, ��������, ��������� ��������
	// ...
	// �� �����������

	outbuff[0] = 4 + i;

	return cell_data_ready;
}
cell_cmd_result_t debug_cell_read_type_num(uint8_t cell_numb, uint8_t * outbuff, SettingCell_t * descript_ptr, uint8_t *p_num){
SettingCell_t * ptr = FindCell(cell_numb, descript_ptr);
	if (ptr == nil){
		return cell_not_found;
	}
	SettingCell_t Cell = *ptr;

	outbuff[0] = (uint8_t)Cell.CellNumber;
	outbuff[1] = Cell.CellType;
	outbuff[2] = Cell.CellAttr;
	uint8_t i = 0;
	while ((Cell.DescriptStr[i] != 0) && (i < DescriptNameLenght )){
		outbuff[3 + i] = Cell.DescriptStr[i];
		i++;
	}
	outbuff[3 + i] = 0x00;
	*p_num = 4 + i;

	return cell_data_ready;
}

//*-----------------------------------------------------------------------------------------------
/**		������ ����������� ������
 *
 * @param	cells - ��������� �� ������������ ������ �������� ����� (����� ������ - ��� 0)
 * @param	outbuff - ��������� �� ����� ��� ���������� ����������� �����
 * 			outbuff[0] - ����� ����� ������, ������ ������������� ������� � outbuff[1]
 * @param	descript_ptr - ��������� �� ������� � ���������
 * @return	��������� ���������� ������� ���� cell_cmd_result_t									*/
//*-----------------------------------------------------------------------------------------------
cell_cmd_result_t debug_cell_read(uint8_t * cells, uint8_t * outbuff, SettingCell_t * desript_ptr)
{
	uint8_t cell_ptr = 0;
	uint8_t out_ptr = 1;
	SettingCell_t * Cell;
	uint8_t * data_p;

	// ���� �� ������������ ���������� �����
	while(cells[cell_ptr] != 0)
	{
		Cell = FindCell(cells[cell_ptr], desript_ptr);

		// ���� ������ �� ����������, �� ������
		if (Cell == nil)
			return cell_not_found;

		// �������� �������� ���������� ������
		if ((Cell->CellAttr & RD_Att) == 0)
		{
			cell_ptr++;
			continue;
		}

		// ������� ��������
		uint8_t data_size = Cell->CellType & 0x1F;
		if ((Cell->CellType & 0xE0) == Fix8ByteArrayCellType)
			data_size *= ByteArrayBlockSize;

		// ���� ������ ����� ������� �����, �� ������ (�����������)
		if (data_size == 0)
			return cell_descript_error;

		// �������� ����������� ����� � ������
		uint16_t temp16 = out_ptr + data_size + sizeof(Cell->CellType) + sizeof(Cell->CellNumber);
		if (temp16 > MaxOutDataLenght)
			return cell_read_overload_buff;

		outbuff[out_ptr++] = Cell->CellNumber;
		outbuff[out_ptr++] = Cell->CellType;

		// ������ �� ROM
		if ((Cell->CellAttr & ROM_Att) != 0){

			return cell_descript_error;

			// ���� ������ �� ������ �� ������� - ������
			out_ptr += data_size;
		}
		else
		// ������ �� RAM
		// ���� �� ����� ����� ������, ������ ��������� �� ���������� ������
		if (Cell->ReadProc == nil)
		{
			// ������, ���� ������ � ������ �������
			if (Cell->VarPtr == nil)
				return cell_descript_error;

			memcpy(&outbuff[out_ptr], Cell->VarPtr,	data_size);
			out_ptr += data_size;
		}
		else if (Cell->ReadProc != nil)
		// ����� ������
		{
			if (Cell->CellAttr & Action_Att){
				if(Cell->CellAttr & RD_Att){
					RD_Funct_Ptr_t_3 RD_Funct_Ptr;
//					uint8_t * data_p;

					RD_Funct_Ptr = (RD_Funct_Ptr_t_3)Cell->ReadProc;
					data_p = (*RD_Funct_Ptr)(Cell->DefaultValue,Cell->LowLim,Cell->HighLim);

				}
			}
			else{
				// ������� ��������� �� ����� ������
				RD_Funct_Ptr_t RD_Funct_Ptr;
//				uint8_t * data_p;
				// ��������� ���������
	////////////////////////////////////////////////////////////////////
	//����� � ������, �� ���!!!!!
	////////////////////////////////////////////////////////////////////
				if ((Cell->VarPtr == nil) && (Cell->DefaultValue != nil)){

					RD_Funct_Ptr_t_void RD_Funct_Ptr_void;
					RD_Funct_Ptr_void = (RD_Funct_Ptr_t_void) Cell->ReadProc;

					data_p = RD_Funct_Ptr_void((void*)Cell->DefaultValue, Cell->LowLim);

				}
				else{

					RD_Funct_Ptr = (RD_Funct_Ptr_t)Cell->ReadProc;
					// ����� ������
					data_p = (*RD_Funct_Ptr)();
				}
			}

			// ��� ��������
			if (data_p == nil)
				return cell_cmd_not_available;

			// ���������� ���� ������
			memcpy(&outbuff[out_ptr], data_p, data_size);
			out_ptr += data_size;
		}
		// ����� �������� ������������
		else
			return cell_descript_error;

		cell_ptr++;
	};

	outbuff[0] = out_ptr - 1;
	// �������� ������
	if (out_ptr > 1)
		return cell_data_ready;
	else
		return cell_not_found;
}
//*-----------------------------------------------------------------------------------------------
/**		������ ����������� ������
 *
 * @param	cells - ��������� �� ������������ ������ �������� ����� (����� ������ - ��� 0)
 * @param	outbuff - ��������� �� ����� ��� ���������� ����������� �����
 * 			outbuff[0] - ����� ����� ������, ������ ������������� ������� � outbuff[1]
 * @param	descript_ptr - ��������� �� ������� � ���������
 * @return	��������� ���������� ������� ���� cell_cmd_result_t									*/
//*-----------------------------------------------------------------------------------------------
cell_cmd_result_t debug_ext_cell_read(uint8_t * cells, uint8_t * outbuff, SettingCell_t * desript_ptr){
uint8_t cell_ptr = 0;
uint8_t out_ptr = 1;
SettingCell_t * Cell;
uint8_t * data_p;
uint32_t data_size = 0;
uint32_t k,n;
uint32_t index;
uint32_t ret_size;


	index = cells[0];
	index += cells[1] << 8;

	cells += 2;

	if(index && (index != (dbg_ext_index + 1)))
		return cell_unknown_error;

	dbg_ext_index = index;

	// ���� �� ������������ ���������� �����
	while(cells[cell_ptr] != 0)	{

		Cell = FindCell(cells[cell_ptr], desript_ptr);

		// ���� ������ �� ����������, �� ������
		if (Cell == nil)
			return cell_not_found;

		// �������� �������� ���������� ������
		if ((Cell->CellAttr & RD_Att) == 0)	{
			return cell_not_found;
		}

		// ������� ��������
		if ((Cell->CellType & 0x80) == FixExtArrayCellType){
			k = Cell->CellType  & 0x03;
			n = (Cell->CellType >> 2)  & 0x07;
			data_size = (4 + k) * (1 << (6 + n));
		}
		else{
			return cell_descript_error;
		}
		// ���� ������ ����� ������� �����, �� ������ (�����������)
		if (data_size == 0)
			return cell_descript_error;

		outbuff[out_ptr++] = index;
		outbuff[out_ptr++] = index >> 8;
		outbuff[out_ptr++] = Cell->CellNumber;
		outbuff[out_ptr++] = Cell->CellType;

		// ������ �� ROM
		if ((Cell->CellAttr & ROM_Att) != 0){
			return cell_descript_error;
			// ���� ������ �� ������ �� ������� - ������
		}
		else
		// ������ �� RAM
		// ���� �� ����� ����� ������, ������ ��������� �� ���������� ������
		if (Cell->ReadProc == nil){
			return cell_descript_error;
		}
		else if (Cell->ReadProc != nil){
			if (Cell->CellAttr & Action_Att){
				if(Cell->CellAttr & RD_Att){
					RD_Funct_Ptr_t_4 RD_Funct_Ptr;
					RD_Funct_Ptr = (RD_Funct_Ptr_t_4)Cell->ReadProc;
					ret_size = 128;
					data_p = (*RD_Funct_Ptr)(Cell->DefaultValue,index,data_size,&ret_size);
				}
			}
			else{
				return cell_descript_error;
			}
			if (data_p == nil)
				return cell_cmd_not_available;
			if (ret_size > 128)
				return cell_read_overload_buff;

			// ���������� ���� ������
			memcpy(&outbuff[out_ptr], data_p, ret_size);
			out_ptr += ret_size;
			break;
		}
		// ����� �������� ������������
		else
			return cell_descript_error;

		cell_ptr++;
	};

	outbuff[0] = out_ptr - 1;
	// �������� ������
	if (out_ptr > 1)
		return cell_data_ready;
	else
		return cell_not_found;
}
//*-----------------------------------------------------------------------------------------------
cell_cmd_result_t debug_ext_cell_read_num(uint8_t * cells, uint8_t * outbuff, SettingCell_t * desript_ptr, uint8_t *p_num, uint8_t num){
uint8_t cell_ptr = 0;
uint8_t out_ptr = 0;
SettingCell_t * Cell;
uint8_t * data_p;
uint32_t data_size = 0;
uint32_t k,n;
uint32_t index;
uint32_t ret_size;


	index = cells[0];
	index += cells[1] << 8;

	cells += 2;

	if(index && (index != (dbg_ext_index + 1)))
		return cell_unknown_error;

	dbg_ext_index = index;

	// ���� �� ������������ ���������� �����
//	while(cells[cell_ptr] != 0)	{
	while(num)	{

		Cell = FindCell(cells[cell_ptr], desript_ptr);

		// ���� ������ �� ����������, �� ������
		if (Cell == nil)
			return cell_not_found;

		// �������� �������� ���������� ������
		if ((Cell->CellAttr & RD_Att) == 0)	{
			return cell_not_found;
		}

		// ������� ��������
		if ((Cell->CellType & 0x80) == FixExtArrayCellType){
			k = Cell->CellType  & 0x03;
			n = (Cell->CellType >> 2)  & 0x07;
			data_size = (4 + k) * (1 << (6 + n));
		}
		else{
			return cell_descript_error;
		}
		// ���� ������ ����� ������� �����, �� ������ (�����������)
		if (data_size == 0)
			return cell_descript_error;

		outbuff[out_ptr++] = index;
		outbuff[out_ptr++] = index >> 8;
		outbuff[out_ptr++] = Cell->CellNumber;
		outbuff[out_ptr++] = Cell->CellType;

		// ������ �� ROM
		if ((Cell->CellAttr & ROM_Att) != 0){
			return cell_descript_error;
			// ���� ������ �� ������ �� ������� - ������
		}
		else
		// ������ �� RAM
		// ���� �� ����� ����� ������, ������ ��������� �� ���������� ������
		if (Cell->ReadProc == nil){
			return cell_descript_error;
		}
		else if (Cell->ReadProc != nil){
			if (Cell->CellAttr & Action_Att){
				if(Cell->CellAttr & RD_Att){
					RD_Funct_Ptr_t_4 RD_Funct_Ptr;
					RD_Funct_Ptr = (RD_Funct_Ptr_t_4)Cell->ReadProc;
					ret_size = 128;
					data_p = (*RD_Funct_Ptr)(Cell->DefaultValue,index,data_size,&ret_size);
				}
			}
			else{
				return cell_descript_error;
			}
			if (data_p == nil)
				return cell_cmd_not_available;
			if (ret_size > 128)
				return cell_read_overload_buff;

			// ���������� ���� ������
			memcpy(&outbuff[out_ptr], data_p, ret_size);
			out_ptr += ret_size;
			break;
		}
		// ����� �������� ������������
		else
			return cell_descript_error;

		cell_ptr++;
		num--;
	};

	*p_num = out_ptr;
	// �������� ������
	if (out_ptr)
		return cell_data_ready;
	else
		return cell_not_found;
}
//*-----------------------------------------------------------------------------------------------
/**		������ ������
 *
 * @param	cell_numb - ����� ������������ ������
 * @param	dataptr - ��������� �� ����� � ��������� ������� ��� ������
 * @param	data_size - ��������� �� ����� � ��������� ������� ��� ������
 * @param	descript_ptr - ��������� �� ������� � ���������
 * @return	��������� ���������� ������� ���� cell_cmd_result_t									*/
//*-----------------------------------------------------------------------------------------------
cell_cmd_result_t debug_cell_write(uint8_t cell_number, uint8_t * dataptr, uint8_t data_size, SettingCell_t * descript_ptr)
{
	// ����� ������
	SettingCell_t * Cell = FindCell(cell_number, descript_ptr);

	// ���� ������ �� ����������, �� - ������ ...
	if (Cell == nil)
		return cell_not_found;

	// �������� ���������
//	if ((Cell->CellAttr & WR_Att) == 0)
//		return cell_cmd_not_available;

	uint8_t cell_data_size = Cell->CellType & 0x1F;
	if ((Cell->CellType & 0xE0) == Fix8ByteArrayCellType)
		cell_data_size *= ByteArrayBlockSize;

	// ������� ����� � ������� �������� ����� ������ (�����������)
	if (cell_data_size == 0)
		return cell_descript_error;

	// ������, ���� ����� ����� ������ �� ������������� ������� ������
	if (data_size != cell_data_size)
		return cell_param_error;

	// �������� �������� ���������� ������
	if (((Cell->CellAttr & WR_Att) == 0) && ((Cell->CellAttr & Default_Attr) == 0))
		return cell_cmd_not_available;

	// �������� ����������� ���������
	if (((Cell->CellType & 0xE0) == FixIntegerCellType) && (Cell->CellAttr & Range_Attr))
	{
		// ������� ��������
		uint64_t value = 0;
		for (uint32_t i = 0; i < cell_data_size; i++)
		{
			value <<= 8;
			value |= dataptr[cell_data_size - i - 1];
		}

		// ������� ������������
		uint64_t low_lim, high_lim;
		if (cell_data_size <= 4)
		{
			low_lim = (uint32_t)(Cell->LowLim);
			high_lim = (uint32_t)(Cell->HighLim);
		}
		else
		{
			low_lim = high_lim = 0;
			for (uint32_t i = 0; i < cell_data_size; i++)
			{
				low_lim <<= 8;
				high_lim <<= 8;
				low_lim |= ((uint8_t *)(Cell->LowLim))[i];
				high_lim |= ((uint8_t *)(Cell->HighLim))[i];
			}
		}

		// ��������� ������������ �������������
		if (low_lim > high_lim)
			return cell_descript_error;

		if ((value < low_lim) ||
			(value > high_lim))
		{
			return cell_param_error;
		}
	}
	if (((Cell->CellType & 0xE0) == FixSignIntegerCellType) && (Cell->CellAttr & Range_Attr))
	{
		// ������� ��������
		uint64_t value = 0;
		for (uint32_t i = 0; i < cell_data_size; i++)
		{
			value <<= 8;
			value |= dataptr[cell_data_size - i - 1];
		}

		// ������� ������������
		uint64_t low_lim, high_lim;
		if (cell_data_size <= 4)
		{
			low_lim = (uint32_t)(Cell->LowLim);
			high_lim = (uint32_t)(Cell->HighLim);
		}
		else
		{
			low_lim = high_lim = 0;
			for (uint32_t i = 0; i < cell_data_size; i++)
			{
				low_lim <<= 8;
				high_lim <<= 8;
				low_lim |= ((uint8_t *)(Cell->LowLim))[i];
				high_lim |= ((uint8_t *)(Cell->HighLim))[i];
			}
		}

		// ��������� ����
		uint64_t sign_flag = 0xFFFFFFFFFFFFFF80;
		for (uint32_t i = 0; i < (cell_data_size-1); i++)
			sign_flag <<= 8;

		if (value & sign_flag) value |= sign_flag;
		if (low_lim & sign_flag) low_lim |= sign_flag;
		if (high_lim & sign_flag) high_lim |= sign_flag;

		// ��������� ������������ �������������
		if ((int64_t)low_lim > (int64_t)high_lim)
			return cell_descript_error;

		// ��������
		if (((int64_t)value < (int64_t)low_lim) ||
			((int64_t)value > (int64_t)high_lim))
		{
			return cell_param_error;
		}
	}

	// ������ � ROM
	if ((Cell->CellAttr & ROM_Att) != 0){
		// ������� ������ � ������ �������
		return cell_descript_error;

		// ������� �������������� ���������, ���� ����� ���������
		if (Cell->WriteProc != nil)
		{
//			memcpy(&temp_cell_buff[0], &dataptr[0], data_size);
			// ������� ��������� �� ����� ������
			WR_Funct_Ptr_t WR_Funct_Ptr;
			// ��������� ���������
			WR_Funct_Ptr = (WR_Funct_Ptr_t)Cell->WriteProc;
			// ������� �����
			//(*WR_Funct_Ptr)(&temp_cell_buff[0]);
			(*WR_Funct_Ptr)(dataptr);
		}
	}

	else if (Cell->WriteProc != nil)
	// ����� ������ ������
	{
		// ���������� ������� ����� ��������� ��������� �� ����� ��� ������,
		// � ��� ����� �� unsigned int. ����� ����� ������� ������������
		// ������������ � ��� �� ������, �������� 4. �� ������������ ����������
		// ������ ����� ������ �� ������������� ������. ������� ����������
		// ��������� ������������ ���� ����������� ������ � ����� � ����������
		// ��������� �� ������
//		memset(&temp_cell_buff[0], 0, sizeof(temp_cell_buff));
//		memcpy(&temp_cell_buff[0], &dataptr[0], data_size);

		// ������� ��������� �� ����� ������

		// ��������� ���������

		// ������� �����
//		(*WR_Funct_Ptr)(&temp_cell_buff[0]);
		if (Cell->CellAttr & Action_Att){
			if(Cell->CellAttr & WR_Att){
				WR_Funct_Ptr_t_3 WR_Funct_Ptr;
				WR_Funct_Ptr =(WR_Funct_Ptr_t_3) Cell->WriteProc;
				(*WR_Funct_Ptr)(Cell->DefaultValue,Cell->LowLim,Cell->HighLim,dataptr);
			}
		}
		else{
			if (Cell->CellAttr & Ptr_Att){
				WR_Funct_Ptr_t_2 WR_Funct_Ptr;

				WR_Funct_Ptr = (WR_Funct_Ptr_t_2)Cell->WriteProc;
				if((Cell->CellAttr & WR_Att) != 0)
					(*WR_Funct_Ptr)(dataptr,Cell->LowLim,Cell->HighLim,(uint8_t*)&Cell->DefaultValue);
				else
					(*WR_Funct_Ptr)(&Cell->DefaultValue,Cell->LowLim,Cell->HighLim,dataptr);
			}
			else{

				WR_Funct_Ptr_t WR_Funct_Ptr;
				WR_Funct_Ptr = (WR_Funct_Ptr_t)Cell->WriteProc;
				if((Cell->CellAttr & WR_Att) != 0)
					(*WR_Funct_Ptr)(dataptr);
				else
					(*WR_Funct_Ptr)(&Cell->DefaultValue);
			}
		}

	}

	// ������ ������ � RAM
	else if (Cell->VarPtr != nil)
	{
		memcpy(Cell->VarPtr, &dataptr[0], data_size);
	}
	else
		return cell_descript_error;

	return cell_cmd_ok;
}
//*-----------------------------------------------------------------------------------------------
/**		������ ������
 *
 * @param	cell_numb - ����� ������������ ������
 * @param	dataptr - ��������� �� ����� � ��������� ������� ��� ������
 * @param	data_size - ��������� �� ����� � ��������� ������� ��� ������
 * @param	descript_ptr - ��������� �� ������� � ���������
 * @return	��������� ���������� ������� ���� cell_cmd_result_t									*/
//*-----------------------------------------------------------------------------------------------
cell_cmd_result_t debug_ext_cell_write(uint8_t cell_number, uint8_t * dataptr, uint8_t data_size, SettingCell_t * descript_ptr){
WR_Funct_Ptr_t_4 WR_Funct_Ptr;
uint32_t index;
//uint32_t num;

	SettingCell_t * Cell = FindCell(cell_number, descript_ptr);

	index = dataptr[-3];
	index += dataptr[-2] << 8;

	if(index && (index != (dbg_ext_index + 1)))
		return cell_unknown_error;

	dbg_ext_index = index;

	if(!dbg_ext_index) dbg_ext_offset = 0;

	// ���� ������ �� ����������, �� - ������ ...
	if (Cell == nil)
		return cell_not_found;

//	uint8_t cell_data_size = Cell->CellType & 0x1F;

	if ((Cell->CellType & 0x80) != FixExtArrayCellType)
		return cell_descript_error;

	// �������� �������� ���������� ������
	if (((Cell->CellAttr & WR_Att) == 0))
		return cell_cmd_not_available;

	if (Cell->WriteProc != nil){
		if (Cell->CellAttr & Action_Att){
			WR_Funct_Ptr = (WR_Funct_Ptr_t_4)Cell->WriteProc;
			(*WR_Funct_Ptr)(Cell->DefaultValue,index,dbg_ext_offset,data_size,dataptr);
			dbg_ext_offset += data_size;
//			index_tst[index] = dbg_ext_offset;
		}
		else{
			return cell_descript_error;;
		}
	}
	else{
		return cell_descript_error;;
	}
	return cell_cmd_ok;
}

//*-----------------------------------------------------------------------------------------------
/**		������ ������ ��������� �� ���������
 *
 * @param	cell_num - ����� ������������ ������
 * @return	false, ���� ������� �� ���������													*/
//*-----------------------------------------------------------------------------------------------
cell_cmd_result_t debug_cell_set_default(uint8_t cell_num, SettingCell_t * descript_ptr)
{
	 SettingCell_t * Cell = FindCell(cell_num, descript_ptr);

	// ���� ������ �� ����������...
	if (Cell == nil)
		return cell_not_found;

	// ���� �� ������������
	if ((Cell->CellAttr & Default_Attr) == 0)
		return cell_cmd_not_available;

	// ���� ��������� �������� ������, ����� ������
	if (Cell->DefaultValue == nil)
		return cell_param_error;

	uint8_t data_size = Cell->CellType & 0x1F;
	if ((Cell->CellType & 0xE0) == Fix8ByteArrayCellType)
		data_size *= ByteArrayBlockSize;

	// ����������� ���������� �������� � ����� ���
	if (data_size <= 4)
		memcpy(&temp_cell_buff[0], &(Cell->DefaultValue), data_size);
	else
		memcpy(&temp_cell_buff[0], &(Cell->DefaultValue), data_size);

	// ������ ���������� ������
	return debug_cell_write(Cell->CellNumber, temp_cell_buff, data_size, descript_ptr);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void name_add_to_cell_table_fix_pos(SettingCell_t *p_cell, uint32_t num, const char *p_name){
SettingCell_t 	cell;

	cell.CellAttr = 0;
	cell.CellType = FixStringCellType + 32;
	cell.CellNumber = num;
	cell.VarPtr = 0;
	cell.ReadProc = 0;
	cell.WriteProc = 0;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = 0;
	cell.DescriptStr = p_name;

	*p_cell = cell;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SettingCell_t *name_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, const char *p_name){
SettingCell_t 	cell;
uint32_t 		i;
	for(i=0;i<COMPONENT_MAX_TABLE_SIZE;i++){
		if(!p_cell[i].CellNumber) break;
	}
	if( i>= COMPONENT_MAX_TABLE_SIZE) return p_cell;

	cell.CellAttr = 0;
	cell.CellType = FixStringCellType + 32;
	cell.CellNumber = num;
	cell.VarPtr = 0;
	cell.ReadProc = 0;
	cell.WriteProc = 0;
	cell.LowLim = 0;
	cell.HighLim = 0;
	cell.DefaultValue = 0;
	cell.DescriptStr = p_name;

	p_cell[i] = cell;
	return &p_cell[i];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SettingCell_t *debug_add_to_cell_table(SettingCell_t *p_cell, SettingCell_t *p_addcell){
uint32_t 		i;
	for(i=0;i<COMPONENT_MAX_TABLE_SIZE;i++){
		if(!p_cell[i].CellNumber) break;
	}
	if( i>= COMPONENT_MAX_TABLE_SIZE) return p_cell;
	p_cell[i] = *p_addcell;
	return &p_cell[i];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
cell_cmd_result_t debug_cell_read_list_num(uint8_t skip_cells, uint8_t * outbuff, SettingCell_t * descript_ptr, uint8_t *p_num){
uint8_t temp, cnt = 0;
uint8_t	buff_ptr;

	temp = descript_ptr->CellNumber;
	while ((skip_cells != 0) && (temp != 0))	{
		descript_ptr++;
		skip_cells--;
		temp = descript_ptr->CellNumber;
	}
	buff_ptr = 0; cnt = 0;
	do
	{
		temp = descript_ptr->CellNumber;
		if (temp == 0) break;
		outbuff[buff_ptr++] = temp;
		cnt++;
		descript_ptr++;
	}
	while ((cnt < 255) && (temp != 0));

	outbuff[buff_ptr] = 0;

	*p_num = buff_ptr;

	if (cnt > 0)
		return cell_data_ready;
	else
		return cell_param_error;
}
cell_cmd_result_t debug_cell_read_num(uint8_t * cells, uint8_t * outbuff, SettingCell_t * desript_ptr, uint8_t *p_num, uint8_t num){
uint8_t cell_ptr = 0;
uint8_t out_ptr = 0;
SettingCell_t * Cell;
uint8_t * data_p;

	// ���� �� ������������ ���������� �����
//	while(cells[cell_ptr] != 0)
	while(num)
	{
		Cell = FindCell(cells[cell_ptr], desript_ptr);

		// ���� ������ �� ����������, �� ������
		if (Cell == nil)
			return cell_not_found;

		// �������� �������� ���������� ������
		if ((Cell->CellAttr & RD_Att) == 0)
		{
			cell_ptr++;
			continue;
		}

		// ������� ��������
		uint8_t data_size = Cell->CellType & 0x1F;
		if ((Cell->CellType & 0xE0) == Fix8ByteArrayCellType)
			data_size *= ByteArrayBlockSize;

		// ���� ������ ����� ������� �����, �� ������ (�����������)
		if (data_size == 0)
			return cell_descript_error;

		// �������� ����������� ����� � ������
		uint16_t temp16 = out_ptr + data_size + sizeof(Cell->CellType) + sizeof(Cell->CellNumber);
		if (temp16 > MaxOutDataLenght)
			return cell_read_overload_buff;

		outbuff[out_ptr++] = Cell->CellNumber;
		outbuff[out_ptr++] = Cell->CellType;

		// ������ �� ROM
		if ((Cell->CellAttr & ROM_Att) != 0){

			return cell_descript_error;

			// ���� ������ �� ������ �� ������� - ������
			out_ptr += data_size;
		}
		else
		// ������ �� RAM
		// ���� �� ����� ����� ������, ������ ��������� �� ���������� ������
		if (Cell->ReadProc == nil)
		{
			// ������, ���� ������ � ������ �������
			if (Cell->VarPtr == nil)
				return cell_descript_error;

			memcpy(&outbuff[out_ptr], Cell->VarPtr,	data_size);
			out_ptr += data_size;
		}
		else if (Cell->ReadProc != nil)
		// ����� ������
		{
			if (Cell->CellAttr & Action_Att){
				if(Cell->CellAttr & RD_Att){
					RD_Funct_Ptr_t_3 RD_Funct_Ptr;
//					uint8_t * data_p;

					RD_Funct_Ptr = (RD_Funct_Ptr_t_3)Cell->ReadProc;
					data_p = (*RD_Funct_Ptr)(Cell->DefaultValue,Cell->LowLim,Cell->HighLim);

				}
			}
			else{
				// ������� ��������� �� ����� ������
				RD_Funct_Ptr_t RD_Funct_Ptr;
//				uint8_t * data_p;
				// ��������� ���������
	////////////////////////////////////////////////////////////////////
	//����� � ������, �� ���!!!!!
	////////////////////////////////////////////////////////////////////
				if ((Cell->VarPtr == nil) && (Cell->DefaultValue != nil)){

					RD_Funct_Ptr_t_void RD_Funct_Ptr_void;
					RD_Funct_Ptr_void = (RD_Funct_Ptr_t_void) Cell->ReadProc;

					data_p = RD_Funct_Ptr_void((void*)Cell->DefaultValue, Cell->LowLim);

				}
				else{

					RD_Funct_Ptr = (RD_Funct_Ptr_t)Cell->ReadProc;
					// ����� ������
					data_p = (*RD_Funct_Ptr)();
				}
			}

			// ��� ��������
			if (data_p == nil)
				return cell_cmd_not_available;

			// ���������� ���� ������
			memcpy(&outbuff[out_ptr], data_p, data_size);
			out_ptr += data_size;
		}
		// ����� �������� ������������
		else
			return cell_descript_error;

		cell_ptr++;
		num--;
	};
	*p_num = out_ptr;
//	outbuff[0] = out_ptr - 1;
	// �������� ������
	if (out_ptr > 1)
		return cell_data_ready;
	else
		return cell_not_found;
}
SettingCell_t * select_cell_table(void){
	 return &control.comp_table[0];
}
