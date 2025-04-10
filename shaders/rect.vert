#version 330 core

layout(location = 0) in vec3 pos; // Vertex positions
layout(location = 1) in vec3 uColor; // Uniform vertex color

out vec3 fragColor; // Output color to fragment shader

void main() {
    gl_Position = vec4(pos, 1.0);  // Position the vertex
    fragColor = uColor;  // Pass color to the fragment shader
}

