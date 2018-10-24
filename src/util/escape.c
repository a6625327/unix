#include "escape.h"

/*
	----帧内容转义----
*/
int8_t escaper(void *data, const size_t escap_num, void **ret_data, size_t *ret_data_num){   //用完需要free返回值
	LOG_FUN;
	// tem_buf需要free
	uint8_t *tem_buf = malloc(sizeof(uint8_t) * escap_num * 2); 
	if(tem_buf == NULL){
		zlog_error(log_cat, "tem_buf malloc error, error msg: %s", strerror(errno));
		return -2;
	}

	uint16_t  escape_str_cnt = 0;         // 转义的次数
	uint16_t  data_buf_position, tem_buf_position;   // 分别代表databuf的下标和tem_buf的下标
	uint8_t  *databuf = (uint8_t *)data;

	tem_buf_position = 0;
	data_buf_position = 0;
	for(; data_buf_position < *ret_data_num; data_buf_position++) {
		if(databuf[data_buf_position] == 0xF0) {
			tem_buf[tem_buf_position] = 0xF0;
			tem_buf_position++;
			tem_buf[tem_buf_position] = 0x02;
			tem_buf_position++;
			escape_str_cnt++;
			continue;
		}
		if(databuf[data_buf_position] == 0xF1) {
			tem_buf[tem_buf_position] = 0xF0;
			tem_buf_position++;
			tem_buf[tem_buf_position] = 0x01;
			tem_buf_position++;
			escape_str_cnt++;
			continue;
		}
		if(databuf[data_buf_position] == 0xF2) {
			tem_buf[tem_buf_position] = 0xF0;
			tem_buf_position++;
			tem_buf[tem_buf_position] = 0x00;
			tem_buf_position++;
			escape_str_cnt++;
			continue;
		}
		tem_buf[tem_buf_position] = databuf[data_buf_position];
		tem_buf_position++;
	}

    zlog_info(log_cat, "escap cnt: %d", escap_num);

	*ret_data = databuf;
	*ret_data_num += escape_str_cnt;

	return 0;
}

/*
	----将被转义的字符转换回来----
*/
int8_t unescaper(void *data, const size_t unescap_num, void **ret_data, size_t *ret_data_num){	
	LOG_FUN;
	// tem_buf需要free
	uint8_t *tem_buf = malloc(sizeof(uint8_t) * unescap_num); 

	uint16_t  escape_str_cnt = 0;         // 转义的次数
	uint16_t  data_buf_position, tem_buf_position;   // 分别代表databuf的下标和tem_buf的下标
	uint8_t  *databuf = (uint8_t *)data;
	if(tem_buf == NULL){
		zlog_error(log_cat, "tem_buf malloc error, error msg: %s", strerror(errno));
		return -2;
	}

	tem_buf_position = 0;
	data_buf_position = 0;
	for(; data_buf_position < unescap_num; data_buf_position++) {
		if(databuf[data_buf_position] == 0xF0) {
			if(databuf[data_buf_position + 1] == 0x00) {
				tem_buf[tem_buf_position] = 0xF2;
			}
			escape_str_cnt++;
			data_buf_position += 2;
			tem_buf_position++;
			continue;
		}
		if(databuf[data_buf_position] == 0xF0) {
			if(databuf[data_buf_position+1] == 0x01) {
				tem_buf[tem_buf_position] = 0xF1;
			}
			escape_str_cnt++;
			data_buf_position += 2;
			tem_buf_position++;
			continue;
		}
		if(databuf[data_buf_position] == 0xF0) {
			if(databuf[data_buf_position+1] == 0x02) {
				tem_buf[tem_buf_position] = 0xF0;
			}
			escape_str_cnt++;
			data_buf_position += 2;
			tem_buf_position++;
			continue;
		}
		tem_buf[tem_buf_position] = databuf[data_buf_position];
		tem_buf_position++;
	}

	*ret_data = databuf;
	*ret_data_num = unescap_num - escape_str_cnt;

    zlog_info(log_cat, "unescap cnt: %d", unescap_num);
	hzlog_info(log_cat, tem_buf, *ret_data_num);
	
	return 0;
}
