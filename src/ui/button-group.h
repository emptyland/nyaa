#pragma once
#ifndef NYAA_UI_BUTTON_GROUP_H_
#define NYAA_UI_BUTTON_GROUP_H_

#include "ui/component.h"
#include <string_view>
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace nyaa {
namespace res {
class FontFace;
}  // namespace res
namespace ui {

class ButtonGroup : public Component {
public:
    class Button;

    class Delegate : public Component::Delegate {
    public:
        inline Delegate() = default;
        virtual void OnBtnPress(ButtonGroup *sender, Id id) {}
        virtual void DidBtnFocus(ButtonGroup *sender, Button *btn) {}
    };  // class Delegate

    explicit ButtonGroup(Id id, int column_count, int row_count, Component *parent);
    ~ButtonGroup() override;

    DEF_VAL_GETTER(int, column_count);
    DEF_VAL_GETTER(int, row_count);

    class Button {
    public:
        DEF_VAL_GETTER(Id, id);
        DEF_VAL_PROP_RW(std::string, name);
        DEF_VAL_PROP_RMW(Vector4f, font_color);
        DEF_VAL_PROP_RMW(Vector4f, bg_color);
        DEF_VAL_PROP_RMW(Vector4f, fg_color);

#define DECLARE_STATE(V) \
    V(Enable)            \
    V(Used)              \
    V(Focus)

        enum Bits { DECLARE_STATE(DEFINE_FLAGS_ENUMS) kMaxFlags };
        DECLARE_STATE(DEFINE_FLAGS_METHOD);

#undef DECLARE_STATE
        friend class ButtonGroup;

    private:
        Id                    id_;
        std::string           name_;
        Flags<Bits, uint32_t> flags_;
        Vector4f              font_color_ = {1, 1, 1, 1};
        Vector4f              bg_color_   = {0, 0, 0, 0.7};
        Vector4f              fg_color_   = {0.4, 0.4, 0.4, 0.7};
        // std::vector<float>    vertices_;
    };  // struct Button

    Button *AddButton(Id id, int column, int row);

    void HandleKeyInput(int key, int code, int action, int mods, bool *should_break) override;
    void HandleMouseButtonInput(int button, int action, int mods, bool *should_break) override;
    void OnMouseMove(double x, double y) override;

    void OnPaint(double delta) override;

    DISALLOW_IMPLICIT_CONSTRUCTORS(ButtonGroup);

private:
    void UpdateBtnFocus(int i, int j);

    Button *button(int i, int j) {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, column_count_);
        DCHECK_GE(j, 0);
        DCHECK_LT(j, row_count_);
        return buttons_.get() + j * column_count_ + i;
    }

    const int column_count_ = 1;
    const int row_count_    = 1;
    Vector2i  cursor_;

    std::unique_ptr<Button[]> buttons_;

    std::map<Id, Button *> id_to_buttons_;
};  // class ButtonGroup

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_BUTTON_GROUP_H_