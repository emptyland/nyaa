#pragma once
#ifndef NYAA_RESOURCE_FONT_SET_H_
#define NYAA_RESOURCE_FONT_SET_H_

#include "base/base.h"

struct FT_LibraryRec_;
struct FT_FaceRec_;

namespace nyaa {

namespace res {

class FontFace final {
public:

private:
    FT_FaceRec_ *face_;
}; // class FontFace

class FontLibrary final {
public:
    FontLibrary();
    ~FontLibrary();

    bool LoadFaces(const char *default_font_file,
                   const char *title_font_file);

private:
    FT_LibraryRec_ *lib_;
}; // class FontRender

    
} // namespace res 

} // namespace nyaa


#endif // NYAA_RESOURCE_FONT_SET_H_