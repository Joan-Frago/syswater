#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/xpath.h>

#include "../inc/tcp_server.h"

char *read_request_function(char *);

void *start_tcp_server(void* arg){
	printf("Server Thread ID is %lu\n",(unsigned long)pthread_self());

	int sockfd, new_sockfd;
	struct sockaddr_in server_addr;
	struct sockaddr_in remote_addr;
	socklen_t addrlen;

	// Socket creation
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("Error creating the socket\n");
		return NULL;
	}

	// Bind the socket
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(8080);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	if(ret < 0){
		perror("Error creating the socket\n");
		return NULL;
	}

	// Listen for connections
	ret = listen(sockfd,5);
	if(ret < 0){
		perror("Error listening for connections\n");
		return NULL;
	}

	addrlen = sizeof(remote_addr);

	char *address = inet_ntoa(server_addr.sin_addr);
	int port = ntohs(server_addr.sin_port);

	printf("Server listening on address %s port %d\n",address,port);

	for(;;){
		new_sockfd = accept(sockfd, (struct sockaddr *)&remote_addr, &addrlen);
		if(new_sockfd < 0){
			perror("Error accepting a connection\n");
			continue;
		}

		printf("New client connected\n");

		talk(&new_sockfd);

		close(new_sockfd);
		printf("Ready for new client\n");
	}

	// Close the socket
	close(sockfd);
	return 0;
}

int talk(int *sockfd){
	while(1){
		// Receive a message
		char recv_buf[MESSAGE_SIZE];
		ssize_t bytes_recv = recv(*sockfd, recv_buf, MESSAGE_SIZE, 0);
		if(bytes_recv == 0){
			printf("Client closed connection\n");
			break;
		}else if(bytes_recv < 0){
			perror("Error receiving data\n");
			break;
		}

		// Buffer where the response will be stored
		char resp_buf[MESSAGE_SIZE];

		// Process received data
		if(process_recv(recv_buf, resp_buf) != 0){
			printf("Error processing received data from client\n");
		}

		// Send a response
		send(*sockfd, resp_buf, strlen(resp_buf), 0);

		memset(resp_buf, 0, strlen(resp_buf)); // Set the buffer to 0 aka "empty the string"
	}
	return 0;
}

/*
 * Process received data and call corresponding function.
 *
 * Data must be an xml.
 */
int process_recv(char *recv_buf, char *resp_buf){
	printf("Received buffer: %s\n", recv_buf);
	strcat(resp_buf, "ok\\n");

	struct Request req;

	// Get function name from xml
	req.function = read_request_function(recv_buf);
	
	// Get the rest of the xml (children node)
	// Set req.data to the rest of the xml
	
	printf("Request function name: %s\n", req.function);

	// Call another function that reads req.function and calls the target function

	return 0;
}

/*
 * Reads the function name from the request and writes it in dest
 */
char *read_request_function(char *xml_doc_str){
	xmlDoc *req_xml_doc = xmlReadDoc(BAD_CAST xml_doc_str, NULL, NULL, 0);
	if(req_xml_doc == NULL){
		printf("Error: Could not parse request xml.\n");
	}

	
	xmlXPathContext *xpath_ctx = xmlXPathNewContext(req_xml_doc);
	if(xpath_ctx == NULL){
		printf("Error: Unable to create new XPath context.\n");
	}

	xmlNode *root = xmlDocGetRootElement(req_xml_doc);
	
	xmlNode *device = root->children;
	device = device->next;

	xpath_ctx->node = device;

	xmlXPathObjectPtr xpath_obj = xmlXPathEvalExpression(BAD_CAST "/request/function", xpath_ctx);

	xmlNode *node = xpath_obj->nodesetval->nodeTab[0];
	xmlChar *content = xmlNodeGetContent(node);

	xmlXPathFreeObject(xpath_obj);

	return (char *)content;
}
