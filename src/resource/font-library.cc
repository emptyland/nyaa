#include "resource/font-library.h"
#include "resource/text-library.h"
#include "resource/texture-library.h"
#include "system/geometry-transform-system.h"
#include "game/game.h"
#include "base/slice.h"
#include "base/queue-macros.h"
#include "freetype2/ft2build.h"
#include FT_FREETYPE_H
#include FT_STROKER_H
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
    default_face_.reset(new FontFace(lib_, face, options.default_font_size, arena_));
    default_face_->Prepare();

    if (err = FT_New_Face(lib_, options.system_font_file.c_str(), 0, &face); err) {
        DLOG(ERROR) << "Load " << options.system_font_file << " font file fail!";
        return false;
    }
    FT_Select_Charmap(face, ft_encoding_unicode);
    FT_Set_Pixel_Sizes(face, 0, options.system_font_size);
    system_face_.reset(new FontFace(lib_, face, options.system_font_size, arena_));
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
        Vector2f s = ApproximateSize(*iter);
        size.x += s.x;
        size.y = std::max(size.y, s.y);
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
    Boundf bound = Render(Vec3(x, y, 0), 1.0, text, &vertices);

    System::This()->transform()->Enter2DProjection();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, buffered_tex_);

    glBegin(GL_QUADS);
    glColor3f(color.x, color.y, color.z);

    for (int i = 0; i < vertices.size(); i += 5) {
        glTexCoord2f(vertices[i + 3], vertices[i + 4]);
        glVertex3f(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    System::This()->transform()->Exit2DProjection();
    return bound;
}

Boundf FontFace::Render(const Vector3f &position, float scale, std::string_view text, std::vector<float> *receiver) {
    Boundf bound{position.x, position.y, 0, 0};

    float cx = position.x;

    base::CodePointIteratorUtf8 iter(text);
    for (iter.SeekFirst(); iter.Valid(); iter.Next()) {
        size_t pos = receiver->size();
        receiver->resize(pos + 5 * 4);
        float *vertices = &receiver->at(pos);

        Vector2f size = Render(Vec3(cx, position.y, position.z), scale, *iter, vertices);

        cx += size.x;
        bound.h = std::max(bound.h, size.y);
    }
    bound.w = cx - bound.x;
    return bound;
}

Boundf FontFace::Render(const Vector3f &position, float scale, std::u32string_view text, std::vector<float> *receiver) {
    Boundf bound{position.x, position.y, 0, 0};

    float cx = position.x;

    for (char32_t codepoint : text) {
        size_t pos = receiver->size();
        receiver->resize(pos + 5 * 4);
        float *vertices = &receiver->at(pos);

        Vector2f size = Render(Vec3(cx, position.y, position.z), scale, codepoint, vertices);

        cx += size.x;
        bound.h = std::max(bound.h, size.y);
    }
    bound.w = cx - bound.x;
    return bound;
}

Vector2f FontFace::Render(const Vector3f &position, float scale, char32_t codepoint, float vertices[20]) {
    const Character *info = FindOrInsertCharacter(codepoint);
    if (!info) { return Vec2(0, 0); }

    float x = position.x + info->bearing.x * scale;
    float y = position.y - (info->glyph.h - info->bearing.y) * scale;

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

    return Vec2((info->advance >> 6) * scale, info->bearing.y * scale);
}

struct Span {
    int x;
    int y;
    int width;
    int coverage;
};

struct Rect {
    int x0;
    int x1;
    int y0;
    int y1;

    void Include(Vector2i r) {
        x0 = std::min(x0, r.x);
        x1 = std::max(x1, r.x);
        y0 = std::min(y0, r.y);
        y1 = std::max(y1, r.y);
    }

    int w() const { return x1 - x0 + 1; }
    int h() const { return y1 - y0 + 1; }
};

void RasterCallback(const int y, const int count, const FT_Span *const spans, void *const user) {
    auto receiver = static_cast<std::vector<Span> *>(user);

    for (int i = 0; i < count; ++i) { receiver->push_back(Span{spans[i].x, y, spans[i].len, spans[i].coverage}); }
}

Vector2i FontFace::MakeOutlineTexture(std::string_view text, const Vector3f &font_color, int outline_w,
                                      const Vector3f &outline_color, uint32_t *tex) {
    struct Metadata {
        Boundi               bound;
        std::vector<uint8_t> pixels;
    };

    Vector2i total_size = {0, 0};
    int      bearing_y  = std::numeric_limits<int>::min();

    Character                   info;
    Vector2i                    size;
    std::vector<Metadata>       mds;
    base::CodePointIteratorUtf8 iter(text);
    for (iter.SeekFirst(); iter.Valid(); iter.Next()) {
        Metadata md;
        size = RenderOutline(*iter, font_color, outline_w, outline_color, &info, &md.pixels);

        md.bound.x = total_size.x + info.bearing.x;
        md.bound.y = info.bearing.y;
        md.bound.w = size.x;
        md.bound.h = size.y;

        total_size.x += (info.advance >> 6);
        total_size.y = std::max(total_size.y, md.bound.h);
        bearing_y    = std::max(bearing_y, info.glyph.h - info.bearing.y);
        mds.emplace_back(std::move(md));
    }
    total_size.x += size.x;
    total_size.y += bearing_y;

    glEnable(GL_TEXTURE_2D);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, total_size.x, total_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (const auto &md : mds) {
        // DLOG(INFO) << "md.y = " << md.bound.y << " md.h = " << md.bound.h;
        if (md.pixels.empty()) {
            continue;
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, md.bound.x, total_size.y - bearing_y - md.bound.y - outline_w * 2, md.bound.w,
                        md.bound.h, GL_RGBA, GL_UNSIGNED_BYTE, &md.pixels[0]);
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 0);

    *tex = texture;
    return total_size;
}

Vector2i FontFace::RenderOutline(char32_t codepoint, const Vector3f &font_color, int outline_w,
                                 const Vector3f &outline_color, Character *info, std::vector<uint8_t> *pixels) {

    FT_UInt  index = FT_Get_Char_Index(face_, codepoint);
    FT_Error err   = FT_Load_Glyph(face_, index, FT_LOAD_NO_BITMAP);
    DCHECK_EQ(err, 0);
    if (face_->glyph->format != FT_GLYPH_FORMAT_OUTLINE) { return {0, 0}; }
    if (info) {
        info->code_point = codepoint;
        info->glyph.w    = face_->glyph->bitmap.width;
        info->glyph.h    = face_->glyph->bitmap.rows;
        info->bearing.x  = face_->glyph->bitmap_left;
        info->bearing.y  = face_->glyph->bitmap_top;
        info->advance    = face_->glyph->advance.x;
    }

    std::vector<Span> spans;

    FT_Raster_Params params;
    memset(&params, 0, sizeof(params));
    params.flags      = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
    params.gray_spans = RasterCallback;
    params.user       = &spans;

    FT_Outline_Render(owns_, &face_->glyph->outline, &params);

    // Next we need the spans for the outline.

    // Set up a stroker.
    FT_Stroker stroker;
    FT_Stroker_New(owns_, &stroker);
    FT_Stroker_Set(stroker, static_cast<int>(outline_w * 64), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

    FT_Glyph glyph;
    err = FT_Get_Glyph(face_->glyph, &glyph);
    DCHECK_EQ(err, 0);

    FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
    // Again, this needs to be an outline to work.
    if (glyph->format != FT_GLYPH_FORMAT_OUTLINE) { return {0, 0}; }
    // Render the outline spans to the span list

    FT_Outline *outline = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;

    std::vector<Span> outline_spans;
    params.user = &outline_spans;
    FT_Outline_Render(owns_, outline, &params);

    // Clean up afterwards.
    FT_Stroker_Done(stroker);
    FT_Done_Glyph(glyph);

    if (spans.empty() || outline_spans.empty()) {
        return {static_cast<int>(face_->glyph->bitmap.width), static_cast<int>(face_->glyph->bitmap.rows)};
    }

    Rect rect{spans.front().x, spans.front().y, spans.front().x, spans.front().y};
    for (const auto &span : spans) {
        rect.Include({span.x, span.y});
        rect.Include({span.x + span.width - 1, span.y});
    }
    for (const auto &span : outline_spans) {
        rect.Include({span.x, span.y});
        rect.Include({span.x + span.width - 1, span.y});
    }

    pixels->resize(rect.w() * rect.h() * 4);

    for (const Span &span : outline_spans) {
        for (int w = 0; w < span.width; ++w) {
            int index = (rect.h() - 1 - (span.y - rect.y0)) * rect.w() + span.x - rect.x0 + w;

            (*pixels)[index * 4 + 0] = 255 * outline_color.x;
            (*pixels)[index * 4 + 1] = 255 * outline_color.y;
            (*pixels)[index * 4 + 2] = 255 * outline_color.z;
            (*pixels)[index * 4 + 3] = span.coverage;
        }
    }
    for (const Span &span : spans) {
        for (int w = 0; w < span.width; ++w) {
            int      index = (rect.h() - 1 - (span.y - rect.y0)) * rect.w() + span.x - rect.x0 + w;
            Vector4i src;
            src.x = 255 * font_color.x;
            src.y = 255 * font_color.y;
            src.z = 255 * font_color.z;
            src.w = span.coverage;

            Vector4i dst;
            dst.x = (*pixels)[index * 4 + 0];
            dst.y = (*pixels)[index * 4 + 1];
            dst.z = (*pixels)[index * 4 + 2];
            dst.w = (*pixels)[index * 4 + 3];

            dst.x = static_cast<int>(dst.x + ((src.x - dst.x) * src.w) / 255.0f);
            dst.y = static_cast<int>(dst.y + ((src.y - dst.y) * src.w) / 255.0f);
            dst.z = static_cast<int>(dst.z + ((src.z - dst.z) * src.w) / 255.0f);
            dst.w = std::min(255, dst.w + src.w);

            (*pixels)[index * 4 + 0] = dst.x;
            (*pixels)[index * 4 + 1] = dst.y;
            (*pixels)[index * 4 + 2] = dst.z;
            (*pixels)[index * 4 + 3] = dst.w;
        }
    }

    // TODO:
    return {rect.w(), rect.h()};
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
