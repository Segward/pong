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

    SDL_Window* window = SDL_CreateWindow("Multiple Rectangles", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
    if (window == NULL) {
        printf("ERROR: Could not create window\n");
        return -1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        printf("ERROR: Could not create OpenGL context\n");
        SDL_DestroyWindow(window);
        return -1;
    }

    glewInit();

    GLuint rectShaderProgram = loadShader("shaders/rect.vert", "shaders/rect.frag");
    if (rectShaderProgram == 0) {
        printf("ERROR: Shader program could not be created!\n");
        return -1;
    }

    GLint uColor = glGetUniformLocation(rectShaderProgram, "uColor");

    // Rectangle 1 (initial position)
    float rect1Vertices[] = {
        -0.5f, -0.5f, 0.0f,   // bottom left
         0.5f, -0.5f, 0.0f,   // bottom right
         0.5f,  0.5f, 0.0f,   // top right
        -0.5f,  0.5f, 0.0f    // top left
    };

    // Rectangle 2 (initial position)
    float rect2Vertices[] = {
        -0.5f, -0.5f, 0.0f,   // bottom left
         0.5f, -0.5f, 0.0f,   // bottom right
         0.5f,  0.5f, 0.0f,   // top right
        -0.5f,  0.5f, 0.0f    // top left
    };

    unsigned int indices[] = {
        0, 1, 2,              // triangle bottom right
        2, 3, 0               // triangle top left
    };

    GLuint VAO1, VBO1, EBO1;
    GLuint VAO2, VBO2, EBO2;

    // First rectangle
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect1Vertices), rect1Vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &EBO1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Second rectangle
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect2Vertices), rect2Vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &EBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    int running = 1;
    SDL_Event event;
    
    float moveX1 = 0.1f;  // Rectangle 1 moves right
    float moveY1 = 0.1f;  // Rectangle 1 moves up
    float moveX2 = -0.1f; // Rectangle 2 moves left
    float moveY2 = -0.1f; // Rectangle 2 moves down

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }

        // Update the vertices with new positions
        float movedVertices1[] = {
            -0.5f + moveX1, -0.5f + moveY1, 0.0f,   // bottom left
             0.5f + moveX1, -0.5f + moveY1, 0.0f,   // bottom right
             0.5f + moveX1,  0.5f + moveY1, 0.0f,   // top right
            -0.5f + moveX1,  0.5f + moveY1, 0.0f    // top left
        };

        float movedVertices2[] = {
            -0.5f + moveX2, -0.5f + moveY2, 0.0f,   // bottom left
             0.5f + moveX2, -0.5f + moveY2, 0.0f,   // bottom right
             0.5f + moveX2,  0.5f + moveY2, 0.0f,   // top right
            -0.5f + moveX2,  0.5f + moveY2, 0.0f    // top left
        };

        // Update the vertex buffers with the new positions
        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(movedVertices1), movedVertices1);

        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(movedVertices2), movedVertices2);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(rectShaderProgram);

        glUniform3f(uColor, 1.0f, 0.0f, 0.0f);  // Set color for Rectangle 1
        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniform3f(uColor, 0.0f, 0.0f, 1.0f);  // Set color for Rectangle 2
        glBindVertexArray(VAO2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);
    }

    // Cleanup (unchanged)
    glDeleteBuffers(1, &VBO1);
    glDeleteBuffers(1, &EBO1);
    glDeleteVertexArrays(1, &VAO1);

    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &EBO2);
    glDeleteVertexArrays(1, &VAO2);

    glDeleteProgram(rectShaderProgram);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

