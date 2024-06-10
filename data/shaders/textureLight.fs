varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;

uniform vec3 eye;
uniform float u_alpha;
uniform float u_specular;
uniform vec3 u_ambient_light;

void main()
{
	vec2 uv = v_uv;
    
	gl_FragColor = u_color * texture2D( u_texture, uv );
}
