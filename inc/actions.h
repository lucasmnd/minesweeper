#ifndef H_ACTIONS
#define H_ACTIONS

#include "minefield.h"

enum Check_Ret{
    CHECKED_MINE, CHECKED_NUMBER, CHECKED_FLAG, CHECKED_KNOWN
};

enum Flag_Ret{
    FLAGGED_KNOWN, FLAGGED_UNKNOWN
};

int check_tile(minefield_t *minefield, uint16_t row, uint16_t col);

int flag_tile(minefield_t *minefield, uint16_t row, uint16_t col);

int unflag_tile(minefield_t *minefield, uint16_t row, uint16_t col);

#endif