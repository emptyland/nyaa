#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D sampler;

varying vec2 fragmentUV;

void main() {
    vec4 color = texture2D(sampler, fragmentUV);
    if (color.a == 0.0) { discard; }
    gl_FragColor = color;
}