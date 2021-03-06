#pragma once
#ifndef NYAA_UI_PROPERTY_BOX_H_
#define NYAA_UI_PROPERTY_BOX_H_

#include "ui/component.h"

namespace nyaa {

namespace ui {

template <class T>
class LabelComponent;

class PropertyBox : public Component {
public:
    class Delegate : public Component::Delegate {
    public:
        inline Delegate() = default;
        virtual void OnIncreaseProperty(PropertyBox *sender){};
        virtual void OnDecreaseProperty(PropertyBox *sender){};
    };  // class Delegate

    class Producer {
    public:
        inline Producer() = default;
        virtual ~Producer() {}
        virtual void OnPropertyProduce(PropertyBox *sender, int *value) = 0;
    };  // class Producer

    PropertyBox(Id id, Component *parent);
    ~PropertyBox() override;

    DEF_VAL_PROP_RW(int, value);

    void AddProducer(Producer *data) { data_ = data; }

    DISALLOW_IMPLICIT_CONSTRUCTORS(PropertyBox);

private:
    void HandleMouseButtonInput(int button, int action, int mods, bool *should_break) override;
    void OnMouseMove(double x, double y) override;
    void OnPaint(double delta) override;

    enum Direction {
        kNone,
        kIncr,
        kDecr,
    };  // enum Direction

    int       value_  = 0;
    Direction cursor_ = kNone;
    double    time_   = 0;
    Producer *data_   = nullptr;
};  // class PropertyBox

using LabelPropertyBox = LabelComponent<PropertyBox>;

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_PROPERTY_BOX_H_