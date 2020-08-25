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

class ShaderLibrary final {
public:
    static const char kShaderDir[];
    static const char kShaderDefFileName[];

    ShaderLibrary(base::Arena *arena) : arena_(arena) {}

    DEF_PTR_GETTER(DemoShaderProgram, demo_program);

    bool Prepare(const std::string &dir);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ShaderLibrary);

private:
    bool MakeProgram(uint32_t shader1, uint32_t shader2, uint32_t *handle);
    bool MakeShader(const std::string &file_name, int type, uint32_t *handle);
    bool ReadAll(const std::string &file_name, std::string *content);

    base::Arena *const arena_;
    DemoShaderProgram *demo_program_;
};  // class ShaderLibrary

class ShaderProgram : public base::ArenaObject {
public:
    DEF_VAL_GETTER(uint32_t, handle);

    void Use();
    void Unuse();

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
    explicit ShaderProgramScope(T *program) : program_(program) {
        program_->Use();
        program_->Enable();
    }

    ~ShaderProgramScope() {
        program_->Disable();
        program_->Unuse();
    }

    T *operator->() { return program_; }

    DISALLOW_IMPLICIT_CONSTRUCTORS(ShaderProgramScope);

private:
    T *const program_;
}; // class ShaderProgramScope

class UniversalShaderProgram : public ShaderProgram {
public:
    DEF_VAL_GETTER(int, projection_matrix);
    DEF_VAL_GETTER(int, view_matrix);
    DEF_VAL_GETTER(int, model_matrix);
    DEF_VAL_GETTER(int, position);
    DEF_VAL_GETTER(int, normal);
    DEF_VAL_GETTER(int, uv);

    void SetProjectionMatrix(const Matrix<float> &mat);
    void SetViewMatrix(const Matrix<float> &mat);
    void SetModelMatrix(const Matrix<float> &mat);

    void SetPositionAttribute(int size, int stride, int offset);
    void SetNormalAttribute(int size, int stride, int offset);
    void SetUVAttribute(int size, int stride, int offset);

    void Enable() override;
    void Disable() override;

    friend class ShaderLibrary;
    DISALLOW_IMPLICIT_CONSTRUCTORS(UniversalShaderProgram);

protected:
    UniversalShaderProgram(uint32_t program);

private:
    int projection_matrix_;
    int view_matrix_;
    int model_matrix_;
    int position_;
    int normal_;
    int uv_;
};  // class UniversalShaderProgram

class DemoShaderProgram : public UniversalShaderProgram {
public:
    friend class ShaderLibrary;
    DISALLOW_IMPLICIT_CONSTRUCTORS(DemoShaderProgram);

private:
    DemoShaderProgram(uint32_t program);

};  // class BlockShaderProgram

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_SHADER_LIBRARY_H_