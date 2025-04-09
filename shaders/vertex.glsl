#version 330 core

layout(location = 0) in vec3 aPos; // Vertex positions
layout(location = 1) in vec3 aColor; // Vertex color

out vec3 fragColor; // Output color to fragment shader

void main() {
    gl_Position = vec4(aPos, 1.0);  // Position the vertex
    fragColor = aColor;             // Pass color to the fragment shader
}

