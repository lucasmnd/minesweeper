#include "engine_api.h"
#include "internals/actions.h"
#include "internals/game_states.h"
#include "internals/minefield.h"

minefield_t *board = NULL;
bool exploded = false;

int api_clear(){
    destroy_minefield(board);
    board = NULL;
    return 0;
}

int api_start_new_game(int rows, int cols, int mines){
    destroy_minefield(board); // há uma checagem para nulos em destroy
    board = create_minefield(rows, cols, mines);
    exploded = false;
    return 0;
}

int api_reveal_tile(int row, int col){
    if(board == NULL) return -1;

    int ret = check_tile(board, row, col);
    if(ret == CHECKED_MINE) exploded = true;

    return 0;
}

int api_toggle_flag(int row, int col){
    if(board == NULL) return -1;

    int32_t (*tile_states)[board->cols] = (int32_t (*)[board->cols]) board->tile_states;
    int32_t state = tile_states[row][col];

    if(state == TS_FLAGGED) return unflag_tile(board, row, col);
    if(state == TS_UNKNOWN) return flag_tile(board, row, col);
    return -1;
}

int api_get_tile(int row, int col, int *state, int *number){
    if(board == NULL) return -1;

    int32_t (*tile_states)[board->cols] = (int32_t (*)[board->cols]) board->tile_states;
    int32_t (*numbers_grid)[board->cols] = (int32_t (*)[board->cols]) board->numbers_grid;

    if(state != NULL)
        *state = tile_states[row][col];
    
    if(number != NULL)
        *number = numbers_grid[row][col];

    return 0;
}

int api_get_info(int *mines_left){
    if(board == NULL) return -1;

    if(mines_left != NULL)
        *mines_left = board->total_mines - board->flags;
    
    return 0;
}

enum Game_State api_get_game_state(){
    return check_game_state(board, exploded);
}