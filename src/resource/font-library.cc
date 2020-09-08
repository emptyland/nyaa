#include "resource/font-library.h"
#include "resource/text-library.h"
#include "system/geometry-transform-system.h"
#include "game/game.h"
#include "base/slice.h"
#include "base/queue-macros.h"
#include "freetype2/ft2build.h"
#include FT_FREETYPE_H
#include "glog/logging.h"
#include <GL/glew.h>
#include <numeric>

namespace nyaa {

namespace res {

FontLibrary::FontLibrary(base::Arena *arena) : arena_(arena) {
    FT_Error err = FT_Init_FreeType(&lib_);
    (void)err;
}

FontLibrary::~FontLibrary() { FT_Done_FreeType(lib_); }

bool FontLibrary::Prepare(const FontLibrary::Options &options) {
    FT_Face  face;
    FT_Error err = FT_New_Face(lib_, options.default_font_file.c_str(), 0, &face);
    if (err) {
        DLOG(ERROR) << "Load " << options.default_font_file << " font file fail!";
        return false;
    }
    FT_Select_Charmap(face, ft_encoding_unicode);
    FT_Set_Pixel_Sizes(face, 0, options.default_font_size);
    default_face_.reset(new FontFace(face, options.default_font_size, arena_));
    default_face_->Prepare();

    if (err = FT_New_Face(lib_, options.system_font_file.c_str(), 0, &face); err) {
        DLOG(ERROR) << "Load " << options.system_font_file << " font file fail!";
        return false;
    }
    FT_Select_Charmap(face, ft_encoding_unicode);
    FT_Set_Pixel_Sizes(face, 0, options.system_font_size);
    system_face_.reset(new FontFace(face, options.system_font_size, arena_));
    system_face_->Prepare();

    return true;
}

FontFace::~FontFace() { /*FT_Done_Face(face_)*/
    ;
}

void FontFace::Prepare() {
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, kBufferTexW, kBufferTexH, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
    buffered_tex_ = texture;

    for (int i = 1; i < 128; i++) {  // Prepare ASCII chars
        FindOrInsertCharacter(i);
    }
}

Vector2f FontFace::ApproximateSize(TextID id) {
    std::string_view text = Game::This()->text_lib()->Load(id);
    return ApproximateSize(text);
}

Vector2f FontFace::ApproximateSize(std::string_view text) {
    Vector2f                    size{0, 0};
    base::CodePointIteratorUtf8 iter(text);
    for (iter.SeekFirst(); iter.Valid(); iter.Next()) {
        const Character *info = FindOrInsertCharacter(iter.ToU32());
        size.x += (info->glyph.w + (info->advance >> 6));
        size.y = std::max(size.y, static_cast<float>(info->glyph.y + info->bearing.y));
    }
    return size;
}

Boundf FontFace::Render(TextID id, float x, float y, Vector3f color) {
    std::string_view text = Game::This()->text_lib()->Load(id);
    return Render(text, x, y, color);
}

Boundf FontFace::Render(std::string_view text, float x, float y, Vector3f color) {
    std::vector<float> vertices;
    vertices.reserve(text.size() * 5 * 4);
    Boundf bound = Render(text, x, y, 0, &vertices);

    Game::This()->transform()->Enter2DProjection();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, buffered_tex_);

    glColor3f(color.x, color.y, color.z);
    glBegin(GL_QUADS);

