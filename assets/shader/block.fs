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
    vec3 ambientColor = ambientMaterial * ambientLight;

    vec3 lightDir = normalize(-directionalLight);
    vec3 normal   = normalize(fragmentNormal);

    float diffuseIntensity = max(0.0, dot(lightDir, normal));
    vec3  diffuseColor     = diffuseLight * diffuseMaterial * diffuseIntensity;

    vec3 reflectDir    = normalize(reflect(-lightDir, normal));
    vec3 viewDir       = normalize(cameraPosition - worldPosition);
    vec3 specularColor = specularLight * specularMaterial * pow(max(0.0, dot(viewDir, reflectDir)), 1.0);

    vec4 color   = texture2D(sampler, fragmentUV);
    gl_FragColor = vec4(ambientColor + diffuseColor + specularColor, 1.0) * color;
}