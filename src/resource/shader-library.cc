#include "resource/shader-library.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <stdio.h>

namespace nyaa {

namespace res {

const char ShaderLibrary::kShaderDir[]         = "shader";
const char ShaderLibrary::kShaderDefFileName[] = "shader/def.txt";

const char kSimpleLightFSFileName[] = "simple-light.fs";
const char kSimpleLightVSFileName[] = "simple-light.vs";

/*
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

attribute vec2 tex2d;
attribute vec3 normal;
attribute vec3 position;
 */
struct SimpleLightParams {
    GLuint projection_matrix;
    GLuint modelView_matrix;
    GLuint tex2d;
    GLuint normal;
    GLuint position;
};

bool ShaderLibrary::Prepare(const std::string &dir) {
    uint32_t vs, fs, program;
    if (!MakeShader(dir + "/" + kSimpleLightVSFileName, GL_VERTEX_SHADER, &vs)) { return false; }
    //if (!MakeShader(dir + "/" + kSimpleLightFSFileName, GL_FRAGMENT_SHADER, &fs)) { return false; }
    if (!MakeProgram(vs, fs, &program)) { return false; }
    simple_light_program_ = program;

    return true;
}

bool ShaderLibrary::MakeProgram(uint32_t shader1, uint32_t shader2, uint32_t *handle) {
    GLuint program = glCreateProgram();
    glAttachShader(program, shader1);
    //glAttachShader(program, shader2);
    glLinkProgram(program);

    bool  ok = true;
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::unique_ptr<GLchar[]> info(new GLchar[length]);
        glGetProgramInfoLog(program, length, nullptr, info.get());
        DLOG(ERROR) << "Link program fail: " << info.get();
        ok = false;
    }
    glDetachShader(program, shader1);
    //glDetachShader(program, shader2);
    glDeleteShader(shader1);
    //glDeleteShader(shader2);
    if (ok) { *handle = program; }
    return ok;
}

bool ShaderLibrary::MakeShader(const std::string &file_name, int type, uint32_t *handle) {
    std::string content;
    if (!ReadAll(file_name, &content)) { return false; }

    GLuint      shader = glCreateShader(type);
    const char *source = content.c_str();
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::unique_ptr<GLchar[]> info(new GLchar[length]);
        glGetShaderInfoLog(shader, length, nullptr, info.get());
        DLOG(ERROR) << "Compile shader: " << file_name << " fail, Detail:\n  " << info.get();
        return false;
    }
    *handle = shader;
    return true;
}

bool ShaderLibrary::ReadAll(const std::string &file_name, std::string *content) {
    FILE *fp = nullptr;
    if (fp = ::fopen(file_name.c_str(), "rb"); !fp) {
        PLOG(ERROR) << "Can not open shader file: " << file_name;
        return false;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        PLOG(ERROR) << "Seek file fail: " << file_name;
        ::fclose(fp);
        return false;
    }
    long size = 0;
    if (size = ::ftell(fp); size < 0) {
        PLOG(ERROR) << "Tell file fail: " << file_name;
        ::fclose(fp);
        return false;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        PLOG(ERROR) << "Seek file fail: " << file_name;
        ::fclose(fp);
        return false;
    }
    content->resize(size);
    if (::fread(&(*content)[0], 1, size, fp) != size) {
        PLOG(ERROR) << "Read file fail: " << file_name;
        ::fclose(fp);
        return false;
    }

    ::fclose(fp);
    return true;
}

}  // namespace res

}  // namespace nyaa
