#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include "http_session.h"
#include "fileupload.h"
int http_process(int client_sfd){
	char recvbuf[MAXSIZE+1];
	memset(recvbuf, '\0', MAXSIZE+1);//清零
	//获取请求信息
	int r = read(client_sfd, recvbuf, sizeof(recvbuf));
	//printf("客户端请求的头信息:%s", recvbuf);
	//write(1, recvbuf, r);
	//int fd = open("tmp", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	//write(fd, recvbuf, r);
	
	int ret = get_req_mode(recvbuf);
	printf("ret = %d\n", ret);
	switch(ret){
		case 1://get方式
			if(get_http_process(client_sfd, recvbuf))
				return 1;
			else
				return 0;
		case 0://post方式
			printf("running:%d\n", __LINE__);
			if(post_http_process(client_sfd, recvbuf))
				return 1;
			else
				return 0;
		default:
			return 0;
	}
}
//判断请求方式：post  return 0, get return 1;
int get_req_mode(char *recvbuf){
	if(recvbuf!=NULL && strlen(recvbuf)!=0){
		char *p = strchr(recvbuf, ' ');
		char tmp[100]="";
		printf("\n\n\nrunning:%d  %s\n", __LINE__, tmp);
		memcpy(tmp, recvbuf, p-recvbuf);//获取GET或POST
		printf("\n\n\nrunning:%d  %s\n", __LINE__, tmp);
		if(strcmp(tmp, "GET")==0)
			return 1;
		else if(strcmp(tmp, "POST")==0)
			return 0;
	}
	return -1;
}
//获得用户的请求uri:/index.html?name=liuxin
int get_uri(char *recvbuf, char *uribuf){
	char *firstblankp = strchr(recvbuf, ' ');
	if(firstblankp==NULL)
		return 0;
	char *secondblankp = strchr(firstblankp+1, ' ');
	if(secondblankp==NULL)
		return 0;
	memcpy(uribuf, firstblankp+1, secondblankp-(firstblankp+1));
	return 1;
}
//获得用户请求的路径:/index.html
int get_path(char *uribuf, char *pathbuf){
	char *qmark = strchr(uribuf, '?');//查找第一个问号的位置
	if(qmark!=NULL)
		memcpy(pathbuf, uribuf, qmark-uribuf);//获得文件路径：/index.html
	else
		strcpy(pathbuf, uribuf);
}
//post请求处理函数
int post_http_process(int client_sfd, char *recvbuf){
	char *p;
	p = strstr(recvbuf, "multipart/form-data");//简单判断
	if(p!=NULL){//文件上传
		fileupload(client_sfd, recvbuf);
	}
	else{
		get_http_process(client_sfd, recvbuf);
	}
	return 1;
}
//get请求处理函数
int get_http_process(int client_sfd, char *recvbuf){
	char uribuf[MAXSIZE+1];
	char pathbuf[MAXSIZE+1];
	memset(uribuf, '\0', MAXSIZE+1);
	get_uri(recvbuf, uribuf);//  例如：/index.html?name=liuxin
	transcode(uribuf);//转码
	get_path(uribuf, pathbuf);// 例如：/index.html
	printf("客户端请求的路径:%s\n", pathbuf);
	if(strcmp(pathbuf, "/")==0)//默认访问的是index.html页面
		strcpy(pathbuf, "/data/index.html");
	memmove(pathbuf+strlen(DEFAULTFILEPATH), pathbuf, strlen(pathbuf)+1);//拼接工作目录:index.html->./data/index.html
	strncpy(pathbuf, DEFAULTFILEPATH, strlen(DEFAULTFILEPATH));
	printf("访问的文件路径:%s\n", pathbuf);
	int file_status = check_uri_status(pathbuf);//获取文件的状态：存在、可访问、cgi程序
	printf("file_status:%d\n", file_status);
	switch(file_status){
		case FILENOTFIND://404
			send_err_info(client_sfd, FILENOTFIND);
			break;
		case FILEFORBIDDEN://403
			send_err_info(client_sfd, FILEFORBIDDEN);
			break;
		case CGIFILEOK://动态处理
			{
				char cgi_arg[MAXSIZE] = "";//拼装传入cgi的参数
				char *qmark = strchr(uribuf, '?');
				if(qmark)
					strcpy(cgi_arg, qmark+1);
				printf("cgi_arg:%s", cgi_arg);
				serve_dynamic(client_sfd, pathbuf, cgi_arg);
				break;
			}
		case FILEOK://静态文件
			serve_static(client_sfd, uribuf, pathbuf);
			break;
	}
	printf("响应成功\n");
	return 1;
}
void serve_static(int client_sfd, char *uribuf, char *pathbuf){
	int fd;
	fd = open(pathbuf, O_RDONLY);
	if(fd<0)
		goto open_err;
	int fsize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	char filebuf[MAXSIZE+1];
	char sendbuf[MAXSIZE] = "HTTP/1.1 200 ok\r\n"
		"Server: liuxin\r\n";
	int len = strlen(sendbuf);
	sprintf(sendbuf+len,
			"Content-Type: %s; charset=utf-8\r\n"
			"Content-length: %d\r\n\r\n",get_mime_type(uribuf), fsize);
	len = strlen(sendbuf);
	write(client_sfd, sendbuf, len);
	int ret;
	while(1){
		memset(filebuf, '\0', MAXSIZE+1);
		ret = read(fd, filebuf, MAXSIZE);//读入文件内容
		if(ret<=0)
			break;
		write(client_sfd, filebuf, ret);
	}
	return;
open_err:
	perror("open() error:");
	return ;
}
void serve_dynamic(int client_sfd, char *pathbuf, char *cgi_arg){
	int pid = fork();
	if(pid==0){//子进程
		setenv("ARG_STRING", cgi_arg, 1);//设置环境变量
		dup2(client_sfd, STDOUT_FILENO);
		if(execve(pathbuf, NULL, environ)<0)
			printf("执行%s失败\n", pathbuf);
		else
			printf("执行%s成功\n", pathbuf);
	}
	wait(NULL);

}
void send_err_info(int client_sfd, int status){
	char *sendbuf;
	switch(status){
		case FILENOTFIND:
			sendbuf = "HTTP/1.1 404 NOT FIND\r\n"
				"Server: liuxin\r\n"
				"Content-Type: text/html; charset=utf-8\r\n\r\n"
				"很抱歉,您请求的文件飞到外太空去了.";
			break;
		case FILEFORBIDDEN:
			sendbuf = "HTTP/1.1 403 \r\n"
				"Server: liuxin\r\n"
				"Content-Type: text/html; charset=utf-8\r\n\r\n"
				"很抱歉,您的权限不够.";
			break;
	}
	write(client_sfd, sendbuf, strlen(sendbuf));
}
//字符串转码:表单提交的数据会把表达式进行转码,通过这个函数将+-*/转回去
void transcode(char *express){
	char *res;
	while((res=strstr(express, "%28"))!=NULL){
		strcpy(res+1, res+3);
		strncpy(res, "(", 1);
	}
	while((res=strstr(express, "%29"))!=NULL){
		strcpy(res+1, res+3);
		strncpy(res, ")", 1);
	}
	while((res=strstr(express, "%2B"))!=NULL){
		strcpy(res+1, res+3);
		strncpy(res, "+", 1);
	}
	while((res=strstr(express, "%2F"))!=NULL){
		strcpy(res+1, res+3);
		strncpy(res, "/", 1);
	}

}
//查看请求路径是否可用
int check_uri_status(char *pathbuf){
	if((access(pathbuf, F_OK))==-1){
		return FILENOTFIND;//文件不存在
	}
	if(strstr(pathbuf, "cgi-bin")!=NULL){//动态内容
		return CGIFILEOK;
	}
	else{//静态内容
		if((access(pathbuf, R_OK))==-1){
			return FILEFORBIDDEN;//文件禁止访问
		}
	}
	return FILEOK;
}
//判断文件的类型
char *get_mime_type(char *uribuf){
	char mine_type[MAXSIZE];
	bzero(mine_type, MAXSIZE);
	char *dotp;
	if((dotp=strrchr(uribuf, '.'))==NULL)
		return "Unknow type";
	strcpy(mine_type, dotp+1);
	printf("mine_type:%s\n", mine_type);
	if(!strcmp(mine_type, "html") || !strcmp(mine_type, "HTML") || !strcmp(mine_type, "cgi"))
	{   
		return "text/html";
	}   
	else if(!strcmp(mine_type, "jpeg") || !strcmp(mine_type, "JPEG"))
	{   
		return "image/jpeg";
	}   
	else if(!strcmp(mine_type, "htm") || !strcmp(mine_type, "HTM"))
	{   
		return "text/html";
	}   
	else if(!strcmp(mine_type, "css") || !strcmp(mine_type, "CSS"))
	{   
		return "text/css";
	}   
	else if(!strcmp(mine_type, "png") || !strcmp(mine_type, "PNG"))
	{   
		return "image/png";
	}   
	else if(!strcmp(mine_type, "jpg") || !strcmp(mine_type, "JPG"))
	{   
		return "image/jpeg";
	}   
	else if(!strcmp(mine_type, "gif") || !strcmp(mine_type, "GIF"))
	{   
		return "image/gif";
	}   
	else if(!strcmp(mine_type, "txt") || !strcmp(mine_type, "TXT"))
	{   
		return "text/plain";
	}   
	else if(!strcmp(mine_type, "js") || !strcmp(mine_type, "JS"))
	{   
		return "text/javascript";
	}
	else{
		return "Unknow type";
	}
}
