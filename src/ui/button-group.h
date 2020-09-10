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
    class Delegate : public Component::Delegate {
    public:
        explicit Delegate(ButtonGroup *owns = nullptr) : Component::Delegate(owns) {}
        virtual void OnButtonPress(ButtonGroup *sender, Id id){};
    };  // class Delegate

    explicit ButtonGroup(Id id, Component *parent = nullptr);
    ~ButtonGroup() override;

    DEF_PTR_PROP_RW(res::FontFace, font);

    void AddColumn(int n) {
        DCHECK(rows_.empty());
        column_count_ += n;
        DCHECK_GE(column_count_, 1);
    }

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
    };  // struct Button

    Button *AddButton(std::string_view name, Id id, int column);

    void HandleKeyEvent(bool *did) override;
    void HandleMouseEvent(double x, double y, bool *did) override;
    void OnMouseMove(double x, double y) override;

    void OnPaint(double delta) override;

    DISALLOW_IMPLICIT_CONSTRUCTORS(ButtonGroup);

private:
    res::FontFace *        font_         = nullptr;
    int                    column_count_ = 1;
    std::map<Id, Button *> id_to_buttons_;
    std::vector<Button *>  rows_;
};  // class ButtonGroup

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_BUTTON_GROUP_H_