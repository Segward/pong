# Pong Game

Just started learning SDL2 with C so this is a simple pong game I made to practice.

## Usage

1. Install SDL2 and SDL2_ttf using Homebrew:

```bash
brew install sdl2 sdl2_ttf
```

2. Compile the code using the following command:

```bash
gcc -o pong src/main.c -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2 -lSDL2_ttf
```

3. Run the game:

```bash
./pong
```
