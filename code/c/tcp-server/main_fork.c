#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MESSAGE_SIZE 1000

int main(){
	int sockfd, new_sockfd;
	struct sockaddr_in server_addr;
	struct sockaddr_in remote_addr;
	int addrlen;

	// Socket creation
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("Error creating the socket");
	}

	// Bind the socket
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(8080);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	if(ret < 0){
		perror("Error creating the socket");
	}

	// Listen for connections
	ret = listen(sockfd,5);
	if(ret < 0){
		perror("Error listening for connections");
	}

	addrlen = sizeof(struct sockaddr);

	for(;;){
		new_sockfd = accept(sockfd, (struct sockaddr *)&remote_addr, &addrlen);
		if(new_sockfd < 0){
			perror("Error accepting a connection");
		}

		// Fork the process
		pid_t cpid;
		cpid = fork();
		if(cpid == -1){
			perror("Error trying to fork process");
			exit(EXIT_FAILURE);
		}
		if(cpid > 0){
			// Code executed by parent
			printf("Child PID is %ld",(long)cpid);
		}
		else{
			// Code executed by child
			printf("My PID is %ld",(long)getpid());

			// Receive a message
			char recv_buf[MESSAGE_SIZE];
			recv(new_sockfd, &recv_buf, MESSAGE_SIZE, 0);

			char *resp_buf = "Received data: ";
			strcat(resp_buf, recv_buf);
			printf("%s",resp_buf);

			// Send a response
			send(new_sockfd, resp_buf, strlen(resp_buf), 0);
		}
	}

	// Close the socket
	close(sockfd);
	return 0;
}
