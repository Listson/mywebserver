#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
void *recv_thread(void *st);
int sfd;//保存socket文件描述符
//UDP 发送端
int main(int argc, char **argv)
{
	//发送和 接收 的 信息
	int n=1;
	sfd = socket(AF_INET,SOCK_DGRAM,0);//创建socket文件描述符 UDP：SOCK_DGRAM
	if(sfd < 0)
		goto err;

	struct sockaddr_in server_info;//保存对方的信息（IP地址 端口号）

	//设置对方的信息：
	//对方的IP地址：	IP版本
	server_info.sin_family = AF_INET; /* address family: AF_INET */
	server_info.sin_port = htons(9999);//对方的端口：htons	转换为网络字节序
	server_info.sin_addr.s_addr = inet_addr("192.168.255.255");//广播地址

	struct sockaddr_in info;
	info.sin_family = AF_INET;//ip版本
	info.sin_port = htons(6666);//端口号
	info.sin_addr.s_addr = inet_addr("0.0.0.0");//本机ip，0.0.0.0表示本机ip

	int bret;
	//将socket文件描述符与本地信息(ip版本，端口号，ip地址)绑定
	bret = bind(sfd, (struct sockaddr *)&info, sizeof(info));
	if(bret)
		goto bind_err;

	//创建一个新线程//用于接受客户端发送的响应
	pthread_t tid;
	pthread_create(&tid, NULL, recv_thread, NULL);

	int val = 1;
	setsockopt(sfd,SOL_SOCKET,SO_BROADCAST,&val,sizeof(val));

	//发送广播信号
	sendto(sfd, &n, sizeof(n), 0, (void*)&server_info, sizeof(server_info));
	//sendto(sfd,buf,sizeof(buf),0,(void*)&server_info,sizeof(server_info));
	//printf("running:%d\n", __LINE__);
	//sleep(10);//主线程先睡眠一会,等待客户端的回应
	pthread_join(tid, NULL);
	close(sfd);

	return 0;
err:
	perror("");
	return -1;
bind_err:
	perror("");
	exit(1);
}
//接收线程执行函数
void *recv_thread(void *st){
	int n;
	//保存发送端的信息(ip版本，端口号，ip地址)
	struct sockaddr_in client_info;
	socklen_t client_info_len = sizeof(client_info);
	while(1){
		//接收发送端发送的数据
		recvfrom(sfd, &n, sizeof(n), 0, (struct sockaddr *)&client_info, &client_info_len);
		printf("%s: %d\n",	inet_ntoa(client_info.sin_addr), n);
		printf("running:%d\n", __LINE__);
	}
}
