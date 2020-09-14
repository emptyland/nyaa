#pragma once
#ifndef NYAA_UI_UI_SERVICE_H_
#define NYAA_UI_UI_SERVICE_H_

#include "game/identifiers.h"
#include "game/vector.h"
#include "base/base.h"
#include <vector>
#include <unordered_map>
#include <type_traits>

namespace nyaa {

namespace ui {

class FlatMenu;
class ItemGridView;
class ButtonGroup;
class InputBox;
class ListBox;
class Component;

class UIService {
public:
    UIService(float dpi_factor);
    ~UIService();

    DEF_VAL_PROP_RW(float, dpi_factor);
    DEF_PTR_GETTER(Component, focus);

    template <class T, class = std::enable_if_t<std::is_base_of<Component, T>::value>, class... Args>
    inline T *New(Args... args) {
        UIComponentId id   = UIComponentId::Of(next_id_++);
        T *           ctrl = new T(id, args...);
        PutController(ctrl);
        return ctrl;
    }

    void Destroy(Component *ctrl);

    void SetFocus(Component *ctrl);

    template <class T, class = std::enable_if_t<std::is_base_of<Component, T>::value>>
    T *Find(UIComponentId id) const {
        auto iter = id_to_ctrl_.find(id);
        DCHECK(iter != id_to_ctrl_.end());
        return down_cast<T *>(iter->second);
    }

    Component *FindOrNull(UIComponentId id) const {
        auto iter = id_to_ctrl_.find(id);
        return iter == id_to_ctrl_.end() ? nullptr : iter->second;
    }

    void HandleMouseMove();
    void HandleMouseButtonInput(int button, int action, int mods, bool *should_break);
    void HandleKeyInput(int key, int code, int action, int mods, bool *should_break);
    void HandleCharInput(char32_t code, bool *should_break);

    void Render(double delta);

    DISALLOW_IMPLICIT_CONSTRUCTORS(UIService);

private:
    void PutController(Component *ctrl);

    double   last_time_;
    uint32_t last_codepoint_ = 0;
    uint32_t next_id_        = 0;
    float    dpi_factor_     = 1;
    Vector2f last_mouse_pos_ = {0, 0};

    std::vector<Component *> roots_;
    Component *              focus_ = nullptr;

    std::unordered_map<UIComponentId, Component *, ControllerHash> id_to_ctrl_;
};  // class UIService

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_UI_SERVICE_H_