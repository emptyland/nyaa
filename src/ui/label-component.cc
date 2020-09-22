#include "ui/label-component.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

void DoPaintLabel(const std::vector<float> &vertices, Component *self, Component *child) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, self->font()->buffered_tex());

    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i < vertices.size(); i += 5) {
        glTexCoord2f(vertices[i + 3], vertices[i + 4]);
        glVertex3f(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

}  // namespace ui

}  // namespace nyaa
