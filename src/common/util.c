#include <stdlib.h>
#include <string.h>

void get_val_by_key(const char *data, const char* key, char **val)
{
	if(!val)
		return;

	int data_len = 0, key_len = 0;
	if(!data || (data_len=strlen(data)) <= 0 || !key || (key_len=strlen(key)) <= 0)
	{
		*val = NULL;
		return;
	}

	const char *index1 = strstr(data, key);
	if(!index1)
	{
		*val = NULL;
		return;
	}

	const char *index2 = strstr(index1+key_len+1, ";");
	if(!index2)
	{
		index2 = data+data_len;
	}
	int val_len = index2-index1-key_len-1;
	*val = (char*)malloc(val_len+1);
	memset(*val, 0, val_len+1);
	strncpy(*val, index1+key_len+1, val_len);
}

