#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>	
#include <arpa/inet.h>
#include <pthread.h>
#include "consix.h"
#include "../common/util.h"
#include "../common/socket_i.h"

#define PORT 30998
#define MAX_CONNECT_NUM 10

void *handle_client(void* args)
{
	if(!args) return NULL;
	int socket_client = *(int*)args;		

	consix_t consix;
	char *preq = NULL, *pres = NULL;
	int req_len = 0, res_len = 0;
	while(1)
	{		
		if(socket_recv(socket_client, &preq, &req_len) < 0 || !preq || req_len == 0)
		{
			break;
		}		

		consix.handle_consix(preq, req_len, &pres, &res_len);

		if(socket_send(socket_client, pres, res_len) < 0)
		{
			break;
		}
		FREE(preq);
		FREE(pres);
		req_len = 0;
		res_len = 0;
	}	
	FREE(preq);	
	FREE(pres);
}

int main(int argc, char * argv[])
{
	int socket_serv = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_serv < 0)
	{
		return socket_error(socket_serv, "socket");
	}

	int on = 1;
	if(setsockopt(socket_serv, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		return socket_error(socket_serv, "setsockopt");
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
	if(bind(socket_serv, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		return socket_error(socket_serv, "bind");
	}

	if(listen(socket_serv, MAX_CONNECT_NUM) < 0)
	{
		return socket_error(socket_serv, "listen");	
	}
	printf("server running...\nlisten address: %s\n", inet_ntoa(server_addr.sin_addr));

	int socket_client;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;
	pthread_t thread_id;
	while(1)
	{
		client_addr_len = sizeof(client_addr);
		socket_client = accept(socket_serv, (struct sockaddr*)&client_addr, &client_addr_len);
		if(socket_client < 0)
		{
			return socket_error(socket_serv, "accept");
		}

		if(pthread_create(&thread_id, NULL, handle_client, (void*)(&socket_client)) < 0)
		{
			printf("[ERROR] function: pthread_create, error: %d, detail: %s\n", errno, strerror(errno));
			return errno;
		}
		pthread_detach(thread_id);
	}

	return 0;
}
