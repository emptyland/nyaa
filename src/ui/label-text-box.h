#pragma once
#ifndef NYAA_UI_LABEL_TEXT_BOX_H_
#define NYAA_UI_LABEL_TEXT_BOX_H_

#include "ui/component.h"

namespace nyaa {

namespace ui {

class LabelTextBox : public Component {
public:
    LabelTextBox(Id id, std::string_view name, Component *parent);
    LabelTextBox(Id id, res::TextID name, Component *parent);
    ~LabelTextBox() override;
private:
    void OnPaint(double delta) override;
};  // class LabelTextBox

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_LABEL_TEXT_BOX_H_