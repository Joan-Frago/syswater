#ifndef TCP_SERVER_H

#define TCP_SERVER_H

#include <libxml2/libxml/parser.h>

#define MESSAGE_SIZE 4096
#define FUNCTION_NAME_SIZE 32

void *start_tcp_server(void*);
int talk(int *);

struct Request{
	char function[FUNCTION_NAME_SIZE];
	xmlNode *data;
};

int process_recv(char *recv_buf, char *resp_buf);

#endif
