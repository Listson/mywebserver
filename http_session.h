#ifndef HTTP_SESSION_H
#define HTTP_SESSION_H
#define MAXSIZE 2048
#define DEFAULTFILEPATH "."
#define FILENOTFIND 404
#define FILEFORBIDDEN 403
#define FILEOK 1
#define CGIFILEOK 2
extern char **environ;

int http_process(int client_sfd);
int get_http_process(int client_sfd, char *recvbuf);
int post_http_process(int client_sfd, char *recvbuf);
char *get_mime_type(char *uribuf);
int get_uri(char *recvbuf, char *uribuf);
int get_req_mode(char *recvbuf);
int get_path(char *uribuf, char *pathbuf);
void transcode(char *express);
void serve_dynamic(int client_sfd, char *pathbuf, char *cgi_arg);
void send_err_info(int client_sfd, int status);
void serve_static(int client_sfd, char *uribuf, char *pathbuf);
#endif
