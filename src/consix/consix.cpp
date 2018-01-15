#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "consix.h"
#include "../common/util.h"

void consix_t::start_game(int mode)
{
	game.InitGame();
	game.GameStart = true;
	game.MaxDepthBackUp = 1;
	game.LargeDepthBackUp = 11;
	if(mode == 1)
	{
		game.ComputerColor = WHITE;
		game.MaxDepth[WHITE] = game.MaxDepthBackUp;
		game.LargeDepth[WHITE] = game.LargeDepthBackUp;
	}
	else if(mode == 2)
	{
		game.ComputerColor = BLACK;
		game.MaxDepth[BLACK] = game.MaxDepthBackUp;
		game.LargeDepth[BLACK] = game.LargeDepthBackUp;
	}
}

Step consix_t::computer_move()
{
	Step step;
	if(game.StepID == 0)
	{
		game.Board[9][9] = BLACK;
		step.First.X = 9, step.First.Y = 9, step.First.side = BLACK;
		step.Second.X = -1, step.Second.Y = -1, step.Second.side = BLACK;
		game.UpdateBoard(step, BLACK);
	}
	else if(game.ComputerColor == BLACK)
	{
		step = game.ComputerMove();
		step.First.side = step.Second.side = BLACK;
		game.Board[step.First.X][step.First.Y] = BLACK;
		game.Board[step.Second.X][step.Second.Y] = BLACK;
		game.UpdateBoard(step, BLACK);
	}
	else if(game.ComputerColor == WHITE)
	{
		step = game.ComputerMove();
		step.First.side = step.Second.side = WHITE;
		game.Board[step.First.X][step.First.Y] = WHITE;
		game.Board[step.Second.X][step.Second.Y] = WHITE;
		game.UpdateBoard(step, WHITE);
	}
	game.Player = NEXT(game.Player);
	game.StepID++;

	if(game.WinOrLose(BLACK) == WINLOSE)
	{
		game.GameStart = false;
	}
	if(game.WinOrLose(WHITE) == WINLOSE)
	{
		game.GameStart = false;
	}
	return step;
}

void consix_t::human_move(const Step &step)
{
	game.Board[step.First.X][step.First.Y] = step.First.side;
	game.Board[step.Second.X][step.Second.Y] = step.Second.side;
	game.UpdateBoard(step, step.First.side);
	game.Player = NEXT(game.Player);
	game.StepID++;
}

void consix_t::handle_consix(const char *preq, int req_len, char **pres, int *pres_len)
{
#define RESULT(s) \
	const char *pret = s; \
	*pres_len = strlen(pret)+1; \
	*pres = (char*)malloc(*pres_len); \
	strcpy(*pres, pret);

#define RET_STEP(s) \
	char str[64] = {0}; \
	sprintf(str, "cmd:move;step:%d,%d,%d,%d,%d", s.First.side, s.First.X, s.First.Y, s.Second.X, s.Second.Y); \
	RESULT(str);

	if(!preq || req_len <= 0 || !pres || !pres_len)
		return;
	char *ptype = NULL;
	get_val_by_key(preq, "type", &ptype);
	if(!ptype)
		return;
	if(strcmp(ptype, "consix")!=0)
	{
		RESULT("error:1;detail:type incorrect!");
		FREE(ptype);
		return;
	}
	FREE(ptype);

	int mode = 0;
	char *pmode = NULL;
	get_val_by_key(preq, "mode", &pmode);
	if(pmode)
	{
		mode = atoi(pmode);
		if(mode != 1 && mode != 2)
		{
			RESULT("error:2;detail:mode incorrect");
			FREE(pmode);
			return;
		}
		start_game(mode);
		if(mode == 1)
		{
			RESULT("error:0;detail: you first");
			FREE(pmode);
			return;
		}
		else
		{
			Step step = computer_move();
			RET_STEP(step);
			FREE(pmode);
			return;
		}
	}
	FREE(pmode);
	
	int side = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	char *pstep = NULL;
	get_val_by_key(preq, "step", &pstep);
	if(pstep)
	{
		Step s1, s2;
		int side = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		sscanf(pstep, "%d,%d,%d,%d,%d", &side, &x1, &y1, &x2, &y2);
		s1.Second.side = s1.First.side = side;
		s1.First.X = x1, s1.First.Y = y1;
		s1.Second.X = x2, s1.Second.Y = y2;
		human_move(s1);
		s2 = computer_move();
		RET_STEP(s2);
		FREE(pstep);
		return;
	}
	RESULT("error:-1;detail:key incorrect");
	FREE(pstep);
	return;
}
