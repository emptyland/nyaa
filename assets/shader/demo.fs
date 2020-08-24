#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D sampler;
const vec3        ambientMaterial  = vec3(0.7, 0.7, 0.7);
const vec3        ambientLight     = vec3(1.0, 1.0, 1.0);
const vec3        diffuseMaterial  = vec3(1.0, 1.0, 1.0);
const vec3        diffuseLight     = vec3(1.0, 1.0, 1.0);
const vec3        directionalLight = vec3(0, 1.0, 1.0);

varying vec2 fragmentUV;
varying vec3 worldPosition;
varying vec3 fragmentNormal;

// vec4(0.5, 0.7, 0.3, 1.0) *
void main() {

    // vec3 color = vec3(texture2D(sampler, fragmentUV));
    // gl_FragColor = vec4(0.5, 0.7, 0.3, 1.0);
    vec4 ambientColor = vec4(ambientMaterial * ambientLight, 1.0);

    vec3 norDirLight       = normalize(directionalLight);  //单位化
    vec3 norFragmentNormal = normalize(fragmentNormal);    //单位化

    //漫反射强度
    float diffuseIntensity = max(0.0, dot(norDirLight, norFragmentNormal));
    vec4  diffuseColor     = vec4(diffuseLight * diffuseMaterial * diffuseIntensity, 1.0);

    // vec4 specularColor = vec4(0.0, 0.0, 0.0, 0.0);  //黑色
    //                                                 //当镜面反射不为零的时候我们才认为它具有反射光
    // if (diffuseIntensity != 0.0) {
    //     // reflect 是shader内置函数
    //     vec3 reflectDir = normalize(reflect(-norDirLight, norFragmentNormal));
    //     vec3 viewDir    = normalize(U_CameraPos.xyz - V_WorldPos.xyz);
    //     specularColor =
    //         U_LightSpecular * U_SpecularMaterial * pow(max(0.0, dot(viewDir, reflectDir)), U_LightController.x);
    // }

    vec4 color   = texture2D(sampler, fragmentUV);
    gl_FragColor = ambientColor * diffuseColor * color;
}