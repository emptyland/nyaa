#pragma once
#ifndef NYAA_GAME_PROPERTIES_H_
#define NYAA_GAME_PROPERTIES_H_

#include "resource/definition.h"
#include "base/base.h"
#include <string>

namespace nyaa {
namespace base {
class AbstractPrinter;
}  // namespace base

class Properties final : public res::Definition<Properties> {
public:
    Properties() = default;

    DEF_VAL_GETTER(std::string, name);
    DEF_VAL_GETTER(std::string, assets_dir);
    DEF_VAL_GETTER(std::string, language);
    DEF_VAL_GETTER(int, window_width);
    DEF_VAL_GETTER(int, window_height);
    DEF_VAL_GETTER(std::string, default_font_file);
    DEF_VAL_GETTER(int, default_font_size);

    void Print(base::AbstractPrinter *out) const;
    void Parse(const std::vector<std::string_view> &items);

    DISALLOW_IMPLICIT_CONSTRUCTORS(Properties);

private:
    struct PropertyDef {
        const char *    name;
        size_t          name_size;
        ptrdiff_t       offset;
        res::DefValType type;
    };

    static const PropertyDef *FindDef(std::string_view target);

    template <class T>
    T *AddressOf(ptrdiff_t offset) {
        return reinterpret_cast<T *>(reinterpret_cast<Address>(this) + offset);
    }

    std::string name_;
    std::string assets_dir_;
    std::string language_;
    int         window_width_  = 0;
    int         window_height_ = 0;
    std::string default_font_file_;
    int         default_font_size_ = 0;

    static const PropertyDef defs_[];
};  // class Properties

}  // namespace nyaa

#endif  // NYAA_GAME_PROPERTIES_H_