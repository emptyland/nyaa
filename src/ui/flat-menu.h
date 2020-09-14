#pragma once
#ifndef NYAA_UI_FLAT_MENU_H_
#define NYAA_UI_FLAT_MENU_H_

#include "ui/component.h"
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

    void AddItem(std::string_view text, Id id);

private:
    struct Item {
        std::string text;
        Id          id;
        Vector2f    original_size;
        Boundf      bound;
    };

    void OnMouseMove(double x, double y) override;
    void OnPaint(double delta) override;

    Vector2f ApproximateScale() const;

    std::vector<Item>::const_iterator FindItem(Id id) const {
        return std::find_if(items_.begin(), items_.end(), [&id](const Item &a) { return a.id == id; });
    }

    res::FontFace *   font_;
    std::vector<Item> items_;
};  // class FlatMenu

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_FLAT_MENU_H_