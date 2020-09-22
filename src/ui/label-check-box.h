#pragma once
#ifndef NYAA_UI_LABEL_CHECK_BOX_H_
#define NYAA_UI_LABEL_CHECK_BOX_H_

#include "ui/component.h"

namespace nyaa {

namespace ui {

class LabelCheckBox : public Component {
public:
    class Delegate : public Component::Delegate {
    public:
        inline Delegate() = default;

        virtual void OnChecked(LabelCheckBox *sender, bool *checked) {}

        DISALLOW_IMPLICIT_CONSTRUCTORS(Delegate);
    };  // class Delegate

    LabelCheckBox(Id id, std::string_view name, Component *parent);
    LabelCheckBox(Id id, res::TextID name, Component *parent);
    ~LabelCheckBox() override;

    DEF_VAL_PROP_RW(bool, checked);
    DEF_VAL_PROP_RW(float, font_scale);

    DISALLOW_IMPLICIT_CONSTRUCTORS(LabelCheckBox);

private:
    void HandleMouseButtonInput(int button, int action, int mods, bool *should_break) override;
    void OnMouseMove(double x, double y) override;
    void OnPaint(double delta) override;

    bool checked_ = false;
    float font_scale_ = 0.8f;
};  // class LabelCheckBox

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_LABEL_CHECK_BOX_H_