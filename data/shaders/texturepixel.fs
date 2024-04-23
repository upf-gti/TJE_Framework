
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;

void main()
{
	vec2 uv = v_uv;
    
    	float Pixels = 256.0;
    	float dx = 25.0 * (1.0 / Pixels);
    	float dy = 25.0 * (1.0 / Pixels);
    	vec2 Coord = vec2(dx * floor(uv.x / dx),
                      dy * floor(uv.y / dy));
                      

	gl_FragColor = u_color * texture2D( u_texture, Coord );
}
