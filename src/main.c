#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../inc/minefield.h"
#include "../inc/actions.h"
#include "../inc/game_states.h"

// Função auxiliar para limpar a tela (funciona em Linux/Mac/Windows na maioria dos terminais)
void clear_screen() {
    printf("\033[H\033[J");
}

// Função para desenhar o tabuleiro
// reveal_all: Se true, mostra onde estão as minas (usado no Game Over)
void print_board(minefield_t *mf, bool reveal_all) {
    // Truque do VLA para acessar como matriz 2D
    int32_t (*states)[mf->cols] = (int32_t (*)[mf->cols])mf->tile_states;
    int32_t (*numbers)[mf->cols] = (int32_t (*)[mf->cols])mf->numbers_grid;

    printf("\n    ");
    for (int j = 0; j < mf->cols; j++) printf("%d ", j); // Cabeçalho das colunas
    printf("\n   ");
    for (int j = 0; j < mf->cols; j++) printf("--");
    printf("\n");

    for (int i = 0; i < mf->rows; i++) {
        printf("%d | ", i); // Cabeçalho das linhas
        for (int j = 0; j < mf->cols; j++) {
            char symbol = '?';
            int state = states[i][j];
            int number = numbers[i][j];

            // Lógica de exibição
            if (state == TS_FLAGGED) {
                symbol = 'F'; // Bandeira
                // Se for game over e a bandeira estava errada, mostramos erro (opcional)
                if (reveal_all && number != -1) symbol = 'X'; 
            } 
            else if (state == TS_KNOWN || reveal_all) {
                if (number == -1) {
                    symbol = '*'; // Mina
                } else if (number == 0) {
                    symbol = ' '; // Vazio
                } else {
                    symbol = number + '0'; // Converte int para char numérico
                }
            } 
            else {
                symbol = '.'; // Fechado
            }
            
            printf("%c ", symbol);
        }
        printf("\n");
    }
    
    // Status do jogo
    printf("\nMinas Totais: %d | Bandeiras: %d\n", mf->total_mines, mf->flags);
}

int main() {
    // Configurações do jogo
    int rows = 8;
    int cols = 8;
    int mines = 10;

    // Cria o campo (lembre-se: minas não são geradas aqui, só no 1º clique)
    minefield_t *mf = create_minefield(rows, cols, mines);
    if (!mf) {
        fprintf(stderr, "Erro ao alocar memoria!\n");
        return 1;
    }

    int state = GAME_UNFINISHED;
    bool exploded = false; // Flag auxiliar para passar pro check_game_state

    while (state == GAME_UNFINISHED) {
        clear_screen();
        printf("=== CAMPO MINADO (C Engine) ===\n");
        print_board(mf, false);

        int r, c;
        char action;
        
        printf("\nComandos:\n");
        printf("  o [lin] [col] -> Abrir (Open)\n");
        printf("  f [lin] [col] -> Bandeira (Flag)\n");
        printf("  u [lin] [col] -> Remover Bandeira (Unflag)\n");
        printf("Digite: ");
        
        // Lê input do usuário
        int read_count = scanf(" %c %d %d", &action, &r, &c);

        // Limpa buffer do teclado caso o usuário digite lixo
        while(getchar() != '\n'); 

        if (read_count != 3) {
            printf("Comando invalido!\n");
            continue;
        }

        // Validação básica de limites
        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            printf("Coordenada fora do limite!\n");
            getchar(); // Pausa pra ler o erro
            continue;
        }

        // Processa Ação
        if (action == 'o') {
            int res = check_tile(mf, r, c);
            if (res == CHECKED_MINE) {
                exploded = true;
            }
        } 
        else if (action == 'f') {
            flag_tile(mf, r, c);
        } 
        else if (action == 'u') {
            unflag_tile(mf, r, c);
        }

        // Verifica vitória/derrota
        state = check_game_state(mf, exploded);
    }

    // Fim de jogo
    clear_screen();
    print_board(mf, true); // Revela tudo

    if (state == GAME_WON) {
        printf("\n>>> PARABENS! VOCE VENCEU! <<<\n");
    } else {
        printf("\n>>> BOOM! VOCE PERDEU! <<<\n");
    }

    destroy_minefield(mf);
    return 0;
}