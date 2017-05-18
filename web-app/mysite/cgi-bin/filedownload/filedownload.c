#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#ifndef MAXBUF
#define MAXBUF 1024
#endif
#define ETH_NAME "eth0"  
#define DEFAULTROOT "/web-app/mysite"
void response(char *req);
char *dir_up(char *dirpath);
char *getcurrentIp();
int main(int argc, char **argv)
{
	char *buf, *p;
	char filepath[MAXBUF+1];
	bzero(filepath, MAXBUF+1);
	if((buf=getenv("ARG_STRING"))!=NULL){
		if((p = strchr(buf, '='))!=NULL)
			strcpy(filepath, p+1);//name=liuxin
		else
			strcpy(filepath, buf);//liuxin
	}
	response(filepath);
}
//响应用户的请求函数
void response(char *request){
	//获取文件的属性:用于判断当前请求的是文件还是文件夹
	struct stat fattr;
	int ret = stat(request, &fattr);
	if(ret){
		printf("HTTP/1.1 200 OK\r\n");
		printf("Server: liuxin\r\n");
		printf("Content-Type: text/html; charset=utf-8\r\n");
		printf("Connection: close\r\n\r\n");
		fflush(NULL);
		printf("<html>");
		printf("<head>");
		printf("<title>%d-%s</title>", errno, strerror(errno));
		printf("</head>");
		printf("<body>");
		printf("<dir id=\"Layer0\" style=\"text-align:right;\"><a href=\"%s/index.html\"><img src=\"%s/images/back.jpg\" width=\"100\" height=\"70\"></img></a></dir>", DEFAULTROOT, DEFAULTROOT);
		printf("<div id=\"Layer1\" style=\"text-align:center;\">");
		printf("<font size=3>Linux文件服务器</font>");
		printf("<br>");
		printf("<hr width=\"100%%\">");
		printf("<br>");
		printf("<font color=\"CC0000\" size=+2>当你看到这条信息,伟大的程序员哥哥又要加班了:\n%s %s</font>", request, strerror(errno));
		printf("</body>");
		printf("</html>");
		return;
	}
	//如果请求的是一个普通的文件:传输文件
	if(S_ISREG(fattr.st_mode)){
		int fd = open(request, O_RDONLY);//打开文件
		if(fd<0)
			goto err;
		int fsize = lseek(fd, 0, SEEK_END);//计算文件大小
		//char *fcontent = (char*)malloc(sizeof(char)*fsize+1);
		char fcontent[MAXBUF+1];
		bzero(fcontent, MAXBUF+1);//清零
		lseek(fd, 0, SEEK_SET);
		//printf("HTTP/1.1 200 OK\r\nServer: liuxin\r\nConnection: keep-alive\r\nContent-type: application/*\r\nContent-Length:%d\r\n\r\n",fsize);
		printf("HTTP/1.1 200 OK\r\nServer: liuxin\r\nConnection: keep-alive\r\n");
		printf("Content-Type: application/octet-stream\r\n");
		printf("Content-Disposition: filename=%s\r\n", strrchr(request, '/')+1);
		printf("Content-Length:%d\r\n\r\n",fsize);
		//printf("Content-Disposition: attachment\r\n\r\n");
		fflush(NULL);//头信息必须先发送到服务器端，不然服务器不知道收到的是什么类型的文件
		int ret;
		while(1){
			ret = read(fd, fcontent, fsize);//读取文件内容
			if(ret<=0)
				break;
			write(1, fcontent, ret);
		}
		close(fd);//关闭文件指针
	}
	//如果请求的是一个文件夹:传输文件夹的文件表项
	else if(S_ISDIR(fattr.st_mode)){
		DIR *dir;
		struct dirent *dirent;
		dir = opendir(request);
		if(dir == NULL)
			goto err;
				printf("HTTP/1.1 200 OK\r\n");
				printf("Server: liuxin\r\n");
				printf("Content-Type: text/html; charset=utf-8\r\n");
				printf("Connection: close\r\n\r\n");
				printf("<html>");
				printf("<head>");
				printf("<title>%s</title>", request);
				printf("</head>");
				printf("<body background=\"%s/images/bg1.jpg\">", DEFAULTROOT);
				printf("<dir id=\"Layer0\" style=\"text-align:right;\"><a href=\"%s/index.html\"><img src=\"%s/images/back.jpg\" width=\"100\" height=\"70\"></img></a></dir>", DEFAULTROOT, DEFAULTROOT);
	printf("<div id=\"Layer1\" style=\"text-align:center;\">");
				printf("<font size=3>Linux文件服务器</font>");
				printf("<br>");
				printf("<hr width=\"100%%\">");
				printf("<br>");
				printf("<table border cols=3 width=\"100%%\"><center>");
				printf("<caption><font size=+3>目录%s</font></caption>", request);
				printf("<tr><td>名称</td><td>文件类型</td><td>修改时间</td></tr>");
				fflush(NULL);
		while((dirent = readdir(dir))!=NULL){
			char realFileName[MAXBUF+1];//保存文件的绝对路径
			if(strcmp(request, "/")==0)
				sprintf(realFileName, "%s%s", request, dirent->d_name);
			else{
				if(request[strlen(request)-1]=='/')
					request[strlen(request)-1] = '\0';
				sprintf(realFileName, "%s/%s", request, dirent->d_name);
				//puts(realFileName);
			}
			//printf("%s,%s\n", dirent->d_name, realFileName);
			struct stat childfattr;//子文件(目录里的目录项)
			bzero(&childfattr, sizeof(childfattr));
			if (stat(realFileName, &childfattr)==0) {
				if(strcmp(dirent->d_name, "..")==0)
					//printf("<tr><td><a href=\"http://%s:%d%s\">%s</a></td>", getcurrentIp(), 80, dir_up(request), dirent->d_name);
					printf("<tr><td><a href=\"/web-app/mysite/cgi-bin/filedownload/filedownload?filepath=%s\">%s</a></td>", dir_up(request), dirent->d_name);
				else
					//printf("<tr><td><a href=\"http://%s:%d%s\">%s</a></td>", getcurrentIp(), 80, realFileName, dirent->d_name);
					printf("<tr><td><a href=\"/web-app/mysite/cgi-bin/filedownload/filedownload?filepath=%s\">%s</a></td>", realFileName, dirent->d_name);
				if (S_ISDIR(childfattr.st_mode))
					printf("<td>目录</td>");
				else if (S_ISREG(childfattr.st_mode))
					printf("<td>%d</td>", childfattr.st_size);
				else if (S_ISLNK(childfattr.st_mode))
					printf("<td>链接</td>");
				else if (S_ISCHR(childfattr.st_mode))
					printf("<td>字符设备</td>");
				else if (S_ISBLK(childfattr.st_mode))
					printf("<td>块设备</td>");
				else if (S_ISFIFO(childfattr.st_mode))
					printf("<td>FIFO</td>");
				else if (S_ISSOCK(childfattr.st_mode))
					printf("<td>Socket</td>");
				else
					printf("<td>(未知)</td>");
				printf("<td>%s</td></tr>", ctime(&childfattr.st_ctime));
			}
		}
		printf("</table></center></body></html>");
	}
	//即不是常规文件,又不是文件夹:禁止访问
	else{
				printf("HTTP/1.1 200 OK\r\n");
				printf("Server: liuxin\r\n");
				printf("Content-Type: text/html; charset=utf-8\r\n");
				printf("Connection: close\r\n\r\n");
				printf("<html>");
				printf("<head>");
				printf("<title>Access Forbidden</title>");
				printf("</head>");
				printf("<body>");
				//printf("<body background=\"%s/images/bg1.jpg\">", DEFAULTROOT);
				printf("<dir id=\"Layer0\" style=\"text-align:right;\"><a href=\"%s/index.html\"><img src=\"%s/images/back.jpg\" width=\"100\" height=\"70\"></img></a></dir>", DEFAULTROOT, DEFAULTROOT);
	printf("<div id=\"Layer1\" style=\"text-align:center;\">");
				printf("<font size=3>Linux文件服务器</font>");
				printf("<br>");
				printf("<hr width=\"100%%\">");
				printf("<br>");
				printf("<font color=\"CC0000\" size=+2>很抱歉,资源:%s禁止访问.</font>");
				printf("</body>");
				printf("</html>", request);
	}
out:
	return;
err:
	perror("");
	return ;
}
char *dir_up(char *dirpath){
	static char path[MAXBUF];
	bzero(path, sizeof(path));
	int len;
	strcpy(path, dirpath);
	len = strlen(path);
	if (len > 1 && path[len - 1] == '/')
		len--;
	while (path[len - 1] != '/' && len > 1)
		len--;
	path[len] = '\0';
	//puts(dirpath);
	//puts(path);
	return path;
}
//获取当前主机的IP地址
char *getcurrentIp(){
	static char curIp[100];
	int sock;  
	struct sockaddr_in sin;  
	struct ifreq ifr;  

	sock = socket(AF_INET, SOCK_DGRAM, 0);  
	if (sock == -1)  
	{  
		perror("socket");  
		return NULL;                  
	}  

	strncpy(ifr.ifr_name, ETH_NAME, IFNAMSIZ);  
	ifr.ifr_name[IFNAMSIZ - 1] = 0;  

	if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)  
	{  
		perror("ioctl");  
		return NULL;  
	}  
	memcpy(&sin, &ifr.ifr_addr, sizeof(sin));  
	bzero(curIp, sizeof(curIp));
	strcpy(curIp, inet_ntoa(sin.sin_addr));
	return curIp;
}
