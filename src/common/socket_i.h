#ifndef SOCKET_I_H
#define SOCKET_I_H

int socket_error(const int socket, const char *function);

int connect_server(const char* ip, unsigned short port);

int socket_send(const int socket, const char* pbuf, const int buf_len);

int socket_recv(const int socket, char** pbuf, int* pbuf_len);
#endif
