#pragma once
#ifndef NYAA_UI_LABEL_INPUT_BOX_H_
#define NYAA_UI_LABEL_INPUT_BOX_H_

#include "ui/component.h"
#include "ui/ui.h"

namespace nyaa {

namespace ui {

class InputBox;

class LabelInputBox : public Component {
public:
    LabelInputBox(Id id, Component *parent);
    ~LabelInputBox() override;

    DEF_PTR_GETTER(InputBox, child);

    DISALLOW_IMPLICIT_CONSTRUCTORS(LabelInputBox);
private:
    void OnPaint(double delta) override;

    InputBox *child_;
};  // class LabelInputBox

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_LABEL_INPUT_BOX_H_