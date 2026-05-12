/**
  ******************************************************************************
  * @file    httpd_cg_ssi.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011
  * @brief   Webserver SSI and CGI handlers
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "httpd_help.h"

#include "lwip/debug.h"
#include "lwip/apps/httpd.h"
#include "lwip/tcp.h"
#include "lwip/apps/fs.h"
#include "fsdata.h"

#include "common_proto_info.h"
#include "jfes.h"
//#include "logger.h"
#include "control.h"
//#include "devconfig.h"
#include "debug_cells_lib.h"
//#include "paramconfig.h"
//#include "lan_cfg.h"

#define MAX_CGI_PARAMS 10

static const char *http_str_zero = "0";
static const char *http_str_one = "1";
#pragma location=".ddr_data"
static char tmp_buf[356];
#pragma location=".ddr_data"
static char tmp_buf1[256];
extern const soft_descript_t soft_descript;
//extern RTC_HandleTypeDef RtcHandle;

//const char * http_common_req_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res);
const char * http_cntrl_navigator_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res, uint32_t, uint16_t);
const char * http_cntrl_sense_ch_set_req_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res, uint32_t, uint16_t);
const char * http_cntrl_pwr_ch_relay_set_req_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res, uint32_t, uint16_t);
const char * http_cntrl_conf_set_req_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res, uint32_t, uint16_t);
const char * http_lan_set_req_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res, uint32_t, uint16_t);
const char * http_pwr_set_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res, uint32_t, uint16_t);
const char * http_sen_set_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res, uint32_t, uint16_t);
const char * http_log_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res, uint32_t, uint16_t);
const char * sys_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res, uint32_t, uint16_t);
//const char * http_inj_set_req_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res);

//const tCGI t_status_common={"/common.js", http_common_req_ack};
const tCGI t_set_pwr_ch={"navigator.js", http_cntrl_navigator_ack};
//const tCGI t_set_sense_ch={"/sense_ch_set.js", http_cntrl_sense_ch_set_req_ack};
//const tCGI t_set_pwr_cfg_rel_ch={"/pwr_relay_config.js", http_cntrl_pwr_ch_relay_set_req_ack};
const tCGI t_conf_ref={"conf_set.js", http_cntrl_conf_set_req_ack};
const tCGI t_lan={"lan_set.js", http_lan_set_req_ack};
//const tCGI t_pwr_set={"/pwr_set.js", http_pwr_set_ack};
//const tCGI t_sen_set={"/sen_set.js", http_sen_set_ack};
//const tCGI t_log={"/log_set.js", http_log_ack};
const tCGI t_sys={"sys.js", sys_ack};

tCGI CGI_TAB[MAX_CGI_PARAMS];


ctrl_table_list_struct* find_table_by_name(ctrl_table_list_struct *p_ctl, char *p_name){
uint8_t i;
	for(i=0;i<MAX_NUM_FUNC_TABLE;i++){
		if(!strcmp(p_ctl->name,p_name)){
			return p_ctl;
		}
		p_ctl++;
	}
return NULL;
}

 double atod(char * string)  //Released to public domain by Jesse Gordon. You may remove this notice.
{
    enum {maxinputchars=32, significantdigits=9};
    static char src[maxinputchars],dst[significantdigits+2];
    static int ii,io,gotdp,exp;
    static double sign;
    static int gotall;
    static double rv;
    rv=0;
    dst[0]=0;
    src[0]=0;
    strncpy(src,string,maxinputchars);//Copy source string to a local string so we can modify it safely.
    src[maxinputchars-1]=0; //Assure terminator.
    ii=0;
    while((src[ii]==' ')||(src[ii]=='\t')){ii++;}   //Cut past spaces and tabs.
    if((src[ii]=='+')||(src[ii]=='-')){ii++;}       //Accept a single sign symnbol.
    while((src[ii]>='0')&&(src[ii]<='9')){ii++;}     //Allow any number of numbers
    if(src[ii]=='.'){ii++;}                         //Allow a dot
    while((src[ii]>='0')&&(src[ii]<='9')){ii++;}     //Allow any number of more numbers.
    src[ii]=0;  //Now src[] contains a possible leading + or -, followed by ONLY numbers, except ONE optional period. A second period will be end of line.

    sign=1;
    gotdp=0;
    exp=0;
    ii=0;
    io=0;
    while((src[ii]==' ')||(src[ii]=='\t')){ii++;}
    if((src[ii]=='+')||(src[ii]=='-'))  //Get the sign, if any.
    {
        if(src[ii]=='-'){sign=-1;}
        ii++;
    }
    while(src[ii]=='0'){ii++;}   //Drop any leading zeros before the decimal point.
    if(src[ii]=='.'){ii++;gotdp=1;}   //If we get a decimal point, now we start keeping track of exponent.
    while(src[ii]=='0'){ii++;exp--;}    //Drop any leading zeros after the decimal point.
    while(((ii<maxinputchars)&&((src[ii]>='0')&&(src[ii]<='9')))||(src[ii]=='.'))   //Get any digits
    {
        if(src[ii]=='.')
        {
            gotdp=1;
        }else{
            if(io<significantdigits){gotall=0;}else{gotall=1;}
            if(gotall==0)
            {
                dst[io]=src[ii];
                io++;
                dst[io]=0;
            }
            if((gotall==0)||(gotdp==0))
            {
                if(gotall){exp++;} //If we got all 7/9 digits but no decimal point, we add to exponent.
                if(gotdp){exp--;}   //If we got a decimal point but haven't got all 7/9 digits, we subtract from exponent.
            }else{
                ii=maxinputchars+1;
            }   //Once our 7/9 significant digits are captured and we got a decimal point, we're done!
        }
        ii++;
    }
    rv=0;
    if(dst[0]==0){return(0);}
    rv=((double)atoi(dst))*sign;
    rv=rv*pow(10,exp);

    return(rv);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void httpd_make_src_string(char *p_str, uint16_t size, uint32_t addr, uint16_t port){
uint8_t a,b,c,d;
	memset(p_str,0,size);
	a = ((uint8_t*)&addr)[0];
	b = ((uint8_t*)&addr)[1];
	c = ((uint8_t*)&addr)[2];
	d = ((uint8_t*)&addr)[3];
	snprintf(p_str,size,"%s %s %d.%d.%d.%d %s %d","Http","ip:",a,b,c,d,"port:",port);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//const char * http_cntrl_conf_set_req_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res1, uint32_t ip, uint16_t port){
//char *p_data;
//jfes_value_t *child;
//uint8_t type_cmd;
//uint8_t b_flash, b_write, b_read;
//char tmp_32[32];
//char *p_char;
//	type_cmd = -1;
//	*p_res1 = NULL;
//	p_data = (char*)make_json_file_header(j_data,sizeof(j_data),(char*)&CGI_TAB[iIndex].pcCGIName[1],strlen(CGI_TAB[iIndex].pcCGIName));
//	child = jfes_create_object_value(&j_config);
//
//	htpp_parser_find_rd_wr_fl(iNumParams,pcParam,pcValue,&b_read,&b_write,&b_flash);
//
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"type_cmd",&type_cmd);
//
//	if(b_write){
////		if (control.cntrl_mode != cntr_control_manual){
////uint8_t i,f;
////uint32_t res_tmp;
////char *ptr;
////uint8_t len;
////uint32_t res;
//		htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"cmd",&p_char);
////		if(!strcmp("set_time",p_char)){
////			struct tm *loc_time,m_time;
////			time_t tim;
////			uint32_t ret_data,ms;
////			RTC_TimeTypeDef time;
////			RTC_DateTypeDef date;
////
////			httpd_make_src_string((char*)tmp_buf,sizeof(tmp_buf),ip,port);
////			cmd_diag(ddebug_id_cmd,"%s %s %s","get cmd = SET TIME", "from=",tmp_buf);
////
////			htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,"time",&ret_data);
////			htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,"ms",&ms);
////			tim = ret_data;
////
//////			__disable_interrupt();
////			__disable_irq();
////			loc_time = localtime(&tim);
////			memcpy(&m_time,loc_time,sizeof(m_time));
////			__enable_interrupt();
////
////			time.Hours = m_time.tm_hour;
////			time.Minutes = m_time.tm_min;
////			time.Seconds = m_time.tm_sec;
////			time.SubSeconds = ((uint64_t)(ms) * 256) / (1000);
////			time.TimeFormat = 0;
////			time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
////			time.StoreOperation = RTC_STOREOPERATION_SET;
////
////			date.Year = (m_time.tm_year + 1900) % 100;
////			date.Month = m_time.tm_mon + 1;
////			date.Date = m_time.tm_mday;
////			date.WeekDay = m_time.tm_wday + 1;
////
////			HAL_RTC_SetTime(&RtcHandle, &time, FORMAT_BIN);
////			HAL_RTC_SetDate(&RtcHandle, &date, FORMAT_BIN);
////
////			HAL_RTCEx_SetSynchroShift(&RtcHandle, 0, 256 - time.SubSeconds);
////		}
////		if(!strcmp("restart",p_char)){
////			httpd_make_src_string((char*)tmp_buf,sizeof(tmp_buf),ip,port);
////			cmd_diag(ddebug_id_cmd,"%s %s %s","get cmd = RESTART", "from=",tmp_buf);
////
////			osSignalSet(control.thread_id,main_cntrl_thread_event_restart);
////		}
//	}
//	if(b_read){
//uint32_t ptr;
////char tmp[sizeof(control.config.sn) + 1];
//
//		htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"cmd",&p_char);
////		if(!strcmp("get_id",p_char)){
////
////			memset(tmp_32,0,sizeof(tmp_32));
////			device_info_add_field((char*)&tmp_32[0],"TYPE=RPU",0);
////			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"id_0", 0);
////
////			memset(tmp_32,0,sizeof(tmp_32));
////			ptr = device_info_add_field((char*)&tmp_32[0],"DEV=",0);
////			ptr += device_info_add_field((char*)&tmp_32[ptr],(char*)soft_descript.Device_type,0);
////			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"id_1", 0);
////
////			memset(tmp_32,0,sizeof(tmp_32));
////			ptr = device_info_add_field((char*)&tmp_32[0],"HV=",0);
////			ptr += device_info_add_field((char*)&tmp_32[ptr],DEVICE_HARDVARE_VERSION,0);
////			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"id_2", 0);
////
////			memset(tmp_32,0,sizeof(tmp_32));
////			ptr = device_info_add_field((char*)&tmp_32[0],"CV=",0);
////			ptr += device_info_add_field((char*)&tmp_32[ptr],DEVICE_HARDVARE_VERSION,0);
////			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"id_3", 0);
////
////			memset(tmp_32,0,sizeof(tmp_32));
////			ptr = device_info_add_field((char*)&tmp_32[0],"FV=",0);
////			ptr += device_info_add_field((char*)&tmp_32[ptr],(char*)soft_descript.Comp_date,0);
////			ptr += device_info_add_field((char*)&tmp_32[ptr],"  ",0);
////			ptr += device_info_add_field((char*)&tmp_32[ptr],(char*)soft_descript.Comp_time,0);
////			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"id_4", 0);
////
////			memset(tmp_32,0,sizeof(tmp_32));
////			memset(tmp,0,sizeof(tmp));
////			memcpy(tmp,control.config.sn,sizeof(control.config.sn));
////
////			ptr = device_info_add_field((char*)&tmp_32[0],"SN=",0);
////			ptr += device_info_add_field((char*)&tmp_32[ptr],tmp,0);
////			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"id_5", 0);
////
////		}
////		if(!strcmp("get_time",p_char)){
////			struct tm t_m;
////			RTC_DateTypeDef					date;
////			RTC_TimeTypeDef					time;
////			uint32_t time_s;
////			uint32_t time_ms;
////			uint64_t tmp;
////		//	__disable_interrupt();
////			__disable_irq();
////				HAL_RTC_GetTime(&RtcHandle, &time, FORMAT_BIN);
////				HAL_RTC_GetDate(&RtcHandle, &date, FORMAT_BIN);
////			__enable_interrupt();
////				t_m.tm_hour = time.Hours;
////				t_m.tm_min = time.Minutes;
////				t_m.tm_sec = time.Seconds;
////				t_m.tm_year = date.Year + 100;
////				t_m.tm_mday = date.Date;
////				t_m.tm_mon = date.Month - 1;
////				t_m.tm_wday = date.WeekDay - 1;
////				t_m.tm_isdst = -1;
////				time_s = mktime(&t_m);
////
////				tmp = 256 - (time.SubSeconds%256);
////				tmp = tmp * 1000;
////				tmp = tmp / 256;
////				time_ms = tmp;
////
////				memset(tmp_32,0,sizeof(tmp_32));
////				sprintf(tmp_32, "%d",time_s);
////				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_32, 0), "time", 0);
////
////				memset(tmp_32,0,sizeof(tmp_32));
////				sprintf(tmp_32, "%d",time_ms);
////				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_32, 0), "ms", 0);
////		}
////		if(!strcmp("get_list",p_char)){
////			htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"sub_cmd",&p_char);
////			if(!strcmp("start",p_char)){
//////				uint8_t num;
////				collect_param_get_active_num(&control.col_param);
////
////				collect_param_clear(&control.col_param);
////				col_param_add_func(&control);
////
////				memset(tmp_32,0,sizeof(tmp_32));
////				sprintf(tmp_32, "%d",collect_param_get_active_num(&control.col_param));
////				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_32, 0), "num", 0);
////			}
////			if(!strcmp("continue",p_char)){
////				uint32_t i,num,start;
////				char *p_str;
////				htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,"num_read",&num);
////				htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,"start_num",&start);
////				for(i=0;i<num;i++){
////					p_str = collect_param_get_name(&control.col_param,start);
////					if(p_str){
////						htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"n_",start);
////						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
////					}
////					else{
////						break;
////					}
////					start++;
////				}
////				memset(tmp_32,0,sizeof(tmp_32));
////				sprintf(tmp_32, "%d",i);
////				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_32, 0), "num", 0);
////			}
////		}
//		if(!strcmp("get_param",p_char)){
//			htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"sub_cmd",&p_char);
//			if(!strcmp("start",p_char)){
////				uint8_t num;
////				collect_param_get_active_num(&control.col_param);
//				memset(tmp_32,0,sizeof(tmp_32));
//				sprintf(tmp_32, "%d",collect_param_get_active_num(&control.col_param));
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_32, 0), "num", 0);
//			}
//			if(!strcmp("continue",p_char)){
//				uint32_t i,num,start;
//				char *p_str;
//				htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,"num_read",&num);
//				htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,"start_num",&start);
//				for(i=0;i<num;i++){
//					p_str = collect_param_get_value(&control.col_param,start);
//					if(p_str){
//						htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"d_",start);
//						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
//					}
//					else{
//						break;
//					}
//					start++;
//				}
//				memset(tmp_32,0,sizeof(tmp_32));
//				sprintf(tmp_32, "%d",i);
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_32, 0), "num", 0);
//			}
//		}
//	}
//	return close_json_file(child,p_data);
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//const char * http_cntrl_ref_set_req_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res1){
//char *p_data;
//jfes_value_t *child;
//uint32_t res;
//uint8_t type_cmd;
//uint8_t b_flash, b_write, b_read;
//	type_cmd = -1;
//
//	*p_res1 = NULL;
//	p_data = (char*)make_json_file_header(j_data,sizeof(j_data),(char*)&CGI_TAB[iIndex].pcCGIName[1],strlen(CGI_TAB[iIndex].pcCGIName));
//	child = jfes_create_object_value(&j_config);
//
//	htpp_parser_find_rd_wr_fl(iNumParams,pcParam,pcValue,&b_read,&b_write,&b_flash);
//
//	res = htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"type_cmd",&type_cmd);
//
//	if(b_write){
//
//	}
//	if(b_read){
//	}
//	return close_json_file(child,p_data);
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//const char * http_inj_set_req_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res1){
//char *p_data;
//jfes_value_t *child;
//uint32_t res;
//uint8_t type_cmd;
//uint8_t b_flash, b_write, b_read;
//char *p_out;
//char tmp_32[32];
//uint8_t b_off,b_13,b_15,b_18,b_ref,b_pwr;
//uint8_t ch_num;
//cmd_result_t  res_cmd=cmd_ok;
//	p_out = (char*)tmp_buf;
//	type_cmd = -1;
//
//	*p_res1 = NULL;
//	p_data = (char*)make_json_file_header(j_data,sizeof(j_data),(char*)&CGI_TAB[iIndex].pcCGIName[1],strlen(CGI_TAB[iIndex].pcCGIName));
//	child = jfes_create_object_value(&j_config);
//
//	htpp_parser_find_rd_wr_fl(iNumParams,pcParam,pcValue,&b_read,&b_write,&b_flash);
//
//	res = htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"type_cmd",&type_cmd);
//
//	if((b_write) && (1)){
//	}
//	if(b_read){
//
//	}
//	return close_json_file(child,p_data);
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//const char * http_lan_set_req_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res1, uint32_t ipsrc, uint16_t port){
//char *p_data;
//jfes_value_t *child;
//uint32_t res,res_error=0;
//uint8_t b_write, b_read, b_flash;
//char *p_out;
//
//	p_out = (char*)tmp_buf;
//	*p_res1 = NULL;
//
//	p_data = (char*)make_json_file_header(j_data,sizeof(j_data),(char*)&CGI_TAB[iIndex].pcCGIName[1],strlen(CGI_TAB[iIndex].pcCGIName));
//	child = jfes_create_object_value(&j_config);
//
//	htpp_parser_find_rd_wr_fl(iNumParams,pcParam,pcValue,&b_read,&b_write,&b_flash);
//
//	if(b_write){
////сетевой адрес
//uint32_t ip[4],mask[4],gate[4];
//char *ptr;
//
//		httpd_make_src_string((char*)tmp_buf,sizeof(tmp_buf),ipsrc,port);
//		cmd_diag(ddebug_id_cmd,"%s %s %s","get cmd = SET LAN PARAMS", "from=",tmp_buf);
//
//		res = htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"lan_cfg_ip_id",&ptr);
//		sscanf(ptr,"%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
//		res |= htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"lan_cfg_mask_id",&ptr);
//		sscanf(ptr,"%d.%d.%d.%d", &mask[0], &mask[1], &mask[2], &mask[3]);
//		res |= htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"lan_cfg_gate_id",&ptr);
//		sscanf(ptr,"%d.%d.%d.%d", &gate[0], &gate[1], &gate[2], &gate[3]);
//		if(!res){
//			tmp_buf[0]=ip[0];
//			tmp_buf[1]=ip[1];
//			tmp_buf[2]=ip[2];
//			tmp_buf[3]=ip[3];
//			tmp_buf[4]=mask[0];
//			tmp_buf[5]=mask[1];
//			tmp_buf[6]=mask[2];
//			tmp_buf[7]=mask[3];
//			tmp_buf[8]=gate[0];
//			tmp_buf[9]=gate[1];
//			tmp_buf[10]=gate[2];
//			tmp_buf[11]=gate[3];
//			res |= lan_cfg_make_cmd(lan_cfg_cmd_set_ip,tmp_buf,12);
//		}
//		if(res) res_error |= 1;//копим ошибку результата
////MAC
//uint32_t mac[6];
//			res = htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"lan_cfg_mac_id",&ptr);
//			sscanf(ptr,"%02x%02x%02x%02x%02x%02x", &mac[0], &mac[1], &mac[2], &mac[3],&mac[4],&mac[5]);
//			tmp_buf[0]=mac[0];
//			tmp_buf[1]=mac[1];
//			tmp_buf[2]=mac[2];
//			tmp_buf[3]=mac[3];
//			tmp_buf[4]=mac[4];
//			tmp_buf[5]=mac[5];
//			if(!res){
//				res |= lan_cfg_make_cmd(lan_cfg_cmd_set_mac,tmp_buf,6);
//			}
//			if(res) res_error |= 1;//копим ошибку результата
//		}
//		if(b_read){
////сетевой адрес
//uint8_t ip[4],mask[4],gate[4];
//			res = lan_cfg_get_ip(ip,mask,gate);
//			if(!res){
//				sprintf(p_out, "%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,p_out, 0), "lan_cfg_ip_id", 0);
//
//				sprintf(p_out, "%d.%d.%d.%d",mask[0],mask[1],mask[2],mask[3]);
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,p_out, 0), "lan_cfg_mask_id", 0);
//
//				sprintf(p_out, "%d.%d.%d.%d",gate[0],gate[1],gate[2],gate[3]);
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,p_out, 0), "lan_cfg_gate_id", 0);
//			}
//			if(res) res_error |= 1;//копим ошибку результата
////mac
//uint8_t mac[6];
//			res = lan_cfg_get_mac(mac);
//			if(!res){
//				sprintf(p_out, "%02x:%02x:%02x:%02x:%02x:%02x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,p_out, 0), "lan_cfg_mac_id", 0);
//			}
//			if(res) res_error |= 1;//копим ошибку результата
//		}
//		return close_json_file(child,p_data);
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char * http_cntrl_navigator_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res1, uint32_t ip, uint16_t port){
char *p_data;
jfes_value_t *child;
uint32_t res;
uint8_t type_cmd;
uint8_t b_flash, b_write, b_read;
char *p_out;
char tmp_32[32];
uint8_t b_off,b_on;
uint8_t ch_num;
//cmd_result_t  res_cmd=cmd_ok;
uint32_t res_tmp;
uint8_t pos=0;
char *ptr;
char* p_char;
char* p_id;


	p_out = (char*)tmp_buf;
	type_cmd = -1;
	memset(tmp_32,0,sizeof(tmp_32));

	*p_res1 = NULL;
	p_data = (char*)make_json_file_header(j_data,sizeof(j_data),(char*)&CGI_TAB[iIndex].pcCGIName[1],strlen(CGI_TAB[iIndex].pcCGIName));
	child = jfes_create_object_value(&j_config);
	printf("ch=%x\r\n",child);

	htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"cmd",&p_char);

//	printf("req=%s\r\n",p_char);

	if(!strcmp("get_table",p_char)){
		htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"sub_cmd",&p_id);

		if(!strcmp("write_cell",p_id)){
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_table", 0),"cmd", 0);
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "write_cell", 0),"sub_cmd", 0);
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "no", 0),"error", 0);

			char* p_name;
			ctrl_table_list_struct* p_table;
			htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"name",&p_name);
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,p_name, 0),"name", 0);
			p_table =  find_table_by_name(&control.table_list[0], p_name);
			if(p_table){
				uint32_t pos = 0;
				htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,"pos",&pos);
				memset(tmp_buf,0,sizeof(tmp_buf));
				sprintf(tmp_buf,"%d",pos);
				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf, 0),"pos", 0);
				char* p_val;
				htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"val",&p_val);
//				printf("%s\r\n",p_val);
				cell_cmd_result_t res_cell;
				uint8_t p_num;
				uint8_t type;
				uint8_t size;
				res_cell = debug_cell_read_type_num(pos, (uint8_t*)tmp_buf,p_table->p_start_cell,&p_num);
//				printf("res cell=%d\r\n",res_cell);
				if(res_cell == cell_data_ready){
//					printf("ready=%d\r\n",res_cell);
					type = tmp_buf[1];
//VarStringCellType
//					printf("typr=%d\r\n",type);
					if((type & (~0x1F)) == VarStringCellType){
						size = type & 0x1F;
//						printf("size=%d\r\n",size);
						if(strlen(p_val)<size) size = strlen(p_val);
						memset(tmp_buf,0,sizeof(tmp_buf));
						uint32_t length,k,m,p;
						length = strlen(p_val);
						k = 0;
						m = 0;
						while(m<length){
							if(p_val[m]=='%'){
//								printf("char=%c\r\n",p_val[m+1]);
//								printf("char=%c\r\n",p_val[m+2]);
								if((p_val[m+1]=='2')&&(p_val[m+2]=='6')){
									tmp_buf[k++]='&';
								}
								else if((p_val[m+1]=='4')&&(p_val[m+2]=='0')){
									tmp_buf[k++]='@';
								}

								else{
									tmp_buf[k++]=' ';
									}
								m+=3;
							}
							else{
								tmp_buf[k++]=p_val[m++];
							}

							if(k>=sizeof(tmp_buf)) break;
						}
						res_cell = debug_cell_write(pos,(uint8_t*)tmp_buf,size,p_table->p_start_cell);
						if(res_cell == cell_cmd_ok){
				//			printf("write ok\r\n");
						}
				//		printf("res cell=%d\r\n",res_cell);
					}
//FixStringCellType
					if((type & (~0x1F)) == FixStringCellType){
						size = type & 0x1F;

						memset(tmp_buf,0,sizeof(tmp_buf));
						uint32_t length,k,m,p;
						length = strlen(p_val);
						k = 0;
						m = 0;
						while(m<length){
							if(p_val[m]=='%'){
//								printf("char=%c\r\n",p_val[m+1]);
//								printf("char=%c\r\n",p_val[m+2]);
								if((p_val[m+1]=='2')&&(p_val[m+2]=='6')){
									tmp_buf[k++]='&';
								}
								else if((p_val[m+1]=='4')&&(p_val[m+2]=='0')){
									tmp_buf[k++]='@';
								}
								else{
									tmp_buf[k++]=' ';
									}
								m+=3;
							}
							else{
								tmp_buf[k++]=p_val[m++];
							}

							if(k>=sizeof(tmp_buf)) break;
						}
						res_cell = debug_cell_write(pos,(uint8_t*)tmp_buf,size,p_table->p_start_cell);
//						if(res_cell == cell_cmd_ok){
//							printf("write ok\r\n");
//						}
					}
//float 4
					if(type==Float4Cell){
						float f4=0;
						double l_d;
						l_d = atod(p_val);
						f4 = l_d;
						memcpy(tmp_buf,&f4,sizeof(f4));
						size = sizeof(f4);
						res_cell = debug_cell_write(pos,(uint8_t*)tmp_buf,size,p_table->p_start_cell);
//						if(res_cell == cell_cmd_ok){
//							printf("write ok\r\n");
//						}
					}
//float 8
					if(type==Float8Cell){
						double l_d;
						l_d = atod(p_val);
						memcpy(tmp_buf,&l_d,sizeof(l_d));
						size = sizeof(l_d);
						res_cell = debug_cell_write(pos,(uint8_t*)tmp_buf,size,p_table->p_start_cell);
//						if(res_cell == cell_cmd_ok){
//							printf("write ok\r\n");
//						}
					}
//unsigned integer
					if((type&(~0x1F)) == FixIntegerCellType){
						type= type & 0x0F;
						size = tmp_buf[1] & 0x1F;
						memset(tmp_buf,0,sizeof(tmp_buf));
						if((type>4)&&(type<=8)){
							uint64_t t64=0;
							size = 8;
							sscanf(p_val,"%lld",&t64);
							memcpy(tmp_buf,&t64,sizeof(t64));
						}
						if(type<=4){
							uint32_t t32=0;
							sscanf(p_val,"%d",&t32);
							if(size==4){
								memcpy(tmp_buf,&t32,sizeof(t32));
							}
							if(size==2){
								uint16_t t16;
								t16 = t32;
								memcpy(tmp_buf,&t16,sizeof(t16));
							}
							if(size==1){
								uint8_t t8;
								t8 = t32;
								memcpy(tmp_buf,&t8,sizeof(t8));
							}
						}
						res_cell = debug_cell_write(pos,(uint8_t*)tmp_buf,size,p_table->p_start_cell);
//						if(res_cell == cell_cmd_ok){
//							printf("write ok\r\n");
//						}
					}
//signed integer
					if((type&(~0x0F)) == FixSignIntegerCellType){
						type= type & 0x0F;
						size = tmp_buf[1] & 0x07;
						memset(tmp_buf,0,sizeof(tmp_buf));
						if(size <= 4){
							int32_t t32=0;
							sscanf(p_val,"%i",&t32);
							if(size==4){
								memcpy(tmp_buf,&t32,sizeof(t32));
							}
							if(size==2){
								int16_t t16;
								t16 = t32;
								memcpy(tmp_buf,&t16,sizeof(t16));
							}
							if(size==1){
								int8_t t8;
								t8 = t32;
								memcpy(tmp_buf,&t8,sizeof(t8));
							}
						}
						if(size == 8){
							int64_t t64=0;
							sscanf(p_val,"%lli",&t64);
							memcpy(tmp_buf,&t64,sizeof(t64));
						}
						res_cell = debug_cell_write(pos,(uint8_t*) tmp_buf,size,p_table->p_start_cell);
//						if(res_cell == cell_cmd_ok){
//							printf("write ok\r\n");
//						}
					}
					if((type & (~0x1F)) == FixByteArrayCellType){
						size = type & 0x1F;
						uint32_t t1;
						memset(tmp_buf,0,sizeof(tmp_buf));
						uint32_t length,k,m,p;
						length = strlen(p_val);
						k = 0;
						m = 0;
						while(m<length){
							if(p_val[m]=='%'){
								tmp_buf[k++]=' ';
								m+=3;
							}
							else{
								tmp_buf[k++]=p_val[m++];
							}

							if(k>=sizeof(tmp_buf)) break;
						}
						memset(tmp_32,0,sizeof(tmp_32));
						length=0;
						m=0;
						k=0;
						p=0;
						//tmp_buf1
						length = strlen(tmp_buf);
						while(m<length){
							if((tmp_buf[m]>='0')&&(tmp_buf[m]<='9')){
								tmp_32[k++] = tmp_buf[m];
								if(k>=sizeof(tmp_32)) break;
							}
							else{
								sscanf(tmp_32,"%d",&t1);
								memset(tmp_32,0,sizeof(tmp_32));
								k=0;
								tmp_buf1[p++] = t1;
							}
							m++;
						}
						res_cell = debug_cell_write(pos,(uint8_t *)tmp_buf1,p,p_table->p_start_cell);
//						if(res_cell == cell_cmd_ok){
//							printf("write ok\r\n");
//						}
					}

				}
			}
		}
		if((!strcmp("read_cell",p_id)) || (!strcmp("read_cells",p_id))){
uint8_t mode = 0;
			if(!strcmp("read_cells",p_id)) mode = 1;
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_table", 0),"cmd", 0);
			if(mode) jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "read_cells", 0),"sub_cmd", 0);
			else jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "read_cell", 0),"sub_cmd", 0);
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "no", 0),"error", 0);

			char* p_name;
			ctrl_table_list_struct* p_table;
			htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"name",&p_name);
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,p_name, 0),"name", 0);
			p_table =  find_table_by_name(&control.table_list[0], p_name);
			if(p_table){
				uint8_t last;
				uint32_t len_sen=0;
				uint32_t pos = 0;
				uint8_t debug_answer_len;
				htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,"pos",&pos);

				memset(tmp_buf,0,sizeof(tmp_buf));
				sprintf(tmp_buf,"%d",pos);
				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf, 0),"pos", 0);
				last = pos;
				uint8_t num_p=0;
				do{
						cell_cmd_result_t result;
						uint8_t p_num;
						uint8_t type;
						uint8_t size;
						result = debug_cell_read_type_num(pos, (uint8_t*)tmp_buf,p_table->p_start_cell,&p_num);
						if(result == cell_data_ready){
							uint8_t attr;
							attr = tmp_buf[2];
							if(attr & RD_Att){
								memset(tmp_buf,0,sizeof(tmp_buf));
								result = debug_cell_read_num((uint8_t*)&pos,
																(uint8_t *)tmp_buf,
																p_table->p_start_cell,
																&debug_answer_len,
																1);
							}
							else{
								result = cell_cmd_not_available;
							}
						}

					if((result==cell_data_ready)&&(debug_answer_len)){
	//					char* p_buf;
						last = pos;
						memset(tmp_buf1,0,sizeof(tmp_buf1));
						if(tmp_buf[1]<=4){
							uint32_t t32=0;
							memcpy(&t32,&tmp_buf[2],tmp_buf[1]);
							sprintf(tmp_buf1,"%d",t32);
							goto convert_exit;
						}
						if(tmp_buf[1]<=8){
							uint64_t t64=0;
							memcpy(&t64,&tmp_buf[2],tmp_buf[1]);
							sprintf(tmp_buf1,"%lld",t64);
							goto convert_exit;
						}
						if(tmp_buf[1]==Float4Cell){
							float f4=0;
							memcpy(&f4,&tmp_buf[2],sizeof(f4));
							sprintf(tmp_buf1,"%f",f4);
							goto convert_exit;
						}
						if(tmp_buf[1]==Float8Cell){
							double f8=0;
							memcpy(&f8,&tmp_buf[2],sizeof(f8));
//							sprintf(tmp_buf1,"%llf",f8);
							sprintf(tmp_buf1,"%lf",f8);
							goto convert_exit;
						}
						if((tmp_buf[1]&(~0x0F)) == FixSignIntegerCellType){
							tmp_buf[1] = tmp_buf[1] & 0x0F;
							if(tmp_buf[1]<=4){
								int32_t t32=0;
								memcpy(&t32,&tmp_buf[2],tmp_buf[1]);
								if(tmp_buf[1]==1){
									int8_t t8;
									t8 = tmp_buf[2];
									//if(t32>127)
										t32=t8;
								}
								if(tmp_buf[1]==2){
									int16_t t16;
									memcpy(&t16,&tmp_buf[2],2);
									//if(t32 > 32767)
										t32=t16;
								}
								sprintf(tmp_buf1,"%i",t32);
								goto convert_exit;
							}
							if(tmp_buf[1]<=8){
								int64_t t64=0;
								memcpy(&t64,&tmp_buf[2],tmp_buf[1]);
								sprintf(tmp_buf1,"%lli",t64);
								goto convert_exit;
							}
						}
						if((tmp_buf[1]&(~0x1F)) == FixByteArrayCellType){
							tmp_buf[1] = tmp_buf[1] & 0x1F;
							uint32_t sp=0,t1=0;
							while(tmp_buf[1]){
								sprintf(&tmp_buf1[sp],"%d ",tmp_buf[2+t1]);
								sp = strlen(tmp_buf1);
								t1++;
								tmp_buf[1]--;
							}
							goto convert_exit;
						}
						if((tmp_buf[1]&(~0x1F)) == FixStringCellType){
							tmp_buf[1] = tmp_buf[1] & 0x1F;
							memcpy(tmp_buf1,&tmp_buf[2],tmp_buf[1]);
						}
						if((tmp_buf[1]&(~0x1F)) == VarStringCellType){
							tmp_buf[1] = tmp_buf[1] & 0x1F;
							uint8_t len;
							len = strlen(&tmp_buf[2]);
							if(len>tmp_buf[1]) len = tmp_buf[1];
							memcpy(tmp_buf1,&tmp_buf[2],len);
						}

		convert_exit:
						if(!mode){
							jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf1, 0),"val", 0);
						}
						else{
							htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"val_",pos);
							jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf1, 0),tmp_32, 0);
							len_sen += strlen(tmp_buf1);
						}
						num_p++;
					}
					else{
						if(!mode) break;
					}
					pos++;
					if(pos>=255) break;
					if((num_p>=20)||(len_sen>400)) break;

				}
				while(mode);
				if(pos>=255){
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,"1", 0),"e", 0);
				}
				if(mode){
					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf(tmp_buf,"%d",last);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf, 0),"l", 0);
				}
			}
		}
		if(!strcmp("get_list",p_id)){
//			printf("get list need\r\n");
			int i;
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_table", 0),"cmd", 0);
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_list", 0),"sub_cmd", 0);
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "no", 0),"error", 0);
			for(i=0;i<MAX_NUM_FUNC_TABLE;i++){
				if(!control.table_list[i].name) break;
				htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"n_",i);
				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, control.table_list[i].name, 0),tmp_32, 0);

				htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"m_",i);
				memset(tmp_buf,0,sizeof(tmp_buf));
				sprintf(tmp_buf,"%d",control.table_list[i].mode);
				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_buf, 0),tmp_32, 0);

	//			printf("get list ack %d\r\n",i);

			}
		}
		if(!strcmp("get_cells",p_id)){
			char* p_name;
			ctrl_table_list_struct* p_table;
			SettingCell_t* p_cell;
			htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"name",&p_name);
			p_table =  find_table_by_name(&control.table_list[0], p_name);
			if(p_table){
				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_table", 0),"cmd", 0);
				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_cells", 0),"sub_cmd", 0);
				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_name, 0),"name", 0);
				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "no", 0),"error", 0);
				memset(tmp_buf,0,sizeof(tmp_buf));
				sprintf(tmp_buf,"%d",p_table->mode);
				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_buf, 0),"m", 0);

				p_cell = p_table->p_start_cell;
				uint32_t pos,i,num_first,num_last;
				uint8_t b_f=0;
				pos = 0;
				i=0;
				num_last = 0;
				num_first = 0;
				htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,"pos",&pos);
				while(1){
					if(!p_cell->DescriptStr){
						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "1", 0),"e", 0);
						break;
					}
					if(p_cell->CellNumber < pos){
						p_cell++;
					}
					else{
						htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"n",i);
						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,p_cell->DescriptStr, 0),tmp_32, 0);

						htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"attr",i);
						memset(tmp_buf,0,sizeof(tmp_buf));
						sprintf(tmp_buf,"%d",p_cell->CellAttr);
						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf, 0),tmp_32, 0);

						htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"pos",i);
						memset(tmp_buf,0,sizeof(tmp_buf));
						sprintf(tmp_buf,"%d",p_cell->CellNumber);
						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf, 0),tmp_32, 0);

						htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"type",i);
						memset(tmp_buf,0,sizeof(tmp_buf));
						sprintf(tmp_buf,"%d",p_cell->CellType);
						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf, 0),tmp_32, 0);

						htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"low",i);
						memset(tmp_buf,0,sizeof(tmp_buf));
						sprintf(tmp_buf,"%d",p_cell->LowLim);
						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf, 0),tmp_32, 0);

						htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"hi",i);
						memset(tmp_buf,0,sizeof(tmp_buf));
						sprintf(tmp_buf,"%d",p_cell->HighLim);
						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf, 0),tmp_32, 0);

						htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"def",i);
						memset(tmp_buf,0,sizeof(tmp_buf));
						if((p_cell->CellAttr & Action_Att)||(!p_cell->CellAttr)){
							tmp_buf[0] = ' ';
						}
						else{
							sprintf(tmp_buf,"%d",p_cell->DefaultValue);
							jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf, 0),tmp_32, 0);
						}

						if(!b_f) {
							num_first = p_cell->CellNumber;
							b_f = 1;
						}
						num_last = p_cell->CellNumber;

						p_cell++;
						i++;
						if(i>4) break;
					}
				}
						memset(tmp_buf,0,sizeof(tmp_buf));
						sprintf(tmp_buf,"%d",num_first);
						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf, 0),"f", 0);

						memset(tmp_buf,0,sizeof(tmp_buf));
						sprintf(tmp_buf,"%d",num_last);
						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config,tmp_buf, 0),"l", 0);

			}
		}
	}

//	if(!strcmp("nav_set",p_char)){
//		htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"id",&p_id);
//		if(!strcmp("cmd_long_id",p_id)){
//			htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"val",&p_char);
//			printf("get = %s\r\n",p_char);
//		}
//		if(!strcmp("cmd_pol_id",p_id)){
//			printf("get = %s\r\n",p_char);
//		}
//	}
//	if(!strcmp("nav_set_raw",p_char)){
//		htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"val",&p_char);
////		nmea_parser_send(&control.nmea_parser,p_char,strlen(p_char));
//		nmea_parser_covert_to_ascii_send(&control.nmea_parser,p_char,strlen(p_char));
////		printf("%s \r\n",p_char);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "nav_set_raw", 0),"cmd", 0);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "no", 0),"error", 0);
//	}
//	if(!strcmp("get_solve",p_char)){
//
//		nv08c_rmc_s rmc_data;
//		nv08c_gga_s gga_data;
//
//		nv08c_rtka_rmc_get_param(&control.nv08c_rtka, &rmc_data);
//		nv08c_rtka_gga_get_param(&control.nv08c_rtka, &gga_data);
//
//		sprintf(tmp_32,"%f%c",rmc_data.latitude,rmc_data.latitude_type);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"lat", 0);
//
//		sprintf(tmp_32,"%f%c",rmc_data.longitude,rmc_data.longitude_type);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"long", 0);
//
//		sprintf(tmp_32,"%f",rmc_data.speed);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"speed", 0);
//
//		sprintf(tmp_32,"%f",gga_data.height);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"alt", 0);
//
//		sprintf(tmp_32,"%d",gga_data.num_sat);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"num_sat", 0);
//
//		sprintf(tmp_32,"%d:%d:%d",rmc_data.date/10000,(rmc_data.date%10000)/100,rmc_data.date%100);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"date", 0);
//
//		sprintf(tmp_32,"%d:%d:%d",rmc_data.time_sec/10000,(rmc_data.time_sec%10000)/100,rmc_data.time_sec%100);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"time", 0);
//
////		sprintf(tmp_32,"%d %d %d",control.nv08c_rtka.rmc.time_sec/3600,(control.nv08c_rtka.rmc.time_sec%3600)/60,control.nv08c_rtka.rmc.time_sec%60);
////		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"time", 0);
//
//
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_solve", 0),"cmd", 0);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "no", 0),"error", 0);
//	}
//	if(!strcmp("get_console",p_char)){
//uint32_t len,len_all,i,b_find,pos,num;
//uint8_t b_get;
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_console", 0),"cmd", 0);
//		len_all = buf_console_get_len(&control.buf_console);
//		len = len_all;
//		if(len){
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			if(len > (sizeof(tmp_buf)-1)) len = sizeof(tmp_buf)-1;
//			buf_console_read(&control.buf_console,tmp_buf,len);
//			pos = 0;
//			num = 0;
//			b_get = 0;
//			for(i=0;i<len;i++){
//				if(tmp_buf[i] > 0x0D) b_get = 1;
//				if(tmp_buf[i] <= 0x0D){
//					if(b_get){
//						b_get = 0;
//						tmp_buf[i] = 0;
//						htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"s_",num);
//						num++;
//						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, &tmp_buf[pos], 0),tmp_32, 0);
//						pos = i+1;
//					}
//					else{
//						tmp_buf[i] = 0;
//						pos = i+1;
//					}
//				}
//			}
//			if(b_get){
//				htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"s_",num);
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, &tmp_buf[pos], 0),tmp_32, 0);
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "yes", 0),"trim", 0);
//			}
//			else{
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "no", 0),"trim", 0);
//			}
//		}
//		sprintf(tmp_32,"%d",len);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"len_ack", 0);
//		memset(tmp_32,0,sizeof(tmp_32));
//		sprintf(tmp_32,"%d",len_all);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"len_all", 0);
//		memset(tmp_32,0,sizeof(tmp_32));
//		sprintf(tmp_32,"%d",control.buf_console.id);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"id", 0);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "no", 0),"error", 0);
//
//	}

//	htpp_parser_find_rd_wr_fl(iNumParams,pcParam,pcValue,&b_read,&b_write,&b_flash);

//	if(b_write){

//		res = htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"type_cmd",&type_cmd);
//
//		pos = 0;
//		res_tmp = htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"pos",&pos);
//
//		if(pos > 4) pos = 4;
//
//		res = htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"num",&ptr);
//		if(!res){
//			res = htpp_parser_find_id_from_str(&tmp_32[0],32,"id_rd_on_",ptr,&ch_num);
//			res |= htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&b_on);
//
//			res |= htpp_parser_find_id_from_str(&tmp_32[0],32,"id_rd_off_",ptr,&ch_num);
//			res |= htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&b_off);
//
//			pwr_cntrl_power_state_e mode = pwr_cntrl_power_state_off;
//
//			if(b_on){
//				mode = pwr_cntrl_power_state_on;
//			}
////			cmd_result_t cmd_res;
//
//			httpd_make_src_string((char*)tmp_buf,sizeof(tmp_buf),ip,port);
//			cmd_diag(ddebug_id_cmd,"%s %s %s","get cmd = SEL_PWR_MODE", "from=",tmp_buf);
//
//			control_select_pwr_on_off_ch(&control,mode, ch_num, 0);
//
//		}
//	}
//	if(b_read){
////		printf("get pwr ch req \r\n");
//const char *p_off;
//const char *p_on;
////uint8_t ref_state;
////uint8_t mshu_state;
////cntrl_power_mode_e mode;
//		pos = 0;
//		res_tmp = htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"pos",&pos);
//		if(res_tmp){
//			pos = ch_num;
//		}
//
//		if(pos > 4) pos = 4;
//
////		printf("get req ch   = %d \r\n", pos);
//
//		sprintf(p_out,"%d",pos+1);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_num_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
/////////////////
////name
/////////////////
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_name_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)&control.dev_config.rec.pwr_ch_name[pos][0], 0),tmp_32, 0);
//
/////////////////
////тип управления
/////////////////
//		if(control.pwr_ch[pos].work_mode == pwr_cntrl_type_not_control){
//			sprintf(p_out,"%s","Отсутствует");
//		}
//		if(control.pwr_ch[pos].work_mode == pwr_cntrl_type_cntrl_relay){
//			sprintf(p_out,"%s","Релейный регулятор");
//		}
//		if(control.pwr_ch[pos].work_mode == pwr_cntrl_type_cntrl_cmd){
//			sprintf(p_out,"%s","Командное");
//		}
//		if(control.pwr_ch[pos].work_mode == pwr_cntrl_type_cntrl_time){
//			sprintf(p_out,"%s","По времени");
//		}
//		if(control.pwr_ch[pos].work_mode == pwr_cntrl_type_cntrl_error){
//			sprintf(p_out,"%s","Ошибка");
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_type_cntrl_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
/////////////////
////состояние
/////////////////
//
//		p_off = http_str_zero;
//		p_on = http_str_zero;
//		if(control.pwr_ch[pos].state == pwr_cntrl_power_state_off){
//			sprintf(p_out,"%s","Выключено");
//			p_off = http_str_one;
//		}
//		if(control.pwr_ch[pos].state == pwr_cntrl_power_state_on){
//			sprintf(p_out,"%s","Включено");
//			p_on = http_str_one;
//		}
//		if(control.pwr_ch[pos].state == pwr_cntrl_power_state_undef){
//			sprintf(p_out,"%s","Неопределено");
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_state_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
/////////////////
////Напряжение
/////////////////
//		sprintf(p_out,"%.1f",control.pwr_ch[pos].voltage_rms);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_vol_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
/////////////////
////Ток
/////////////////
//		sprintf(p_out,"%.3f",control.pwr_ch[pos].current_rms);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_cur_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
/////////////////
////запрещен - разрешен
/////////////////
//		if(control.pwr_ch[pos].enable_disable == pwr_cntrl_en_state_disable){
//			sprintf(p_out,"%s","Запрещен");
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "0", 0),"install", 0);
////			p_off = http_str_one;
//		}
//		if(control.pwr_ch[pos].enable_disable == pwr_cntrl_en_state_enable){
//			sprintf(p_out,"%s","Разрешен");
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "1", 0),"install", 0);
////			p_off = http_str_one;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_sen_type_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
////////////////
////комментарий к работе
////////////////
//
//		if(control.pwr_ch[pos].work_mode == pwr_cntrl_type_cntrl_relay){
////пар1
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%s","Контроль: ");
//
//			if(control.pwr_ch[pos].relay_sns.sen_num == sensor_cntrl_collect_sen_temp_e){
//				strcat((char*)tmp_buf,"температуры.");
//			}
//			if(control.pwr_ch[pos].relay_sns.sen_num == sensor_cntrl_collect_sen_hum_e){
//				strcat((char*)tmp_buf,"влажности.");
//			}
//			if(control.pwr_ch[pos].relay_sns.sen_num == sensor_cntrl_collect_sen_lux_e){
//				strcat((char*)tmp_buf,"освещенности.");
//			}
//			if(control.pwr_ch[pos].relay_sns.sen_num == sensor_cntrl_collect_sen_press_e){
//				strcat((char*)tmp_buf,"давления.");
//			}
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"id_cmt1", 0);
////пар2
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%s","Закон управления: ");
//
//			if(control.pwr_ch[pos].relay_sns.mode == pwr_cntrl_relay_mode_cooling){
//				strcat((char*)tmp_buf,"охладитель. (Включаем по увеличению порога.)");
//			}
//			if(control.pwr_ch[pos].relay_sns.mode == pwr_cntrl_relay_mode_heater){
//				strcat((char*)tmp_buf,"нагреватель. (Включаем по уменьшению порога.)");
//			}
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"id_cmt2", 0);
////пар 3
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%s","Контроль по датчикам: ");
//
//			if(control.pwr_ch[pos].relay_sns.sensor_mask & 1 & control.pwr_ch[pos].relay_sns.p_sns_collect_data->collect_sensor_mask){
//				strcat((char*)tmp_buf,(char*)&control.dev_config.rec.sensor_ch_name[0][0]);
//				strcat((char*)tmp_buf," ");
//			}
//			if(control.pwr_ch[pos].relay_sns.sensor_mask & 2 & control.pwr_ch[pos].relay_sns.p_sns_collect_data->collect_sensor_mask){
//				strcat((char*)tmp_buf,(char*)&control.dev_config.rec.sensor_ch_name[1][0]);
//				strcat((char*)tmp_buf," ");
//			}
//			if(control.pwr_ch[pos].relay_sns.sensor_mask & 4 &control.pwr_ch[pos].relay_sns.p_sns_collect_data->collect_sensor_mask){
//				strcat((char*)tmp_buf,(char*)&control.dev_config.rec.sensor_ch_name[2][0]);
//				strcat((char*)tmp_buf," ");
//			}
//			if(control.pwr_ch[pos].relay_sns.sensor_mask & 8 & control.pwr_ch[pos].relay_sns.p_sns_collect_data->collect_sensor_mask){
//				strcat((char*)tmp_buf,(char*)&control.dev_config.rec.sensor_ch_name[3][0]);
//				strcat((char*)tmp_buf," ");
//			}
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"id_cmt3", 0);
//
////пар4
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%s","Пороги срабатывания: ");
//			sprintf(tmp_32,"%s %i","нижний= ",control.pwr_ch[pos].relay_sns.min);
//			strcat((char*)tmp_buf,tmp_32);
//			sprintf(tmp_32,"%s %i","верхний= ",control.pwr_ch[pos].relay_sns.max);
//			strcat((char*)tmp_buf,tmp_32);
//
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"id_cmt4", 0);
//
////пар5
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%s","Принятие решения по значению: ");
//			if(control.pwr_ch[pos].relay_sns.sns_cntrl == pwr_cntrl_sns_mode_min){
//				strcat((char*)tmp_buf,"минимальному");
//			}
//			if(control.pwr_ch[pos].relay_sns.sns_cntrl == pwr_cntrl_sns_mode_max){
//				strcat((char*)tmp_buf,"максимальному");
//			}
//			if(control.pwr_ch[pos].relay_sns.sns_cntrl == pwr_cntrl_sns_mode_avr){
//				strcat((char*)tmp_buf,"среднему");
//			}
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"id_cmt5", 0);
//
////пар6
//			if(control.pwr_ch[pos].relay_sns.sensor_mask){
//				memset(tmp_buf,0,sizeof(tmp_buf));
//				sprintf((char*)tmp_buf,"%s %i","Текущее значение датчика: ",control.pwr_ch[pos].data_pwr_cur_32);
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"id_cmt6", 0);
//			}
//			else{
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, " ", 0),"id_cmt6", 0);
//			}
//		}
//		if(control.pwr_ch[pos].work_mode == pwr_cntrl_type_cntrl_cmd){
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "Командный режим работы ", 0),"id_cmt1", 0);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, " ", 0),"id_cmt2", 0);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, " ", 0),"id_cmt3", 0);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, " ", 0),"id_cmt4", 0);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, " ", 0),"id_cmt5", 0);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, " ", 0),"id_cmt6", 0);
//		}
//
//
////////////////
////радио
////////////////
//		p_off = http_str_zero;
//		p_on = http_str_zero;
//		if(control.pwr_ch[pos].state == pwr_cntrl_power_state_on){
//			p_on = http_str_one;
//		}
//		if(control.pwr_ch[pos].state == pwr_cntrl_power_state_off){
//			p_off = http_str_one;
//		}
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_rd_off_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_off, 0),tmp_32, 0);
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_rd_on_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_on, 0),tmp_32, 0);
//
//	}
	return close_json_file(child,p_data);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//const char * http_cntrl_sense_ch_set_req_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res1, uint32_t ip, uint16_t port){
//char *p_data;
//jfes_value_t *child;
////uint32_t res;
////uint8_t type_cmd;
//uint8_t b_flash, b_write, b_read;
//char *p_out;
//char tmp_32[32];
////uint8_t b_off,b_on;
////uint8_t ch_num;
////cmd_result_t  res_cmd=cmd_ok;
////uint32_t res_tmp;
//uint8_t pos=0;
////char *ptr;
//
//	p_out = (char*)tmp_buf;
////	type_cmd = -1;
//
//	*p_res1 = NULL;
//	p_data = (char*)make_json_file_header(j_data,sizeof(j_data),(char*)&CGI_TAB[iIndex].pcCGIName[1],strlen(CGI_TAB[iIndex].pcCGIName));
//	child = jfes_create_object_value(&j_config);
//
//	htpp_parser_find_rd_wr_fl(iNumParams,pcParam,pcValue,&b_read,&b_write,&b_flash);
//
//	if(b_write){
//
//	}
//	if(b_read){
//		pos = 0;
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"pos",&pos);
//
//		if(pos > 4) pos = 4;
//
/////////////////
////номер канала
/////////////////
//		sprintf(p_out,"%d",pos+1);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_num_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
/////////////////
////name
/////////////////
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_name_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)&control.dev_config.rec.sensor_ch_name[pos][0], 0),tmp_32, 0);
//
/////////////////
////разрешение
/////////////////
//		if(control.sensors.mode[pos].en_dis == sensor_en_dis_enable){
//			sprintf(p_out,"%s","Разрешен");
//		}
//		else{
//			sprintf(p_out,"%s","Запрещен");
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "0", 0),"install", 0);
//		}
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_state_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
////		http_json_make_field_ext_str(1, pos,"st_",tmp_32,32,child," ","Запрещен");
//
//		////////////////////
//		//состояние подключения - физика
//		////////////////////
//		if(control.sensors.sen[pos].connect == sensor_connect_disable){
//			http_json_make_field_ext_str(1, pos,"st_",tmp_32,32,child," ","Запрещен");
//		}
//		if(control.sensors.sen[pos].connect == sensor_connect_ok){
//			http_json_make_field_ext_str(1, pos,"st_",tmp_32,32,child," ","Подключен");
//		}
//		if(control.sensors.sen[pos].connect == sensor_connect_fail){
//			http_json_make_field_ext_str(1, pos,"st_",tmp_32,32,child," ","Ошибка соединения");
//		}
//		if(control.sensors.sen[pos].connect == sensor_connect_undef){
//			http_json_make_field_ext_str(1, pos,"st_",tmp_32,32,child," ","Неопределено");
//		}
//
///////////////////
//////Температура
///////////////////
//		if(control.sensors.mode[pos].en_dis == sensor_en_dis_enable){
//			if((control.sensors.sen[pos].error & (1<<sensor_cntrl_collect_sen_temp_e)) || (control.sensors.sen[pos].error & sensor_cntrl_ch_error_not_ack) || (control.sensors.sen[pos].error & sensor_cntrl_ch_error_i2c)){
//				sprintf(p_out,"%s","ошибка");
//			}
//			else{
//				sprintf(p_out,"%i",control.sensors.collect_sense[sensor_cntrl_collect_sen_temp_e].data[pos]);
//			}
//		}
//		else{
//			sprintf(p_out,"%s","Недоступно");
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_temp_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
///////////////////
//////Влажность
///////////////////
//		if(control.sensors.mode[pos].en_dis == sensor_en_dis_enable){
//			if((control.sensors.sen[pos].error & (1<<sensor_cntrl_collect_sen_hum_e))  || (control.sensors.sen[pos].error & sensor_cntrl_ch_error_not_ack) || (control.sensors.sen[pos].error & sensor_cntrl_ch_error_i2c)){
//				sprintf(p_out,"%s","ошибка");
//			}
//			else{
//				sprintf(p_out,"%i",control.sensors.collect_sense[sensor_cntrl_collect_sen_hum_e].data[pos]);
//			}
//		}
//		else{
//			sprintf(p_out,"%s","Недоступно");
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_hum_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
///////////////////
//////Освещенность
///////////////////
//		if(control.sensors.mode[pos].en_dis == sensor_en_dis_enable){
//			if((control.sensors.sen[pos].error & (1<<sensor_cntrl_collect_sen_lux_e)) || (control.sensors.sen[pos].error & sensor_cntrl_ch_error_not_ack)|| (control.sensors.sen[pos].error & sensor_cntrl_ch_error_i2c)){
//				sprintf(p_out,"%s","ошибка");
//			}
//			else{
//				sprintf(p_out,"%i",control.sensors.collect_sense[sensor_cntrl_collect_sen_lux_e].data[pos]);
//			}
//		}
//		else{
//			sprintf(p_out,"%s","Недоступно");
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_lux_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
///////////////////
//////давление
///////////////////
//		if(control.sensors.mode[pos].en_dis == sensor_en_dis_enable){
//			if((control.sensors.sen[pos].error & (1<<sensor_cntrl_collect_sen_press_e))  || (control.sensors.sen[pos].error & sensor_cntrl_ch_error_not_ack) || (control.sensors.sen[pos].error & sensor_cntrl_ch_error_i2c)){
//				sprintf(p_out,"%s","ошибка");
//			}
//			else{
//				sprintf(p_out,"%i",control.sensors.collect_sense[sensor_cntrl_collect_sen_press_e].data[pos]);
//			}
//		}
//		else{
//			sprintf(p_out,"%s","Недоступно");
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_press_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
//		memset(tmp_buf,0,sizeof(tmp_buf));
//		sprintf((char*)tmp_buf, "%d/%d",control.sensors.sen[pos].num_send,control.sensors.sen[pos].num_ack);
//
//		http_json_make_field_ext_str(1, pos,"s_send",tmp_32,32,child,"Нет",(char*)tmp_buf);
//
//	}
//	return close_json_file(child,p_data);
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//const char * http_cntrl_pwr_ch_relay_set_req_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res1, uint32_t ip, uint16_t port){
//char *p_data;
//jfes_value_t *child;
////uint32_t res;
//uint8_t type_cmd;
//uint8_t b_flash, b_write, b_read;
//char *p_out;
//char tmp_32[32];
////uint8_t b_off,b_on;
////uint8_t ch_num;
////cmd_result_t  res_cmd=cmd_ok;
////uint32_t res_tmp;
//uint8_t pos=0;
////char *ptr;
////uint8_t data;
//
//	p_out = (char*)tmp_buf;
//	type_cmd = -1;
//
//	*p_res1 = NULL;
//	p_data = (char*)make_json_file_header(j_data,sizeof(j_data),(char*)&CGI_TAB[iIndex].pcCGIName[1],strlen(CGI_TAB[iIndex].pcCGIName));
//	child = jfes_create_object_value(&j_config);
//
//	htpp_parser_find_rd_wr_fl(iNumParams,pcParam,pcValue,&b_read,&b_write,&b_flash);
//
//	if(b_write){
//
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"type_cmd",&type_cmd);
//
//
//	pos = 0;
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"num",&pos);
//	if (pos>CLIMATE_MAX_NUM_PWR_CNTRL) pos=0;
//uint8_t data1,data2,data3,data4;
////int32_t dat32;
//pwr_cntrl_config_s rel_cfg;
//rel_cfg.cntrl_mode = pwr_cntrl_type_cntrl_relay;
//rel_cfg.relay.mode = pwr_cntrl_relay_mode_cooling;
//rel_cfg.relay.sns_cntrl = pwr_cntrl_sns_mode_avr;
//rel_cfg.relay.max = 0;
//rel_cfg.relay.min = 0;
//rel_cfg.relay.sensor_mask = 0;
//rel_cfg.relay.sen_num = sensor_cntrl_collect_sen_temp_e;
///*
//ch_cmd = pwr_cntrl_cmd_config;
//ack_msg_on_start(&p_ctl->ack);
//res_cmd = pwr_cntrl_make_cmd(&p_ctl->pwr_ch[0],(uint8_t)ch_cmd,sizeof(pwr_cntrl_config_s),(uint8_t*)&rel_cfg,dev_cntrl_ack_response);
//res_cmd |= control_wait_ack(p_ctl,500);
//*/
//////////////////////
////датчики для канала
//////////////////////
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_0_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data1);
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_1_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data2);
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_2_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data3);
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_3_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data4);
//
//	if(data1){
//		rel_cfg.relay.sensor_mask |= 1;
//	}
//	if(data2){
//		rel_cfg.relay.sensor_mask |= 2;
//	}
//	if(data3){
//		rel_cfg.relay.sensor_mask |= 4;
//	}
//	if(data4){
//		rel_cfg.relay.sensor_mask |= 8;
//	}
//////////////////////
////рабочий параметр для канала
//////////////////////
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"par_0_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data1);
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"par_1_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data2);
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"par_2_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data3);
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"par_3_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data4);
//
//	if(data1){
//		rel_cfg.relay.sen_num = sensor_cntrl_collect_sen_temp_e;
//	}
//	else{
//		if(data2){
//			rel_cfg.relay.sen_num = sensor_cntrl_collect_sen_hum_e;
//		}
//		else{
//			if(data3){
//				rel_cfg.relay.sen_num = sensor_cntrl_collect_sen_lux_e;
//			}
//			else{
//				if(data4){
//					rel_cfg.relay.sen_num = sensor_cntrl_collect_sen_press_e;
//				}
//			}
//		}
//	}
//////////////////////
////min max параметр для канала
//////////////////////
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"e_par_0_",pos);
//	htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,tmp_32,(uint32_t*)&rel_cfg.relay.min);
//
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"e_par_1_",pos);
//	htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,tmp_32,(uint32_t*)&rel_cfg.relay.max);
//
//////////////////////
////закон управления охладитель нагреватель
//////////////////////
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"cntrl_0_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data1);
//
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"cntrl_1_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data2);
//	if(data1){
//		rel_cfg.relay.mode = pwr_cntrl_relay_mode_cooling;
//	}
//	else{
//		if(data2){
//			rel_cfg.relay.mode = pwr_cntrl_relay_mode_heater;
//		}
//	}
//////////////////////
////усреднение параметров
//////////////////////
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"avr_0_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data1);
//
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"avr_1_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data2);
//
//	htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"avr_2_",pos);
//	htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data3);
//
//	if(data1){
//		rel_cfg.relay.sns_cntrl = pwr_cntrl_sns_mode_min;
//	}
//	else{
//		if(data2){
//			rel_cfg.relay.sns_cntrl = pwr_cntrl_sns_mode_max;
//		}
//		else{
//			if(data3){
//				rel_cfg.relay.sns_cntrl = pwr_cntrl_sns_mode_avr;
//			}
//		}
//	}
//	rel_cfg.def_state = control.dev_config.rec.pwr_cfg_default_state[pos];
//	rel_cfg.en_disable = control.dev_config.rec.pwr_cfg_en[pos];
//
//	httpd_make_src_string((char*)tmp_buf,sizeof(tmp_buf),ip,port);
//	cmd_diag(ddebug_id_cmd,"%s %s %s","get cmd = CONFIG PWR CH", "from=",tmp_buf);
//
//	control_config_pwr_ch(&control, &rel_cfg, pos, b_flash);
//
//	}
//	if(b_read){
//char *p_str;
// char *p_str1,*p_str2,*p_str3,*p_str4;
//		pos = 0;
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"num",&pos);
//		if (pos>CLIMATE_MAX_NUM_PWR_CNTRL) pos=0;
//
/////////////////
////номер канала
/////////////////
//		sprintf(p_out,"%d",pos+1);
//		memset(&tmp_32[0],0,sizeof(tmp_32));
//		memcpy(tmp_32,"num",3);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
/////////////////
////name
/////////////////
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_pwr_rel_cfg_name_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)&control.dev_config.rec.pwr_ch_name[pos][0], 0),tmp_32, 0);
//
////////////////
////датчики
////////////////
////1
//		if(control.pwr_ch[pos].relay_sns.sensor_mask & 1){
//			p_str = (char*)http_str_one;
//		}
//		else{
//			p_str = (char*)http_str_zero;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_0_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
////2
//		if(control.pwr_ch[pos].relay_sns.sensor_mask & 2){
//			p_str = (char*)http_str_one;
//		}
//		else{
//			p_str = (char*)http_str_zero;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_1_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
////3
//		if(control.pwr_ch[pos].relay_sns.sensor_mask & 4){
//			p_str = (char*)http_str_one;
//		}
//		else{
//			p_str = (char*)http_str_zero;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_2_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
////4
//		if(control.pwr_ch[pos].relay_sns.sensor_mask & 8){
//			p_str = (char*)http_str_one;
//		}
//		else{
//			p_str = (char*)http_str_zero;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_3_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
//
///////////////////
////выбор параметров
///////////////////
//		p_str1 = (char*)http_str_zero;
//		p_str2 = (char*)http_str_zero;
//		p_str3 = (char*)http_str_zero;
//		p_str4 = (char*)http_str_zero;
//
//		if(control.pwr_ch[pos].relay_sns.sen_num == sensor_cntrl_collect_sen_temp_e){
//			p_str1 = (char*)http_str_one;
//		}
//		if(control.pwr_ch[pos].relay_sns.sen_num == sensor_cntrl_collect_sen_hum_e){
//			p_str2 = (char*)http_str_one;
//		}
//		if(control.pwr_ch[pos].relay_sns.sen_num == sensor_cntrl_collect_sen_lux_e){
//			p_str3 = (char*)http_str_one;
//		}
//		if(control.pwr_ch[pos].relay_sns.sen_num == sensor_cntrl_collect_sen_press_e){
//			p_str4 = (char*)http_str_one;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"par_0_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str1, 0),tmp_32, 0);
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"par_1_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str2, 0),tmp_32, 0);
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"par_2_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str3, 0),tmp_32, 0);
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"par_3_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str4, 0),tmp_32, 0);
//
////min max
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"e_par_0_",pos);
//		sprintf(p_out,"%i",control.pwr_ch[pos].relay_sns.min);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"e_par_1_",pos);
//		sprintf(p_out,"%i",control.pwr_ch[pos].relay_sns.max);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
//
///////////////////
////закон управления
///////////////////
//		p_str1 = (char*)http_str_zero;
//		p_str2 = (char*)http_str_zero;
//
//		if(control.pwr_ch[pos].relay_sns.mode == pwr_cntrl_relay_mode_cooling){
//			p_str1 = (char*)http_str_one;
//		}
//		if(control.pwr_ch[pos].relay_sns.mode == pwr_cntrl_relay_mode_heater){
//			p_str2 = (char*)http_str_one;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"cntrl_0_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str1, 0),tmp_32, 0);
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"cntrl_1_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str2, 0),tmp_32, 0);
//
///////////////////
////усреднение
///////////////////
//		p_str1 = (char*)http_str_zero;
//		p_str2 = (char*)http_str_zero;
//		p_str3 = (char*)http_str_zero;
//
//		if(control.pwr_ch[pos].relay_sns.sns_cntrl == pwr_cntrl_sns_mode_min){
//			p_str1 = (char*)http_str_one;
//		}
//		if(control.pwr_ch[pos].relay_sns.sns_cntrl == pwr_cntrl_sns_mode_max){
//			p_str2 = (char*)http_str_one;
//		}
//		if(control.pwr_ch[pos].relay_sns.sns_cntrl == pwr_cntrl_sns_mode_avr){
//			p_str3 = (char*)http_str_one;
//		}
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"avr_0_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str1, 0),tmp_32, 0);
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"avr_1_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str2, 0),tmp_32, 0);
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"avr_2_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str3, 0),tmp_32, 0);
//
//	}
//	return close_json_file(child,p_data);
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//const char * http_pwr_set_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res1, uint32_t ip, uint16_t port){
//char *p_data;
//jfes_value_t *child;
////uint32_t res;
//uint8_t type_cmd;
//uint8_t b_flash, b_write, b_read;
//char *p_out;
//char tmp_32[32];
////uint8_t b_off,b_on;
////uint8_t ch_num;
////cmd_result_t  res_cmd=cmd_ok;
////uint32_t res_tmp;
//uint8_t pos=0;
////char *ptr;
////uint8_t data;
////
//	p_out = (char*)tmp_buf;
//	type_cmd = -1;
//
//	*p_res1 = NULL;
//	p_data = (char*)make_json_file_header(j_data,sizeof(j_data),(char*)&CGI_TAB[iIndex].pcCGIName[1],strlen(CGI_TAB[iIndex].pcCGIName));
//	child = jfes_create_object_value(&j_config);
//
//	htpp_parser_find_rd_wr_fl(iNumParams,pcParam,pcValue,&b_read,&b_write,&b_flash);
//
//	if(b_write){
//
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"type_cmd",&type_cmd);
//
//
//		pos = 0;
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"num",&pos);
//		if (pos >= CLIMATE_MAX_NUM_PWR_CNTRL) pos=0;
//	uint8_t data1,data2,data3;
//	//int32_t dat32;
//	pwr_cntrl_config_s rel_cfg;
//		memcpy(&rel_cfg.relay,&control.dev_config.rec.pwr_cfg_relay[pos],sizeof(rel_cfg.relay));
//		rel_cfg.cntrl_mode = pwr_cntrl_type_cntrl_relay;
//
//	////////////////////
//	//тип управления
//	////////////////////
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_0_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data1);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_1_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data2);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_2_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data3);
//
//		if(data1){
//			rel_cfg.cntrl_mode = pwr_cntrl_type_cntrl_cmd;
//		}
//		if(data2){
//			rel_cfg.cntrl_mode = pwr_cntrl_type_cntrl_relay;
//		}
//		if(data3){
//			rel_cfg.cntrl_mode = pwr_cntrl_type_cntrl_time;
//		}
//
//	////////////////////
//	//Состояние поумолчанию
//	////////////////////
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"par_0_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data1);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"par_1_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data2);
//
//		if(data1){
//			rel_cfg.def_state = pwr_cntrl_def_state_off;
//		}
//		if(data2){
//			rel_cfg.def_state = pwr_cntrl_def_state_on;
//		}
//
//	////////////////////
//	//запрет - разрешение
//	////////////////////
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"act_0_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data1);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"act_1_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data2);
//
//		if(data1){
//			rel_cfg.en_disable = pwr_cntrl_en_state_disable;
//		}
//		if(data2){
//			rel_cfg.en_disable = pwr_cntrl_en_state_enable;
//		}
////команда - в канал pwr
//		httpd_make_src_string((char*)tmp_buf,sizeof(tmp_buf),ip,port);
//		cmd_diag(ddebug_id_cmd,"%s %s %s","get cmd = CONFIG PWR RELAY CH", "from=",tmp_buf);
//
//		control_config_pwr_ch(&control, &rel_cfg, pos, b_flash);
////////////////
////имя канала
////////////////
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_pwr_set_name_",pos);
//	char *ptr;
//		htpp_parser_find_param_str(iNumParams,pcParam,pcValue,tmp_32,&ptr);
//uint16_t str_len;
//		str_len = strlen(ptr);
//		if(str_len > 31) str_len = 31;
//		memset(&control.dev_config.rec.pwr_ch_name[pos],0,32);
//		memcpy(&control.dev_config.rec.pwr_ch_name[pos],ptr,str_len);
//		dev_config_write(&control.dev_config);
//
//	}
//	if(b_read){
//char *p_str;
////char *p_str1,*p_str2,*p_str3;
//		pos = 0;
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"num",&pos);
//		if (pos>=CLIMATE_MAX_NUM_PWR_CNTRL) pos=0;
//
/////////////////
////номер канала
/////////////////
//		sprintf(p_out,"%d",pos+1);
//		memset(&tmp_32[0],0,sizeof(tmp_32));
//		memcpy(tmp_32,"num",3);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
/////////////////
////name
/////////////////
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_pwr_set_name_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)&control.dev_config.rec.pwr_ch_name[pos][0], 0),tmp_32, 0);
//
//		////////////////////
//		//тип управления
//		////////////////////
////1
//		if(control.pwr_ch[pos].work_mode == pwr_cntrl_type_cntrl_cmd){
//			p_str = (char*)http_str_one;
//		}
//		else{
//			p_str = (char*)http_str_zero;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_0_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
//
////2
//		if(control.pwr_ch[pos].work_mode == pwr_cntrl_type_cntrl_relay){
//			p_str = (char*)http_str_one;
//		}
//		else{
//			p_str = (char*)http_str_zero;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_1_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
//
////3
//		if(control.pwr_ch[pos].work_mode == pwr_cntrl_type_cntrl_time){
//			p_str = (char*)http_str_one;
//		}
//		else{
//			p_str = (char*)http_str_zero;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_2_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
//
//		////////////////////
//		//состояние по умолчанию
//		////////////////////
////1
//		if(control.pwr_ch[pos].default_state == pwr_cntrl_def_state_off){
//			p_str = (char*)http_str_one;
//		}
//		else{
//			p_str = (char*)http_str_zero;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"par_0_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
//
////2
//		if(control.pwr_ch[pos].default_state == pwr_cntrl_def_state_on){
//			p_str = (char*)http_str_one;
//		}
//		else{
//			p_str = (char*)http_str_zero;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"par_1_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
//
//		////////////////////
//		//разрешение работы
//		////////////////////
////3
//		if(control.pwr_ch[pos].enable_disable == pwr_cntrl_en_state_disable){
//			p_str = (char*)http_str_one;
//		}
//		else{
//			p_str = (char*)http_str_zero;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"act_0_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
//
//		if(control.pwr_ch[pos].enable_disable == pwr_cntrl_en_state_enable){
//			p_str = (char*)http_str_one;
//		}
//		else{
//			p_str = (char*)http_str_zero;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"act_1_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str, 0),tmp_32, 0);
//
//	}
//	return close_json_file(child,p_data);
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//const char * http_sen_set_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res1, uint32_t ip, uint16_t port){
//char *p_data;
//jfes_value_t *child;
////uint32_t res;
//uint8_t type_cmd;
//uint8_t b_flash, b_write, b_read;
//char *p_out;
//char tmp_32[32];
////uint8_t b_off,b_on;
////uint8_t ch_num;
////cmd_result_t  res_cmd=cmd_ok;
////uint32_t res_tmp;
//uint8_t pos=0;
////char *ptr;
////uint8_t data;
//
////
//	p_out = (char*)tmp_buf;
//	type_cmd = -1;
//
//	*p_res1 = NULL;
//	p_data = (char*)make_json_file_header(j_data,sizeof(j_data),(char*)&CGI_TAB[iIndex].pcCGIName[1],strlen(CGI_TAB[iIndex].pcCGIName));
//	child = jfes_create_object_value(&j_config);
//
//	htpp_parser_find_rd_wr_fl(iNumParams,pcParam,pcValue,&b_read,&b_write,&b_flash);
//
//	if(b_write){
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"type_cmd",&type_cmd);
//		pos = 0;
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"num",&pos);
//		if (pos >= SENSOR_CNTRL_MAX_COLLECT_NUM_SNS) pos=0;
//	uint8_t data1,data2,data3,data4,data5,data6;
//	//int32_t dat32;
//	sensor_cntrl_mode_s sen_cfg;
//		memcpy(&sen_cfg,&control.dev_config.rec.sen[pos],sizeof(sen_cfg));
//
//
//	////////////////////
//	//разрешен - запрещен
//	////////////////////
//		sen_cfg.en_dis = sensor_en_dis_disable;
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_0_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data1);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_1_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data2);
//
//		if(data1){
//			sen_cfg.en_dis = sensor_en_dis_disable;
//		}
//		if(data2){
//			sen_cfg.en_dis = sensor_en_dis_enable;
//		}
//	////////////////////
//	//режим дисплея
//	////////////////////
//		sen_cfg.disp_mode = sensor_cntrl_disp_num_ch;
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"d_en_0_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data1);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"d_en_1_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data2);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"d_en_2_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data3);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"d_en_3_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data4);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"d_en_4_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data5);
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"d_en_5_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data6);
//
//		if(data1){
//			sen_cfg.disp_mode = sensor_cntrl_disp_off;
//		}
//		if(data2){
//			sen_cfg.disp_mode = sensor_cntrl_disp_num_ch;
//		}
//		if(data3){
//			sen_cfg.disp_mode = sensor_cntrl_disp_temp;
//		}
//		if(data4){
//			sen_cfg.disp_mode = sensor_cntrl_disp_hum;
//		}
//		if(data5){
//			sen_cfg.disp_mode = sensor_cntrl_disp_press;
//		}
//		if(data6){
//			sen_cfg.disp_mode = sensor_cntrl_disp_lux;
//		}
//
//	////////////////////
//	//подключенные датчики
//	////////////////////
//		sen_cfg.sen_type_en_mask = 0;
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_en_0_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data1);
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_en_1_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data2);
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_en_2_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data3);
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_en_3_",pos);
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,tmp_32,&data4);
//
//		if(data1) sen_cfg.sen_type_en_mask |= 1 <<sensor_cntrl_collect_sen_temp_e;
//		if(data2) sen_cfg.sen_type_en_mask |= 1 <<sensor_cntrl_collect_sen_hum_e;
//		if(data3) sen_cfg.sen_type_en_mask |= 1 <<sensor_cntrl_collect_sen_lux_e;
//		if(data4) sen_cfg.sen_type_en_mask |= 1 <<sensor_cntrl_collect_sen_press_e;
//
////команда - в канал sen
//		httpd_make_src_string((char*)tmp_buf,sizeof(tmp_buf),ip,port);
//		cmd_diag(ddebug_id_cmd,"%s %s %s","get cmd = CONFIG SEN CH", "from=",tmp_buf);
//
//		control_config_sen_ch(&control, &sen_cfg, pos, b_flash);
////////////////
////имя канала
////////////////
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_set_name_",pos);
//	char *ptr;
//		htpp_parser_find_param_str(iNumParams,pcParam,pcValue,tmp_32,&ptr);
//uint16_t str_len;
//		str_len = strlen(ptr);
//		if(str_len > 31) str_len = 31;
//		memset(&control.dev_config.rec.sensor_ch_name[pos],0,32);
//		memcpy(&control.dev_config.rec.sensor_ch_name[pos],ptr,str_len);
//		dev_config_write(&control.dev_config);
//
//	}
//	if(b_read){
////char *p_str;
//char *p_str1,*p_str2;//,*p_str3,*p_str4,*p_str5;
//		pos = 0;
//		htpp_parser_find_param_int8(iNumParams,pcParam,pcValue,"num",&pos);
//		if (pos>=CLIMATE_MAX_NUM_PWR_CNTRL) pos=0;
//
/////////////////
////номер канала
/////////////////
//		sprintf(p_out,"%d",pos+1);
//		memset(&tmp_32[0],0,sizeof(tmp_32));
//		memcpy(tmp_32,"num",3);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_out, 0),tmp_32, 0);
/////////////////
////name
/////////////////
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"id_ch_set_name_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)&control.dev_config.rec.sensor_ch_name[pos][0], 0),tmp_32, 0);
//
//		////////////////////
//		//разрешение общее
//		////////////////////
//
//		p_str1 = (char*)http_str_zero;
//		p_str2 = (char*)http_str_zero;
//		if(control.sensors.mode[pos].en_dis == sensor_en_dis_enable){
//			p_str1 = (char*)http_str_one;
//		}
//		if(control.sensors.mode[pos].en_dis == sensor_en_dis_disable){
//			p_str2 = (char*)http_str_one;
//		}
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_0_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str2, 0),tmp_32, 0);
//
//		htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"sen_1_",pos);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, p_str1, 0),tmp_32, 0);
//
//		////////////////////
//		//разрешение отдельных датчиков
//		http_json_make_field(control.sensors.mode[pos].sen_type_en_mask & (1<<sensor_cntrl_collect_sen_temp_e), pos,"sen_en_0_",tmp_32,32,child);
//		http_json_make_field(control.sensors.mode[pos].sen_type_en_mask & (1<<sensor_cntrl_collect_sen_hum_e), pos,"sen_en_1_",tmp_32,32,child);
//		http_json_make_field(control.sensors.mode[pos].sen_type_en_mask & (1<<sensor_cntrl_collect_sen_lux_e), pos,"sen_en_2_",tmp_32,32,child);
//		http_json_make_field(control.sensors.mode[pos].sen_type_en_mask & (1<<sensor_cntrl_collect_sen_press_e), pos,"sen_en_3_",tmp_32,32,child);
//
//		////////////////////
//		//режим работы индикатора
//		////////////////////
//		http_json_make_field(control.sensors.mode[pos].disp_mode == sensor_cntrl_disp_off, pos,"d_en_0_",tmp_32,32,child);
//		http_json_make_field(control.sensors.mode[pos].disp_mode == sensor_cntrl_disp_num_ch, pos,"d_en_1_",tmp_32,32,child);
//		http_json_make_field(control.sensors.mode[pos].disp_mode == sensor_cntrl_disp_temp, pos,"d_en_2_",tmp_32,32,child);
//		http_json_make_field(control.sensors.mode[pos].disp_mode == sensor_cntrl_disp_hum, pos,"d_en_3_",tmp_32,32,child);
//		http_json_make_field(control.sensors.mode[pos].disp_mode == sensor_cntrl_disp_press, pos,"d_en_4_",tmp_32,32,child);
//		http_json_make_field(control.sensors.mode[pos].disp_mode == sensor_cntrl_disp_lux, pos,"d_en_5_",tmp_32,32,child);
//		////////////////////
//		//состояние подключения - физика
//		////////////////////
//		if(control.sensors.sen[pos].connect == sensor_connect_disable){
//			http_json_make_field_ext_str(1, pos,"st_",tmp_32,32,child," ","Запрещен");
//		}
//		if(control.sensors.sen[pos].connect == sensor_connect_ok){
//			http_json_make_field_ext_str(1, pos,"st_",tmp_32,32,child," ","Подключен");
//		}
//		if(control.sensors.sen[pos].connect == sensor_connect_fail){
//			http_json_make_field_ext_str(1, pos,"st_",tmp_32,32,child," ","Ошибка соединения");
//		}
//		if(control.sensors.sen[pos].connect == sensor_connect_undef){
//			http_json_make_field_ext_str(1, pos,"st_",tmp_32,32,child," ","Неопределено");
//		}
//
////ошибка
//		memset(tmp_buf,0,sizeof(tmp_buf));
//		if(control.sensors.sen[pos].error & sensor_cntrl_ch_error_sen1){
//			strcat((char*)tmp_buf," sen1");
//		}
//		if(control.sensors.sen[pos].error & sensor_cntrl_ch_error_sen2){
//			strcat((char*)tmp_buf," sen2");
//		}
//		if(control.sensors.sen[pos].error & sensor_cntrl_ch_error_sen3){
//			strcat((char*)tmp_buf," sen3");
//		}
//		if(control.sensors.sen[pos].error & sensor_cntrl_ch_error_sen4){
//			strcat((char*)tmp_buf," sen4");
//		}
//		if(control.sensors.sen[pos].error & sensor_cntrl_ch_error_i2c){
//			strcat((char*)tmp_buf," i2c");
//		}
//		if(control.sensors.sen[pos].error & sensor_cntrl_ch_error_not_ack){
//			strcat((char*)tmp_buf," ack");
//		}
//		http_json_make_field_ext_str(control.sensors.sen[pos].error, pos,"e_c",tmp_32,32,child,"Нет",(char*)tmp_buf);
////ошибка сохр
//		memset(tmp_buf,0,sizeof(tmp_buf));
//		if(control.sensors.sen[pos].hold_error & sensor_cntrl_ch_error_sen1){
//			strcat((char*)tmp_buf," sen1");
//		}
//		if(control.sensors.sen[pos].hold_error & sensor_cntrl_ch_error_sen2){
//			strcat((char*)tmp_buf," sen2");
//		}
//		if(control.sensors.sen[pos].hold_error & sensor_cntrl_ch_error_sen3){
//			strcat((char*)tmp_buf," sen3");
//		}
//		if(control.sensors.sen[pos].hold_error & sensor_cntrl_ch_error_sen4){
//			strcat((char*)tmp_buf," sen4");
//		}
//		if(control.sensors.sen[pos].hold_error & sensor_cntrl_ch_error_i2c){
//			strcat((char*)tmp_buf," i2c");
//		}
//		if(control.sensors.sen[pos].hold_error & sensor_cntrl_ch_error_not_ack){
//			strcat((char*)tmp_buf," ack");
//		}
//		http_json_make_field_ext_str(control.sensors.sen[pos].error, pos,"e_s",tmp_32,32,child,"Нет",(char*)tmp_buf);
//
//	//число запросов ответов
//		memset(tmp_buf,0,sizeof(tmp_buf));
//		sprintf((char*)tmp_buf, "%d/%d",control.sensors.sen[pos].num_send,control.sensors.sen[pos].num_ack);
//
//		http_json_make_field_ext_str(1, pos,"s_send",tmp_32,32,child,"Нет",(char*)tmp_buf);
//
//
//
//	}
//	return close_json_file(child,p_data);
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//const char * http_log_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res1, uint32_t ip, uint16_t port){
//char *p_data;
//jfes_value_t *child;
//uint32_t res_tmp;
//char time_buf[16];
//char date_buf[16];
//char tmp_buf1[32];
//char id_buf[4];
//char *p_ret_str;
//uint8_t i;
//uint8_t b_read,b_flash,b_write;
//char *p_char;
//
//	*p_res1 = NULL;
//	p_data = (char*)make_json_file_header(j_data,sizeof(j_data),(char*)&CGI_TAB[iIndex].pcCGIName[1],strlen(CGI_TAB[iIndex].pcCGIName));
//	child = jfes_create_object_value(&j_config);
//
//
//	htpp_parser_find_rd_wr_fl(iNumParams,pcParam,pcValue,&b_read,&b_write,&b_flash);
//
//uint32_t res;
//	htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"cmd",&p_char);
//	if(!strcmp("start_read",p_char)){
//		for(i=0;i<10;i++){
//			control.m_log.id_list[i] = 0;
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%s%d","id",i+1);
//			res = htpp_parser_find_param_str(iNumParams,pcParam,pcValue,(char*)tmp_buf,&p_char);
//			if(!res){
//				control.m_log.id_list[i] = 1;
//			}
//		}
//		res_tmp = logger_show_data_start((uint32_t)&control.m_log,0,0,NULL);
//		if(!res_tmp){
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "ok", 0),"cmd_ack_res", 0);
//		}
//		else{
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "stop_read", 0),"cmd_ack_res", 0);
//		}
//	}
//	if(!strcmp("continue_read",p_char)){
//uint16_t str_len=0;
//jfes_value_t *tmp1;
//		int i = 0;
//		while(1){
//			memset(time_buf,0,sizeof(time_buf));
//			memset(date_buf,0,sizeof(date_buf));
//			memset(id_buf,0,sizeof(id_buf));
//			res_tmp = logger_show_data_continue(&control.m_log,0,id_buf,date_buf,time_buf, (char*)tmp_buf, sizeof(tmp_buf),&p_ret_str);
//			if(!res_tmp){
//
//				tmp1 = jfes_create_string_value(&j_config, p_ret_str, 0);
//				str_len += tmp1->data.string_val.size;
//				memset(tmp_buf1,0,sizeof(tmp_buf1));
//				snprintf((char*)tmp_buf1,sizeof(tmp_buf1),"d_id%d",i);
//				str_len += strlen(tmp_buf1) + 8 ;
//				jfes_set_object_property(&j_config, child, tmp1,(char*)tmp_buf1, 0);
//
//				tmp1 = jfes_create_string_value(&j_config, date_buf, 0);
//				str_len += tmp1->data.string_val.size;
//				memset(tmp_buf1,0,sizeof(tmp_buf1));
//				snprintf((char*)tmp_buf1,sizeof(tmp_buf1),"date_id%d",i);
//				str_len += strlen(tmp_buf1) + 8;
//				jfes_set_object_property(&j_config, child, tmp1,(char*)tmp_buf1, 0);
//
//				tmp1 = jfes_create_string_value(&j_config, time_buf, 0);
//				str_len += tmp1->data.string_val.size;
//				memset(tmp_buf1,0,sizeof(tmp_buf1));
//				snprintf((char*)tmp_buf1,sizeof(tmp_buf),"time_id%d",i);
//				str_len += strlen(tmp_buf1) + 8;
//				jfes_set_object_property(&j_config, child, tmp1,(char*)tmp_buf1, 0);
//
//				tmp1 = jfes_create_string_value(&j_config, id_buf, 0);
//				str_len += tmp1->data.string_val.size;
//				memset(tmp_buf1,0,sizeof(tmp_buf1));
//				snprintf((char*)tmp_buf1,sizeof(tmp_buf1),"id_id%d",i);
//				str_len += strlen(tmp_buf1) + 8;
//				jfes_set_object_property(&j_config, child, tmp1,(char*)tmp_buf1, 0);
//
//				if(str_len > 500) break;
//				i++;
//			}
//			else{
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "stop_read", 0),"cmd_ack_res", 0);
//				break;
//			}
//		}
//	}
//	if(!strcmp("get_id_list",p_char)){
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "sys", 0),"id1", 0);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "cmd", 0),"id2", 0);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "error", 0),"id3", 0);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "internal", 0),"id4", 0);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "mot cntr", 0),"id6", 0);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "view", 0),"id7", 0);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_id_list_ack", 0),"cmd_ack_res", 0);
//	}
//	return close_json_file(child,p_data);
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char * sys_ack(int iIndex, int iNumParams, char *pcParam[], char *pcValue[],char *p_res1, uint32_t ip, uint16_t port){
char *p_data;
jfes_value_t *child;
uint32_t res;
uint8_t type_cmd;
uint8_t b_flash, b_write, b_read;
char *p_out;
char tmp_32[32];
uint8_t b_off,b_on;
uint8_t ch_num;
//cmd_result_t  res_cmd=cmd_ok;
uint32_t res_tmp;
uint8_t pos=0;
char *ptr;
char* p_char;
char* p_id;

	if(!iNumParams) return NULL;


#ifdef UDP_SENDER
	control.udp_sender_dest_ip.addr = ip;
#endif


	p_out = (char*)tmp_buf;
	type_cmd = -1;
	memset(tmp_32,0,sizeof(tmp_32));

//	printf("sys app\r\n");
//	return NULL;

	*p_res1 = NULL;
	p_data = (char*)make_json_file_header(j_data,sizeof(j_data),(char*)&CGI_TAB[iIndex].pcCGIName[1],strlen(CGI_TAB[iIndex].pcCGIName));
	printf("json=%x\r\n",(uint32_t)p_data);
//	return NULL;
	child = jfes_create_object_value(&j_config);
	printf("jfes=%x\r\n",(uint32_t)child);
	printf("in num =%x %x\r\n",strlen(pcParam[0]),strlen(pcValue[0]));
	printf("app str =%s %s\r\n",pcParam[0],pcValue[0]);
//	return NULL;

	htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"cmd",&p_char);

