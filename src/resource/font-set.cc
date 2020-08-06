#include "resource/font-set.h"
#include "freetype2/ft2build.h"
#include FT_FREETYPE_H


namespace nyaa {

namespace res {

FontLibrary::FontLibrary() {
    FT_Error err = FT_Init_FreeType(&lib_);
    (void)err;
}

FontLibrary::~FontLibrary() {
    FT_Face
    FT_Done_FreeType(lib_);
}
    
} // namespace res 
    
} // namespace nyaa
