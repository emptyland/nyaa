#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 centerPosition;

attribute vec3 position;
attribute vec2 uv;

varying vec2 fragmentUV;

void main () {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(centerPosition, 1.0);
    gl_Position /= gl_Position.w;
    gl_Position.xy += position.xy * vec2(0.2, 0.05);
    fragmentUV = uv;
}