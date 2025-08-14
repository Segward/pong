#version 330

uniform float iTime;
uniform vec2 iResolution;

out vec4 fragColor;

float rand(vec2 co) {
  return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
  vec3 color = vec3(0.0);

  for (int i = 0; i < 200; i++) {
    // Random base star position
    vec2 basePos = vec2(
      rand(vec2(i, 1)) * iResolution.x * 2.0,
      rand(vec2(i, 2)) * iResolution.y * 2.0
    );

    // Random star color
    vec3 starColor = vec3(
      rand(vec2(i, 3)), // random R
      rand(vec2(i, 4)), // random G
      rand(vec2(i, 5))  // random B
    );

    // Normalize and scale the star color
    starColor = normalize(starColor) * 2;

    // Offset stars based on time
    vec2 starPos = basePos;
    starPos.x = mod(basePos.x - iTime * 30.0, iResolution.x * 2.0);
    starPos.y = mod(basePos.y - iTime * 10.0, iResolution.y * 2.0);

    // Distance to current pixel
    float d = distance(gl_FragCoord.xy, starPos);
    float star = smoothstep(3.0, 0.0, d);

    // Twinkling
    star *= 0.5 + 0.5 * sin(iTime * 3.0 + float(i));
    color += starColor * star;
  }

  color = clamp(color, 0.0, 1.0);
  fragColor = vec4(color, 1.0);
}

