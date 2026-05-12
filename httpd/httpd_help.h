/*
 * httpd_help.h
 *
 *  Created on: 19 апр. 2017 г.
 *      Author: krasikov
 */

#ifndef SRC_INC_HTTPD_HELP_H_
#define SRC_INC_HTTPD_HELP_H_

#include "stdint.h"
#ifndef	FSBL
#include "jfes.h"
#endif
#include "cmsis_os2.h"

extern jfes_config_t j_config;
extern jfes_parser_t j_parser;
extern char j_data[1024];
//uint8_t tmp_buf[64];
extern struct fsdata_file tmp_ajax_file;

void json_parser_init(void);
void urldecode(char *st);
uint32_t htpp_parser_find_param_index(int iNumParams, char *pcParam[], char *p_name, uint32_t *p_index);
uint32_t htpp_parser_find_param_flag(int iNumParams, char *pcParam[],char *pcValue[], char *p_name, uint8_t *p_flag);
uint32_t htpp_parser_find_param_int32(int iNumParams, char *pcParam[],char *pcValue[], char *p_name, uint32_t *p_d);
uint32_t htpp_parser_find_rd_wr_fl(int iNumParams, char *pcParam[],char *pcValue[],uint8_t *pb_rd, uint8_t *pb_wr, uint8_t *pb_fl);
uint32_t htpp_parser_find_param_int8(int iNumParams, char *pcParam[],char *pcValue[], char *p_name, uint8_t *p_d);
uint32_t htpp_parser_find_param_int16(int iNumParams, char *pcParam[],char *pcValue[], char *p_name, uint16_t *p_d);
uint32_t htpp_parser_find_param_str(int iNumParams, char *pcParam[],char *pcValue[], char *p_name, char **p_str);
uint32_t htpp_parser_find_id_from_str(char *out_str, uint8_t out_str_size, char *start_str, char *num_str, uint8_t *p_num);
uint32_t htpp_parser_make_id_from_str_and_val(char *out_str, uint8_t out_str_size, char *start_str, uint8_t val);
uint8_t * make_json_file_header(char *p_data8, uint16_t len, char *p_name, uint8_t f_len);

void http_json_make_field( uint8_t bit_val, uint8_t pos, char *p_base_name, char * p_str, uint8_t str_len, jfes_value_t *p_child);
void http_json_make_field_ext_str( uint8_t bit_val, uint8_t pos, char *p_base_name, char * p_str, uint8_t str_len, jfes_value_t *p_child, char* p_zero_str, char * p_one_str);

char *close_json_file(jfes_value_t *child, char *p_data);

void htpp_parser_make_string_from_error(osStatus_t code, uint8_t *p_ack, uint16_t str_len);


#endif /* SRC_INC_HTTPD_HELP_H_ */
