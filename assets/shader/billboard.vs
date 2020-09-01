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

/*
vec3 vertexPosition_worldspace =
    particleCenter_wordspace
    + CameraRight_worldspace * squareVertices.x * BillboardSize.x
    + CameraUp_worldspace * squareVertices.y * BillboardSize.y;
*/
// void main() {
//     vec3 cameraRightInWorldspace = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
//     vec3 cameraUpInWorldspace    = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

//     vec3 positionInWorldspace = billboardCenterPosition + cameraRightInWorldspace * position.x * billboardSize.x +
//                                 cameraUpInWorldspace * position.y * billboardSize.y;
//     gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(positionInWorldspace, 1.0);
//     fragmentUV = uv;
// }

void main() {
    // Get the screen-space position of the particle's center
    gl_Position = projectionMatrix * viewMatrix * vec4(billboardCenterPosition, 1.0);
    // Here we have to do the perspective division ourselves.
    gl_Position /= gl_Position.w;

    // Move the vertex in directly screen space. No need for CameraUp/Right_worlspace here.
    gl_Position.xy += position.xy * billboardSize;

    fragmentUV = uv;
}

// vec2(0.2, 0.05);