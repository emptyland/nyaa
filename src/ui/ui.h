#pragma once
#ifndef NYAA_UI_UI_H_
#define NYAA_UI_UI_H_

#include "game/vector.h"

namespace nyaa {

namespace ui {

static constexpr Vector4f kBorderColor = {0.82, 0.53, 0.29, 1};
static constexpr Vector4f kFontColor   = {1, 1, 1, 1};

static constexpr Vector4f kFontOutlineColor = {0.22, 0.6, 0.92, 1.0};

static constexpr Vector4f kBgColor = {0, 0, 0, 0.5};
static constexpr Vector4f kFgColor = {1, 1, 1, 1};

static constexpr int kButtonH      = 72;
static constexpr int kScreenBorder = 32;

static constexpr uint32_t kOk     = 1u;
static constexpr uint32_t kCancel = 1u << 1;
static constexpr uint32_t kYes    = 1u << 2;
static constexpr uint32_t kNo     = 1u << 3;
static constexpr uint32_t kAbort  = 1u << 4;
static constexpr uint32_t kDone   = 1u << 5;

enum Alignment {
    kAlignCenter,
    kAlignLeft,
    kAlignRight,
    kAlignTop,
    kAlignBottom,
};

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_UI_H_
