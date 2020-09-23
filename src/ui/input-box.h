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

template <class T>
class LabelComponent;

class InputBox : public Component {
public:
    class Delegate : public Component::Delegate {
    public:
        inline Delegate() = default;
        virtual void OnChange(InputBox *sender){};
        virtual void OnChanged(InputBox *sender){};
        virtual void DidEnter(InputBox *sender){};
    };  // class Delegate

    InputBox(Id id, Component *parent);
    ~InputBox() override;

    DEF_VAL_PROP_RW(bool, number_only);

    std::string Utf8Text();
    void SetUtf8Text(std::string_view text);

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
    int            cursor_      = 0;
    double         time_        = 0;
    bool           number_only_ = false;
};  // class InputBox

using LabelInputBox = LabelComponent<InputBox>;

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_INPUT_BOX_H_