#pragma once
#ifndef NYAA_UI_INPUT_BOX_H_
#define NYAA_UI_INPUT_BOX_H_

#include "ui/controller.h"

namespace nyaa {

namespace ui {

class InputBox : public Controller {
public:
    class Delegate : public Controller::Delegate {
    public:
        Delegate(InputBox *owns): Controller::Delegate(owns) {}
        virtual void OnChange(InputBox *sender, std::string_view text) = 0;
        virtual void OnChanged(InputBox *sender, std::string_view text) = 0;
        virtual void DidEnter(InputBox *sender) = 0;
    }; // class Delegate

    explicit InputBox(Id id, Controller *parent = nullptr);
    ~InputBox() override;

    void HandleKeyEvent(bool *did) override;
    void HandleCharInput(uint32_t code, bool *did) override;
    void DidFocus(bool focus) override;
    void OnPaint(double delta) override;

private:
    std::string text_;
};  // class InputBox

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_INPUT_BOX_H_