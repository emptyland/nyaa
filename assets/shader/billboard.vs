#ifdef GL_ES
precision mediump float;
#endif

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 billboardCenterPosition;
uniform vec2 billboardSize;

attribute vec3 position;
attribute vec2 uv;

varying vec2 fragmentUV;

void main() {
    // Get the screen-space position of the particle's center
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(billboardCenterPosition, 1.0);
    // Here we have to do the perspective division ourselves.
    gl_Position /= gl_Position.w;

    // Move the vertex in directly screen space. No need for CameraUp/Right_worlspace here.
    gl_Position.xy += position.xy * billboardSize;

    fragmentUV = uv;
}

// vec2(0.2, 0.05);