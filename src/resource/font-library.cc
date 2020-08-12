#include "resource/font-library.h"
#include "resource/text-library.h"
#include "game/game.h"
#include "base/slice.h"
#include "base/queue-macros.h"
#include "freetype2/ft2build.h"
#include FT_FREETYPE_H
#include "glog/logging.h"
#include <GL/glew.h>

namespace nyaa {

namespace res {

FontLibrary::FontLibrary(base::Arena *arena) : arena_(arena) {
    FT_Error err = FT_Init_FreeType(&lib_);
    (void)err;
}

FontLibrary::~FontLibrary() { FT_Done_FreeType(lib_); }

bool FontLibrary::LoadFaces(const FontLibrary::Options &options) {
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

void FontFace::Render(TextID id, float x, float y, Vertex3f color) {
    std::string_view text = ThisGame->text_lib()->Load(id);
    Render(text, x, y, color);
}

void FontFace::Render(std::string_view text, float x, float y, Vertex3f color) {
    std::vector<const Character *> chars;
    glBindTexture(GL_TEXTURE_2D, buffered_tex_);
    base::CodePointIteratorUtf8 iter(text);
    for (iter.SeekFirst(); iter.Valid(); iter.Next()) {
        const Character *info = FindOrInsertCharacter(iter.ToU32());
        chars.push_back(info);
    }

    ThisGame->EnterProjection2D();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor3f(color.x, color.y, color.z);
    glBegin(GL_QUADS);

    // TL TR | BL BR
    // BL BR | TL TR
    for (const Character *info : chars) {
        if (!info) { continue; }
        double x_pos = x + info->bearing.x;
        double y_pos = y - (info->glyph.h - info->bearing.y);

        double x_tex = static_cast<double>(info->glyph.x) / kBufferTexWf;
        double y_tex = static_cast<double>(info->glyph.y) / kBufferTexHf;
        glTexCoord2d(x_tex, y_tex);
        glVertex3d(x_pos, y_pos + info->glyph.h, 0);  // TL

        x_tex = static_cast<double>(info->glyph.x + info->glyph.w) / kBufferTexWf;
        y_tex = static_cast<double>(info->glyph.y) / kBufferTexHf;
        glTexCoord2d(x_tex, y_tex);
        glVertex3d(x_pos + info->glyph.w, y_pos + info->glyph.h, 0);  // TR

        x_tex = static_cast<double>(info->glyph.x + info->glyph.w) / kBufferTexWf;
        y_tex = static_cast<double>(info->glyph.y + info->glyph.h) / kBufferTexHf;
        glTexCoord2d(x_tex, y_tex);
        glVertex3d(x_pos + info->glyph.w, y_pos, 0);  // BR

        x_tex = static_cast<double>(info->glyph.x) / kBufferTexWf;
        y_tex = static_cast<double>(info->glyph.y + info->glyph.h) / kBufferTexHf;
        glTexCoord2d(x_tex, y_tex);
        glVertex3d(x_pos, y_pos, 0);  // BL

        x += (info->advance >> 6);
    }
    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    ThisGame->LeaveProjection2D();
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
