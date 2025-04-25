clean:
	rm -f pong *.o

compile:
	@echo "Building the project..."
	gcc -o pong src/main.c -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib -lm -framework OpenGL -framework Cocoa -framework IOKit
