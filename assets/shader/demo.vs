attribute vec3 position;

uniform mat4 matrix;

void main () {
    gl_Position = vec4(position.x, position.y, position.z, 1.0) * matrix;
}
