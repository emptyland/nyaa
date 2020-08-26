#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D sampler;

uniform vec3 ambientMaterial;
uniform vec3 ambientLight;
uniform vec3 diffuseMaterial;
uniform vec3 diffuseLight;
uniform vec3 specularMaterial;
uniform vec3 specularLight;
uniform vec3 directionalLight;
uniform vec3 cameraPosition;

varying vec2 fragmentUV;
varying vec3 worldPosition;
varying vec3 fragmentNormal;

void main() {
    vec4 ambientColor = vec4(ambientMaterial * ambientLight, 1.0);

    vec3 norDirLight       = normalize(directionalLight);  //单位化
    vec3 norFragmentNormal = normalize(fragmentNormal);    //单位化

    //漫反射强度
    float diffuseIntensity = max(0.0, dot(norDirLight, norFragmentNormal));
    vec4  diffuseColor     = vec4(diffuseLight * diffuseMaterial * diffuseIntensity, 1.0);

    vec4 specularColor = vec4(0.0, 0.0, 0.0, 1.0);  //黑色
                                                    //当镜面反射不为零的时候我们才认为它具有反射光
    if (diffuseIntensity != 0.0) {
        vec3 reflectDir = normalize(reflect(-norDirLight, norFragmentNormal));
        vec3 viewDir    = normalize(cameraPosition - worldPosition);
        specularColor   = vec4(specularLight * specularMaterial, 1.0) * pow(max(0.0, dot(viewDir, reflectDir)), 0.1);
    }

    vec4 color   = texture2D(sampler, fragmentUV);
    gl_FragColor = (ambientColor + diffuseColor + specularColor) * color;
}