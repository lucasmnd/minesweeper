#ifndef H_ENGINE_API
#define H_ENGINE_API

#include "api_enums.h"

int api_clear();

int api_start_new_game(int rows, int cols, int mines);

int api_left_click(int row, int col);

int api_right_click(int row, int col);

int api_get_tile(int row, int col, int *state, int *number);

int api_get_info(int *mines_left);

enum Game_State api_get_game_state();

#endif