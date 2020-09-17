#pragma once
#ifndef NYAA_SCENE_CONTROLLER_H_
#define NYAA_SCENE_CONTROLLER_H_

#include "ui/ui-service.h"
#include "ui/ui.h"
#include "game/game.h"
#include "game/vector.h"
#include <type_traits>

namespace nyaa {

class Scene;

template <class T, class = std::enable_if_t<std::is_base_of<Scene, T>::value>>
class UIControllerT {
public:
    using Base = typename T::UIController;

    UIControllerT(T *owns) : owns_(owns), service_(new ui::UIService(1)) {}

    DEF_VAL_GETTER(bool, initialized);
    DEF_PTR_GETTER(T, owns);

    Game *game() const { return owns_->game(); }

    ui::UIService *service() const { return service_.get(); }

    bool has_focus() const { return service_->focus() != nullptr; }

    void Attach() { owns_->game()->AddUIService(service_.get()); }

    void Detach() { owns_->game()->RemoveUIService(service_.get()); }

    void Prepare() {
        if (initialized()) { return; }
        service()->set_dpi_factor(game()->dpi_factor());
        static_cast<Base *>(this)->DoInitialize(service());
        initialized_ = true;
    }

    void Render(double delta) {
        static_cast<Base *>(this)->DoLayout(Rect(0, 0, game()->fb_w(), game()->fb_h()));
        service_->Render(delta);
    }

    DISALLOW_IMPLICIT_CONSTRUCTORS(UIControllerT);

private:
    T *const owns_;
    bool     initialized_ = false;

    std::unique_ptr<ui::UIService> service_;
};  // class UIControllerT

}  // namespace nyaa

#endif  // NYAA_SCENE_CONTROLLER_H_