#pragma once
#ifndef NYAA_UI_MESSAGE_BOX_H_
#define NYAA_UI_MESSAGE_BOX_H_

#include "ui/component.h"

namespace nyaa {

namespace ui {

class ButtonGroup;

class MessageBox : public Component {
public:
    static constexpr auto kIdOk     = Id::Of(1000);
    static constexpr auto kIdCancel = Id::Of(1001);
    static constexpr auto kIdYes    = Id::Of(1000);
    static constexpr auto kIdNo     = Id::Of(1002);
    static constexpr auto kIdAbort  = Id::Of(1003);
    static constexpr auto kIdDone   = Id::Of(1004);

    MessageBox(Id id, uint32_t buttons, Component *parent);

    void Add(const Vector3f &color, std::string_view msg);
    void Printf(const Vector3f &color, const char *fmt, ...);
    void Vsprintf(const Vector3f &color, const char *fmt, va_list ap);

    void AddDelegate(Delegate *value, bool ownership = false) override;

    DISALLOW_IMPLICIT_CONSTRUCTORS(MessageBox);

private:
    void OnPaint(double delta) override;
    void UpdateBound();

    struct Row {
        std::string text;
        Vector3f    color;
        Vector2f    size;
    };  // class Row

    std::vector<Row> rows_;
    ButtonGroup *    buttons_;
};  // class MessageBox

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_MESSAGE_BOX_H_