//	printf("app cmd=%s\r\n",p_char);

	if(!strcmp("startboot",p_char)){

//		httpd_make_src_string((char*)tmp_buf,sizeof(tmp_buf),ip,port);
//		printf("ip from= %s\r\n",tmp_buf);



//		osSignalSet(control.thread_id,main_cntrl_thread_event_restart);






//		make_record_for_restart(&control,1,control.config.type_wifi_connect,(uint8_t*)control.config.dev_name,strlen(control.config.dev_name),(uint8_t*)control.config.dev_pass,strlen(control.config.dev_pass));
//		make_record_for_restart(&control,1);
	}
//	if(!strcmp("get_console",p_char)){
//uint32_t len,len_all,i,b_find,pos,num;
//uint8_t b_get;
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_console", 0),"cmd", 0);
//		len_all = buf_console_get_len(&control.buf_console);
//		len = len_all;
//		if(len){
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			if(len > (sizeof(tmp_buf)-1)) len = sizeof(tmp_buf)-1;
//			buf_console_read(&control.buf_console,(uint8_t*)tmp_buf,len);
//			pos = 0;
//			num = 0;
//			b_get = 0;
//			for(i=0;i<len;i++){
//				if(tmp_buf[i] > 0x0D) b_get = 1;
//				if(tmp_buf[i] <= 0x0D){
//					if(b_get){
//						b_get = 0;
//						tmp_buf[i] = 0;
//						htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"s_",num);
//						num++;
//						jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, &tmp_buf[pos], 0),tmp_32, 0);
//						pos = i+1;
//					}
//					else{
//						tmp_buf[i] = 0;
//						pos = i+1;
//					}
//				}
//			}
//			if(b_get){
//				htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"s_",num);
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, &tmp_buf[pos], 0),tmp_32, 0);
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "yes", 0),"trim", 0);
//			}
//			else{
//				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "no", 0),"trim", 0);
//			}
//		}
//		sprintf(tmp_32,"%d",len);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"len_ack", 0);
//		memset(tmp_32,0,sizeof(tmp_32));
//		sprintf(tmp_32,"%d",len_all);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"len_all", 0);
//		memset(tmp_32,0,sizeof(tmp_32));
//		sprintf(tmp_32,"%d",control.buf_console.id);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_32, 0),"id", 0);
//		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "no", 0),"error", 0);
//	}
	if(!strcmp("get_table",p_char)){

		htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"sub_cmd",&p_id);
		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_table", 0),"cmd", 0);
		jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "no", 0),"error", 0);
		if(!strcmp("get_list",p_id)){
			int i;
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_list", 0),"sub_cmd", 0);
			for(i=0;i<MAX_NUM_FUNC_TABLE;i++){
				if(!control.table_list[i].name) break;
				htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"n_",i);
				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, control.table_list[i].name, 0),tmp_32, 0);

				htpp_parser_make_id_from_str_and_val(&tmp_32[0],32,"m_",i);
				memset(tmp_buf,0,sizeof(tmp_buf));
				sprintf(tmp_buf,"%d",control.table_list[i].mode);
				jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, tmp_buf, 0),tmp_32, 0);
			}
		}
		if(!strcmp("get_param",p_id)){
			int i;
			htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"name",&p_id);
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_param", 0),"sub_cmd", 0);
			for(i=0;i<MAX_NUM_FUNC_TABLE;i++){
				if(!control.table_list[i].name) break;
				if(!strcmp(control.table_list[i].name,p_id)){
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, control.table_list[i].name, 0),"name", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->top_x);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"x", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->top_y);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"y", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->width);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"w", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->height);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"h", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->flag);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"f", 0);
					break;
				}
			}
		}
