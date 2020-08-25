#include "resource/shader-library.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <stdio.h>

namespace nyaa {

namespace res {

const char ShaderLibrary::kShaderDir[]         = "shader";
const char ShaderLibrary::kShaderDefFileName[] = "shader/def.txt";

const char kDemoFSFileName[] = "demo.fs";
const char kDemoVSFileName[] = "demo.vs";

const char kSimpleLightFSFileName[] = "simple-light.fs";
const char kSimpleLightVSFileName[] = "simple-light.vs";

void ShaderProgram::Use() { glUseProgram(handle()); }
void ShaderProgram::Unuse() { glUseProgram(0); }

UniversalShaderProgram::UniversalShaderProgram(uint32_t program) : ShaderProgram(program) {
    projection_matrix_ = glGetUniformLocation(handle_, "projectionMatrix");
    view_matrix_       = glGetUniformLocation(handle_, "viewMatrix");
    model_matrix_      = glGetUniformLocation(handle_, "modelMatrix");
    position_          = glGetAttribLocation(handle_, "position");
    normal_            = glGetAttribLocation(handle_, "normal");
    uv_                = glGetAttribLocation(handle_, "uv");
}

void UniversalShaderProgram::SetProjectionMatrix(const Matrix<float> &mat) {
    DCHECK_NE(-1, projection_matrix());
    glUniformMatrix4fv(projection_matrix(), 1, GL_FALSE, mat.values());
}

void UniversalShaderProgram::SetViewMatrix(const Matrix<float> &mat) {
    DCHECK_NE(-1, view_matrix());
    glUniformMatrix4fv(view_matrix(), 1, GL_FALSE, mat.values());
}

void UniversalShaderProgram::SetModelMatrix(const Matrix<float> &mat) {
    DCHECK_NE(-1, model_matrix());
    glUniformMatrix4fv(model_matrix(), 1, GL_FALSE, mat.values());
}

void UniversalShaderProgram::SetPositionAttribute(int size, int stride, int offset) {
    DCHECK_NE(-1, position());
    glVertexAttribPointer(position(), size, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat),
                          reinterpret_cast<void *>(offset * sizeof(GLfloat)));
}

void UniversalShaderProgram::SetNormalAttribute(int size, int stride, int offset) {
    DCHECK_NE(-1, normal());
    glVertexAttribPointer(normal(), size, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat),
                          reinterpret_cast<void *>(offset * sizeof(GLfloat)));
}

void UniversalShaderProgram::SetUVAttribute(int size, int stride, int offset) {
    DCHECK_NE(-1, uv());
    glVertexAttribPointer(uv(), size, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat),
                          reinterpret_cast<void *>(offset * sizeof(GLfloat)));
}

void UniversalShaderProgram::Enable() /*override*/ {
    glEnableVertexAttribArray(position());
    glEnableVertexAttribArray(normal());
    glEnableVertexAttribArray(uv());
}

void UniversalShaderProgram::Disable() /*override*/ {
    glDisableVertexAttribArray(position());
    glDisableVertexAttribArray(normal());
    glDisableVertexAttribArray(uv());
}

DemoShaderProgram::DemoShaderProgram(uint32_t program) : UniversalShaderProgram(program) {}

bool ShaderLibrary::Prepare(const std::string &dir) {
    uint32_t vs, fs, program;
    if (!MakeShader(dir + "/" + kDemoVSFileName, GL_VERTEX_SHADER, &vs)) { return false; }
    if (!MakeShader(dir + "/" + kDemoFSFileName, GL_FRAGMENT_SHADER, &fs)) { return false; }
    if (!MakeProgram(vs, fs, &program)) { return false; }
    demo_program_ = new (arena_) DemoShaderProgram(program);

    return true;
}

bool ShaderLibrary::MakeProgram(uint32_t shader1, uint32_t shader2, uint32_t *handle) {
    GLuint program = glCreateProgram();
    glAttachShader(program, shader1);
    glAttachShader(program, shader2);
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
    glDetachShader(program, shader2);
    glDeleteShader(shader1);
    glDeleteShader(shader2);
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
