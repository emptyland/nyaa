#pragma once
#ifndef NYAA_UI_UI_H_
#define NYAA_UI_UI_H_

#include "game/vector.h"

namespace nyaa {

namespace ui {

static constexpr Vector4f kBorderColor = {0.82, 0.53, 0.29, 1};
static constexpr Vector4f kFontColor = {1, 1, 1, 1};

static constexpr Vector4f kFontOutlineColor = {0.22, 0.6, 0.92, 1.0};

static constexpr Vector4f kBgColor = {0, 0, 0, 0.5};
static constexpr Vector4f kFgColor = {1, 1, 1, 1};

static constexpr int kButtonH = 72;
static constexpr int kScreenBorder = 32;

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_UI_H_
