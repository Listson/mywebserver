#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define DEFAULTROOT "/web-app/mysite"
//UDP 发送端
int main(int argc, char **argv)
{
	//发送和 接收 的 信息
	char buf[1024]="";
	char ip[1024]="";
	char *p, *q, *qp;
	if((p=getenv("ARG_STRING"))!=NULL && strlen(p)>0){//order=halt?ip=192.168.14.146
		if((q=strchr(p, '='))!=NULL){
			if((qp=strchr(q, '?'))!=NULL)
				*qp='\0';//将问号替换
			strcpy(buf, q+1);
		}
		p = qp+1;
		if((q=strchr(p, '='))!=NULL)
			strcpy(ip, q+1);
	}
	else
		buf[0] = '1';
	int sfd;//保存socket文件描述符
	sfd = socket(AF_INET,SOCK_DGRAM,0);//创建socket文件描述符 UDP：SOCK_DGRAM
	if(sfd < 0)
		goto err;

	struct sockaddr_in server_info;//保存对方的信息（IP地址 端口号）

	//设置对方的信息：
	//对方的IP地址：	IP版本
	server_info.sin_family = AF_INET; /* address family: AF_INET */
	server_info.sin_port = htons(9999);//对方的端口：htons	转换为网络字节序
	if(strlen(ip)>0)
		server_info.sin_addr.s_addr = inet_addr(ip);//广播地址
	else
		server_info.sin_addr.s_addr = inet_addr("192.168.255.255");//广播地址
		//server_info.sin_addr.s_addr = inet_addr("192.168.15.255");//广播地址
	//192.168.255.255

	struct sockaddr_in info;
	info.sin_family = AF_INET;//ip版本
	info.sin_port = htons(6666);//端口号
	info.sin_addr.s_addr = inet_addr("0.0.0.0");//本机ip，0.0.0.0表示本机ip

	int bret;
	//将socket文件描述符与本地信息(ip版本，端口号，ip地址)绑定
	bret = bind(sfd, (struct sockaddr *)&info, sizeof(info));
	if(bret)
		goto bind_err;

	int val = 1;
	setsockopt(sfd,SOL_SOCKET,SO_BROADCAST,&val,sizeof(val));


	printf("HTTP/1.1 200 ok\r\n");
	printf("Server: liuxin\r\n");
	printf("Content-Type: text/html; charset=utf-8\r\n\r\n");

	printf("<head><head><title>远程命令</title>");
	printf("<style type=\"text/css\">#Layer1{position:absolute;width:800px;height:600px;left:300px;top:50px;z-index:1;}</style>");
	printf("</head><body background=\"%s/images/bg1.jpg\">", DEFAULTROOT);
	printf("<dir id=\"Layer0\" style=\"text-align:right;\"><a href=\"%s/cgi-bin/remote-control/control\"><img src=\"%s/images/refresh.jpg\" width=\"100\" height=\"70\"></img>&nbsp;&nbsp;<a href=\"%s/index.html\"><img src=\"%s/images/back.jpg\" width=\"100\" height=\"70\"></img></a></dir>", DEFAULTROOT, DEFAULTROOT, DEFAULTROOT, DEFAULTROOT);
	printf("<hr/>");
	printf("<dir>");
	fflush(NULL);


	int pid = fork();
	int client_sfd = dup(STDOUT_FILENO);
	if(pid==0){//子进程
		//保存发送端的信息(ip版本，端口号，ip地址)
		struct sockaddr_in client_info;
		socklen_t client_info_len = sizeof(client_info);
		while(1){
			//接收发送端发送的数据
			recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&client_info, &client_info_len);
			sprintf(buf, "<div style=\"float:left;margin:10 20 10 20\" width=\"200\" height=\"150\"><span>IP: %s</span><br/><br/><img src=\"%s/images/pc.jpg\" width=\"120px\", height=\"100px\"><br/>", inet_ntoa(client_info.sin_addr), DEFAULTROOT);
			write(client_sfd, buf, strlen(buf));
			sprintf(buf, "<br/>&nbsp;&nbsp;&nbsp;&nbsp;<span><a href=\"%s/cgi-bin/remote-control/control?order=halt?ip=%s\">关机</a></span>&nbsp;&nbsp;&nbsp;&nbsp;<span><a href=\"%s/cgi-bin/remote-control/control?order=reboot?ip=%s\">重启</a></span></div>", DEFAULTROOT, inet_ntoa(client_info.sin_addr), DEFAULTROOT, inet_ntoa(client_info.sin_addr));
			write(client_sfd, buf, strlen(buf));
		}
		return 0;//子进程结束
	}
	//发送广播信号
	sendto(sfd, buf, sizeof(buf), 0, (void*)&server_info, sizeof(server_info));
	sleep(2);//父进程睡一会儿
	kill(pid, SIGINT);//杀死子进程
	printf("</div></body></html>");
	close(sfd);
	return 0;
err:
	perror("");
	exit(1);
bind_err:
	perror("");
	exit(1);
}
