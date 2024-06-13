
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture; // The texture (spritesheet)
uniform vec2 u_tileSize; // The size of each tile in pixels (e.g., vec2(16, 16))
uniform vec2 u_tileGrid; // The number of tiles in the grid (e.g., vec2(10, 10))
uniform vec2 u_selectedTile; // The tile to select (e.g., vec2(2, 3))

void main( )
{
    // Normalize the fragment coordinates to [0, 1]
    vec2 uv = v_uv;

    // Calculate the size of each tile in UV space
    vec2 tileSizeUV = vec2(u_tileSize) / (u_tileGrid * u_tileSize);

    // Calculate the UV offset for the selected tile
    vec2 tileOffsetUV = vec2(u_selectedTile) * tileSizeUV;

    // Adjust the UV coordinates to sample the correct tile
    uv = uv * tileSizeUV + tileOffsetUV;

    // Sample the texture
    gl_FragColor = texture(u_texture, uv);
}
