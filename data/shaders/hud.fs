#version 330 core

in vec2 v_uv;
out vec4 FragColor;

uniform vec3 u_color;
uniform float u_percentage;
uniform float u_decrease;

void main() {


    vec3 finalColor = u_color;
    if (v_uv.x > u_percentage)
        finalColor = vec3(0.0);
    else if (v_uv.x > u_percentage + u_decrease) {
        finalColor = vec3(1.0); // Set color to white
    } else {
        finalColor = u_color ;
    }

    // Set the color for the remaining fragments
    FragColor = vec4(finalColor* (1.0 - 0.7 * v_uv.y), 1.0);
}
