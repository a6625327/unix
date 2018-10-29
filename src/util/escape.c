#include "escape.h"

/*
	----帧内容转义----
*/
int8_t escaper(void *data, const size_t escap_num, void **ret_data, size_t *ret_data_num){   //用完需要free返回值
	LOG_FUN;
	// tem_buf需要free
	uint8_t *tem_buf = malloc_print_addr(sizeof(uint8_t) * escap_num * 2); 
	uint8_t *tem_buf_p = tem_buf;
	if(tem_buf == NULL){
		zlog_error(log_cat, "tem_buf malloc error, error msg: %s", strerror(errno));
		return -1;
	}

	uint8_t  *databuf = (uint8_t *)data;

	uint16_t escap_cnt = 0;
	uint16_t  escape_str_cnt = 0;         // 转义的次数

	*tem_buf_p++ = *databuf++;   // 帧头先复制，同时帧头不转义
	escap_cnt++;
	while(escap_cnt < escap_num - 1) { // -1则帧尾不转义
		if(*databuf == 0xF0) {
			*tem_buf_p++ = 0xF0;
			*tem_buf_p++ = 0x02;
			databuf++;
			escape_str_cnt++;
			escap_cnt++;
			continue;
		}
		if(*databuf == 0xF1) {
			*tem_buf_p++ = 0xF0;
			*tem_buf_p++ = 0x01;
			databuf++;
			escape_str_cnt++;
			escap_cnt++;
			continue;
		}
		if(*databuf == 0xF2) {
			*tem_buf_p++ = 0xF0;
			*tem_buf_p++ = 0x00;
			databuf++;
			escape_str_cnt++;
			escap_cnt++;
			continue;
		}
		*tem_buf_p++ = *databuf++;
		escap_cnt++;
	}
	*tem_buf_p = *databuf; // 复制帧尾

	*ret_data = tem_buf;
	*ret_data_num = escap_num + escape_str_cnt;

    zlog_info(log_cat, "escap cnt: %ld", *ret_data_num);

	return 0;
}

/*
	----将被转义的字符转换回来----
*/
int8_t unescaper(void *data, const size_t unescap_num, void **ret_data, size_t *ret_data_num){	
	LOG_FUN;
	// tem_buf需要free
	uint8_t *tem_buf = malloc_print_addr(sizeof(uint8_t) * unescap_num); 
	uint8_t *tem_buf_p = tem_buf;

	uint16_t  unescape_str_cnt = 0;         // 转义的次数
	uint8_t  *databuf = (uint8_t *)data;
	if(tem_buf == NULL){
		zlog_error(log_cat, "tem_buf malloc error, error msg: %s", strerror(errno));
		return -1;
	}

	uint16_t unescap_cnt = 0;

	*tem_buf_p++ = *databuf++;   // 帧头先复制，同时帧头不转义
	unescap_cnt++;
	while(unescap_cnt < unescap_num - 1){ // -1则帧尾不转义
		if(*databuf == 0xF0) {
			if(*(databuf + 1) == 0x00) {
				*tem_buf_p++ = 0xF2;
			}else if(*(databuf + 1)  == 0x01) {
				*tem_buf_p++ = 0xF1;
			}else if(*(databuf + 1)  == 0x02) {
				*tem_buf_p++ = 0xF0;
			}
			unescap_cnt++;
			unescape_str_cnt++;
			databuf += 2;
			continue;
		}
		unescap_cnt++;
		*tem_buf_p++ = *databuf++;
	}
	*tem_buf_p = *databuf; // 复制帧尾

	*ret_data = tem_buf;
	*ret_data_num = unescap_num - unescape_str_cnt;

    zlog_info(log_cat, "字符数: %ld， 转义结果：", unescap_num);
	hzlog_debug(log_cat, tem_buf, *ret_data_num);
	
	return 0;
}
