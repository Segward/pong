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

    float vertices[] = {
      -0.5f, -0.5f, 0.0f,   // bottom left
       0.5f, -0.5f, 0.0f,   // bottom right
       0.5f,  0.5f, 0.0f,   // top right
      -0.5f,  0.5f, 0.0f    // top left
    };

    unsigned int indices[] = {
      0, 1, 2,              // triangle bottom right
      2, 3, 0               // triangle top left
    };
    
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO); glBindVertexArray(VAO);
    glGenBuffers(1, &VBO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &EBO); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    int running = 1;
    SDL_Event event;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    while (running) {
      while (SDL_PollEvent(&event)) {
          if (event.type == SDL_QUIT) running = 0;
      }
      
      glClear(GL_COLOR_BUFFER_BIT);
      glUseProgram(rectShaderProgram);
      GLint color = glGetUniformLocation(rectShaderProgram, "color");
      glUniform3f(color, 1.0f, 0.0f, 0.0f);
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      SDL_GL_SwapWindow(window);
    }

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(rectShaderProgram);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

