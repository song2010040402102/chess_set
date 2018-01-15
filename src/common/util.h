#ifndef UTIL_H
#define UTIL_H

#define FREE(s) \
	if(s) \
	{ \
		free(s); \
		s = NULL; \
	}

void get_val_by_key(const char *data, const char* key, char **val);

#endif
