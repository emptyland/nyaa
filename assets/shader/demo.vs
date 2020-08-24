#ifdef GL_ES
precision mediump float;
#endif


uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

varying vec2 fragmentUV;
varying vec3 fragmentNormal;
varying vec3 worldPosition;

void main () {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    fragmentUV = uv;
    worldPosition = vec3(modelMatrix * vec4(position, 1.0));
    fragmentNormal = normal;
}
