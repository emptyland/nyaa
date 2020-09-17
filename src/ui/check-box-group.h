#pragma once
#ifndef NYAA_UI_CHECK_BOX_GROUP_H_
#define NYAA_UI_CHECK_BOX_GROUP_H_

#include "ui/label-check-box.h"

namespace nyaa {

namespace ui {

class CheckBoxGroup : public Component {
public:
    CheckBoxGroup(Id id, std::string_view name, Component *parent);
    CheckBoxGroup(Id id, res::TextID name, Component *parent);
    ~CheckBoxGroup() override;

    LabelCheckBox *AddCheckBox(Id id, std::string_view name);
    LabelCheckBox *AddCheckBox(Id id, res::TextID name);

private:
    void OnPaint(double delta) override;
}; // class CheckBoxGroup

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_CHECK_BOX_GROUP_H_