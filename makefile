main:my_webserver.c http_session.c fileupload.c
	gcc my_webserver.c http_session.c fileupload.c -lpthread -o main