    for (int i = 0; i < vertices.size(); i += 5) {
        glTexCoord2f(vertices[i + 3], vertices[i + 4]);
        glVertex3f(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    Game::This()->transform()->Exit2DProjection();
    return bound;
}

Boundf FontFace::Render(std::string_view text, float x, float y, float z, std::vector<float> *receiver) {
    Boundf                      bound{x, y, 0, 0};
    base::CodePointIteratorUtf8 iter(text);
    for (iter.SeekFirst(); iter.Valid(); iter.Next()) {
        const Character *info = FindOrInsertCharacter(iter.ToU32());
        if (!info) { continue; }
        float x_pos = x + info->bearing.x;
        float y_pos = y - (info->glyph.h - info->bearing.y);

        size_t pos = receiver->size();
        receiver->resize(pos + 5 * 4);
        float *vertices = &receiver->at(pos);

        vertices[0] = x_pos;
        vertices[1] = y_pos + info->glyph.h;
        vertices[2] = z;
        vertices[3] = static_cast<float>(info->glyph.x) / kBufferTexWf;
        vertices[4] = static_cast<float>(info->glyph.y) / kBufferTexHf;

        vertices[5] = x_pos + info->glyph.w;
        vertices[6] = y_pos + info->glyph.h;
        vertices[7] = z;
        vertices[8] = static_cast<float>(info->glyph.x + info->glyph.w) / kBufferTexWf;
        vertices[9] = static_cast<float>(info->glyph.y) / kBufferTexHf;

        vertices[10] = x_pos + info->glyph.w;
        vertices[11] = y_pos;
        vertices[12] = z;
        vertices[13] = static_cast<float>(info->glyph.x + info->glyph.w) / kBufferTexWf;
        vertices[14] = static_cast<float>(info->glyph.y + info->glyph.h) / kBufferTexHf;

        vertices[15] = x_pos;
        vertices[16] = y_pos;
        vertices[17] = z;
        vertices[18] = static_cast<float>(info->glyph.x) / kBufferTexWf;
        vertices[19] = static_cast<float>(info->glyph.y + info->glyph.h) / kBufferTexHf;

        x += (info->advance >> 6);
        bound.h = std::max(bound.h, static_cast<float>(y_pos + info->glyph.h - y));

        // y - (info->glyph.h - info->bearing.y) + info->glyph.h
        // y - info->glyph.h + info->bearing.y + info->glyph.h
        // y + info->bearing.y
    }
    bound.w = x - bound.x;
    return bound;
}

Boundf FontFace::Render(const Vector3f &position, float scale, std::u32string_view text,
                        std::vector<float> *receiver) {
    Boundf bound{position.x, position.y, 0, 0};

    float cx = position.x;

    for (char32_t codepoint : text) {
        const Character *info = FindOrInsertCharacter(codepoint);
        if (!info) { continue; }

        float x = cx + info->bearing.x * scale;
        float y = position.y - (info->glyph.h - info->bearing.y) * scale;

        size_t pos = receiver->size();
        receiver->resize(pos + 5 * 4);
        float *vertices = &receiver->at(pos);

        vertices[0] = x;
        vertices[1] = y + info->glyph.h * scale;
        vertices[2] = position.z;
        vertices[3] = static_cast<float>(info->glyph.x) / kBufferTexWf;
        vertices[4] = static_cast<float>(info->glyph.y) / kBufferTexHf;

        vertices[5] = x + info->glyph.w * scale;
        vertices[6] = y + info->glyph.h * scale;
        vertices[7] = position.z;
        vertices[8] = static_cast<float>(info->glyph.x + info->glyph.w) / kBufferTexWf;
        vertices[9] = static_cast<float>(info->glyph.y) / kBufferTexHf;

        vertices[10] = x + info->glyph.w * scale;
        vertices[11] = y;
        vertices[12] = position.z;
        vertices[13] = static_cast<float>(info->glyph.x + info->glyph.w) / kBufferTexWf;
        vertices[14] = static_cast<float>(info->glyph.y + info->glyph.h) / kBufferTexHf;

        vertices[15] = x;
        vertices[16] = y;
        vertices[17] = position.z;
        vertices[18] = static_cast<float>(info->glyph.x) / kBufferTexWf;
        vertices[19] = static_cast<float>(info->glyph.y + info->glyph.h) / kBufferTexHf;

        cx += (info->advance >> 6) * scale;
        bound.h = std::max(bound.h, static_cast<float>(y + info->glyph.h - y));
    }
    bound.h *= scale;
    bound.w = cx - bound.x;
    return bound;
}

const FontFace::Character *FontFace::FindOrInsertCharacter(uint32_t code_point) {
    auto iter = buffered_chars_.find(code_point);
    if (iter != buffered_chars_.end()) {
        QUEUE_REMOVE(iter->second);
        QUEUE_INSERT_HEAD(&chars_dummy_, iter->second);
        return iter->second;
    }

    FT_Error err = FT_Load_Char(face_, code_point, FT_LOAD_RENDER);
    if (err) { return nullptr; }

    Character *info = nullptr;
    if (last_y_offset_ + pixel_size_ >= kBufferTexH) {
        last_x_offset_ = 0;
        info           = chars_dummy_.prev_;
        QUEUE_REMOVE(info);
        buffered_chars_.erase(info->code_point);
    } else {
        if (last_x_offset_ + pixel_size_ >= kBufferTexW) {
            last_y_offset_ += pixel_size_;
            last_x_offset_ = 0;
        }
        info          = new (arena_) Character;
        info->glyph.x = last_x_offset_;
        info->glyph.y = last_y_offset_;
        last_x_offset_ += pixel_size_;
    }
    info->code_point = code_point;
    info->glyph.w    = face_->glyph->bitmap.width;
    info->glyph.h    = face_->glyph->bitmap.rows;
    info->bearing.x  = face_->glyph->bitmap_left;
    info->bearing.y  = face_->glyph->bitmap_top;
    info->advance    = face_->glyph->advance.x;

    glBindTexture(GL_TEXTURE_2D, buffered_tex_);
    glDisable(GL_BLEND);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, info->glyph.x, info->glyph.y, info->glyph.w, info->glyph.h, GL_ALPHA,
                    GL_UNSIGNED_BYTE, face_->glyph->bitmap.buffer);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 0);

    QUEUE_INSERT_HEAD(&chars_dummy_, info);
    buffered_chars_[code_point] = info;

    return info;
}

}  // namespace res

}  // namespace nyaa
