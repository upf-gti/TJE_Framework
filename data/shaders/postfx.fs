#version 330 core

in vec2 v_uv; // Texture coordinates from the vertex shader

uniform vec2 iResolution;
uniform sampler2D u_texture;
uniform float u_ambient_light;

out vec4 FragColor; // Output color

void main() {
    // Use the texture coordinates directly
    float PX = 4.0/iResolution.x;
    vec2 pUv = vec2( PX*floor(v_uv.x/PX), PX*floor(v_uv.y/PX) );
    vec4 fCor = texture(u_texture, pUv);

    // vec4 color = texture(u_texture, v_uv);

    // Output the color
    FragColor = fCor;
}
