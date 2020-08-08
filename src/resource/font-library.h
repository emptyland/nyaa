#pragma once
#ifndef NYAA_RESOURCE_FONT_SET_H_
#define NYAA_RESOURCE_FONT_SET_H_

#include "resource/text-id.h"
#include "base/base.h"
#include <unordered_map>
#include <string_view>
#include <memory>
#include <vector>
#include <string>
#include <list>

struct FT_LibraryRec_;
struct FT_FaceRec_;

namespace nyaa {

namespace res {

class FontFace;

class FontLibrary final {
public:
    struct Options {
        std::string default_font_file;
        int default_font_size = 0;
        std::string title_font_file;
        int title_font_size = 0;
        std::string system_font_file;
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
    static constexpr int kBufferTexW = 1024;
    static constexpr int kBufferTexH = 4096;
    static constexpr float kBufferTexWf = static_cast<float>(kBufferTexW);
    static constexpr float kBufferTexHf = static_cast<float>(kBufferTexH);

    ~FontFace();

    void Render(std::string_view text, float x, float y, Vertex3f color = {1,1,1});
    void Render(TextID id, float x, float y, Vertex3f color = {1,1,1});

    friend class FontLibrary;
private:
    struct Character {
        Character *next_;
        Character *prev_;
        uint32_t code_point;
        Vertex4<uint32_t> glyph;
        Vertex2i bearing;
        long advance;
    };

    FontFace(FT_FaceRec_ *face, int pixel_size)
        : face_(face)
        , pixel_size_(pixel_size) {
        chars_dummy_.next_ = &chars_dummy_;
        chars_dummy_.prev_ = &chars_dummy_;
    }

    void Prepare();
    const Character *FindOrInsertCharacter(uint32_t code_point);

    FT_FaceRec_ *face_;
    const int pixel_size_;
    int last_x_offset_ = 0;
    int last_y_offset_ = 0;
    uint32_t buffered_tex_ = -1;
    std::unordered_map<uint32_t, Character *> buffered_chars_;
    Character chars_dummy_;
}; // class FontFace

} // namespace res 

} // namespace nyaa


#endif // NYAA_RESOURCE_FONT_SET_H_