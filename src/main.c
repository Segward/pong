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

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_W]) paddle1Y += paddleSpeed * deltaTime;
        if (keystate[SDL_SCANCODE_S]) paddle1Y -= paddleSpeed * deltaTime;
        if (keystate[SDL_SCANCODE_UP]) paddle2Y += paddleSpeed * deltaTime;
        if (keystate[SDL_SCANCODE_DOWN]) paddle2Y -= paddleSpeed * deltaTime;

        ballX += ballVelX * ballSpeed * deltaTime;
        ballY += ballVelY * ballSpeed * deltaTime;

        if (paddle1Y + 0.2f >= 1.0f) paddle1Y = 1.0f - 0.2f;
        if (paddle1Y - 0.2f <= -1.0f) paddle1Y = -1.0f + 0.2f;
        if (paddle2Y + 0.2f >= 1.0f) paddle2Y = 1.0f - 0.2f;
        if (paddle2Y - 0.2f <= -1.0f) paddle2Y = -1.0f + 0.2f;

        float paddle1X = -0.95f;
        float paddle2X = 0.95f;

        if (ballY + 0.03f >= 1.0f || ballY - 0.03f <= -1.0f) ballVelY *= -1;

        float delta1X = ballX - paddle1X;
        float delta1Y = ballY - paddle1Y;
        float overlap1X = 0.03f + 0.05f - fabs(delta1X);
        float overlap1Y = 0.2f + 0.03f - fabs(delta1Y);

        if (overlap1X > 0 && overlap1Y > 0) {
            if (overlap1X < overlap1Y) {
              ballVelX *= -1;
              if (delta1X > 0) {
                ballX += overlap1X;
              } else {
                ballX -= overlap1X;
              }
            } else {
              ballVelY *= -1;
              if (delta1Y > 0) {
                ballY += overlap1Y;
              } else {
                ballY -= overlap1Y;
              }
            }
        }

        float delta2X = ballX - paddle2X;
        float delta2Y = ballY - paddle2Y;
        float overlap2X = 0.03f + 0.05f - fabs(delta2X);
        float overlap2Y = 0.2f + 0.03f - fabs(delta2Y);

        if (overlap2X > 0 && overlap2Y > 0) {
            if (overlap2X < overlap2Y) {
              ballVelX *= -1;
              if (delta2X > 0) {
                ballX += overlap2X;
              } else {
                ballX -= overlap2X;
              }
            } else {
              ballVelY *= -1;
              if (delta2Y > 0) {
                ballY += overlap2Y;
              } else {
                ballY -= overlap2Y;
              }
            }
        }

        if (ballX <= -1.0f || ballX >= 1.0f) {
            ballX = 0.0f;
            ballY = 0.0f;
            ballVelX = -ballVelX;
        }

        float movedVertices0[] = {
            -0.05f + paddle1X, -0.2f + paddle1Y, 0.0f,
             0.05f + paddle1X, -0.2f + paddle1Y, 0.0f,
             0.05f + paddle1X,  0.2f + paddle1Y, 0.0f,
            -0.05f + paddle1X,  0.2f + paddle1Y, 0.0f
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(movedVertices0), movedVertices0);

        float movedVertices1[] = {
            -0.05f + paddle2X, -0.2f + paddle2Y, 0.0f,
             0.05f + paddle2X, -0.2f + paddle2Y, 0.0f,
             0.05f + paddle2X,  0.2f + paddle2Y, 0.0f,
            -0.05f + paddle2X,  0.2f + paddle2Y, 0.0f
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(movedVertices1), movedVertices1);

        float movedBall[] = {
            -0.03f + ballX, -0.03f + ballY, 0.0f,
             0.03f + ballX, -0.03f + ballY, 0.0f,
             0.03f + ballX,  0.03f + ballY, 0.0f,
            -0.03f + ballX,  0.03f + ballY, 0.0f
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(movedBall), movedBall);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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

