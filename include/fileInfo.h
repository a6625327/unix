#ifndef __FILE_INFO_
#define __FILE_INFO_

#include "common.h"

/*****************************************************************************
    结构体描述：文件信息结构体，储存文件信息
    time: 记录该文件创建时间
    file_name: 文件名
    upload_flag: 上传标志位（保留）
    src_dev_ip: 记录该文件内容从哪个ip过来
    save_path: 记录该文件储存路径
    fp: 储存该文件的文件指针
*****************************************************************************/
typedef struct _fileInfo_t{
    time_t time;
    char *file_name;
    bool upload_flag;
    char *src_dev_ip;
    char *save_path;
    FILE *fp;
}FileInfo;

typedef FileInfo* FileInfoPtr;


/*****************************************************************************
 函数描述  :  文件信息结构体初始化函数
 输入参数  :  
             fileName: const char *，文件名
             src_ip: const char *，该文件内容来源ip
 返回值    :  
             文件信息结构体 FileInfoPtr
*****************************************************************************/
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

/*****************************************************************************
 函数描述  :  在日志系统中使用info标签输出接收到的文件信息，信息包括：
              文件名、文件接收时间、上传标志（保留未使用）、来源IP、储存路径
 输入参数  :  
             f_info: FileInfoPtr，文件信息结构体
 返回值    :  无 
*****************************************************************************/
void print_fileinfo_struct(FileInfoPtr f_info){
    zlog_info(log_cat, "=================== the file info : ===================");
    zlog_info(log_cat, "fileName: %s", f_info->file_name);
    zlog_info(log_cat, "time: %ld", f_info->time);
    zlog_info(log_cat, "upload_flag: %c", f_info->upload_flag);
    zlog_info(log_cat, "src_ip: %s", f_info->src_dev_ip);
    zlog_info(log_cat, "save_path: %s", f_info->save_path);
    zlog_info(log_cat, "=======================================================\n");
}

/*****************************************************************************
 函数描述  :  在日志系统中使用errpr标签输出接收到的文件信息，信息包括：
              文件名、文件接收时间、上传标志（保留未使用）、来源IP、储存路径
 输入参数  :  
             f_info: FileInfoPtr，文件信息结构体
 返回值    :  无 
*****************************************************************************/
void print_error_fileinfo_struct(FileInfoPtr f_info){
    zlog_error(log_cat, "=================== the file info : ===================");
    zlog_error(log_cat, "fileName: %s", f_info->file_name);
    zlog_error(log_cat, "time: %ld", f_info->time);
    zlog_error(log_cat, "upload_flag: %c", f_info->upload_flag);
    zlog_error(log_cat, "src_ip: %s", f_info->src_dev_ip);
    zlog_error(log_cat, "save_path: %s", f_info->save_path);
    zlog_error(log_cat, "=======================================================\n");
}

/*****************************************************************************
 函数描述  :  释放文件信息结构体中所有资源
 输入参数  :  
             p: FileInfoPtr，文件信息结构体
 返回值    :  无 
*****************************************************************************/
void file_info_destory(FileInfoPtr p){
    // unlink file and close the file dp
    unlink(p->file_name);
    fclose_and_set_null(p->fp);
    free_and_set_null(p->src_dev_ip);
    free_and_set_null(p->save_path);
    free_and_set_null(p->file_name);
    free_and_set_null(p);
}

/*****************************************************************************
 函数描述  :  释放文件信息结构体中所有资源
 输入参数  :  
             p: FileInfoPtr，文件信息结构体
 返回值    :  无 
*****************************************************************************/
void set_file_info_time(FileInfoPtr p, time_t t){
    p->time = t;
}

#endif // !__FILE_INFO_
