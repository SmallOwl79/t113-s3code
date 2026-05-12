/*
 * httpd_help.c
 *
 *  Created on: 19 апр. 2017 г.
 *      Author: krasikov
 */
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "jfes.h"
#include "lwip/apps/fs.h"
#include "fsdata.h"
#include "httpd_help.h"
#include "ff.h"
#include "cmsis_os2.h"
#include "control.h"

//osPoolDef(http_file_pool, 2, FIL); // Define memory pool

#ifdef				EXT_FLASH_SUPPORT
osPoolDef(http_file_pool, 1, FIL); // Define memory pool
osPoolDef(http_vid_file_pool, 1, FIL_EMPTY); // Define memory pool
#endif

static const char *http_str_vid_data_name = "vid.dat";

//struct fs_file *p_ext_file;
#pragma location=".ddr_data"
char j_data[1024];
//uint8_t tmp_buf[64];

jfes_config_t j_config;
jfes_parser_t j_parser;
//FIL fl_file;

#ifdef				EXT_FLASH_SUPPORT
osPoolId httpd_file_pool_id;
osPoolId httpd_video_file_pool_id;
#endif

struct fsdata_file tmp_ajax_file = {
	NULL,
	(unsigned char*)&j_data[0],
	(unsigned char*)&j_data[20],
	0,
	1
};
static const char *http_str_zero = "0";
static const char *http_str_one = "1";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void htpp_parser_make_string_from_error(osStatus_t code, uint8_t *p_ack, uint16_t str_len){
	memset(p_ack,0,str_len);
	switch(code){
		case osOK: 					memcpy(p_ack,"No error",strlen("No error")); break;
		case osErrorTimeout: 		memcpy(p_ack,"TimeOut error",strlen("TimeOut error")); break;
		case osErrorParameter: 		memcpy(p_ack,"Parameter error",strlen("Parameter error")); break;
		case osErrorResource: 		memcpy(p_ack,"Resource error",strlen("Resource error")); break;
	//	case osErrorValue: 			memcpy(p_ack,"Remoute ack error",strlen("Remoute ack error")); break;
		default: 					memcpy(p_ack,"Unknow error",strlen("Unknow error")); break;
	}
	 return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void urldecode(char *st) {
  char *p=st;  // указывает на текущий символ строки
  char hex[3]; // временный буфер для хранения %XX
  int code;    // преобразованный код
  // запускаем цикл, пока не кончится строка (то есть, пока не
  // появится символ с кодом 0, см. ниже)
  do {
    // Если это %-код ...
    if(*st == '%') { // тогда копируем его во временный буфер
      hex[0]=*(++st); hex[1]=*(++st); hex[2]=0;
      // переводим его в число
      sscanf(hex,"%X",&code);
      // и записываем обратно в строку
      *p++=(char)code;
      // указатель p всегда отмечает то место в строке, в которое
      // будет помещен очередной декодированный символ
    }
    // иначе, если это "+", то заменяем его на " "
    else if(*st=='+') *p++=' ';
    // а если не то, ни другое - оставляем как есть
    else *p++=*st;
  } while(*st++!=0); // пока не найдем нулевой код
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int fs_open_custom(struct fs_file *file, const char *name){
struct fsdata_file *f;
FIL *p_file;
FIL_EMPTY* p_vid_file;

	if ((file == NULL) || (name == NULL)) {
//		printf("fopen error \r\n");
		return ERR_ARG;
	}
	file->pextension = NULL;
	f = &tmp_ajax_file;

	if (!strcmp(name, "/vid.dat")) {

//#ifdef UDP_SENDER
//		if(control.b_udp_send){
//			return 0;
//		}
//#endif

#ifdef NOT_VIDEO_RECEIVER_HTTP
		return 0;
#endif
#ifndef NOT_VIDEO_RECEIVER_HTTP
//		printf("open vid \r\n");
		p_vid_file = osPoolAlloc(httpd_video_file_pool_id);
		if(p_vid_file==NULL){
//			printf("vid file alloc error \r\n");
			return 0;
		}
		control.b_http_file_open = 1;
		control.grabber.file_cntr ++;
	//	p_ext_file = p_file;
		file->p_name = http_str_vid_data_name;
		  file->data = NULL;//http_str_vid_data_name;
		  file->len = control.grabber.wigth*(control.grabber.height);///2;///2;//control.grabber.output_buff_size;;//control.grabber.wigth* control.grabber.height;//control.grabber.output_buff_size;//720*16;
//		  control.grabber.b_start = 1;
//		  control.grabber.b_first_read = 0;
		  file->index = 0;
		  file->pextension = p_vid_file;
		  file->flags = 0;
//		  osDelay(100);
//		  HAL_GPIO_WritePin(control.led_active_green.port, control.led_active_green.pin,GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10,GPIO_PIN_SET);
#endif
		return 1;
	}
	if (!strcmp(name, "sys.js")) {
//		cntr_start_grabb(&control);
	}

	if (!strcmp(name, (const char *)f->name)) {
		file->data = (const char *)f->data;
		file->len = f->len;
		file->index = f->len;
		file->pextension = NULL;
		file->flags = f->flags;
		return 1;
	}

//	return 0;
#ifdef				EXT_FLASH_SUPPORT
	HAL_DCMI_Stop(&control.grabber.hdcmi);
	__HAL_DMA_DISABLE(control.grabber.hdcmi.DMA_Handle);
#endif

#ifdef				EXT_FLASH_SUPPORT
	p_file = osPoolAlloc(httpd_file_pool_id);
	if(p_file==NULL) return 1;
	  if (f_open(p_file, (const TCHAR*)&name[1], FA_READ) != FR_OK){
		  osPoolFree(httpd_file_pool_id,p_file);
	    return 0;
	  }
	  else{
		  file->data = NULL;
		  file->len = f_size(p_file);
		  file->index = 0;
		  file->pextension = p_file;
		  printf("file open custom  %s %x\r\n",name, file);
		  return 1;
	  }
#endif


  return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int fs_read_custom(struct fs_file *file, char *buffer, int count){
FRESULT res;
UINT num_read;
	num_read = 0;
//	printf("count  = %d \r\n", count);
	if(file->p_name == http_str_vid_data_name){
#ifdef NOT_VIDEO_RECEIVER_HTTP
		return 0;
#endif
#ifndef NOT_VIDEO_RECEIVER_HTTP
		res = grabber_get_data (file->pextension, buffer, count, &num_read,file->index);
#endif

//		num_read = count;

		file->index += num_read;
	}
	else{
#ifdef				EXT_FLASH_SUPPORT
		res = f_read (file->pextension, buffer, count, &num_read);
#else
		num_read = 0;
#endif
		file->index += num_read;
	}
	return num_read;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void fs_close_custom(struct fs_file *file){
	if(file->pextension != NULL){

		if(file->p_name == http_str_vid_data_name){
////			printf("close video\r\n");
////			HAL_GPIO_WritePin(control.led_active_green.port, control.led_active_green.pin,GPIO_PIN_RESET);
//			osPoolFree(httpd_video_file_pool_id,file->pextension);
//			//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10,GPIO_PIN_RESET);
//
//			control.b_http_file_open = 0;
//
//			if(control.b_wait_udp_send == 1){
//				control.b_udp_send = 1;
//				control.b_wait_udp_send = 0;
//			}
		}
		else{
#ifdef				EXT_FLASH_SUPPORT
			f_close(file->pextension);
#endif
//			printf("close custom =%x \r\n",(uint32_t)file);
#ifdef				EXT_FLASH_SUPPORT
			osPoolFree(httpd_file_pool_id,file->pextension);
#endif
		}

		file->pextension = NULL;
//		printf("file close %x\r\n",file);
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void * my_malloc(size_t num){
//	printf("alloc size = %d ",num);
//	return malloc(num);
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void json_parser_init(void){
//    j_config.jfes_malloc = my_malloc;
//    j_config.jfes_free = my_free;

#ifdef				EXT_FLASH_SUPPORT
	httpd_file_pool_id = osPoolCreate(osPool(http_file_pool));
	httpd_video_file_pool_id = osPoolCreate(osPool(http_vid_file_pool));
#endif
    j_config.jfes_malloc = malloc;
    j_config.jfes_free = free;

    jfes_init_parser(&j_parser, &j_config);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t htpp_parser_find_param_index(int iNumParams, char *pcParam[], char *p_name, uint32_t *p_index){
uint32_t res=1;
char i,*p_param;
	for(i=0;i<iNumParams;i++){
		p_param = pcParam[i];
		if (!strcmp(p_param,p_name)){
			res = 0;
			*p_index = i;
			break;
		}
	}
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t htpp_parser_find_param_flag(int iNumParams, char *pcParam[],char *pcValue[], char *p_name, uint8_t *p_flag){
uint32_t ind,res;
char *p_data;
uint8_t test;
	*p_flag = 0;
	res = htpp_parser_find_param_index(iNumParams,pcParam,p_name, &ind);
	if(!res){
		p_data = pcValue[ind];
		test = atoi(p_data);
		if(test==1) *p_flag = 1;
	}
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint32_t htpp_parser_find_param_double(int iNumParams, char *pcParam[],char *pcValue[], char *p_name, double *p_d){
//uint32_t ind,res;
//char *p_data;
//double test;
//	*p_d = 0;
//	res = htpp_parser_find_param_index(iNumParams,pcParam,p_name, &ind);
//	if(!res){
//		p_data = pcValue[ind];
//		mysscanf_d(p_data,&test);
//		*p_d = test;
//	}
//	return res;
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t htpp_parser_find_param_int32(int iNumParams, char *pcParam[],char *pcValue[], char *p_name, uint32_t *p_d){
uint32_t ind,res;
char *p_data;
int test;
	*p_d = 0;
	res = htpp_parser_find_param_index(iNumParams,pcParam,p_name, &ind);
	if(!res){
		p_data = pcValue[ind];
		sscanf(p_data,"%u", p_d);
//		test = atoi(p_data);
//		*p_d = test;
	}
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t htpp_parser_find_rd_wr_fl(int iNumParams, char *pcParam[],char *pcValue[],uint8_t *pb_rd, uint8_t *pb_wr, uint8_t *pb_fl){
uint32_t ind,res;
int test;
	*pb_rd = 0;
	*pb_wr = 0;
	*pb_fl = 0;

	res = htpp_parser_find_param_index(iNumParams,pcParam,"flash_id", &ind);
	if(!res){
		test = atoi(pcValue[ind]);
		if(test==1) *pb_fl = 1;
	}
	res = htpp_parser_find_param_index(iNumParams,pcParam,"write_id", &ind);
	if(!res){
		test = atoi(pcValue[ind]);
		if(test==1) *pb_wr = 1;
	}

	res = htpp_parser_find_param_index(iNumParams,pcParam,"read_id", &ind);
	if(!res){
		test = atoi(pcValue[ind]);
		if(test==1) *pb_rd = 1;
	}
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t htpp_parser_find_param_int8(int iNumParams, char *pcParam[],char *pcValue[], char *p_name, uint8_t *p_d){
uint32_t ind,res;
char *p_data;
int test;
	*p_d = 0;
	res = htpp_parser_find_param_index(iNumParams,pcParam,p_name, &ind);
	if(!res){
		p_data = pcValue[ind];
		test = atoi(p_data);
		*p_d = test;
	}
	return res;
}
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t htpp_parser_find_param_int16(int iNumParams, char *pcParam[],char *pcValue[], char *p_name, uint16_t *p_d){
uint32_t ind,res;
char *p_data;
int test;
	*p_d = 0;
	res = htpp_parser_find_param_index(iNumParams,pcParam,p_name, &ind);
	if(!res){
		p_data = pcValue[ind];
		test = atoi(p_data);
		*p_d = test;
	}
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t htpp_parser_find_param_str(int iNumParams, char *pcParam[],char *pcValue[], char *p_name, char **p_str){
uint32_t ind,res;
	*p_str = NULL;
	res = htpp_parser_find_param_index(iNumParams,pcParam,p_name, &ind);
	if(!res){
		*p_str = pcValue[ind];
	}
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t htpp_parser_find_id_from_str(char *out_str, uint8_t out_str_size, char *start_str, char *num_str, uint8_t *p_num){
uint32_t res=0;
char tmp_16[16];
	memset(tmp_16,0,16);
	memcpy(tmp_16,&num_str[10],strlen(num_str)-10);
	*p_num = atoi(tmp_16);
	memset(out_str,0,out_str_size);
	memcpy(out_str,start_str,strlen(start_str));
	strcat(out_str,tmp_16);
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t htpp_parser_make_id_from_str_and_val(char *out_str, uint8_t out_str_size, char *start_str, uint8_t val){
uint32_t res=0;
char tmp_16[16];
	memset(tmp_16,0,16);
	sprintf(tmp_16,"%d",val);
	memset(out_str,0,out_str_size);
	memcpy(out_str,start_str,strlen(start_str));
	strcat(out_str,tmp_16);
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void http_json_make_field( uint8_t bit_val, uint8_t pos, char *p_base_name, char * p_str, uint8_t str_len, jfes_value_t *p_child){
const char *p_val;
	p_val = http_str_zero;
	if(bit_val) p_val = http_str_one;
	htpp_parser_make_id_from_str_and_val(p_str,str_len,p_base_name,pos);
	jfes_set_object_property(&j_config, p_child, jfes_create_string_value(&j_config, p_val, 0),p_str, 0);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void http_json_make_field_ext_str( uint8_t bit_val, uint8_t pos, char *p_base_name, char * p_str, uint8_t str_len, jfes_value_t *p_child, char* p_zero_str, char * p_one_str){
const char *p_val;
	p_val = p_zero_str;
	if(bit_val) p_val = p_one_str;
	htpp_parser_make_id_from_str_and_val(p_str,str_len,p_base_name,pos);
	jfes_set_object_property(&j_config, p_child, jfes_create_string_value(&j_config, p_val, 0),p_str, 0);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t * make_json_file_header(char *p_data8, uint16_t len, char *p_name, uint8_t f_len) {
uint32_t off;
uint8_t *ptr;

	memset(p_data8,0,len);
	memcpy(p_data8,p_name,f_len);

	ptr = (uint8_t*)&p_data8[20];

	off = strlen("HTTP/1.0 200 OK\r\n");
	memcpy(ptr,"HTTP/1.0 200 OK\r\n",off);
	ptr += off;

	off = strlen("Server: lwIP2\r\n");
	memcpy(ptr,"Server: lwIP2\r\n",off);
	ptr += off;

	off = strlen("Content-type: application/json\r\n");
	memcpy(ptr,"Content-type: application/json\r\n",off);
	ptr += off;

	off = strlen("Access-Control-Allow-Origin: *\r\n");
	memcpy(ptr,"Access-Control-Allow-Origin: *\r\n",off);
	ptr += off;

	off = strlen("Connection: close\r\n\r\n");
	memcpy(ptr,"Connection: close\r\n\r\n",off);
	ptr += off;

	return ptr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char *close_json_file(jfes_value_t *child, char *p_data){
jfes_size_t len;
	tmp_ajax_file.len = (p_data - j_data);
	len = sizeof(j_data) - tmp_ajax_file.len;

	jfes_value_to_string(child,p_data,&len,1);

	tmp_ajax_file.len += len;
	tmp_ajax_file.len -= 20;

	jfes_free_value(&j_config,child);
	free(child);
	tmp_ajax_file.data = (unsigned char const*)&j_data[20];

	return (char*)tmp_ajax_file.name;
}
