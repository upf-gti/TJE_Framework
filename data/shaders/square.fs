#version 330 core

in vec2 v_uv;
out vec4 FragColor;

uniform vec4 u_color;
uniform float u_percentage;

void main() {


    vec3 finalColor = u_color;
    if (v_uv.y > u_percentage)
        finalColor = vec4(0.0);
    else {
        finalColor = u_color;
    }

    // Set the color for the remaining fragments
    FragColor = vec4(finalColor* (1.0 - 0.7 * v_uv.y), 1.0);
}
