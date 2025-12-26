#include "internals/minefield.h"
#include <stdlib.h>
#include <time.h>
#include <sysexits.h>

static int generate_mines(minefield_t *minefield, uint16_t x0, uint16_t y0);
static int generate_numbers(minefield_t *minefield);

minefield_t* create_minefield(uint16_t rows, uint16_t cols, uint16_t mines){
    minefield_t* minefield = malloc(sizeof(minefield_t));

    minefield->rows = rows;
    minefield->cols = cols;
    minefield->numbers_grid = calloc(rows * cols, sizeof(int32_t));
    minefield->tile_states = calloc(rows * cols, sizeof(int32_t));
    minefield->total_mines = mines;
    minefield->known_tiles = 0;
    minefield->flags = 0;
    minefield->initialized = false;
    minefield->seed = time(NULL);

    return minefield;
}

void destroy_minefield(minefield_t *minefield){
    if(minefield == NULL) return;

    free(minefield->numbers_grid);
    free(minefield->tile_states);

    free(minefield);
}

int setup_minefield(minefield_t *minefield, uint16_t x0, uint16_t y0){
    generate_mines(minefield, x0, y0);
    generate_numbers(minefield);
    minefield->initialized = true;
    return 0;
}

static int generate_mines(minefield_t *minefield, uint16_t x0, uint16_t y0){
    srand(minefield->seed);
    uint16_t slots = minefield->rows * minefield->cols;
    uint16_t mines = minefield->total_mines;

    if(mines > slots - 9) return -1;

    for(int i = 0; i < mines; i++){
        int r = rand() % slots;

        int row = r / minefield->cols;
        int col = r % minefield->cols;

        if(minefield->numbers_grid[r] == -1 || (abs(x0 - row) <= 1 && abs(y0 - col) <= 1)){
            i--;
            continue;
        }

        minefield->numbers_grid[r] = -1;
    }

    return 0;
}

static int generate_numbers(minefield_t *minefield){
    int32_t (*numbers_grid)[minefield->cols] = (int32_t (*)[minefield->cols]) minefield->numbers_grid;

    for(int i = 0; i < minefield->rows; i++){
        for(int j = 0; j < minefield->cols; j++){
            if(numbers_grid[i][j] != -1) continue;

            for(int u = i-1; u <= i+1; u++){
                for(int v = j-1; v <= j+1; v++){
                    if((u < 0) || (u >= minefield->rows) || 
                            (v < 0) || (v >= minefield->cols))
                        continue;
                    
                    if(numbers_grid[u][v] == -1) continue;

                    numbers_grid[u][v]++;
                }
            }
        }
    }
    return 0;
}

bool set_tile_state(minefield_t *minefield, uint16_t row, uint16_t col, enum Tile_State ts){
    int32_t (*tile_states)[minefield->cols] = (int32_t (*)[minefield->cols]) minefield->tile_states;
    int32_t (*numbers_grid)[minefield->cols] = (int32_t (*)[minefield->cols]) minefield->numbers_grid;
    enum Tile_State current_state = tile_states[row][col];
    
    if(current_state == ts) return true;
    
    switch(current_state){
        case TS_UNKNOWN:
            break;
        
        case TS_KNOWN:
            exit(EX_USAGE);
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
            if(numbers_grid[row][col] == -1)
                return false;
            minefield->known_tiles++;
            break;
    }

    return true;
}
