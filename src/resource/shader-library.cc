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

const char kBillboardFSFileName[] = "billboard.fs";
const char kBillboardVSFileName[] = "billboard.vs";

const char kBlockFSFileName[] = "block.fs";
const char kBlockVSFileName[] = "block.vs";

const char kTextFSFileName[] = "text.fs";
const char kTextVSFileName[] = "text.vs";

void ShaderProgram::Use() { glUseProgram(handle()); }
void ShaderProgram::Unuse() { glUseProgram(0); }

void ShaderProgram::SetAttribute(int location, int size, int stride, int offset) {
    DCHECK_NE(-1, location);
    glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat),
                          reinterpret_cast<void *>(offset * sizeof(GLfloat)));
}

UniversalShaderProgram::UniversalShaderProgram(uint32_t program)
    : ShaderProgram(program)
    , projection_matrix_(glGetUniformLocation(handle_, "projectionMatrix"))
    , view_matrix_(glGetUniformLocation(handle_, "viewMatrix"))
    , model_matrix_(glGetUniformLocation(handle_, "modelMatrix")) {}

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

DemoShaderProgram::DemoShaderProgram(uint32_t program)
    : UniversalShaderProgram(program)
    , position_(glGetAttribLocation(handle_, "position"))
    , normal_(glGetAttribLocation(handle_, "normal"))
    , uv_(glGetAttribLocation(handle_, "uv")) {}

void DemoShaderProgram::Enable() /*override*/ {
    glEnableVertexAttribArray(position());
    glEnableVertexAttribArray(normal());
    glEnableVertexAttribArray(uv());
    SetAttribute(position(), 4, 8, 0);
    SetAttribute(normal(), 4, 8, 3);
    SetAttribute(uv(), 4, 8, 6);
}

void DemoShaderProgram::Disable() /*override*/ {
    glDisableVertexAttribArray(position());
    glDisableVertexAttribArray(normal());
    glDisableVertexAttribArray(uv());
}

BillboardShaderProgram::BillboardShaderProgram(uint32_t program)
    : UniversalShaderProgram(program)
    , center_position_(glGetUniformLocation(handle_, "billboardCenterPosition"))
    , paint_color_(glGetUniformLocation(handle_, "paintColor"))
    , size_(glGetUniformLocation(handle_, "billboardSize"))
    , position_(glGetAttribLocation(handle_, "position"))
    , uv_(glGetAttribLocation(handle_, "uv")) {}

void BillboardShaderProgram::SetCenterPosition(const Vector3f &pos) {
    DCHECK_NE(-1, center_position());
    glUniform3f(center_position(), pos.x, pos.y, pos.z);
}

void BillboardShaderProgram::SetPaintColor(const Vector3f &color) {
    DCHECK_NE(-1, paint_color());
    glUniform3f(paint_color(), color.x, color.y, color.z);
}

void BillboardShaderProgram::SetSize(const Vector2f &input) {
    DCHECK_NE(-1, size());
    glUniform2f(size(), input.x, input.y);
}

void BillboardShaderProgram::Enable() /*override*/ {
    glEnableVertexAttribArray(position());
    glEnableVertexAttribArray(uv());
    SetAttribute(position(), 4, 5, 0);
    SetAttribute(uv(), 4, 5, 3);
}

void BillboardShaderProgram::Disable() /*override*/ {
    glDisableVertexAttribArray(position());
    glDisableVertexAttribArray(uv());
}

BlockShaderProgram::BlockShaderProgram(uint32_t program)
    : UniversalShaderProgram(program)
    , directional_light_(glGetUniformLocation(program, "directionalLight"))
    , directional_light_color_(glGetUniformLocation(program, "directionalLightColor"))
    , point_light_position_(glGetUniformLocation(program, "pointLightPosition"))
    , point_light_color_(glGetUniformLocation(program, "pointLightColor"))
    , point_light_constant_(glGetUniformLocation(program, "pointLightConstant"))
    , point_light_linear_(glGetUniformLocation(program, "pointLightLinear"))
    , point_light_quadratic_(glGetUniformLocation(program, "pointLightQuadratic"))
    , camera_position_(glGetUniformLocation(program, "cameraPosition"))
    , ambient_material_(glGetUniformLocation(program, "ambientMaterial"))
    , diffuse_material_(glGetUniformLocation(program, "diffuseMaterial"))
    , specular_material_(glGetUniformLocation(program, "specularMaterial"))
    , position_(glGetAttribLocation(handle_, "position"))
    , normal_(glGetAttribLocation(handle_, "normal"))
    , uv_(glGetAttribLocation(handle_, "uv")) {}

void BlockShaderProgram::Enable() /*override*/ {
    glEnableVertexAttribArray(position());
    glEnableVertexAttribArray(normal());
    glEnableVertexAttribArray(uv());
    SetAttribute(position(), 4, 8, 0);
    SetAttribute(normal(), 4, 8, 3);
    SetAttribute(uv(), 4, 8, 6);
}

void BlockShaderProgram::Disable() /*override*/ {
    glDisableVertexAttribArray(position());
    glDisableVertexAttribArray(normal());
    glDisableVertexAttribArray(uv());
}

void BlockShaderProgram::SetDirectionalLight(const Vector3f &value) {
    DCHECK_NE(-1, directional_light());
    glUniform3f(directional_light(), value.x, value.y, value.z);
}

