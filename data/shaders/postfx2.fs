#version 330 core

#define SCANLINES 1
#define CHROMATIC_ABERRATION 1
#define VIGNETTE 1
#define DISTORT 1

//Chromatic aberration parameters
#define CA_STRENGTH 15.0

// Vignette parameters
#define CORNER_OFFSET 0.99
#define CORNER_MASK_INTENSITY_MULT 1000.0
#define BORDER_OFFSET 0.3



in vec2 v_uv; // Texture coordinates from the vertex shader

uniform vec2 iResolution;
uniform sampler2D u_texture;
uniform float u_ambient_light;

out vec4 FragColor; // Output color


vec3 ChromaticAberration(vec2 uv)
{
    vec3 color = texture(u_texture, uv).rgb;
	color.r = texture(u_texture, (uv - 0.5) * (1.0 + CA_STRENGTH / iResolution.xy) + 0.5).r;
	color.b = texture(u_texture, (uv - 0.5) * (1.0 - CA_STRENGTH / iResolution.xy) + 0.5).b;

    return color;
}


vec2 BrownConradyDistortion(in vec2 uv)
{
    // Increased values of K1 for a stronger fisheye effect
    float k1 = 0.4;  
    float k2 = 0.2;

    uv = uv * 2.0 - 1.0; // Convert to [-1:1]
    
    float r2 = uv.x * uv.x + uv.y * uv.y;
    float radial_distortion = 1.0 + k1 * r2 + k2 * r2 * r2;
    
    // Reduce the impact of distortion as it moves towards the edges
    float edgeFactor = smoothstep(0.0, 0.8, 1.0 - r2);
    uv *= mix(1.0, radial_distortion, edgeFactor);
    
    uv = uv * 0.5 + 0.5; // Convert back to [0:1]
    
    // Scaling factor to keep the image within viewport dimensions
    float scale = 1.0 / (1.0 + k1 + k2 * 0.25); // Adjust scaling
    
    uv = uv * scale - (scale * 0.5) + 0.5; // Scale from center

    return uv;
}

vec2 applyFisheye(vec2 uv) {
    // Center coordinates (0.5, 0.5)
    vec2 centeredUV = uv - 0.5;

    // Distance from the center
    float dist = length(centeredUV);

    // Factor to control the strength of the fisheye effect
    float strength = 0.5;

    // Apply fisheye distortion
    float radius = dist * strength + (1.0 - strength) * dist * dist;

    // Clamp radius to move highly distorted parts outside the screen
    radius = clamp(radius, 0.0, 1.0);

    // Calculate the new distorted coordinates
    vec2 fisheyeUV = (radius / dist) * centeredUV;

    // Return the modified UV coordinates, ensuring they stay within [0, 1]
    return fisheyeUV + 0.5;
}



float Vignette(vec2 uv)
{
    uv = uv - 0.5; // [-0.5; 0.5]
    
    // Cut the corners
    float res = length(uv)- CORNER_OFFSET;
    res *= CORNER_MASK_INTENSITY_MULT;
    res = clamp(res, 0.0, 1.0);
    res = 1.0 - res;
    
    // Cut the out of bounds information
    uv = abs(uv); // [0.0; 0.5]
    uv = uv - (0.5 - BORDER_OFFSET);
    uv = smoothstep(1.0, 0.0, uv / BORDER_OFFSET);
    
    // combine
	return min(uv.x, uv.y) * res;
}

float Scanlines(vec2 uv)
{
    return (abs(sin( iResolution.y * uv.y)) + abs(sin( iResolution.x * uv.x / 2))) / 1.0;
}

void main() {
    // Use the texture coordinates directly
    float PX = 7.0/iResolution.x;
    vec2 pUv = vec2( PX*floor(v_uv.x/PX), PX*floor(v_uv.y/PX) );
    
    // pUv = applyFisheye(pUv);
    vec3 result = ChromaticAberration(pUv);
    // result *= Scanlines(pUv);
    vec4 fCor = texture(u_texture, pUv);
    // result *= Vignette(pUv);

    result *= 1.3 - length(pUv - vec2(0.5));
    // vec4 color = texture(u_texture, v_uv);

    // Output the color
    FragColor = vec4( result, 1.0 );
    // FragColor = fCor;


    // screen space coordinates [0;1]
// 	vec2 uv = v_uv.xy / iResolution.xy;
    
// #if DISTORT
//     uv = BrownConradyDistortion(uv);
// #endif
    
// #if CHROMATIC_ABERRATION
//     vec3 result = ChromaticAberration(uv);
// #else
//     vec3 result = texture(u_texture, uv).rgb;
// #endif
    
// #if SCANLINES
//     result *= Scanlines(uv);
// #endif    

// #if VIGNETTE
//     result *= Vignette(uv);
// #endif
     
//     // final output
//     FragColor = vec4( result, 1.0 );
}
