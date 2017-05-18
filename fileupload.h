#ifndef FILEUPLOAD_H
#define FILEUPLOAD_H
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
int filupload(int client_sfd, char *recvbuf);
#endif
