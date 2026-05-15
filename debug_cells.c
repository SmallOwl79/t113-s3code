/*! @file ***************************************************************************************
 *
 * @brief
 * 			������ �������� ���������� �����
 *
 * $Revision: 1 $
 * $Date: 2012-06-21 $
 *
 *
 *************************************************************************************************/

//#define DEBUG_CELLS_H_


//*-----------------------------------------------------------------------------------------------
//*		������������ �����
//*-----------------------------------------------------------------------------------------------
//#include "timers.h"
#include "debug_cells.h"
//#include "cmd_help.h"
//#include "devconfig.h"
#include "control.h"

//const SettingCell_t rf_MainDebugCellTable[] ={
//
//		{	220,
//			FixByteArrayCellType + sizeof(dev_config.eth_ip),
//			RD_Att+WR_Att,
//			nil,
//			(void*)&cmd_h_read_eth_ip, (void*)&cmd_h_write_eth_ip,
//			nil, nil, nil,
//			"Eth IP"
//		},
//		{	221,
//			FixByteArrayCellType + sizeof(dev_config.eth_mask),
//			RD_Att+WR_Att,
//			nil,
//			(void*)&cmd_h_read_eth_mask, (void*)&cmd_h_write_eth_mask,
//			nil, nil, nil,
//			"Eth MASK"
//		},
//		{	222,
//				FixByteArrayCellType + sizeof(dev_config.eth_gate),
//			RD_Att+WR_Att,
//			nil,
//			(void*)&cmd_h_read_eth_gate, (void*)&cmd_h_write_eth_gate,
//			nil, nil, nil,
//			"Eth GATE"
//		},
//		{	223,
//				FixByteArrayCellType + sizeof(dev_config.eth_mac),
//			RD_Att+WR_Att,
//			nil,
//			(void*)&cmd_h_read_eth_mac, (void*)&cmd_h_write_eth_mac,
//			nil, nil, nil,
//			"Eth MAC"
//		},
//		{	240,
//			VarStringCellType + sizeof(dev_config.sn),
//			RD_Att+WR_Att,
//			nil,
//			(void*)&cmd_h_read_sn, (void*)&cmd_h_write_sn,
//			nil, nil, nil,
//			"Serial number"
//		},
//
//	// ����� ������
//	{
//		0,0,0, nil, nil, nil, nil, nil, nil, ""
//	}
//};
//*-----------------------------------------------------------------------------------------------
/** 	����� ������� �������� �����
 *
 * @return	��������� �� �������� ���������� �����
 *
 * @note	����� ��� ������� ����� �������� ��������� ���������� ������
 *			��� ������ ������ ����������
 * @note	��������� ���������� ��������� � ������ � ��������� ���������� �����				*/
//*-----------------------------------------------------------------------------------------------
// SettingCell_t * select_cell_table(void){
//	 return &control.comp_table[0];
// }
