#pragma once
#ifndef NYAA_UI_LABEL_COMPONENT_H_
#define NYAA_UI_LABEL_COMPONENT_H_

#include "ui/component.h"
#include "resource/font-library.h"
#include <type_traits>

namespace nyaa {

namespace ui {

template <class T>
class LabelComponent : public Component {
public:
    inline LabelComponent(Id id, std::string_view name, Component *parent) : Component(id, name, parent) {
        T *child = new T(id, this);
        child->SetVisible(true);
        child->SetEnable(true);
    }
    inline LabelComponent(Id id, res::TextID name, Component *parent) : Component(id, name, parent) {
        T *child = new T(id, this);
        child->SetVisible(true);
        child->SetEnable(true);
    }

    T *Child() {
        Component *child = nullptr;
        for (Component *c : *mutable_children()) {
            child = c;
            break;
        }
        return down_cast<T>(child);
    }

    void AddDelegate(Delegate *value, bool ownership = false) {
        for (Component *c : *mutable_children()) { c->AddDelegate(value, ownership); }
    }

    DISALLOW_IMPLICIT_CONSTRUCTORS(LabelComponent);

private:
    void OnPaint(double delta) override;
};  // template<class T> class LabelComponent

void DoPaintLabel(const std::vector<float> &vertices, Component *self, Component *child);

template <class T>
void LabelComponent<T>::OnPaint(double delta) {
    Component *child = Child();

    Vector2f size = font()->ApproximateSize(name()) * font_scale();
    child->set_bound({
        bound().x + 16 + static_cast<int>(size.x),
        bound().y,
        bound().w - 16 - static_cast<int>(size.x),
        bound().h,
    });

    std::vector<float> vertices;
    font()->Render(Vec3(bound().x, bound().y + font_bearing(), 0), font_scale(), name(), &vertices);
    DoPaintLabel(vertices, this, child);
}

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_LABEL_COMPONENT_H_