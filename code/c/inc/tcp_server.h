#define MESSAGE_SIZE 1000

void *start_tcp_server(void*);
int talk(int *);
int process_recv(char *buf, int buf_len);
