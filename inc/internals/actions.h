#ifndef H_ACTIONS
#define H_ACTIONS

#include "minefield.h"

int check_tile(minefield_t *minefield, uint16_t row, uint16_t col);

int flag_tile(minefield_t *minefield, uint16_t row, uint16_t col);

int unflag_tile(minefield_t *minefield, uint16_t row, uint16_t col);

#endif