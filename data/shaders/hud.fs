#version 330 core

in vec2 v_uv;
out vec4 FragColor;

uniform vec3 u_color;
uniform float u_percentage;

void main() {
    if (v_uv.x > u_percentage)
        discard;

    // Set the color for the remaining fragments
    FragColor = vec4(u_color * (1 - 0.7*v_uv.y), 1.0);
}
