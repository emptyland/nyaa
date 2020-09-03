#include "game/properties.h"
#include "base/slice.h"
#include "glog/logging.h"

namespace nyaa {

/*static*/ const Properties::PropertyDef Properties::defs_[] = {
#define PROPERTY_DEF(name, type) \
    { #name, sizeof(#name) - 1, offsetof(Properties, name##_), res::DefValType::type }
    PROPERTY_DEF(name, STRING),
    PROPERTY_DEF(assets_dir, STRING),
    PROPERTY_DEF(show_fps, I32),
    PROPERTY_DEF(language, STRING),
    PROPERTY_DEF(window_width, I32),
    PROPERTY_DEF(window_height, I32),
    PROPERTY_DEF(default_font_file, STRING),
    PROPERTY_DEF(default_font_size, I32),
    PROPERTY_DEF(system_font_file, STRING),
    PROPERTY_DEF(system_font_size, I32),
    {nullptr, 0},
#undef PROPERTY_DEF
};

void Properties::Print(base::AbstractPrinter *out) const {
    out->Println("----[properties]----");
    out->Println("name: %s", name().c_str());
    out->Println("assets_dir: %s", assets_dir().c_str());
    out->Println("language: %s", language().c_str());
    out->Println("window_width: %d", window_width());
    out->Println("window_height: %d", window_height());
    out->Println("default_font_file: %s", default_font_file().c_str());
    out->Println("default_font_size: %d", default_font_size());
    out->Println("system_font_file: %s", system_font_file().c_str());
    out->Println("system_font_size: %d", system_font_size());
}

void Properties::Parse(const std::vector<std::string_view> &items) {
    if (items.size() < 2) { return; }
    const PropertyDef *def = FindDef(items[0]);
    if (!def) { return; }
    ParseValue(items[1], def->type, AddressOf<void>(def->offset));
}

/*static*/ const Properties::PropertyDef *Properties::FindDef(std::string_view target) {
    for (const PropertyDef *def = &defs_[0]; def->name != nullptr; def++) {
        std::string_view name(def->name, def->name_size);
        if (name == target) { return def; }
    }
    return nullptr;
}

}  // namespace nyaa