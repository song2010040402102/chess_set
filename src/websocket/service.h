#ifndef SERVICE_H
#define SERVICE_H

void handle_service(const void* pclient, const char* preq, const int req_len, char **pres, int *pres_len);

void handle_close(const void* pclient);

#endif
