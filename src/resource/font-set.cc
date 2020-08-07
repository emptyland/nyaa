#include "resource/font-set.h"
#include "game/game.h"
#include "freetype2/ft2build.h"
#include FT_FREETYPE_H
#include "glog/logging.h"
#include <GL/glew.h>

namespace nyaa {

namespace res {

FontLibrary::FontLibrary() {
    FT_Error err = FT_Init_FreeType(&lib_);
    (void)err;
}

FontLibrary::~FontLibrary() { FT_Done_FreeType(lib_); }

bool FontLibrary::LoadFaces(const FontLibrary::Options &options) {
    FT_Face face;
    FT_Error err = FT_New_Face(lib_, options.default_font_file, 0, &face);
    if (err) {
        DLOG(ERROR) << "Load " << options.default_font_file << " font file fail!";
        return false;
    }
    FT_Select_Charmap(face , ft_encoding_unicode);
    FT_Set_Pixel_Sizes(face, 0, options.default_font_size);
    default_face_.reset(new FontFace(face, options.default_font_size));
    default_face_->Prepare();

    return true;
}

FontFace::~FontFace() { /*FT_Done_Face(face_)*/; }

void FontFace::Prepare() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_ALPHA, 
        1024,
        1024, 
        0, 
        GL_ALPHA, 
        GL_UNSIGNED_BYTE, 
        0);
    
    for (int i = 1; i < 128; i++) { // Prepare ASCII chars
        FT_Error err = FT_Load_Char(face_, i, FT_LOAD_RENDER);
        if (err) {
            continue;
        }

        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            last_x_offset_,
            last_y_offset_,
            face_->glyph->bitmap.width,
            face_->glyph->bitmap.rows,
            GL_ALPHA,
            GL_UNSIGNED_BYTE,
            face_->glyph->bitmap.buffer
        );
        Character info{
            texture,
            static_cast<uint32_t>(last_x_offset_),
            static_cast<uint32_t>(last_y_offset_),
            face_->glyph->bitmap.width, face_->glyph->bitmap.rows,
            face_->glyph->bitmap_left, face_->glyph->bitmap_top,
            face_->glyph->advance.x
        };
        buffered_chars_[i] = info;

        last_x_offset_ += pixel_size_;
        if (last_x_offset_ > 1024 - pixel_size_) {
            last_x_offset_ = 0;
            last_y_offset_ += pixel_size_;
        }
    }

    buffered_texs_.push_back(texture);
}

void FontFace::Render(std::string_view text, float x, float y) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, ThisGame->fb_w(), 0, ThisGame->fb_h(), -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_CULL_FACE);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, buffered_texs_.front());
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_QUADS);

    // TL TR | BL BR
    // BL BR | TL TR
    for (size_t i = 0; i < text.size(); i++) {
        auto iter = buffered_chars_.find(text[i]);
        if (iter == buffered_chars_.end()) {
            continue;
        }

        const Character &info = iter->second;
        float x_pos = x + info.bearing_w;
        float y_pos = y - (info.glyph_h - info.bearing_h);

        float x_tex = static_cast<float>(info.glyph_x) / 1024.0;
        float y_tex = static_cast<float>(info.glyph_y) / 1024.0;
        glTexCoord2f(x_tex, y_tex);
        glVertex3f(x_pos, y_pos + info.glyph_h, 0); // TL

        x_tex = static_cast<float>(info.glyph_x + info.glyph_w) / 1024.0;
        y_tex = static_cast<float>(info.glyph_y) / 1024.0;
        glTexCoord2f(x_tex, y_tex);
        glVertex3f(x_pos + info.glyph_w, y_pos + info.glyph_h, 0); // TR

        x_tex = static_cast<float>(info.glyph_x + info.glyph_w) / 1024.0;
        y_tex = static_cast<float>(info.glyph_y + info.glyph_h) / 1024.0;
        glTexCoord2f(x_tex, y_tex);
        glVertex3f(x_pos + info.glyph_w, y_pos, 0); // BR

        x_tex = static_cast<float>(info.glyph_x) / 1024.0;
        y_tex = static_cast<float>(info.glyph_y + info.glyph_h) / 1024.0;
        glTexCoord2f(x_tex, y_tex);
        glVertex3f(x_pos, y_pos, 0); // BL

        x += (info.advance >> 6);
    }
    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

} // namespace res 
    
} // namespace nyaa
