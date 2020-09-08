#pragma once
#ifndef NYAA_UI_LIST_BOX_H_
#define NYAA_UI_LIST_BOX_H_

#include "ui/component.h"
#include "game/vector.h"
#include <string>
#include <deque>

namespace nyaa {
namespace res {
class FontFace;
}  // namespace res
namespace ui {

class ListBox : public Component {
public:
    explicit ListBox(Id id, Component *parent = nullptr);
    ~ListBox() override;

    DEF_PTR_PROP_RW(res::FontFace, font);
    DEF_VAL_PROP_RW(float, font_scale);
    DEF_VAL_PROP_RW(int, limit_rows);

    void Append(std::string_view text, Vector3f color = Vec3(1, 1, 1));

    void HandleKeyEvent(bool *did) override;
    void OnPaint(double delta) override;

private:
    struct Row {
        std::string text;
        Vector3f    color;
        float       h;
    };  // class Row

    res::FontFace * font_;
    std::deque<Row> rows_;
    float           font_scale_ = 0.8;
    int             limit_rows_ = 1000;
};  // class ListBox

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_LIST_BOX_H_