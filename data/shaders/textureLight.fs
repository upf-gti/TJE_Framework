#version 330 core

in vec3 v_position;
in vec3 v_world_position;
in vec3 v_normal;
in vec2 v_uv;
in vec4 v_color;

uniform vec4 u_color;

uniform sampler2D u_texture;

uniform float u_alpha_cutoff;
uniform vec3 u_ambient_light;
uniform vec3 u_emissive_factor;
uniform vec3 u_light_position;
uniform vec3 u_light_color;
uniform vec3 u_light_front;
uniform float u_specular;
uniform float u_light_max_distance;
uniform int u_light_type;
uniform vec2 u_light_cone_info;
uniform float u_alpha;
uniform vec3 eye;

uniform int u_light_cast_shadows;
uniform sampler2D u_shadowmap;
uniform mat4 u_shadowmap_viewprojection;
uniform float u_shadow_bias;

#define POINTLIGHT 1
#define SPOTLIGHT 2
#define DIRECTIONALLIGHT 3

out vec4 FragColor;

float computeShadow(vec3 wp)
{
    //project our 3D position to the shadowmap
    vec4 proj_pos = u_shadowmap_viewprojection * vec4(wp,1.0);

    //from homogeneus space to clip space
    vec2 shadow_uv = proj_pos.xy / proj_pos.w;

    //from clip space to uv space
    shadow_uv = shadow_uv * 0.5 + vec2(0.5);

    if (shadow_uv.x < 0.0 || shadow_uv.x > 1.0 ||
    shadow_uv.y < 0.0 || shadow_uv.y > 1.0)
    {
        return 1.0;
    }
    //get point depth [-1 .. +1] in non-linear space
    float real_depth = (proj_pos.z - u_shadow_bias) / proj_pos.w;

    //normalize from [-1..+1] to [0..+1] still non-linear
    real_depth = real_depth * 0.5 + 0.5;

    //read depth from depth buffer in [0..+1] non-linear
    float shadow_depth = texture2D(u_shadowmap, shadow_uv).x;

    //compute final shadow factor by comparing
    float shadow_factor = 1.0;

    //we can compare them, even if they are not linear
    if( shadow_depth < real_depth )
        shadow_factor = 0.0;

    return shadow_factor;
}

vec3 multipass(vec3 N, vec3 light, vec4 color, vec3 world_position)
{
    //initialize further used variables
    vec3 L;
    vec3 factor = vec3(1.0);
    float NdotL;
    float shadow_factor = 1.0;

    if (u_light_type == DIRECTIONALLIGHT)
    {
        //all rays are parallel, so using light front, and no attenuation
        L = u_light_front;
        NdotL = clamp(dot(N, L), 0.0, 1.0);

        if ( u_light_cast_shadows == 1)
            shadow_factor *= computeShadow(world_position);
    }
    else if (u_light_type == SPOTLIGHT  || u_light_type == POINTLIGHT)
    {   //emitted from single point in all directions

        //vector from point to light
        L = u_light_position - world_position;
        float dist = length(L);
        //ignore light distance
        L = L/dist;

        NdotL = clamp(dot(N, L), 0.0, 1.0);

        //calculate area affected by spotlight
        if (u_light_type == SPOTLIGHT)
        {
            if ( u_light_cast_shadows == 1)
                shadow_factor *= computeShadow(world_position);

            float cos_angle = dot( u_light_front.xyz, L );
            
            if ( cos_angle < u_light_cone_info.x )
                NdotL = 0.0;
            else if ( cos_angle < u_light_cone_info.y )
                NdotL *= ( cos_angle - u_light_cone_info.x ) / ( u_light_cone_info.y - u_light_cone_info.x );
        }

        //Compute attenuation
        float att_factor = u_light_max_distance - dist;
        att_factor /= u_light_max_distance;
        att_factor = max(att_factor, 0.0);

        //accumulate light attributes in single factor
        factor *= att_factor;
    }
    
    //compute specular light if option activated, otherwise simply sum 0
    vec3 specular = vec3(0);

    //view vector, from point being shaded on surface to camera (eye) 
    vec3 V = normalize(eye-world_position);
    //reflected light vector from L, hence the -L
    vec3 R = normalize(reflect(-L, N));
    //pow(dot(R, V), alpha) computes specular power
    specular = factor*u_specular*(clamp(pow(dot(R, V), u_alpha), 0.0, 1.0))* NdotL * u_light_color * color.xyz ;

    light += NdotL * u_light_color * factor * shadow_factor + specular;

    return light;
}

void main()
{
    vec2 uv = v_uv;
    vec4 color = u_color;
    color *= texture( u_texture, uv );

    vec3 N = normalize( v_normal );
    vec3 light = vec3(u_ambient_light);
    
    light = multipass(N, light, color, v_world_position);

    //calculate final colours
    vec4 final_color;
    final_color.xyz = color.xyz*light;

    final_color.a = color.a;

    FragColor = final_color;
}