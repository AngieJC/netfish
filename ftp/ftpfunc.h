#include "../global.h"

void * ftp_control_std_local(void * clnt_sock);
void * ftp_control_std_remote(void * clnt_sock);


void * ftp_data_std_local(void * clnt_sock);
void * ftp_file_std_local(void * clnt_sock);  // 用于上传文件
void * ftp_file_std_remote(void * clnt_sock);  // 用于下载文件