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

void print_fileinfo_struct(FileInfoPtr f_info){
    zlog_info(log_cat, "=================== the file info : ===================");
    zlog_info(log_cat, "fileName: %s", f_info->file_name);
    zlog_info(log_cat, "time: %ld", f_info->time);
    zlog_info(log_cat, "upload_flag: %c", f_info->upload_flag);
    zlog_info(log_cat, "src_ip: %s", f_info->src_dev_ip);
    zlog_info(log_cat, "save_path: %s", f_info->save_path);
    zlog_info(log_cat, "=======================================================\n");
}

void print_error_fileinfo_struct(FileInfoPtr f_info){
    zlog_error(log_cat, "=================== the file info : ===================");
    zlog_error(log_cat, "fileName: %s", f_info->file_name);
    zlog_error(log_cat, "time: %ld", f_info->time);
    zlog_error(log_cat, "upload_flag: %c", f_info->upload_flag);
    zlog_error(log_cat, "src_ip: %s", f_info->src_dev_ip);
    zlog_error(log_cat, "save_path: %s", f_info->save_path);
    zlog_error(log_cat, "=======================================================\n");
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
