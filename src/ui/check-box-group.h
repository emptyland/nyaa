#pragma once
#ifndef NYAA_UI_CHECK_BOX_GROUP_H_
#define NYAA_UI_CHECK_BOX_GROUP_H_

#include "ui/label-check-box.h"

namespace nyaa {

namespace ui {

class CheckBoxGroup : public Component {
public:
    class Producer {
    public:
        inline Producer() = default;
        virtual ~Producer() {}
        virtual void OnCheckBoxProduce(CheckBoxGroup *sender, Id id, bool *value) = 0;

        DISALLOW_IMPLICIT_CONSTRUCTORS(Producer);
    };  // class Producer

    CheckBoxGroup(Id id, std::string_view name, Component *parent);
    CheckBoxGroup(Id id, res::TextID name, Component *parent);
    ~CheckBoxGroup() override;

    LabelCheckBox *AddCheckBox(Id id, std::string_view name);
    LabelCheckBox *AddCheckBox(Id id, res::TextID name);

    void AddDelegate(Component::Delegate *value, bool ownership = false) override {
        for (Component *child : *mutable_children()) { child->AddDelegate(value, ownership); }
    }
    void AddProducer(Producer *data) { data_ = data; }

private:
    void OnPaint(double delta) override;

    Producer *data_        = nullptr;
};  // class CheckBoxGroup

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_CHECK_BOX_GROUP_H_