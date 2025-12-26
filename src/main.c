// GERADO POR IA (parcialmente)

#include "raylib.h"
#include "engine_api.h"
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
    api_start_new_game(rows, cols, mines);

    // --- GAME LOOP (Substitui o while do scanf) ---
    while (!WindowShouldClose()) {
        // ================= UPDATE (Lógica) =================
        enum Game_State game_state = api_get_game_state();

        // Reset com a tecla 'R'
        if (IsKeyPressed(KEY_R)) {
            api_start_new_game(rows, cols, mines);
        }

        // Só processa cliques se o jogo não acabou
        if (game_state == GAME_UNFINISHED) {
            Vector2 mouse = GetMousePosition();
            
            // Verifica se o clique foi dentro da área do grid
            // Matemática: (Posição Mouse - Margem) / Tamanho da Célula
            int c = (mouse.x - SIDE_MARGIN) / (CELL_SIZE + GAP);
            int r = (mouse.y - TOP_MARGIN) / (CELL_SIZE + GAP);

            bool insideGrid = (c >= 0 && c < cols && r >= 0 && r < rows);

            if (insideGrid) {
                // Botão Esquerdo: Abrir (Open)
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    api_reveal_tile(r, c);
                }
                // Botão Direito: Bandeira (Flag)
                else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    api_toggle_flag(r, c);
                }
            }
        }

        // ================= DRAW (Desenho) =================
        BeginDrawing();
        ClearBackground(COL_BG);

        // UI Superior
        int mines_left;
        api_get_info(&mines_left);
        DrawText(TextFormat("Minas: %d", mines_left), SIDE_MARGIN, 20, 20, WHITE);
        
        if (game_state == GAME_WON) 
            DrawText("VITORIA! (R para reiniciar)", SIDE_MARGIN + 150, 20, 20, GREEN);
        else if (game_state == GAME_LOST) 
            DrawText("GAME OVER (R para reiniciar)", SIDE_MARGIN + 150, 20, 20, RED);

        // Desenhando o Grid
        // Acesso aos ponteiros para leitura

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                
                // Calcula posição X, Y deste quadrado na tela
                int posX = SIDE_MARGIN + j * (CELL_SIZE + GAP);
                int posY = TOP_MARGIN + i * (CELL_SIZE + GAP);

                int tile_state, number;
                api_get_tile(i, j, &tile_state, &number);
                bool reveal = (game_state != GAME_UNFINISHED); // Revela tudo se acabou

                // Lógica de Desenho de cada Célula
                if (tile_state == TS_KNOWN || (reveal && tile_state != TS_FLAGGED)) {
                    // Célula Aberta
                    DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, COL_KNOWN);
                    
                    if (number == -1) {
                        DrawRectangle(posX + 5, posY + 5, CELL_SIZE - 10, CELL_SIZE - 10, BLACK); // Mina Quadrada
                    } else if (number > 0) {
                        // Centralizar texto é chatinho, aqui vai um valor aproximado
                        DrawText(TextFormat("%d", number), posX + 8, posY + 5, 20, GetNumberColor(number));
                    }
                } 
                else if (tile_state == TS_FLAGGED) {
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
    api_clear();
    CloseWindow();

    return 0;
}