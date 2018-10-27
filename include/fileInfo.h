#ifndef __FILE_INFO_
#define __FILE_INFO_

#include "common.h"

// file_info struct
typedef struct _fileInfo_t{
    time_t time;
    char *file_name;
    bool upload_flag;
    char *src_dev_ip;
    char *save_path;
    FILE *fp;
}FileInfo;

typedef FileInfo* FileInfoPtr;

/**************************************/

FileInfoPtr file_info_init(const char *fileName, const char *src_ip){
    LOG_FUN;
    // p需要 free
    FileInfoPtr p = (FileInfoPtr)malloc_print_addr(sizeof(FileInfo));
    zlog_info(log_cat, "FileInfoPtr p %p", p);
    if(p == NULL){
		zlog_error(log_cat, "file_info_init malloc error, error msg: %s", strerror(errno));
		return NULL;
	}
    char path_buf[128] = {0};
    getcwd(path_buf, sizeof(path_buf));

    p->time = time(NULL);
    p->upload_flag = false;
    p->save_path = strdup(path_buf);
    p->src_dev_ip = strdup(src_ip);
    p->file_name = strdup(fileName);

    return p;
}

void file_info_destory(FileInfoPtr p){
    // unlink file and close the file dp
    unlink(p->file_name);
    fclose_and_set_null(p->fp);
    free_and_set_null(p->src_dev_ip);
    free_and_set_null(p->save_path);
    free_and_set_null(p->file_name);
    free_and_set_null(p);
}

void set_file_info_time(FileInfoPtr p, time_t t){
    p->time = t;
}

void set_file_info_src_dev_ip(FileInfoPtr p, const char *ip){
    p->src_dev_ip = strdup(ip);
}

void set_file_save_path(FileInfoPtr p, const char *path){
    p->save_path = strdup(path);
}

void set_file_info_upload_flag(FileInfoPtr p, bool b){
    p->upload_flag = b;
}

#endif // !__FILE_INFO_
