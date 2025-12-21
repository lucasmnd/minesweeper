#ifndef H_GAME_STATES
#define H_GAME_STATES

#include "minefield.h"

enum Game_States{
    GAME_UNFINISHED, GAME_LOST, GAME_WON
};

int check_game_state(minefield_t *minefield, bool mine_checked);

#endif