void BlockShaderProgram::SetDirectionalLightColor(const Vector3f &value) {
    DCHECK_NE(-1, directional_light_color());
    glUniform3f(directional_light_color(), value.x, value.y, value.z);
}

void BlockShaderProgram::SetPointLightPosition(const Vector3f &value) {
    DCHECK_NE(-1, point_light_position());
    glUniform3f(point_light_position(), value.x, value.y, value.z);
}

void BlockShaderProgram::SetPointLightColor(const Vector3f &value) {
    DCHECK_NE(-1, point_light_color());
    glUniform3f(point_light_color(), value.x, value.y, value.z);
}

void BlockShaderProgram::SetPointLightConstant(float value) {
    DCHECK_NE(-1, point_light_constant());
    glUniform1f(point_light_constant(), value);
}

void BlockShaderProgram::SetPointLightLinear(float value) {
    DCHECK_NE(-1, point_light_linear());
    glUniform1f(point_light_linear(), value);
}

void BlockShaderProgram::SetPointLightQuadratic(float value) {
    DCHECK_NE(-1, point_light_quadratic());
    glUniform1f(point_light_quadratic(), value);
}

void BlockShaderProgram::SetCameraPosition(const Vector3f &value) {
    DCHECK_NE(-1, camera_position());
    glUniform3f(camera_position(), value.x, value.y, value.z);
}

void BlockShaderProgram::SetAmbientMaterial(const Vector3f &value) {
    DCHECK_NE(-1, ambient_material());
    glUniform3f(ambient_material(), value.x, value.y, value.z);
}

void BlockShaderProgram::SetDiffuseMaterial(const Vector3f &value) {
    DCHECK_NE(-1, diffuse_material());
    glUniform3f(diffuse_material(), value.x, value.y, value.z);
}

void BlockShaderProgram::SetSpecularMaterial(const Vector3f &value) {
    DCHECK_NE(-1, specular_material());
    glUniform3f(specular_material(), value.x, value.y, value.z);
}

TextShaderProgram::TextShaderProgram(uint32_t program)
    : ShaderProgram(program)
    , projection_matrix_(glGetUniformLocation(handle_, "projectionMatrix"))
    , view_model_matrix_(glGetUniformLocation(handle_, "viewModelMatrix"))
    , text_color_(glGetUniformLocation(handle_, "textColor"))
    , position_(glGetAttribLocation(handle_, "position"))
    , uv_(glGetAttribLocation(handle_, "uv")) {}

void TextShaderProgram::SetProjectionMatrix(const Matrix<float> &mat) {
    DCHECK_NE(-1, projection_matrix());
    glUniformMatrix4fv(projection_matrix(), 1, GL_FALSE, mat.values());
}

void TextShaderProgram::SetViewModelMatrix(const Matrix<float> &mat) {
    DCHECK_NE(-1, view_model_matrix());
    glUniformMatrix4fv(view_model_matrix(), 1, GL_FALSE, mat.values());
}

void TextShaderProgram::SetTextColor(const Vector4f &color) {
    DCHECK_NE(-1, text_color());
    glUniform4fv(text_color(), 1, &color.x);
}

void TextShaderProgram::Enable() {
    glEnableVertexAttribArray(position());
    glEnableVertexAttribArray(uv());
    SetAttribute(position(), 4, 5, 0);
    SetAttribute(uv(), 4, 5, 3);
}
void TextShaderProgram::Disable() {
    glDisableVertexAttribArray(position());
    glDisableVertexAttribArray(uv());
}

bool ShaderLibrary::Prepare(const std::string &dir) {
    uint32_t vs, fs, program;
    if (!MakeShader(dir + "/" + kDemoVSFileName, GL_VERTEX_SHADER, &vs)) { return false; }
    if (!MakeShader(dir + "/" + kDemoFSFileName, GL_FRAGMENT_SHADER, &fs)) { return false; }
    if (!MakeProgram(vs, fs, &program)) { return false; }
    demo_program_ = new (arena_) DemoShaderProgram(program);

    if (!MakeShader(dir + "/" + kBillboardVSFileName, GL_VERTEX_SHADER, &vs)) { return false; }
    if (!MakeShader(dir + "/" + kBillboardFSFileName, GL_FRAGMENT_SHADER, &fs)) { return false; }
    if (!MakeProgram(vs, fs, &program)) { return false; }
    billboard_program_ = new (arena_) BillboardShaderProgram(program);

    if (!MakeShader(dir + "/" + kBlockVSFileName, GL_VERTEX_SHADER, &vs)) { return false; }
    if (!MakeShader(dir + "/" + kBlockFSFileName, GL_FRAGMENT_SHADER, &fs)) { return false; }
    if (!MakeProgram(vs, fs, &program)) { return false; }
    block_program_ = new (arena_) BlockShaderProgram(program);

    if (!MakeShader(dir + "/" + kTextVSFileName, GL_VERTEX_SHADER, &vs)) { return false; }
    if (!MakeShader(dir + "/" + kTextFSFileName, GL_FRAGMENT_SHADER, &fs)) { return false; }
    if (!MakeProgram(vs, fs, &program)) { return false; }
    text_program_ = new (arena_) TextShaderProgram(program);
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
