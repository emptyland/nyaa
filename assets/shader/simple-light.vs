#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

attribute vec2 tex2d;
attribute vec3 position;

varying vec3 V_Normal;
varying vec4 V_WorldPos;  //模型受光点世界坐标轴的位置

void main() {
    gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
    //V_Normal    = normal;
    V_WorldPos  = modelViewMatrix * vec4(position, 1.0);
}