#version 120
#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 projectionMatrix;
uniform mat4 viewModelMatrix;

attribute vec3 position;
attribute vec2 uv;

varying vec2 fragmentUV;

void main() {
    gl_Position = projectionMatrix * viewModelMatrix * vec4(position, 1.0);
    fragmentUV = uv;
}