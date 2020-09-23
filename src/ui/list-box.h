#pragma once
#ifndef NYAA_UI_LIST_BOX_H_
#define NYAA_UI_LIST_BOX_H_

#include "ui/component.h"
#include "game/vector.h"
#include <stdarg.h>
#include <string>
#include <deque>

namespace nyaa {
namespace res {
class FontFace;
}  // namespace res
namespace ui {

class ListBox : public Component {
public:
    explicit ListBox(Id id, int limit, Component *parent = nullptr);
    ~ListBox() override;

    DEF_VAL_PROP_RW(int, limit_rows);

    void Append(std::string_view text, Vector3f color = Vec3(1, 1, 1));

    void Printf(const Vector3f &color, const char *fmt, ...);
    void Vprintf(const Vector3f &color, const char *fmt, va_list ap);

private:
    void OnPaint(double delta) override;

    struct Row {
        std::string text;
        Vector3f    color;
        float       h;
    };  // class Row

    std::deque<Row> rows_;
    int             limit_rows_ = 1000;
};  // class ListBox

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_LIST_BOX_H_