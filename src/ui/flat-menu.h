#pragma once
#ifndef NYAA_UI_FLAT_MENU_H_
#define NYAA_UI_FLAT_MENU_H_

#include "ui/component.h"
#include "resource/text-def-inl.h"
#include "glog/logging.h"

namespace nyaa {
namespace res {
class FontFace;
}  // namespace res
namespace ui {

class FlatMenu : public ui::Component {
public:
    explicit FlatMenu(Id id, Component *parent = nullptr);
    ~FlatMenu() override;

    DEF_VAL_PROP_RW(int, padding_size);

    void AddItem(res::TextID text, Id id);
    void AddItem(std::string_view text, Id id);

private:
    struct Item {
        std::string text;
        Id          id;
        uint32_t    tex;
        Vector2f    original_size;
        Boundf      bound;
    };

    void HandleMouseButtonInput(int button, int action, int mods, bool *should_break) override;
    void OnMouseMove(double x, double y) override;
    void OnPaint(double delta) override;

    Vector2f ApproximateScale() const;
    void     DrawText(float x, float y, float scale, Item *item) const;

    std::vector<Item>::const_iterator FindItem(Id id) const {
        return std::find_if(items_.begin(), items_.end(), [&id](const Item &a) { return a.id == id; });
    }

    int               padding_size_ = 48;
    int               cursor_       = -1;
    float             scale_        = 1.0;
    std::vector<Item> items_;
};  // class FlatMenu

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_FLAT_MENU_H_