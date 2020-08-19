#ifdef GL_ES
precision mediump float;
#endif

uniform vec4 U_LightAmbient;      //环境光
uniform vec4 U_LightPosition;     //平行光位置
uniform vec4 U_LightDiffuse;      //光线漫反射分量
uniform vec4 U_LightController;   //额外参数 可调节的参数
uniform vec4 U_LightSpecular;     //镜面反射光的分量
uniform vec4 U_SpecularMaterial;  //材质的镜面反射系数
uniform vec4 U_DiffuseMaterial;   //模型漫反射系数
uniform vec4 U_AmbientMaterial;   //模型表面材质的反射系数
uniform vec4 U_CameraPos;         // 摄像机坐标

varying vec3 V_Normal;
varying vec4 V_WorldPos;  //模型受光点世界坐标轴的位置

// vec4 ambientColor = U_AmbientMaterial * U_LightAmbient;

void main() {
    vec4 ambientColor = U_AmbientMaterial * U_LightAmbient;

    //光源是方向光
    vec3 DirectionalLight = U_LightPosition.xyz;

    DirectionalLight = normalize(DirectionalLight);  //单位化
    vec3 nor         = normalize(V_Normal.xyz);      //单位化

    //漫反射强度
    float diffuseIntensity = max(0.0, dot(DirectionalLight, nor));

    vec4 diffuseColor = U_LightDiffuse * U_DiffuseMaterial * diffuseIntensity;

    vec4 specularColor = vec4(0.0, 0.0, 0.0, 0.0);  //黑色
                                                    //当镜面反射不为零的时候我们才认为它具有反射光
    if (diffuseIntensity != 0.0) {
        // reflect 是shader内置函数
        vec3 reflectDir = normalize(reflect(-DirectionalLight, nor));
        vec3 viewDir    = normalize(U_CameraPos.xyz - V_WorldPos.xyz);
        specularColor =
            U_LightSpecular * U_SpecularMaterial * pow(max(0.0, dot(viewDir, reflectDir)), U_LightController.x);
    }

    // Finialize result
    gl_FragColor = ambientColor + diffuseColor + specularColor;
}