#include <sysexits.h>
#include <stdlib.h>
#include "internals/actions.h"
#include "api_enums.h"

static void check_adjacent(minefield_t *minefield, uint16_t row, uint16_t col);
static void set_tile_state(minefield_t *minefield, uint16_t row, uint16_t col,
    enum Tile_State ts);
static int chord_tile(minefield_t *minefield, uint16_t row, uint16_t col);

int check_tile(minefield_t *minefield, uint16_t row, uint16_t col){
    if(!minefield->initialized)
        setup_minefield(minefield, row, col);

    int32_t (*tile_states)[minefield->cols] = (int32_t (*)[minefield->cols]) minefield->tile_states;
    int32_t (*numbers_grid)[minefield->cols] = (int32_t (*)[minefield->cols]) minefield->numbers_grid;

    if(tile_states[row][col] == TS_KNOWN){
        return chord_tile(minefield, row, col);
    }

    if(tile_states[row][col] == TS_FLAGGED) return CHECKED_FLAG;

    set_tile_state(minefield, row, col, TS_KNOWN);

    if(numbers_grid[row][col] == -1) return CHECKED_MINE;

    if(numbers_grid[row][col] == 0) 
        check_adjacent(minefield, row, col);
    
    return CHECKED_NUMBER;
}

static void check_adjacent(minefield_t *minefield, uint16_t row, uint16_t col){
    int32_t (*tile_states)[minefield->cols] = (int32_t (*)[minefield->cols]) minefield->tile_states;
    int32_t (*numbers_grid)[minefield->cols] = (int32_t (*)[minefield->cols]) minefield->numbers_grid;

    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            int u = row + i, v = col + j;

            // evitando acessos a índices invalidos
            if((u < 0) || (u >= minefield->rows) || 
                    (v < 0) || (v >= minefield->cols))
                continue;
            
            if(tile_states[u][v] == TS_KNOWN) continue;

            set_tile_state(minefield, u, v, TS_KNOWN);

            if(numbers_grid[u][v] == 0){
                check_adjacent(minefield, u, v);
            }
        }
    }
}

int flag_tile(minefield_t *minefield, uint16_t row, uint16_t col){
    int32_t (*tile_states)[minefield->cols] = (int32_t (*)[minefield->cols]) minefield->tile_states;

    if(tile_states[row][col] == TS_KNOWN) return FLAGGED_KNOWN;

    set_tile_state(minefield, row, col, TS_FLAGGED);
    return FLAGGED_UNKNOWN;
}

int unflag_tile(minefield_t *minefield, uint16_t row, uint16_t col){
    int32_t (*tile_states)[minefield->cols] = (int32_t (*)[minefield->cols]) minefield->tile_states;

    if(tile_states[row][col] == TS_KNOWN) return FLAGGED_KNOWN;

    set_tile_state(minefield, row, col, TS_UNKNOWN);
    return FLAGGED_UNKNOWN;
}

static int chord_tile(minefield_t *minefield, uint16_t row, uint16_t col){
    int32_t (*tile_states)[minefield->cols] = (int32_t (*)[minefield->cols]) minefield->tile_states;
    int32_t (*numbers_grid)[minefield->cols] = (int32_t (*)[minefield->cols]) minefield->numbers_grid;

    if(tile_states[row][col] != TS_KNOWN) return -1;

    int32_t number = numbers_grid[row][col];

    // verificando se há flags marcadas o suficiente em torno do tile para fazer um chord
    int flags = 0;
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            int u = row + i, v = col + j;

            if((u < 0) || (u >= minefield->rows) || 
                    (v < 0) || (v >= minefield->cols))
                continue;
            
            if(tile_states[u][v] == TS_FLAGGED) flags++;
        }
    }

    if(number != flags) return -1;
    int ret = CHECKED_NUMBER;

    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            int u = row + i, v = col + j;

            if((u < 0) || (u >= minefield->rows) || 
                    (v < 0) || (v >= minefield->cols))
                continue;
            
            if(tile_states[u][v] != TS_KNOWN && check_tile(minefield, u, v) == CHECKED_MINE) ret = CHECKED_MINE;
        }
    }

    return ret;
}

static void set_tile_state(minefield_t *minefield, uint16_t row, uint16_t col, enum Tile_State ts){
    int32_t (*tile_states)[minefield->cols] = (int32_t (*)[minefield->cols]) minefield->tile_states;
    enum Tile_State current_state = tile_states[row][col];
    
    if(current_state == ts) return;
    
    switch(current_state){
        case TS_UNKNOWN:
            break;
        
        case TS_KNOWN:
            return;
            break;

        case TS_FLAGGED:
            minefield->flags--;
            break;
    }

    tile_states[row][col] = ts;

    switch(ts){
        case TS_UNKNOWN:
            break;
        
        case TS_FLAGGED:
            minefield->flags++;
            break;
        
        case TS_KNOWN:
            minefield->known_tiles++;
            break;
    }
}