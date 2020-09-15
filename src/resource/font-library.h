#pragma once
#ifndef NYAA_RESOURCE_FONT_SET_H_
#define NYAA_RESOURCE_FONT_SET_H_

#include "resource/text-def-inl.h"
#include "game/vector.h"
#include "base/arena.h"
#include <unordered_map>
#include <string_view>
#include <memory>
#include <string>

struct FT_LibraryRec_;
struct FT_FaceRec_;

namespace nyaa {

namespace res {

class FontFace;

class FontLibrary final {
public:
    struct Options {
        std::string default_font_file;
        int         default_font_size = 0;
        std::string title_font_file;
        int         title_font_size = 0;
        std::string system_font_file;
        int         system_font_size = 0;
    };

    FontLibrary(base::Arena *arena);
    ~FontLibrary();

    FontFace *default_face() const { return default_face_.get(); }
    FontFace *system_face() const { return system_face_.get(); }

    bool Prepare(const Options &options);

    DISALLOW_IMPLICIT_CONSTRUCTORS(FontLibrary);

private:
    FT_LibraryRec_ *          lib_;
    std::unique_ptr<FontFace> default_face_;
    std::unique_ptr<FontFace> system_face_;
    base::Arena *const        arena_;
};  // class FontRender

class FontFace final {
public:
    static constexpr int   kBufferTexW  = 1024;
    static constexpr int   kBufferTexH  = 4096;
    static constexpr float kBufferTexWf = static_cast<float>(kBufferTexW);
    static constexpr float kBufferTexHf = static_cast<float>(kBufferTexH);

    ~FontFace();

    DEF_VAL_GETTER(uint32_t, buffered_tex);
    DEF_VAL_GETTER(int, pixel_size);

    Vector2f ApproximateSize(TextID id);
    Vector2f ApproximateSize(std::string_view text);

    Vector2f ApproximateSize(char32_t codepoint) {
        const Character *info = FindOrInsertCharacter(codepoint);
        return Vec2(info->advance >> 6, info->bearing.y);
    }

    Boundf Render(std::string_view text, float x, float y, Vector3f color = {1, 1, 1});
    Boundf Render(TextID id, float x, float y, Vector3f color = {1, 1, 1});
    Boundf Render(const Vector3f &pos, float scale, std::string_view text, std::vector<float> *vertices);
    Boundf Render(const Vector3f &pos, float scale, std::u32string_view text, std::vector<float> *vertices);

    Vector2f Render(const Vector3f &pos, float scale, char32_t codepoint, float vertices[20]);

    Vector2i RenderOutline(char32_t codepoint, const Vector3f &font_color, int outline_w, const Vector3f &outline_color,
                           std::vector<uint8_t> *pixels);

    struct Character : public base::ArenaObject {
        Character *next_ = nullptr;
        Character *prev_ = nullptr;
        uint32_t   code_point;
        Boundi     glyph;
        Vector2i   bearing;
        long       advance;
    };

    const Character *FindOrInsertCharacter(uint32_t code_point);

    friend class FontLibrary;

private:
    FontFace(FT_LibraryRec_ *owns, FT_FaceRec_ *face, int pixel_size, base::Arena *arena)
        : owns_(owns), face_(face), arena_(arena), pixel_size_(pixel_size) {
        chars_dummy_.next_ = &chars_dummy_;
        chars_dummy_.prev_ = &chars_dummy_;
    }

    void Prepare();

    FT_LibraryRec_ *const                     owns_;
    FT_FaceRec_ *                             face_;
    base::Arena *const                        arena_;
    const int                                 pixel_size_;
    int                                       last_x_offset_ = 0;
    int                                       last_y_offset_ = 0;
    uint32_t                                  buffered_tex_  = -1;
    std::unordered_map<uint32_t, Character *> buffered_chars_;
    Character                                 chars_dummy_;
};  // class FontFace

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_FONT_SET_H_