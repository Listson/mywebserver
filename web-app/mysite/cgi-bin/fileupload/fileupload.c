#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main(int argc, char **argv){
	char filename[100]="";
	

	
	int fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if(fd<0)
		goto open_err;
	while(1){
		
	}

	return 0;
open_err:
	perror("");
	return -1;
}
