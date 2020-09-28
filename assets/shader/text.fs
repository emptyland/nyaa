#version 120
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D sampler;
uniform vec4      textColor;

varying vec2 fragmentUV;

void main() {
    vec4 color = texture2D(sampler, fragmentUV);
    color.rgb += textColor.rgb;
    color.a *= textColor.a;
    gl_FragColor = color;
}