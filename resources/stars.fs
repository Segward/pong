#version 330

uniform float iTime;
uniform vec2 iResolution;

out vec4 fragColor;

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec3 color = vec3(0.1, 0.1, 0.1);

    for (int i = 0; i < 200; i++) {
        vec2 starPos = vec2(
          rand(vec2(i, 1)) * iResolution.x * 2,
          rand(vec2(i, 2)) * iResolution.y * 2
        );

        float d = distance(gl_FragCoord.xy, starPos);
        float star = smoothstep(1.5, 0.0, d);
        star *= 0.5 + 0.5 * sin(iTime * 3.0 + float(i));
        color += vec3(star);
    }

    color = clamp(color, 0.0, 1.0);
    fragColor = vec4(color, 1.0);
}

