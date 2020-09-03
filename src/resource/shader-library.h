#pragma once
#ifndef NYAA_RESOURCE_SHADER_LIBRARY_H_
#define NYAA_RESOURCE_SHADER_LIBRARY_H_

#include "game/matrix.h"
#include "base/arena.h"
#include <string>
#include <type_traits>

namespace nyaa {

namespace res {

class DemoShaderProgram;
class BillboardShaderProgram;
class BlockShaderProgram;

class ShaderLibrary final {
public:
    static const char kShaderDir[];
    static const char kShaderDefFileName[];

    ShaderLibrary(base::Arena *arena) : arena_(arena) {}

    DEF_PTR_GETTER(DemoShaderProgram, demo_program);
    DEF_PTR_GETTER(BillboardShaderProgram, billboard_program);
    DEF_PTR_GETTER(BlockShaderProgram, block_program);

    bool Prepare(const std::string &dir);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ShaderLibrary);

private:
    bool MakeProgram(uint32_t shader1, uint32_t shader2, uint32_t *handle);
    bool MakeShader(const std::string &file_name, int type, uint32_t *handle);
    bool ReadAll(const std::string &file_name, std::string *content);

    base::Arena *const      arena_;
    DemoShaderProgram *     demo_program_;
    BillboardShaderProgram *billboard_program_;
    BlockShaderProgram *    block_program_;
};  // class ShaderLibrary

class ShaderProgram : public base::ArenaObject {
public:
    DEF_VAL_GETTER(uint32_t, handle);

    void Use();
    void Unuse();

    void SetAttribute(int location, int size, int stride, int offset);

    virtual void Enable(){};
    virtual void Disable(){};

    DISALLOW_IMPLICIT_CONSTRUCTORS(ShaderProgram);

protected:
    ShaderProgram(uint32_t program) : handle_(program) { DCHECK_NE(-1, handle_); }

    uint32_t handle_;
};  // ShaderProgram

template <class T, class = std::enable_if_t<std::is_base_of<ShaderProgram, T>::value>>
class ShaderProgramScope {
public:
    explicit ShaderProgramScope(T *program) : program_(program) { program_->Use(); }

    ~ShaderProgramScope() { program_->Unuse(); }

    T *operator->() { return program_; }

    DISALLOW_IMPLICIT_CONSTRUCTORS(ShaderProgramScope);

private:
    T *const program_;
};  // class ShaderProgramScope

class UniversalShaderProgram : public ShaderProgram {
public:
    DEF_VAL_GETTER(int, projection_matrix);
    DEF_VAL_GETTER(int, view_matrix);
    DEF_VAL_GETTER(int, model_matrix);

    void SetProjectionMatrix(const Matrix<float> &mat);
    void SetViewMatrix(const Matrix<float> &mat);
    void SetModelMatrix(const Matrix<float> &mat);

    friend class ShaderLibrary;
    DISALLOW_IMPLICIT_CONSTRUCTORS(UniversalShaderProgram);

protected:
    UniversalShaderProgram(uint32_t program);

private:
    int projection_matrix_;
    int view_matrix_;
    int model_matrix_;
};  // class UniversalShaderProgram

class DemoShaderProgram : public UniversalShaderProgram {
public:
    DEF_VAL_GETTER(int, position);
    DEF_VAL_GETTER(int, normal);
    DEF_VAL_GETTER(int, uv);

    void Enable() override;
    void Disable() override;

    friend class ShaderLibrary;
    DISALLOW_IMPLICIT_CONSTRUCTORS(DemoShaderProgram);

private:
    DemoShaderProgram(uint32_t program);

    int position_;
    int normal_;
    int uv_;
};  // class BlockShaderProgram

class BillboardShaderProgram : public UniversalShaderProgram {
public:
    DEF_VAL_GETTER(int, uv);
    DEF_VAL_GETTER(int, position);
    DEF_VAL_GETTER(int, center_position);
    DEF_VAL_GETTER(int, size);
    DEF_VAL_GETTER(int, paint_color);

    void SetCenterPosition(const Vector3f &pos);
    void SetPaintColor(const Vector3f &color);
    void SetSize(const Vector2f &size);

    void Enable() override;
    void Disable() override;

    friend class ShaderLibrary;
    DISALLOW_IMPLICIT_CONSTRUCTORS(BillboardShaderProgram);

private:
    BillboardShaderProgram(uint32_t program);

    int center_position_;
    int paint_color_;
    int size_;
    int position_;
    int uv_;
};  // class BillboardShaderProgram

class BlockShaderProgram : public UniversalShaderProgram {
public:
    DEF_VAL_GETTER(int, directional_light);
    DEF_VAL_GETTER(int, directional_light_color);
    DEF_VAL_GETTER(int, point_light_position);
    DEF_VAL_GETTER(int, point_light_color);
    DEF_VAL_GETTER(int, point_light_constant);
    DEF_VAL_GETTER(int, point_light_linear);
    DEF_VAL_GETTER(int, point_light_quadratic);
    DEF_VAL_GETTER(int, camera_position);
    DEF_VAL_GETTER(int, ambient_material);
    DEF_VAL_GETTER(int, diffuse_material);
    DEF_VAL_GETTER(int, specular_material);
    DEF_VAL_GETTER(int, position);
    DEF_VAL_GETTER(int, normal);
    DEF_VAL_GETTER(int, uv);

    // distance  constant  linear  quadratic
    // 7  1.0  0.7  1.8
    // 13  1.0  0.35  0.44
    // 20  1.0  0.22  0.20
    // 32  1.0  0.14  0.07
    // 50  1.0  0.09  0.032
    // 65  1.0  0.07  0.017
    // 100  1.0  0.045  0.0075
    // 160  1.0  0.027  0.0028
    // 200  1.0  0.022  0.0019
    // 325  1.0  0.014  0.0007
    // 600  1.0  0.007  0.0002
    // 3250  1.0  0.0014  0.000007
    void SetDirectionalLight(const Vector3f &value);
    void SetDirectionalLightColor(const Vector3f &value);
    void SetPointLightPosition(const Vector3f &value);
    void SetPointLightColor(const Vector3f &value);
    void SetPointLightConstant(float value);
    void SetPointLightLinear(float value);
    void SetPointLightQuadratic(float value);
    void SetCameraPosition(const Vector3f &value);

    void SetAmbientMaterial(const Vector3f &value);
    void SetDiffuseMaterial(const Vector3f &value);
    void SetSpecularMaterial(const Vector3f &value);

    void Enable() override;
    void Disable() override;

    friend class ShaderLibrary;
    DISALLOW_IMPLICIT_CONSTRUCTORS(BlockShaderProgram);

private:
    BlockShaderProgram(uint32_t program);
    // uniform vec3 ambientMaterial;
    // uniform vec3 ambientLight;
    // uniform vec3 diffuseMaterial;
    // uniform vec3 diffuseLight;
    // uniform vec3 specularMaterial;
    // uniform vec3 specularLight;
    // uniform vec3 directionalLight;
    // uniform vec3 cameraPosition;
    // uniform float pointLightConstant;
    // uniform float pointLightLinear;
    // uniform float pointLightQuadratic;
    int directional_light_;
    int directional_light_color_;
    int point_light_position_;
    int point_light_color_;
    int point_light_constant_;
    int point_light_linear_;
    int point_light_quadratic_;
    int camera_position_;

    int ambient_material_;
    int diffuse_material_;
    int specular_material_;

    int position_;
    int normal_;
    int uv_;
};  // class BlockShaderProgram

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_SHADER_LIBRARY_H_