CC = gcc
CFLAGS = -Iinc -Wall -Wextra -g -static

SRC_DIR = src
BUILD_DIR = build
TARGET = minesweeper

# 1. MUDANÇA CRÍTICA: Usamos 'shell find' em vez de wildcard.
# O 'find' cava em todas as subpastas procurando arquivos .c
SRCS := $(shell find $(SRC_DIR) -name '*.c')

# 2. Mantemos a substituição, mas agora ela lida com caminhos longos
# Ex: src/engine/core.c vira build/engine/core.o
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

# 3. MUDANÇA CRÍTICA: Criação dinâmica de pastas
# O alvo (%) agora pode conter barras (ex: engine/core.o)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@# $(dir $@) pega o diretório do arquivo alvo (ex: build/engine/)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)