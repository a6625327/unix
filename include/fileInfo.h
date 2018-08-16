#ifndef __FILE_INFO_
#define __FILE_INFO_

#include "common.h"

#define QUEUE_LENGTH 64

// file_info struct
typedef struct _fileInfo_t{
    time_t time;
    bool upload_flag;
    const char* src_dev_ip;
    const char* save_path;
}FileInfo;

typedef FileInfo* FileInfoPtr;

/**************************************/
FileInfo file_info[QUEUE_LENGTH];

FileInfoPtr file_info_init(FileInfoPtr p){
    p->src_dev_ip = NULL;
    p->save_path = NULL;
    p->time = 0;
    p->upload_flag = false;

    return p;
}

void set_file_info_time(FileInfoPtr p, time_t t){
    p->time = t;
}

void set_file_info_src_dev_ip(FileInfoPtr p, const char *ip){
    p->src_dev_ip = ip;
}

void set_file_save_path(FileInfoPtr p, const char *path){
    p->save_path = path;
}

void set_file_info_upload_flag(FileInfoPtr p, bool b){
    p->upload_flag = b;
}

#endif // !__FILE_INFO_
