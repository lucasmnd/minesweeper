#include "internals/game_states.h"

int check_game_state(minefield_t *minefield, bool mine_checked){
    if(mine_checked) return GAME_LOST;

    if(minefield->known_tiles == minefield->rows * minefield->cols - minefield->total_mines)
        return GAME_WON;
    
    return GAME_UNFINISHED;
}