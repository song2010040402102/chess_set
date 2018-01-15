#ifndef CONSIX_H
#define CONSIX_H

#include "search.h"

class consix_t
{
public:
	void handle_consix(const char *preq, int req_len, char **pres, int *pres_len);
private:
	void start_game(int mode);
	Step computer_move();
	void human_move(const Step &step);
private:
	CSearch game;
};

#endif
