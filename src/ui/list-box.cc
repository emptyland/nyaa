#include "ui/list-box.h"
#include "game/game.h"
#include "resource/font-library.h"
#include "base/slice.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

ListBox::ListBox(Id id, Component *parent /*= nullptr*/)
    : Component(id, parent), font_(Game::This()->font_lib()->default_face()) {}

ListBox::~ListBox() {}

void ListBox::Printf(const Vector3f &color, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    Sprintf(color, fmt, ap);
    va_end(ap);
}

void ListBox::Sprintf(const Vector3f &color, const char *fmt, va_list ap) { Append(base::Vsprintf(fmt, ap), color); }

// Auto ling warp
void ListBox::Append(std::string_view text, Vector3f color /* = Vec3(1, 1, 1)*/) {
    base::CodePointIteratorUtf8 iter(text);
    iter.SeekFirst();

    float     w = 0, h = 0;
    ptrdiff_t pos = 0, start = 0;
    while (iter.Valid()) {
        Vector2f size = font()->ApproximateSize(*iter);
        size.x *= font_scale();
        size.y *= font_scale();
        h = std::max(h, size.y);
        if (w + size.x > bound().w - 8) {
            Row row;
            row.text  = text.substr(start, pos - start);
            row.color = color;
            row.h     = h;
            rows_.push_front(row);
            w     = 0;
            start = pos;
        }
        w += size.x;
        iter.Next();
        pos = iter.position();
    }
    if (pos - start > 0) {
        Row row;
        row.text  = text.substr(start, pos - start);
        row.color = color;
        row.h     = h;
        rows_.push_front(row);
    }

    while (rows_.size() >= limit_rows_) { rows_.pop_back(); }
}

void ListBox::HandleKeyEvent(bool *did) {}

void ListBox::OnPaint(double delta) {
    if (!font()) { set_font(Game::This()->font_lib()->default_face()); }

    DrawBorder(delta);

    std::vector<float> vertices;

    float y = bound().y + 4;
    for (const auto &row : rows_) {
        
        Boundf rect = font()->Render(Vec3(bound().x + 4, y, 0), font_scale(), row.text, &vertices);

        y += 8 + rect.h;
        if (y - bound().y - 4 > bound().h) { break; }

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, font()->buffered_tex());

        glBegin(GL_QUADS);
        glColor3f(row.color.x, row.color.y, row.color.z);
        for (int i = 0; i < vertices.size(); i += 5) {
            glTexCoord2f(vertices[i + 3], vertices[i + 4]);
            glVertex3fv(&vertices[i + 0]);
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);

        vertices.clear();
    }
}

}  // namespace ui

}  // namespace nyaa
