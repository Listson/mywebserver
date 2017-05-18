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
#define DEFAULTPATH "/root/Desktop/my_webserver3"
void response(char *request, char *filepath);
char *dir_up(char *dirpath);
char *getcurrentIp();
int main(int argc, char **argv)
{
	char *buf, *p;
	char filepath[MAXBUF+1];
	char buff[MAXBUF+1];
	bzero(filepath, MAXBUF+1);
	if((buf=getenv("ARG_STRING"))!=NULL){
		if((p = strchr(buf, '='))!=NULL)
			strcpy(filepath, p+1);//filepath=/root
		else
			strcpy(filepath, buf);// /root
	}
	//strcpy(filepath, "../../images");
	//printf("filepath:%s\n", filepath);
	sprintf(buff, "%s%s/%s", DEFAULTPATH, DEFAULTROOT, filepath);
	//printf("buff:%s\n", buff);
	response(buff, filepath);
}
//响应用户的请求函数
void response(char *request, char *filepath){
		if(filepath!=NULL && strlen(filepath)>0){
			DIR *dir;
			struct dirent *dirent;
			dir = opendir(request);
			if(dir == NULL)
				goto err;
			printf("HTTP/1.1 200 OK\r\n");
			printf("Server: liuxin\r\n");
			printf("Content-Type: text/html; charset=utf-8\r\n");
			printf("Connection: close\r\n\r\n");
			printf("<!DOCTYPE html><html><head><meta charset=\"UTF-8\">");
			printf("<title>图片查看</title>");
			//printf("<link rel=\"stylesheet\" href=\"zjy_1.css\" type=\"text/css\"/>");
			printf("</head>");
			printf("<body background=\"%s/images/bg4.png\">", DEFAULTROOT);
			printf("<div id=\"top\">");
			printf("<dir id=\"Layer0\" style=\"text-align:right;\"><a href=\"%s/index.html\"><img src=\"%s/images/back.jpg\" width=\"100\" height=\"70\"></img></a></dir>", DEFAULTROOT, DEFAULTROOT);
			printf("<center>");
			printf("<form action=\"%s/cgi-bin/picture/picture\" method=\"get\">", DEFAULTROOT);
				printf("输入图片路径：<input type=\"text\" value=\"%s\", name=\"filepath\" />", filepath);
				printf("<input type=\"submit\" value=\"查看图片\">");
			printf("</form>");
			printf("</center>");
			printf("</div>");
			printf("<hr/>");
			printf("<center><div id=\"picture\"><ul>");
			fflush(NULL);
			int count = 0;
			printf("<ul>");
			char frealpath[MAXBUF+1];
			bzero(frealpath, MAXBUF+1);
			while((dirent = readdir(dir))!=NULL){
				if(strstr(dirent->d_name, ".png") || strstr(dirent->d_name, ".jpeg") || strstr(dirent->d_name, ".gif") || strstr(dirent->d_name, ".jpg")){
				sprintf(frealpath, "%s/%s", strstr(request, DEFAULTROOT), dirent->d_name);	
				printf("<a href=\"%s\"><img src=\"%s\" width=\"150px\" height=\"150px\"></a>", frealpath, frealpath);
				}
			}
			printf("</ul>");
			printf("</ul></div></center></body></html>");
		}
		else{
			printf("HTTP/1.1 200 OK\r\n");
			printf("Server: liuxin\r\n");
			printf("Content-Type: text/html; charset=utf-8\r\n");
			printf("Connection: close\r\n\r\n");
			printf("<!DOCTYPE html><html><head><meta charset=\"UTF-8\">");
			printf("<title>图片查看</title>");
			//printf("<link rel=\"stylesheet\" href=\"zjy_1.css\" type=\"text/css\"/>");
			printf("</head>");
			printf("<body background=\"%s/images/bg4.png\">", DEFAULTROOT);
			printf("<div id=\"top\">");
			printf("<dir id=\"Layer0\" style=\"text-align:right;\"><a href=\"%s/index.html\"><img src=\"%s/images/back.jpg\" width=\"100\" height=\"70\"></img></a></dir>", DEFAULTROOT, DEFAULTROOT);
			printf("<center>");
			printf("<form action=\"%s/cgi-bin/picture/picture\" method=\"get\">", DEFAULTROOT);
				printf("输入图片路径：<input type=\"text\" value=\"%s\", name=\"filepath\" />", filepath);
				printf("<input type=\"submit\" value=\"查看图片\">");
			printf("</form>");
			printf("</center>");
			printf("</div>");
			printf("<hr/>");
			printf("</body></html>");
		}
	return;
err:
	perror("");
	return;
}
