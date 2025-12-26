// GERADO POR IA

#include "raylib.h"
#include <stdio.h>
#include "../inc/minefield.h"
#include "../inc/actions.h"
#include "../inc/game_states.h"

// --- Configurações Visuais ---
#define CELL_SIZE 30
#define TOP_MARGIN 60
#define SIDE_MARGIN 20
#define GAP 2 // Espaço entre células

// Cores Clássicas do Campo Minado
Color COL_UNKNOWN = { 192, 192, 192, 255 }; // Cinza
Color COL_KNOWN   = { 220, 220, 220, 255 }; // Cinza claro
Color COL_BG      = { 100, 100, 100, 255 }; // Fundo da janela

// Mapeamento de cores dos números (1=Azul, 2=Verde, etc)
Color GetNumberColor(int number) {
    switch(number) {
        case 1: return BLUE;
        case 2: return DARKGREEN;
        case 3: return RED;
        case 4: return DARKBLUE;
        case 5: return MAROON;
        case 6: return LIME;
        case 7: return BLACK;
        case 8: return GRAY;
        default: return BLACK;
    }
}

int main(void) {
    // 1. Configuração do Jogo
    int rows = 5;
    int cols = 5;
    int mines = 5;

    // Cálculo do tamanho da janela baseado no grid
    int screenWidth = (cols * (CELL_SIZE + GAP)) + (SIDE_MARGIN * 2);
    int screenHeight = (rows * (CELL_SIZE + GAP)) + TOP_MARGIN + SIDE_MARGIN;

    InitWindow(screenWidth, screenHeight, "Campo Minado - C Engine + Raylib");
    SetTargetFPS(60);

    // Inicializa sua engine
    minefield_t *mf = create_minefield(rows, cols, mines);
    
    int gameState = GAME_UNFINISHED;
    bool exploded = false;

    // --- GAME LOOP (Substitui o while do scanf) ---
    while (!WindowShouldClose()) {
        
        // ================= UPDATE (Lógica) =================
        
        // Reset com a tecla 'R'
        if (IsKeyPressed(KEY_R)) {
            destroy_minefield(mf);
            mf = create_minefield(rows, cols, mines);
            gameState = GAME_UNFINISHED;
            exploded = false;
        }

        // Só processa cliques se o jogo não acabou
        if (gameState == GAME_UNFINISHED) {
            Vector2 mouse = GetMousePosition();
            
            // Verifica se o clique foi dentro da área do grid
            // Matemática: (Posição Mouse - Margem) / Tamanho da Célula
            int c = (mouse.x - SIDE_MARGIN) / (CELL_SIZE + GAP);
            int r = (mouse.y - TOP_MARGIN) / (CELL_SIZE + GAP);

            bool insideGrid = (c >= 0 && c < cols && r >= 0 && r < rows);

            if (insideGrid) {
                // Botão Esquerdo: Abrir (Open)
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    int res = check_tile(mf, r, c);
                    if (res == CHECKED_MINE) exploded = true;
                }
                // Botão Direito: Bandeira (Flag)
                else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    // Alterna entre Flag e Unflag
                    // Precisaria ler o estado antes, ou criar uma função toggle_flag na engine
                    // Como sua engine tem flag e unflag separados, vamos verificar manual:
                    
                    // Acesso "sujo" para verificar estado (idealmente use um getter)
                    int32_t (*states)[mf->cols] = (int32_t (*)[mf->cols])mf->tile_states;
                    if (states[r][c] == TS_FLAGGED)
                        unflag_tile(mf, r, c);
                    else
                        flag_tile(mf, r, c);
                }
            }
            
            gameState = check_game_state(mf, exploded);
        }

        // ================= DRAW (Desenho) =================
        BeginDrawing();
        ClearBackground(COL_BG);

        // UI Superior
        DrawText(TextFormat("Minas: %d", mf->total_mines - mf->flags), SIDE_MARGIN, 20, 20, WHITE);
        
        if (gameState == GAME_WON) 
            DrawText("VITORIA! (R para reiniciar)", SIDE_MARGIN + 150, 20, 20, GREEN);
        else if (gameState == GAME_LOST) 
            DrawText("GAME OVER (R para reiniciar)", SIDE_MARGIN + 150, 20, 20, RED);

        // Desenhando o Grid
        // Acesso aos ponteiros para leitura
        int32_t (*states)[mf->cols] = (int32_t (*)[mf->cols])mf->tile_states;
        int32_t (*numbers)[mf->cols] = (int32_t (*)[mf->cols])mf->numbers_grid;

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                
                // Calcula posição X, Y deste quadrado na tela
                int posX = SIDE_MARGIN + j * (CELL_SIZE + GAP);
                int posY = TOP_MARGIN + i * (CELL_SIZE + GAP);

                int state = states[i][j];
                int number = numbers[i][j];
                bool reveal = (gameState != GAME_UNFINISHED); // Revela tudo se acabou

                // Lógica de Desenho de cada Célula
                if (state == TS_KNOWN || (reveal && state != TS_FLAGGED)) {
                    // Célula Aberta
                    DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, COL_KNOWN);
                    
                    if (number == -1) {
                        DrawRectangle(posX + 5, posY + 5, CELL_SIZE - 10, CELL_SIZE - 10, BLACK); // Mina Quadrada
                    } else if (number > 0) {
                        // Centralizar texto é chatinho, aqui vai um valor aproximado
                        DrawText(TextFormat("%d", number), posX + 8, posY + 5, 20, GetNumberColor(number));
                    }
                } 
                else if (state == TS_FLAGGED) {
                    // Célula com Bandeira
                    DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, COL_UNKNOWN);
                    DrawRectangle(posX + 8, posY + 8, CELL_SIZE - 16, CELL_SIZE - 16, RED); // Bandeira "abstrata"
                    
                    // Se perdeu e a bandeira tava errada
                    if (reveal && number != -1) {
                        DrawLine(posX, posY, posX + CELL_SIZE, posY + CELL_SIZE, RED); // X de erro
                        DrawLine(posX + CELL_SIZE, posY, posX, posY + CELL_SIZE, RED);
                    }
                } 
                else {
                    // Célula Fechada (Unknown)
                    DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, COL_UNKNOWN);
                    // Efeito de relevo 3D simples (borda branca e escura)
                    DrawRectangleLines(posX, posY, CELL_SIZE, CELL_SIZE, DARKGRAY);
                }
            }
        }

        EndDrawing();
    }

    // Limpeza
    destroy_minefield(mf);
    CloseWindow();

    return 0;
}