#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define DEFAULTROOT "/web-app/mysite"
#define LEN 100
#define MAXBUF 1024
int main(int argc, char **argv)
{
	char *buf,*p;
	char order[MAXBUF]="";
	if((buf=getenv("ARG_STRING"))!=NULL){
		if((p = strchr(buf, '='))!=NULL)
			strcpy(order, p+1);
		else
			strcpy(order, buf);
	}
	while((p=strchr(order, '+'))!=NULL){
		*p = ' ';
	}

	printf("HTTP/1.1 200 ok\r\n");
	printf("Server: liuxin\r\n");
	printf("Content-Type: text/html; charset=utf-8\r\n\r\n");

	printf("<head><head><title>远程命令</title>");
	printf("<style type=\"text/css\">#Layer1{position:absolute;width:800px;height:600px;left:300px;top:50px;z-index:1;}</style>");
	printf("</head><body background=\"%s/images/bg1.jpg\">", DEFAULTROOT);
	printf("<dir id=\"Layer0\" style=\"text-align:right;\"><a href=\"%s/index.html\"><img src=\"%s/images/back.jpg\" width=\"100\" height=\"70\"></img></a></dir>", DEFAULTROOT, DEFAULTROOT);
	printf("<div id=\"Layer1\" style=\"text-align:center;\"><br/>");
	printf("<font color=\"#FFFFFF\" size=\"6\">命令<font><br/>");
	printf("<form action=\"%s/cgi-bin/order/order\" method=\"get\">", DEFAULTROOT);
	printf("<input name=\"order\" type=\"text\" size=\"60\" maxlength=\"50\" value=\"%s\"><br/>", order);
	printf("<input type=\"submit\" value=\"提交\">");
	printf("<input type=\"submit\" value=\"重置\">");
	printf("</form>");
	printf("<font color=\"#FFFFFF\" size=\"6\">命令结果<font><br/>");
	printf("<textarea name =\"order\" rows=\"15\" cols=\"70\">");
	fflush(NULL);
	if(strlen(order)!=0){
		system(order);
	}
	printf("</textarea><br/>", order);
	printf("</div></body></html>");
	fflush(NULL);
}