//		if(!strcmp("set_param",p_id)){
//			int i;
//			htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"name",&p_id);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "set_param", 0),"sub_cmd", 0);
//			for(i=0;i<MAX_NUM_FUNC_TABLE;i++){
//				if(!control.table_list[i].name) break;
//				if(!strcmp(control.table_list[i].name,p_id)){
//
//
//
////					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, control.table_list[i].name, 0),"name", 0);
////
////					memset(tmp_buf,0,sizeof(tmp_buf));
////					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->top_x);
////					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"x", 0);
////
////					memset(tmp_buf,0,sizeof(tmp_buf));
////					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->top_y);
////					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"y", 0);
////
////					memset(tmp_buf,0,sizeof(tmp_buf));
////					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->width);
////					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"w", 0);
////
////					memset(tmp_buf,0,sizeof(tmp_buf));
////					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->height);
////					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"h", 0);
////
////					memset(tmp_buf,0,sizeof(tmp_buf));
////					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->flag);
////					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"f", 0);
//					break;
//				}
//			}
//		}

		if(!strcmp("get_c_p",p_id)){
			int i;
			htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"name",&p_id);
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "get_c_p", 0),"sub_cmd", 0);
			for(i=0;i<MAX_NUM_FUNC_TABLE;i++){
				if(!control.table_list[i].name) break;
				if(!strcmp(control.table_list[i].name,p_id)){
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, control.table_list[i].name, 0),"name", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_cell_param->edit_wide);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"ew", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_cell_param->height);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"h", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_cell_param->height_space);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"hs", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_cell_param->name_wide);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"nw", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_cell_param->x_offset);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"xo", 0);


