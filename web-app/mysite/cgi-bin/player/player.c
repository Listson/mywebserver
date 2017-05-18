#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
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
			printf("<title>播放器</title>");
			//printf("<link rel=\"stylesheet\" href=\"zjy_1.css\" type=\"text/css\"/>");
			
			printf("<script language=\"javascript\" type=\"text/javascript\">");
				printf("function play(dom){");
					printf("var myAuto = document.getElementById(\"myaudio\");");
					printf("var src = dom.getAttribute(\"name\");");
					//printf("alert(src);");
					printf("myAuto.src=src;");
				printf("}");
			printf("</script>");
			printf("</head>");
			printf("<body background=\"%s/images/bg2.jpg\">", DEFAULTROOT);
			printf("<div id=\"top\">");
			printf("<dir id=\"Layer0\" style=\"text-align:right;\"><a href=\"%s/index.html\"><img src=\"%s/images/back.jpg\" width=\"100\" height=\"70\"></img></a></dir>", DEFAULTROOT, DEFAULTROOT);
			printf("<center>");
			printf("<form action=\"%s/cgi-bin/player/player\" method=\"get\">", DEFAULTROOT);
				printf("输入音乐文件夹路径：<input type=\"text\" value=\"%s\", name=\"filepath\" />", filepath);
				printf("<input type=\"submit\" value=\"打开文件夹\">");
			printf("</form>");
			printf("</center>");
			printf("</div>");
			printf("<hr/>");
			printf("<center>");
			printf("<div style=\"padding-top:100px;\">");
			printf("<div id=\"list\" style=\"float:left; padding-left:200px;\">");
			fflush(NULL);
			char frealpath[MAXBUF+1];
			bzero(frealpath, MAXBUF+1);
			while((dirent = readdir(dir))!=NULL){
				if(strstr(dirent->d_name, ".mp3")){
					sprintf(frealpath, "%s/%s", strstr(request, DEFAULTROOT), dirent->d_name);
					printf("<span>%s</span>&nbsp;<img width=\"25px\" height=\"25px\" name=\"%s\" onclick=\"play(this)\" src=\"%s/images/play.png\"><br/>", dirent->d_name, frealpath, DEFAULTROOT);
					//printf("<span>%s</span><input name=\"%s\" type=\"button\" onclick=\"play(this)\" value=\"点击播放\"><br/>", dirent->d_name, frealpath);
				}
			}
			printf("</div>");
			printf("<div style=\"\">");
			printf("<embed id=\"myaudio\" src=\"\" loop=false autostart=true hidden=no width=500 height=100 type=\"audio/mpeg\">");
			printf("</div>");
			printf("</div>");
			printf("</center>");

			printf("</body></html>");
		}
		else{
			printf("HTTP/1.1 200 OK\r\n");
			printf("Server: liuxin\r\n");
			printf("Content-Type: text/html; charset=utf-8\r\n");
			printf("Connection: close\r\n\r\n");
			printf("<!DOCTYPE html><html><head><meta charset=\"UTF-8\">");
			printf("<title>音乐播放器</title>");
			printf("<link rel=\"stylesheet\" href=\"zjy_1.css\" type=\"text/css\"/>");
			printf("</head>");
			printf("<body background=\"%s/images/bg2.jpg\">", DEFAULTROOT);
			printf("<div id=\"top\">");
			printf("<dir id=\"Layer0\" style=\"text-align:right;\"><a href=\"%s/index.html\"><img src=\"%s/images/back.jpg\" width=\"100\" height=\"70\"></img></a></dir>", DEFAULTROOT, DEFAULTROOT);
			printf("<center>");
			printf("<form action=\"%s/cgi-bin/player/player\" method=\"get\">", DEFAULTROOT);
				printf("输入音乐文件夹路径：<input type=\"text\" value=\"%s\", name=\"filepath\" />", filepath);
				printf("<input type=\"submit\" value=\"打开文件夹\">");
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
