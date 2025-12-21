#ifndef H_MINEFIELD
#define H_MINEFIELD

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

enum Tile_State{
    TS_UNKNOWN, TS_KNOWN, TS_FLAGGED
};

typedef struct s_minefield {
    uint16_t rows, cols;
    int32_t *numbers_grid;
    int32_t *tile_states;
    uint16_t total_mines;
    uint16_t known_tiles;
    uint16_t flags;
    bool initialized;
    time_t seed;
} minefield_t;

minefield_t* create_minefield(uint16_t rows, uint16_t cols, uint16_t mines);

void destroy_minefield(minefield_t *minefield);

int setup_minefield(minefield_t *minefield, uint16_t x0, uint16_t y0);

bool set_tile_state(minefield_t *minefield, uint16_t row, uint16_t col,
    enum Tile_State ts);

#endif