/*
					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->top_y);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"y", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->width);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"w", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->height);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"h", 0);

					memset(tmp_buf,0,sizeof(tmp_buf));
					sprintf((char*)tmp_buf,"%d",control.table_list[i].p_param->flag);
					jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"f", 0);
*/
					break;
				}
			}
		}



		if(!strcmp("set_param",p_id)){
			int i;
			htpp_parser_find_param_str(iNumParams,pcParam,pcValue,"name",&p_id);
			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, "set_param", 0),"sub_cmd", 0);
			for(i=0;i<MAX_NUM_FUNC_TABLE;i++){
				if(!control.table_list[i].name) break;
				if(!strcmp(control.table_list[i].name,p_id)){
					htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"x",&control.table_list[i].p_param->top_x);
					htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"y",&control.table_list[i].p_param->top_y);
					htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"w",&control.table_list[i].p_param->width);
					htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"h",&control.table_list[i].p_param->height);
					htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,"f",&control.table_list[i].p_param->flag);
					uint16_t l;
					htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"l",&l);
					if(l){
						//dev_config_write(&control.dev_config);
					}
					break;
				}
			}
		}
//		if(!strcmp("get_sys",p_id)){
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%d",control.config.log_console.flag);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"int_f", 0);
//
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%d",control.config.dev_console.flag);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"ext_f", 0);
//
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%d",control.config.log_console.top_x);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"int_x", 0);
//
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%d",control.config.log_console.top_y);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"int_y", 0);
//
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%d",control.config.log_console.width);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"int_w", 0);
//
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%d",control.config.log_console.height);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"int_h", 0);
//
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%d",control.config.dev_console.top_x);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"ext_x", 0);
//
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%d",control.config.dev_console.top_y);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"ext_y", 0);
//
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%d",control.config.dev_console.width);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"ext_w", 0);
//
//			memset(tmp_buf,0,sizeof(tmp_buf));
//			sprintf((char*)tmp_buf,"%d",control.config.dev_console.height);
//			jfes_set_object_property(&j_config, child, jfes_create_string_value(&j_config, (char*)tmp_buf, 0),"ext_h", 0);
//		}
//		if(!strcmp("set_sys",p_id)){
//			htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"int_x",&control.config.log_console.top_x);
//			htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"int_y",&control.config.log_console.top_y);
//			htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"int_w",&control.config.log_console.width);
//			htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"int_h",&control.config.log_console.height);
//			htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,"int_f",&control.config.log_console.flag);
//
//			htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"ext_x",&control.config.dev_console.top_x);
//			htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"ext_y",&control.config.dev_console.top_y);
//			htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"ext_w",&control.config.dev_console.width);
//			htpp_parser_find_param_int16(iNumParams,pcParam,pcValue,"ext_h",&control.config.dev_console.height);
//			htpp_parser_find_param_int32(iNumParams,pcParam,pcValue,"ext_f",&control.config.dev_console.flag);
//
//			dev_config_write(&control.dev_config);
//		}
	}
	return close_json_file(child,p_data);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t num_cgi;
