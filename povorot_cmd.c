/*
 * povorot_cmd.c
 *
 *  Created on: 31 окт. 2023 г.
 *      Author: Petr
 */

#include "string.h"
#include "povorot.h"
#include "control.h"

// void main_cntrl_dbg_set_offset_w(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data);
// void main_cntrl_dbg_set_offset_h(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data);
// uint8_t* main_cntrl_dbg_get_offset_w(uint32_t adf32, uint32_t tmp0, uint32_t cmd);
// uint8_t* main_cntrl_dbg_get_offset_h(uint32_t adf32, uint32_t tmp0, uint32_t cmd);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t povorot_cmd_make_ctrl_cmd(povorot_struct *p_ctl,uint8_t cmd, uint8_t len, uint8_t *p_data, dev_cntrl_ack_response_e have_ack){
	return povorot_make_ctrl_cmd(p_ctl,cmd,len,p_data,have_ack);
//cntrl_dev_sys_msg_que_type_s *p_buf;
//uint32_t res=0;
//	p_buf = osPoolAlloc(p_ctl->ctrl_cmd.cntrl_msg_pool_id);
//	if(p_buf == NULL) {
//		p_ctl->error |= povorot_err_soft;
//		res = 1;
//	}
//	else{
//		p_buf->cmd = cmd;
//		p_buf->have_ack = have_ack;
//		p_buf->time = osKernelSysTick();
//		if(len){
//			memcpy(&p_buf->buf[0],p_data,len);
//		}
//
//		ack_msg_on_send_cmd (p_ctl->p_ack, p_buf, p_ctl->id);
//
//		if (osMessagePut(p_ctl->ctrl_cmd.cntrl_msg_que_id, (uint32_t)p_buf, 1000) != osOK){
//
//			p_ctl->error |= povorot_err_soft;
//
//			ack_msg_on_clear_send_cmd(p_ctl->p_ack, p_buf, p_ctl->id);
//
//			osPoolFree(p_ctl->ctrl_cmd.cntrl_msg_pool_id,p_buf);
//
//			res = 2;
//		}
//		else{
//			osSignalSet(p_ctl->thread_id,p_parser_event_get_cmd);
//		}
//	}
//	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t povorot_cmd_send_cmd_direction(povorot_struct *p_ctl,povorot_cmd_direction direction){
uint32_t res=0;
	res |= povorot_cmd_make_ctrl_cmd(p_ctl,povorot_cntrl_cmd_direction,1,(uint8_t*)&direction,dev_cntrl_ack_not_response);
	return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t povorot_cmd_send_cmd_stop(povorot_struct *p_ctl){
uint32_t res=0;
	res |= povorot_cmd_make_ctrl_cmd(p_ctl,povorot_cntrl_cmd_stop,0,NULL,dev_cntrl_ack_not_response);
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t povorot_cmd_exec_rotate(povorot_struct *p_ctl){
uint32_t res=0;
	res |= povorot_cmd_make_ctrl_cmd(p_ctl,povorot_cntrl_cmd_exec_direction,0,NULL,dev_cntrl_ack_not_response);
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* povorot_cmd_dbg_get_sel_direction(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	p_ctl->ack_dbg_tmp[0] = 0;
	if(p_ctl->cmd_direct == cmd){
		p_ctl->ack_dbg_tmp[0] = 1;
	}
	return (uint8_t*)p_ctl->ack_dbg_tmp;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_cmd_dbg_set_sel_direction(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
//	if(p_ctl->work_mode == povorot_work_mode_cmd) return;
//	p_ctl->cmd_direct = (povorot_cmd_direction)cmd;
	povorot_cmd_send_cmd_direction(p_ctl,(povorot_cmd_direction)cmd);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* povorot_cmd_dbg_get_cmd_direction(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	p_ctl->ack_dbg_tmp[0] = 0;
	if(p_ctl->work_mode == povorot_work_mode_cmd) p_ctl->ack_dbg_tmp[0] = 1;
	return (uint8_t*)&p_ctl->ack_dbg_tmp[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_cmd_dbg_set_cmd_direction(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	if(*p8_data){
		povorot_cmd_exec_rotate(p_ctl);
	}
	else{
		povorot_cmd_send_cmd_stop(p_ctl);
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* povorot_cmd_dbg_get_correct_angle(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->correct_angle;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* povorot_cmd_dbg_get_cmd_rotate_angle(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->cmd_rotate_angle;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_cmd_dbg_set_cmd_rotate_angle(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	povorot_cmd_make_ctrl_cmd(p_ctl,povorot_cntrl_cmd_angle_pos,sizeof(float),p8_data,dev_cntrl_ack_not_response);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* povorot_cmd_dbg_get_cmd_rotate_tilt(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->cmd_tilt_angle;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_cmd_dbg_set_cmd_rotate_tilt(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	povorot_cmd_make_ctrl_cmd(p_ctl,povorot_cntrl_cmd_tilt_pos,sizeof(float),p8_data,dev_cntrl_ack_not_response);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_cmd_dbg_get_cmd_start_scan_angle(uint32_t adf32, uint32_t tmp0, uint32_t tmp){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->scan_down_angle;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_cmd_dbg_get_cmd_stop_scan_angle(uint32_t adf32, uint32_t tmp0, uint32_t tmp){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->scan_up_angle;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_cmd_dbg_get_cmd_num_scan_point(uint32_t adf32, uint32_t tmp0, uint32_t tmp){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&p_ctl->scan_num_step;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_cmd_dbg_get_cmd_num_scan_point_delay(uint32_t adf32, uint32_t tmp0, uint32_t tmp){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	p_ctl->ack_dbg_tmp[0] = p_ctl->scan_step_time_max / 1000;
	return (uint8_t*)&p_ctl->ack_dbg_tmp[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_cmd_dbg_set_cmd_start_scan_angle(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	povorot_cmd_make_ctrl_cmd(p_ctl,povorot_cntrl_set_angle_start_scan,4,p8_data,dev_cntrl_ack_not_response);
return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_cmd_dbg_set_cmd_stop_scan_angle(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	povorot_cmd_make_ctrl_cmd(p_ctl,povorot_cntrl_set_angle_stop_scan,4,p8_data,dev_cntrl_ack_not_response);
return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_cmd_dbg_set_cmd_correct_angle(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	povorot_cmd_make_ctrl_cmd(p_ctl,povorot_cntrl_set_angle_correct,4,p8_data,dev_cntrl_ack_not_response);
return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_cmd_dbg_set_cmd_num_scan_point(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	povorot_cmd_make_ctrl_cmd(p_ctl,povorot_cntrl_set_num_step_scan,1,p8_data,dev_cntrl_ack_not_response);
return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_cmd_dbg_set_cmd_num_scan_point_delay(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	povorot_cmd_make_ctrl_cmd(p_ctl,povorot_cntrl_set_time_step_scan,1,p8_data,dev_cntrl_ack_not_response);
return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_cmd_dbg_send_cmd_start_scan(uint32_t adf32, uint32_t tmp0, uint32_t tmp, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	if(*p8_data){
		povorot_cmd_make_ctrl_cmd(p_ctl,povorot_cntrl_cmd_start_scan,0,NULL,dev_cntrl_ack_not_response);
	}
	else{
//добавить команду на стоп???!!!
		povorot_cmd_send_cmd_stop(p_ctl);
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void povorot_cmd_dbg_set_en_main_tune(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	control.dev_config.rec.tmp[1] = 0;
	if(*p8_data){
		control.dev_config.rec.tmp[1] = 1;
	}
	dev_config_write(&control.dev_config);
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t* povorot_cmd_dbg_get_en_main_tune(uint32_t adf32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	return (uint8_t*)&control.dev_config.rec.tmp[1];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t*  povorot_cmd_dbg_get_cmd_start_active(uint32_t adf32, uint32_t tmp0, uint32_t tmp){
	povorot_struct *p_ctl;
	p_ctl = (povorot_struct *)adf32;
	p_ctl->ack_dbg_tmp[0] = 0;
	if(p_ctl->work_mode == povorot_work_mode_scan){
		p_ctl->ack_dbg_tmp[0] = 1;
	}
	return (uint8_t*)&p_ctl->ack_dbg_tmp[0];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void povorot_cmd_make_html_cntrl_table(povorot_struct* p_ctl, ctrl_table_list_struct* p_list){
SettingCell_t* p_cell;
uint8_t num=1;
SettingCell_t 	cell;
uint32_t 		i;
	memset(&p_ctl->html_table,0,sizeof(p_ctl->html_table));
	p_list->name = "Rotate_cntrl";
	p_list->mode = div_time_att | div_wr_att | div_rd_att;// | div_collect_mode;//div_collect_mode
	//div_all_wr | div_collect_mode;// | div_collect_mode_wr;//div_all_rd
	p_list->p_start_cell = &p_ctl->html_table[0];
	p_cell = p_list->p_start_cell;

	cmd_h_add_to_cell_table_ext_size(
							p_cell,
							num++,
							p_ctl,
							NULL,
							NULL,
							0,
							FixIntegerCellType,
							"Rotate config");
	i = 1;

	cell.CellAttr = RD_Att;
	cell.CellType = Float4Cell;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_dbg_get_rotate_angle;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "rotate angle";
	p_cell[i++] = cell;

	cell.CellAttr = RD_Att + Action_Att;
	cell.CellType = Float4Cell;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_dbg_get_tilt_angle;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "tilt angle";
	p_cell[i++] = cell;

	cell.CellAttr = RD_Att + Action_Att;
	cell.CellType = Float4Cell;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_base_angle;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "base angle";
	p_cell[i++] = cell;


	cell.CellAttr = RD_Att + Action_Att + WR_Att;
	cell.CellType = Float4Cell;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_correct_angle;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_cmd_correct_angle;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "correct angle";
	p_cell[i++] = cell;

	cell.CellAttr = 0;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = NULL;
	cell.ReadProc = NULL;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "manual cntr";
	p_cell[i++] = cell;


	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_sel_direction;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_sel_direction;
	cell.LowLim = 1;
	cell.HighLim = povorot_cmd_rotate_left;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "rotate left";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_sel_direction;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_sel_direction;
	cell.LowLim = 1;
	cell.HighLim = povorot_cmd_rotate_right;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "rotate right";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_sel_direction;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_sel_direction;
	cell.LowLim = 1;
	cell.HighLim = povorot_cmd_tilt_up;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "tilt up";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_sel_direction;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_sel_direction;
	cell.LowLim = 1;
	cell.HighLim = povorot_cmd_tilt_down;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "tilt down";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_cmd_direction;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_cmd_direction;
	cell.LowLim = 1;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "rot. start/stop";
	p_cell[i++] = cell;

	cell.CellAttr = 0;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = NULL;
	cell.ReadProc = NULL;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "manual to angle";
	p_cell[i++] = cell;


	cell.CellAttr = RD_Att + Action_Att + WR_Att;
	cell.CellType = Float4Cell;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_cmd_rotate_angle;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_cmd_rotate_angle;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "to angle";
	p_cell[i++] = cell;

	cell.CellAttr = RD_Att + Action_Att + WR_Att;
	cell.CellType = Float4Cell;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_cmd_rotate_tilt;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_cmd_rotate_tilt;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "to tilt";
	p_cell[i++] = cell;

	cell.CellAttr = 0;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = NULL;
	cell.ReadProc = NULL;
	cell.WriteProc = NULL;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "auto scan cntrl";
	p_cell[i++] = cell;

	cell.CellAttr = RD_Att + Action_Att + WR_Att;
	cell.CellType = Float4Cell;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_cmd_start_scan_angle;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_cmd_start_scan_angle;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "start scan angle";
	p_cell[i++] = cell;

	cell.CellAttr = RD_Att + Action_Att + WR_Att;
	cell.CellType = Float4Cell;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_cmd_stop_scan_angle;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_cmd_stop_scan_angle;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "stop scan angle";
	p_cell[i++] = cell;

	cell.CellAttr = RD_Att + Action_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_cmd_num_scan_point;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_cmd_num_scan_point;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "num scan point";
	p_cell[i++] = cell;

	cell.CellAttr = RD_Att + Action_Att + WR_Att;
	cell.CellType = FixIntegerCellType + 1;
	cell.VarPtr = NULL;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_cmd_num_scan_point_delay;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_cmd_num_scan_point_delay;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.CellNumber = num++;
	cell.LowLim = 0;
	cell.DescriptStr = "scan point time";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_cmd_start_active;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_send_cmd_start_scan;
	cell.LowLim = 1;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "scan. start/stop";
	p_cell[i++] = cell;

	cell.CellAttr = Default_Attr + WR_Att + Action_Att + BIN_Att + RD_Att,
	cell.CellType = FixIntegerCellType + 1,
	cell.CellNumber = num++;
	cell.VarPtr = 0;
	cell.ReadProc = (void*)(void*)povorot_cmd_dbg_get_en_main_tune;
	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_set_en_main_tune;
	cell.LowLim = 2;
	cell.HighLim = 0;
	cell.DefaultValue = (uint32_t)p_ctl;
	cell.DescriptStr = "en. main tune";
	p_cell[i++] = cell;

//#ifndef NOT_VIDEO_RECEIVER
//	cell.CellAttr = RD_Att + Action_Att + WR_Att;
//	cell.CellType = FixIntegerCellType + 2;
//	cell.VarPtr = NULL;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_offset_w;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_offset_w;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)&control;
//	cell.CellNumber = num++;
//	cell.LowLim = 0;
//	cell.DescriptStr = "Offset W";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = RD_Att + Action_Att + WR_Att;
//	cell.CellType = FixIntegerCellType + 2;
//	cell.VarPtr = NULL;
//	cell.ReadProc = (void*)(void*)main_cntrl_dbg_get_offset_h;
//	cell.WriteProc = (void*)(void*)main_cntrl_dbg_set_offset_h;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)&control;
//	cell.CellNumber = num++;
//	cell.LowLim = 0;
//	cell.DescriptStr = "Offset H";
//	p_cell[i++] = cell;
//#endif

//	cell.CellAttr = Default_Attr + Action_Att + WR_Att;
//	cell.CellType = FixIntegerCellType + 1;
//	cell.VarPtr = NULL;
//	cell.ReadProc = NULL;
//	cell.WriteProc = (void*)(void*)povorot_cmd_dbg_send_cmd_start_scan;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.CellNumber = num++;
//	cell.LowLim = 0;
//	cell.DescriptStr = "cmd scan";
//	p_cell[i++] = cell;


//	cell.CellAttr = Action_Att + RD_Att;
//	cell.CellType = FixIntegerCellType + 2;
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)air_cntr_dbg_get_flow;
//	cell.WriteProc = NULL;
//	cell.LowLim = 0;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Flow m3/2";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = Action_Att + RD_Att;
//	cell.CellType = FixSignIntegerCellType + 2;
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)air_cntr_dbg_get_k16_error;
//	cell.WriteProc = NULL;
//	cell.LowLim = 0;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Error m3/2";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 1,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)air_cntr_dbg_get_cntrl_cmd_mode;
//	cell.WriteProc = (void*)(void*)air_cntr_dbg_set_cntrl_cmd_mode;
//	cell.LowLim = 1;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Cmd mode";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 1,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)air_cntr_dbg_get_cntrl_vol_mode;
//	cell.WriteProc = (void*)(void*)air_cntr_dbg_set_cntrl_vol_mode;
//	cell.LowLim = 1;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Vol. mode";
//	p_cell[i++] = cell;
//
//	cell.CellAttr = Default_Attr + WR_Att + Action_Att + Group_Att + RD_Att,
//	cell.CellType = FixIntegerCellType + 1,
//	cell.CellNumber = num++;
//	cell.VarPtr = 0;
//	cell.ReadProc = (void*)(void*)air_cntr_dbg_get_cntrl_manual_mode;
//	cell.WriteProc = (void*)(void*)air_cntr_dbg_set_cntrl_manual_mode;
//	cell.LowLim = 1;
//	cell.HighLim = 0;
//	cell.DefaultValue = (uint32_t)p_ctl;
//	cell.DescriptStr = "Manual mode";
//	p_cell[i++] = cell;

}
