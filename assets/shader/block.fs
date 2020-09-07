#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D sampler;

uniform vec3 ambientMaterial;
uniform vec3 diffuseMaterial;
uniform vec3 specularMaterial;
uniform vec3 directionalLight;
uniform vec3 directionalLightColor;

uniform vec3  pointLightPosition;
uniform vec3  pointLightColor;
uniform float pointLightConstant;
uniform float pointLightLinear;
uniform float pointLightQuadratic;

uniform vec3 cameraPosition;

varying vec2 fragmentUV;
varying vec3 worldPosition;
varying vec3 fragmentNormal;

//const vec4 diffuseSegment = vec4(0.1, 0.3, 0.6, 1.0);

vec3 doDirectionalLight(vec3 normal, vec3 viewDir) {
    vec3 ambientColor = ambientMaterial * directionalLightColor;

    vec3 lightDir = normalize(-directionalLight);

    float diffuseIntensity = max(0.0, dot(lightDir, normal));
    // if (diffuseIntensity < diffuseSegment.x) {
    //     diffuseIntensity = diffuseSegment.x;
    // } else if (diffuseIntensity < diffuseSegment.y) {
    //     diffuseIntensity = diffuseSegment.y;
    // } else if (diffuseIntensity < diffuseSegment.z) {
    //     diffuseIntensity = diffuseSegment.z;
    // } else {
    //     diffuseIntensity = diffuseSegment.w;
    // }
    vec3 diffuseColor = directionalLightColor * diffuseMaterial * diffuseIntensity;

    vec3 reflectDir    = normalize(reflect(-lightDir, normal));
    vec3 specularColor = directionalLightColor * specularMaterial * pow(max(0.0, dot(viewDir, reflectDir)), 1.0);
    return ambientColor + diffuseColor + specularColor;
}

vec3 doPointLight(vec3 normal, vec3 viewDir) {
    vec3  lightDir          = normalize(pointLightPosition - worldPosition);
    float diffuseIntensity  = max(dot(normal, lightDir), 0.0);
    vec3  reflectDir        = reflect(-lightDir, normal);
    float specularIntensity = pow(max(0.0, dot(viewDir, reflectDir)), 1.0);

    float distance = length(pointLightPosition - worldPosition);
    float attenuation =
        1.0 / (pointLightConstant + pointLightLinear * distance + pointLightQuadratic * (distance * distance));
    vec3 ambientColor  = pointLightColor * ambientMaterial * attenuation;
    vec3 diffuseColor  = pointLightColor * diffuseMaterial * diffuseIntensity * attenuation;
    vec3 specularColor = pointLightColor * specularMaterial * specularIntensity * attenuation;

    return ambientColor + diffuseColor + specularColor;
}

void main() {
    vec3 normal  = normalize(fragmentNormal);
    vec3 viewDir = normalize(cameraPosition - worldPosition);

    vec3 result = doPointLight(normal, viewDir) + doDirectionalLight(normal, viewDir);
    // result += doPointLight(normal, viewDir);
    gl_FragColor = vec4(result, 1.0) * texture2D(sampler, fragmentUV);
}