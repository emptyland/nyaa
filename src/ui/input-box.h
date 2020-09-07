#pragma once
#ifndef NYAA_UI_INPUT_BOX_H_
#define NYAA_UI_INPUT_BOX_H_

#include "ui/controller.h"

namespace nyaa {

namespace ui {

class InputBox : public Controller {
public:
    explicit InputBox(Id id, Controller *parent = nullptr);
    ~InputBox() override;

    void HandleKeyEvent(bool *did) override;
    void DidFocus(bool focus) override;
    void OnPaint(double delta) override;

private:
    std::string text_;
};  // class InputBox

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_INPUT_BOX_H_