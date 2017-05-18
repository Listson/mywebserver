#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
void *recv_thread(void *st);
int sfd;//保存socket文件描述符
//UDP 发送端
int main(int argc, char **argv)
{
	//发送和 接收 的 信息
	char buf[1024]="";
	sfd = socket(AF_INET,SOCK_DGRAM,0);//创建socket文件描述符 UDP：SOCK_DGRAM
	if(sfd < 0)
		goto err;
	//本机信息
	struct sockaddr_in info;
	info.sin_family = AF_INET;//ip版本
	info.sin_port = htons(9999);//端口号
	info.sin_addr.s_addr = inet_addr("0.0.0.0");//本机ip，0.0.0.0表示本机ip

	int bret;
	//将socket文件描述符与本地信息(ip版本，端口号，ip地址)绑定
	bret = bind(sfd, (struct sockaddr *)&info, sizeof(info));
	if(bret)
		goto bind_err;
	//支持接受广播
	int val = 1;
	setsockopt(sfd,SOL_SOCKET,SO_BROADCAST,&val,sizeof(val));

	//保存发送端的信息(ip版本，端口号，ip地址)
	struct sockaddr_in client_info;
	socklen_t client_info_len = sizeof(client_info);
	while(1){
		memset(&client_info, '\0', sizeof(client_info));
		memset(buf, '\0', sizeof(buf));
		recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&client_info, &client_info_len);//接受广播
		printf("%s: %s\n",	inet_ntoa(client_info.sin_addr), buf);
		if(strcmp(buf, "reboot")==0 || strcmp(buf, "halt")==0){
			system(buf);
		}
		sendto(sfd, buf, sizeof(buf), 0, (void*)&client_info, sizeof(client_info));//响应广播
	}
	close(sfd);
	return 0;
err:
	perror("");
	return -1;
bind_err:
	perror("");
	exit(1);
}
