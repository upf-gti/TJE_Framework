#version 330 core

// Input vertex attribute from the vertex buffer
attribute vec3 a_vertex;
attribute vec2 a_uv;

// Output UV coordinate to the fragment shader
varying vec2 v_uv;

// Uniform for the view-projection matrix from the orthographic camera
uniform mat4 u_viewprojection;

void main() {
    // Pass through the UV coordinates to the fragment shader
    v_uv = a_uv;

    // Transform the vertex position using the view-projection matrix
    gl_Position = u_viewprojection * vec4(a_vertex, 1.0);
}