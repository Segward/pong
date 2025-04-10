#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

char* readFile(const char* path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        printf("ERROR: Could not open shader file %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* source = (char*)malloc(fileSize + 1);
    if (!source) {
        printf("ERROR: Could not allocate memory for shader file\n");
        fclose(file);
        return NULL;
    }

    fread(source, 1, fileSize, file);
    source[fileSize] = '\0';

    fclose(file);
    return source;
}

GLuint compileShader(GLenum shaderType, const char* shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* infoLog = (char*)malloc(length);
        glGetShaderInfoLog(shader, length, &length, infoLog);
        printf("ERROR: Shader compilation failed\n%s\n", infoLog);
        free(infoLog);
    }

    return shader;
}

GLuint loadShader(const char* vertexPath, const char* fragmentPath) {
    char* vertexSource = readFile(vertexPath);
    char* fragmentSource = readFile(fragmentPath);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("Pong", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    glewInit();

    GLuint shaderProgram = loadShader("shaders/rect.vert", "shaders/rect.frag");
    GLint uColor = glGetUniformLocation(shaderProgram, "uColor");

    float rectVertices[] = {
        -0.05f, -0.2f, 0.0f,
         0.05f, -0.2f, 0.0f,
         0.05f,  0.2f, 0.0f,
        -0.05f,  0.2f, 0.0f
    };

    float ballVertices[] = {
        -0.03f, -0.03f, 0.0f,
         0.03f, -0.03f, 0.0f,
         0.03f,  0.03f, 0.0f,
        -0.03f,  0.03f, 0.0f
    };

    unsigned int indices[] = {0, 1, 2, 2, 3, 0};

    GLuint VAO[3], VBO[3], EBO[3];
    glGenVertexArrays(3, VAO);
    glGenBuffers(3, VBO);
    glGenBuffers(3, EBO);

    for (int i = 0; i < 3; i++) {
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ballVertices), ballVertices, GL_DYNAMIC_DRAW);

    float paddle1Y = 0.0f;
    float paddle2Y = 0.0f;
    float ballX = 0.0f;
    float ballY = 0.0f;
    float ballVelX = 1.0f;
    float ballVelY = 1.0f;

    Uint32 lastTime = SDL_GetTicks();
    const float paddleSpeed = 1.5f;
    const float ballSpeed = 0.5f;

    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;


        // Input
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_W]) paddle1Y += paddleSpeed * deltaTime;
        if (keystate[SDL_SCANCODE_S]) paddle1Y -= paddleSpeed * deltaTime;
        if (keystate[SDL_SCANCODE_UP]) paddle2Y += paddleSpeed * deltaTime;
        if (keystate[SDL_SCANCODE_DOWN]) paddle2Y -= paddleSpeed * deltaTime;

        // Move ball
        ballX += ballVelX * ballSpeed * deltaTime;
        ballY += ballVelY * ballSpeed * deltaTime;

        // Bounce on top/bottom
        if (ballY + 0.03f >= 1.0f || ballY - 0.03f <= -1.0f) ballVelY *= -1;

        // Clamp paddles
        if (paddle1Y + 0.2f > 1.0f) paddle1Y = 1.0f - 0.2f;
        if (paddle1Y - 0.2f < -1.0f) paddle1Y = -1.0f + 0.2f;
        if (paddle2Y + 0.2f > 1.0f) paddle2Y = 1.0f - 0.2f;
        if (paddle2Y - 0.2f < -1.0f) paddle2Y = -1.0f + 0.2f;

        // Paddle positions
        float paddle1X = -0.925f;
        float paddle2X = 0.925f;

        // Ball-paddle collision
        if (ballX - 0.03f <= paddle1X + 0.05f &&
            ballX >= paddle1X &&
            ballY <= paddle1Y + 0.2f &&
            ballY >= paddle1Y - 0.2f) {
            ballVelX *= -1;
        }

        if (ballX + 0.03f >= paddle2X - 0.05f &&
            ballX <= paddle2X &&
            ballY <= paddle2Y + 0.2f &&
            ballY >= paddle2Y - 0.2f) {
            ballVelX *= -1;
        }

        // Score check
        if (ballX < -1.0f || ballX > 1.0f) {
            ballX = 0.0f;
            ballY = 0.0f;
            ballVelX = -ballVelX;
        }

        // Update paddles
        for (int i = 0; i < 2; i++) {
            float y = (i == 0) ? paddle1Y : paddle2Y;
            float x = (i == 0) ? paddle1X : paddle2X;
            float movedVertices[] = {
                -0.05f + x, -0.2f + y, 0.0f,
                 0.05f + x, -0.2f + y, 0.0f,
                 0.05f + x,  0.2f + y, 0.0f,
                -0.05f + x,  0.2f + y, 0.0f
            };
            glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(movedVertices), movedVertices);
        }

        // Update ball buffer
        float movedBall[] = {
            -0.03f + ballX, -0.03f + ballY, 0.0f,
             0.03f + ballX, -0.03f + ballY, 0.0f,
             0.03f + ballX,  0.03f + ballY, 0.0f,
            -0.03f + ballX,  0.03f + ballY, 0.0f
        };
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(movedBall), movedBall);

        // Draw
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        glUniform3f(uColor, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniform3f(uColor, 1.0f, 0.0f, 0.0f); // red ball
        glBindVertexArray(VAO[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);
    }

    for (int i = 0; i < 3; i++) {
        glDeleteBuffers(1, &VBO[i]);
        glDeleteBuffers(1, &EBO[i]);
        glDeleteVertexArrays(1, &VAO[i]);
    }

    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

