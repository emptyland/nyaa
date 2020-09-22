#pragma once
#ifndef NYAA_UI_PROPERTY_BOX_GROUP_H_
#define NYAA_UI_PROPERTY_BOX_GROUP_H_

#include "ui/property-box.h"

namespace nyaa {

namespace ui {

class PropertyBoxGroup : public Component {
public:
    class Producer {
    public:
        inline Producer() = default;
        virtual ~Producer() {}
        virtual void OnPropertyBoxProduce(PropertyBoxGroup *sender, Id id, int *value) = 0;
    };  // class Producer

    PropertyBoxGroup(Id id, std::string_view name, Component *parent);
    PropertyBoxGroup(Id id, res::TextID name, Component *parent);
    ~PropertyBoxGroup() override;

    LabelPropertyBox *AddPropertyBox(Id id, std::string_view name);
    LabelPropertyBox *AddPropertyBox(Id id, res::TextID name);

    void AddDelegate(Delegate *value, bool ownership = false) override {
        for (Component *c : *mutable_children()) { c->AddDelegate(value, ownership); }
    }
    void AddProducer(Producer *data) { data_ = data; }

    DISALLOW_IMPLICIT_CONSTRUCTORS(PropertyBoxGroup);

private:
    void OnPaint(double delta) override;

    Producer *data_ = nullptr;
};  // class PropertyBoxGroup

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_PROPERTY_BOX_GROUP_H_