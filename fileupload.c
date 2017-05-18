#include "fileupload.h"
#define MAXSIZE 2048
int fileupload(int client_sfd, char *recvbuf){
	char boundary[MAXSIZE+1]="";
	char filename[100]="";
	//获取boundary
	char *startp = strstr(recvbuf, "boundary=");
	char *endp = strstr(startp, "\r\n");
	startp = startp+strlen("boundary=");
	memcpy(boundary, startp, endp-startp);
	//获取filename
	startp = strstr(recvbuf, "filename=");
	endp = strstr(startp, "\r\n");
	startp = startp+strlen("filename=")+1;//注意:filename有" "
	endp = endp -1;
	memcpy(filename, startp, endp-startp);
	printf("boundary:%s;filename:%s\n", boundary, filename);
	int fd  = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if(fd<0)
		goto open_err;
	char startmark[MAXSIZE];
	char endmark[MAXSIZE];
	sprintf(startmark, "--%s", boundary);
	sprintf(endmark, "--%s--", boundary);
	startp = strstr(recvbuf, startmark);
	if(startp!=NULL){
		startp = strstr(startp, "\r\n\r\n");
		if(startp!=NULL){
			startp = startp + 4;
			endp = strstr(recvbuf, endmark);
			if(endp!=NULL)
			{
				*endp = '\0';
				write(fd, startp, strlen(startp));//写入数据
			}
			else{
				write(fd, startp, strlen(startp));//写入数据
				int count;
				while(1){
					memset(recvbuf, '\0', MAXSIZE+1);//清零
					//获取请求信息
					count = read(client_sfd, recvbuf, MAXSIZE);
					if(endp!=NULL)
					{
						*endp = '\0';
						write(fd, recvbuf, count);//写入数据
						break;
					}
					write(fd, recvbuf, count);//写入数据
				}
			}
		}
	}
	printf("文件上传成功\n");
	close(fd);//关闭文件
	return 1;
open_err:
	perror("");
	return 0;
}
