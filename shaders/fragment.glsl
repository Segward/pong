#version 330 core

in vec3 fragColor; // Color from vertex shader

out vec4 color; // Final pixel color

void main() {
    color = vec4(fragColor, 1.0); // Output color with full opacity
}

