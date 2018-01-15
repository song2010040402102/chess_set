#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>	
#include <arpa/inet.h>
#include "service.h"
#include "../common/util.h"
#include "../common/socket_i.h"

#define MAX_CONNECT 10

struct connect_para
{
	const void* pclient;
	int socket;
};

int g_cur_con_num = 0;

struct connect_para g_con_para[MAX_CONNECT];

int get_socket(const void* pclient)
{
	for(int i = 0; i< g_cur_con_num; i++)
	{
		if(g_con_para[i].pclient == pclient)
			return g_con_para[i].socket;
	}
	return 0;
}

int add_socket(const void* pclient, const int socket)
{
	if(g_cur_con_num >= MAX_CONNECT)
		return -1;
	g_con_para[g_cur_con_num].pclient = pclient;
	g_con_para[g_cur_con_num].socket = socket;
	g_cur_con_num++;
	return 0;
}

void del_socket(const int socket)
{
	int index = 0;
	for(int i = 0; i < g_cur_con_num; i++)
	{
		if(g_con_para[i].socket == socket)
		{
			index = i;
			break;
		}
	}
	for(int i = index; i < g_cur_con_num-1; i++)
	{
		g_con_para[i] = g_con_para[i+1];
	}
	g_con_para[g_cur_con_num-1].pclient = NULL;
	g_con_para[g_cur_con_num-1].socket = 0;
	g_cur_con_num--;
}

void handle_service(const void* pclient, const char* preq, const int req_len, char** pres, int* pres_len)
{
#define RESULT(s) \
	const char *pret = s; \
	*pres_len = strlen(pret)+1; \
	*pres = (char*)malloc(*pres_len); \
	strcpy(*pres, pret);

	if(!pclient || !preq || req_len <= 0 || !pres || !pres_len)
		return;
	char *ptype = NULL;
	get_val_by_key(preq, "type", &ptype);
	if(!ptype)
		return;
	if(strcmp(ptype, "consix")==0)
	{
		char *pcmd = NULL;
		get_val_by_key(preq, "cmd", &pcmd);
		if(!pcmd)
		{
			FREE(ptype);
			return;
		}
		if(strcmp(pcmd, "start") == 0)
		{
			if(g_cur_con_num >= MAX_CONNECT)
			{
				RESULT("error:2;detail:exceed max connect!");
				FREE(ptype);
				FREE(pcmd);
				return;
			}
			int socket = connect_server("127.0.0.1", 30998);
			if(add_socket(pclient, socket) != 0)
			{
				FREE(ptype);
				FREE(pcmd);
				return;
			}
		}
	}
	else
	{
		RESULT("error:1;detail:type incorrect!");			
		FREE(ptype);
		return;
	}
	FREE(ptype);

	int socket = get_socket(pclient);
	if(socket > 0)
	{
		socket_send(socket, preq, req_len);
		socket_recv(socket, pres, pres_len);
	}
}

void handle_close(const void* pclient)
{
	int socket = get_socket(pclient);
	if(socket != 0)
	{
		del_socket(socket);
		shutdown(socket, SHUT_RDWR);
		close(socket);
	}	
}