extern struct fsdata_file tmp_ajax_file;
void httpd_cgi_init(void){
//  CGI_TAB[0] = t_set_pwr_ch;
//  CGI_TAB[1] = t_set_sense_ch;
//  CGI_TAB[2] = t_conf_ref;
//  CGI_TAB[3] = t_lan;
//  CGI_TAB[4] = t_set_pwr_cfg_rel_ch;
//  CGI_TAB[5] = t_pwr_set;
//  CGI_TAB[6] = t_sen_set;
//  CGI_TAB[7] = t_log;

	  CGI_TAB[0] = t_set_pwr_ch;//t_set_pwr_ch;
	  CGI_TAB[1] = t_sys;

	  num_cgi = 2;

  http_set_cgi_handlers(CGI_TAB, 2);
}

void http_cgi_81_thread(void *arg) {
    struct netconn *conn, *newconn;
    err_t err;
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, IP_ADDR_ANY, 81); // Тот самый 81 порт
    netconn_listen(conn);

    while (1) {
        err = netconn_accept(conn, &newconn);
        if (err != ERR_OK) continue;

        struct netbuf *inbuf;
        char *buf;
        u16_t buflen;

        if (netconn_recv(newconn, &inbuf) == ERR_OK) {
            netbuf_data(inbuf, (void**)&buf, &buflen);

            // 1. Примитивный парсер GET
            if (buflen > 10 && memcmp(buf, "GET /", 5) == 0) {
                char *pcParam[MAX_CGI_PARAMS];
                char *pcValue[MAX_CGI_PARAMS];
                int iNumParams = 0;

                // Находим URI и параметры
                char *uri = buf + 5;
                char *end_uri = strchr(uri, ' ');
                if (end_uri) *end_uri = '\0';

                char *query = strchr(uri, '?');
                if (query) {
                    *query = '\0'; // Отделяем имя файла от параметров
                    char *token = query + 1;


                    int safety_counter = 0;




                    // Разбираем параметры &key=value
//                    while (token && iNumParams < MAX_CGI_PARAMS) {
                        while (token && *token != '\0' && iNumParams < MAX_CGI_PARAMS) {
                            if (safety_counter++ > 100) break; // Защита от вечного цикла

                        pcParam[iNumParams] = token;
                        char *equal = strchr(token, '=');
                        if (equal) {
                            *equal = '\0';
                            pcValue[iNumParams] = equal + 1;
                        } else {
                            pcValue[iNumParams] = "";
                        }

                        char *amp = strchr(pcValue[iNumParams], '&');
                        if (amp) {
                            *amp = '\0';
                            token = amp + 1;
                        } else {
                            token = NULL;
                        }
                        iNumParams++;
                    }
                }

                // 2. Ищем, какую функцию вызвать (пример для navigator.js)
                const char *response_json = NULL;
                char p_res[32]; // Если твои функции используют этот буфер

                for (int i = 0; i < num_cgi; i++) {
                    if (strcmp(uri, CGI_TAB[i].pcCGIName) == 0) {
                        // Нашли! Зовем штатную функцию из твоего списка
                        response_json = CGI_TAB[i].pfnCGIHandler(i, iNumParams, pcParam, pcValue, p_res, 0, 0);
//                    	response_json = "12";
                        break;
                    }
                }

//                if (strcmp(uri, "navigator.js") == 0) {
//                    // Вызываем твой обработчик напрямую!
//                    // Здесь безопасно использовать osEventFlagsWait внутри твоих функций
//                    response_json = http_cntrl_navigator_ack(0, iNumParams, pcParam, pcValue, p_res, 0, 0);
//                }
                // ... тут остальные strcmp для других функций ...

                // 3. Отправляем HTTP ответ с CORS
                if (response_json) {
//                    char header[256];
//                    int hlen = sprintf(header,
//                        "HTTP/1.1 200 OK\r\n"
//                        "Content-Type: application/json\r\n"
//                        "Access-Control-Allow-Origin: *\r\n"
//                        "Content-Length: %d\r\n"
//                        "Connection: close\r\n\r\n", strlen(response_json));
//
//                    netconn_write(newconn, header, hlen, NETCONN_COPY);
//                    netconn_write(newconn, response_json, strlen(response_json), NETCONN_COPY);
                    netconn_write(newconn, tmp_ajax_file.data, tmp_ajax_file.len, NETCONN_COPY);
//                    netconn_write(newconn, newconn, 134, NETCONN_COPY);
                }
            }
            netbuf_delete(inbuf);
        }
        netconn_close(newconn);
        netconn_delete(newconn);
    }
}
