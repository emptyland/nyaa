#pragma once
#ifndef NYAA_UI_AVATAR_SELECTOR_H_
#define NYAA_UI_AVATAR_SELECTOR_H_

#include "ui/component.h"

namespace nyaa {

namespace res {
class Avatar;
}  // namespace res

namespace ui {

class AvatarSelector : public Component {
public:
    class Delegate : public Component::Delegate {
    public:
        inline Delegate() = default;
        virtual void OnNextAvatar(AvatarSelector *sender){};
        virtual void OnPrevAvatar(AvatarSelector *sender){};
    };  // class Delegate

    class Producer {
    public:
        inline Producer() = default;
        virtual ~Producer() {}
        virtual void OnAvatarProduce(AvatarSelector *sender, res::Avatar **value) = 0;
    };  // class Producer

    AvatarSelector(Id id, Component *parent);
    ~AvatarSelector() override;

    void AddProducer(Producer *data) { data_ = data; }

    DISALLOW_IMPLICIT_CONSTRUCTORS(AvatarSelector);

private:
    void HandleMouseButtonInput(int button, int action, int mods, bool *should_break) override;
    void OnMouseMove(double x, double y) override;
    void OnPaint(double delta) override;

    enum Direction {
        kNone,
        kNext,
        kPrev,
    };

    Producer *data_;
    Direction cursor_ = kNone;
};  // class AvatarSelector

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_AVATAR_SELECTOR_H_