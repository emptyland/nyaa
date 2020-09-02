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

// void main() {
//     vec3 CameraRight_worldspace = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
//     vec3 CameraUp_worldspace    = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

//     vec3 vertexPosition_worldspace = billboardCenterPosition + CameraRight_worldspace * position.x * billboardSize.x +
//                                      CameraUp_worldspace * position.y * billboardSize.y;

//     mat4 p = modelMatrix;

//     gl_Position = projectionMatrix * viewMatrix * vec4(vertexPosition_worldspace, 1.0);
//     fragmentUV  = uv;
// }

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    // gl_Position /= gl_Position.w;
    // gl_Position.xy += position.xy * billboardSize;
    vec2 p = billboardSize;
    vec3 t = billboardCenterPosition;
    fragmentUV = uv;
}
