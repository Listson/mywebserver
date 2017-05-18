#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include "http_session.h"
void *req_process_pthread(void *st);
int main(int argc, char **argv)
{
	int server_sfd;
	int ret;
	//忽略 SIGPIPE 信号	
	signal(SIGPIPE,SIG_IGN);
	//创建tcp文件描述符:用于接受请求
    server_sfd = socket(PF_INET, SOCK_STREAM, 0);
	if(server_sfd<0)
		goto socket_err;
	//端口快速重用
	 int val = 1;
	 ret = setsockopt(server_sfd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));
	 if(ret)
		 goto setsockopt_err;
	//服务器信息
	struct sockaddr_in server_info;
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(80);//服务器端口
	server_info.sin_addr.s_addr = inet_addr("0.0.0.0");
	//绑定服务器信息
	ret = bind(server_sfd, (struct sockaddr *)&server_info, sizeof(server_info));
	if(ret)
		goto bind_err;
	//监听端口
	ret = listen(server_sfd, 10);
	if(ret)
		goto listen_err;
	while(1){
		int client_sfd;//用于服务器与客户端收发数据
		struct sockaddr_in client_info;//保存请求的客户端信息
		socklen_t client_info_len = sizeof(client_info);
		//建立连接
		ret = client_sfd = accept(server_sfd, (struct sockaddr *)&client_info, &client_info_len);
		if(ret<0)
			goto accept_err;
		//创建线程
		pthread_t tid;
		//pthread_attr_t attr;//线程属性
		//pthread_attr_init(&attr);//初始化线程属性
		//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//设置线程属性detached:自动清理内存
		ret = pthread_create(&tid, 0, req_process_pthread, (void*)client_sfd);
		if(ret)
			goto pthread_create_err;
		printf("客户机:%s通过端口:%d访问本服务器,处理的线程id:%x\n", inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port), tid);
	}
	close(server_sfd);
	return 0;
//一下代码用于错误处理
socket_err:
	perror("socket() error:");
	return -1;
bind_err:
	perror("bind() err:");
	return -1;
setsockopt_err:
	perror("setsockopt() error:");
	return -1;
listen_err:
	perror("listen() error:");
	return -1;
accept_err:
	perror("accept() error:");
	return -1;
pthread_create_err:
	perror("pthread_create() error:");
	return -1;
}
void *req_process_pthread(void *st){
	//pthread_detach(pthread_self());
	int client_sfd = (int)st;
	if(http_process(client_sfd))
		printf("服务器响应成功\n");
	else
		printf("服务器响应失败\n");
	//断开连接
	close(client_sfd);
	pthread_exit(NULL);
}
