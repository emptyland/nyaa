#pragma once
#ifndef NYAA_RESOURCE_SHADER_LIBRARY_H_
#define NYAA_RESOURCE_SHADER_LIBRARY_H_

#include "base/base.h"
#include <string>

namespace nyaa {

namespace res {

class ShaderLibrary final {
public:
    static const char kShaderDir[];
    static const char kShaderDefFileName[];

    ShaderLibrary() = default;

    bool Prepare(const std::string &dir);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ShaderLibrary);

private:
    bool MakeProgram(uint32_t shader1, uint32_t shader2, uint32_t *handle);
    bool MakeShader(const std::string &file_name, int type, uint32_t *handle);
    bool ReadAll(const std::string &file_name, std::string *content);

};  // class ShaderLibrary

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_SHADER_LIBRARY_H_