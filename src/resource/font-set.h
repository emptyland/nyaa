#pragma once
#ifndef NYAA_RESOURCE_FONT_SET_H_
#define NYAA_RESOURCE_FONT_SET_H_

#include "base/base.h"
#include <unordered_map>
#include <string_view>
#include <memory>
#include <vector>

struct FT_LibraryRec_;
struct FT_FaceRec_;

namespace nyaa {

namespace res {

class FontFace;

class FontLibrary final {
public:
    struct Options {
        const char *default_font_file = nullptr;
        int default_font_size = 0;
        const char *title_font_file = nullptr;
        int title_font_size = 0;
        const char *system_font_file = nullptr;
        int system_font_size = 0;
    };

    FontLibrary();
    ~FontLibrary();

    FontFace *default_face() const { return default_face_.get(); }

    bool LoadFaces(const Options &options);

    DISALLOW_IMPLICIT_CONSTRUCTORS(FontLibrary);
private:
    FT_LibraryRec_ *lib_;
    std::unique_ptr<FontFace> default_face_;
}; // class FontRender

class FontFace final {
public:
    ~FontFace();

    void Render(std::string_view text, float x, float y);

    friend class FontLibrary;
private:
    struct Character {
        uint32_t tex_id;
        uint32_t glyph_x;
        uint32_t glyph_y;
        uint32_t glyph_w;
        uint32_t glyph_h;
        int bearing_w;
        int bearing_h;
        long advance;
    };

    FontFace(FT_FaceRec_ *face, int pixel_size): face_(face), pixel_size_(pixel_size) {}
    void Prepare();

    FT_FaceRec_ *face_;
    const int pixel_size_;
    int last_x_offset_ = 0;
    int last_y_offset_ = 0;
    std::vector<uint32_t> buffered_texs_;
    std::unordered_map<uint32_t, Character> buffered_chars_;
}; // class FontFace

} // namespace res 

} // namespace nyaa


#endif // NYAA_RESOURCE_FONT_SET_H_