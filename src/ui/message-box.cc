#include "ui/message-box.h"
#include "ui/button-group.h"
#include "game/game.h"
#include "resource/font-library.h"
#include "resource/text-library.h"
#include "base/slice.h"
#include "base/bit-ops.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdarg.h>

namespace nyaa {

namespace ui {

MessageBox::MessageBox(Id id, uint32_t buttons, Component *parent) : Component(id, parent) {
    int n = base::Bits::CountOne32(buttons);

    buttons_ = new ButtonGroup(id, n <= 0 ? 1 : n, 1, this);
    buttons_->SetVisible(true);
    buttons_->SetEnable(true);

    int col = 0;

    ButtonGroup::Button *btn = nullptr;
    if (buttons & kOk) {
        btn = buttons_->AddButton(kIdOk, col++, 0);
        btn->SetName(res::BTN_OK);
    }
    if (buttons & kYes) {
        btn = buttons_->AddButton(kIdYes, col++, 0);
        btn->SetName(res::BTN_YES);
    }
    if (buttons & kNo) {
        btn = buttons_->AddButton(kIdNo, col++, 0);
        btn->SetName(res::BTN_NO);
    }
    if (buttons & kCancel) {
        btn = buttons_->AddButton(kIdCancel, col++, 0);
        btn->SetName(res::BTN_CANCEL);
    }
    if (buttons & kAbort) {
        btn = buttons_->AddButton(kIdAbort, col++, 0);
        btn->SetName(res::BTN_ABORT);
    }
    if (buttons & kDone) {
        btn = buttons_->AddButton(kIdDone, col++, 0);
        btn->SetName(res::BTN_DONE);
    }
}

void MessageBox::Printf(const Vector3f &color, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    Vsprintf(color, fmt, ap);
    va_end(ap);
}

void MessageBox::Vsprintf(const Vector3f &color, const char *fmt, va_list ap) { Add(color, base::Vsprintf(fmt, ap)); }

void MessageBox::Add(const Vector3f &color, std::string_view msg) {
    if (msg.empty()) {
        Row row;
        row.size = {0, font()->pixel_size() * font_scale()};
        rows_.push_back(row);
        return;
    }
    Row row;
    row.text  = msg;
    row.color = color;
    row.size  = font()->ApproximateSize(msg) * font_scale();
    rows_.push_back(row);
}

void MessageBox::Add(const Vector3f &color, res::TextID text) { Add(color, Game::This()->text_lib()->Load(text)); }

void MessageBox::AddDelegate(Delegate *value, bool ownership) { buttons_->AddDelegate(value, ownership); }

void MessageBox::OnPaint(double delta) {
    UpdateBound();
    DrawBorder(kBorderColor, delta);

    int x = bound().x + kScreenBorder;
    int y = bound().y + bound().h - kScreenBorder;
    for (const auto &row : rows_) {
        y -= row.size.y;
        DrawLabel(Vec3(x, y, 0), row.color, row.text);
        y -= padding_size();
    }
}

void MessageBox::UpdateBound() {
    float h = 0, w = 0;
    for (auto &row : rows_) {
        if (row.text.empty()) {
            row.size = {0, font()->pixel_size() * font_scale()};
        } else {
            row.size = font()->ApproximateSize(row.text) * font_scale();
        }
        h += row.size.y;
        w = std::max(w, row.size.x);
    }
    h += rows_.size() * padding_size() + kScreenBorder * 2 + kButtonH;
    w += kScreenBorder * 2;

    w = std::max(w, buttons_->column_count() * 170.0f + kScreenBorder * 2);

    set_bound({
        (Game::This()->fb_w() - static_cast<int>(w)) / 2,
        (Game::This()->fb_h() - static_cast<int>(h)) / 2,
        static_cast<int>(w),
        static_cast<int>(h),
    });

    w = buttons_->column_count() * 170.0f;
    h = kButtonH;
    buttons_->set_bound({
        bound().x + (bound().w - static_cast<int>(w)) / 2,
        bound().y + kScreenBorder,
        static_cast<int>(w),
        static_cast<int>(h),
    });
}

}  // namespace ui

}  // namespace nyaa