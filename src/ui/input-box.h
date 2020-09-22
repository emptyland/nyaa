#pragma once
#ifndef NYAA_UI_INPUT_BOX_H_
#define NYAA_UI_INPUT_BOX_H_

#include "ui/component.h"
#include <string>
#include <vector>

namespace nyaa {

namespace res {
class FontFace;
}  // namespace res

namespace ui {

template <class T> class LabelComponent;

class InputBox : public Component {
public:
    class Delegate : public Component::Delegate {
    public:
        inline Delegate() = default;
        virtual void OnChange(InputBox *sender){};
        virtual void OnChanged(InputBox *sender){};
        virtual void DidEnter(InputBox *sender){};
    };  // class Delegate

    explicit InputBox(Id id, Component *parent = nullptr);
    ~InputBox() override;

    DEF_VAL_PROP_RW(int, font_bearing);
    DEF_VAL_PROP_RW(float, font_scale);

    std::string Utf8Text();

    void ClearText() {
        text_.clear();
        cursor_ = 0;
    }

private:
    void HandleKeyInput(int key, int code, int action, int mods, bool *should_break) override;
    void HandleCharInput(char32_t code, bool *did) override;
    void DidFocus(bool focus) override;
    void OnPaint(double delta) override;

    std::u32string text_;
    int            cursor_       = 0;
    int            font_bearing_ = 10;
    float          font_scale_   = 0.8;
    double         time_         = 0;
};  // class InputBox

using LabelInputBox = LabelComponent<InputBox>;

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_INPUT_BOX_H_