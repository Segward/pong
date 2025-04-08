CC = gcc
CFLAGS = -Wall -g -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lSDL2 -lSDL2_ttf

SRC = src/main.c
OBJ = src/main.o
OUT = pong

src/main.o: $(SRC)
	@$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

$(OUT): $(OBJ)
	@$(CC) $(OBJ) -o $(OUT) $(LDFLAGS)

build: $(OUT)
	@echo "Built $(OUT)"

run: $(OUT)
	@echo "Running $(OUT)"
	@./$(OUT)

clean:
	@echo "Cleaning up"
	@rm -f $(OBJ) $(OUT)

.PHONY: clean run build

