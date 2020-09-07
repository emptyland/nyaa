#pragma once
#ifndef NYAA_UI_UI_SERVICE_H_
#define NYAA_UI_UI_SERVICE_H_

#include "game/identifiers.h"
#include "base/base.h"
#include <vector>
#include <unordered_map>
#include <type_traits>

namespace nyaa {

namespace ui {

class InputBox;
class Controller;

class UIService {
public:
    UIService();
    ~UIService();

    InputBox *NewInputBox(std::string_view text, Controller *parent);

    void Destroy(Controller *ctrl);

    template <class T, class = std::enable_if_t<std::is_base_of<Controller, T>::value>>
    T *Find(ControllerId id) const {
        auto iter = id_to_ctrl_.find(id);
        DCHECK(iter != id_to_ctrl_.end());
        return down_cast<T *>(iter->second);
    }

    Controller *FindOrNull(ControllerId id) const {
        auto iter = id_to_ctrl_.find(id);
        return iter == id_to_ctrl_.end() ? nullptr : iter->second;
    }

    void HandleCharInput(uint32_t code, bool *did);

    void HandleInput(bool *did) {
        *did = false;
        HandleKeyEvent(did);
        if (*did) { return; }
        HandleMouseEvent(did);
    }

    void Render(double delta);

    DISALLOW_IMPLICIT_CONSTRUCTORS(UIService);

private:
    void PutController(Controller *ctrl);

    void HandleKeyEvent(bool *did);
    void HandleMouseEvent(bool *did);

    uint32_t                  next_id_ = 0;
    std::vector<Controller *> roots_;

    Controller *focus_ = nullptr;

    std::unordered_map<ControllerId, Controller *, ControllerHash> id_to_ctrl_;
};  // class UIService

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_UI_SERVICE_H_