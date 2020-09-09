#include "system/geometry-transform-system.h"
#include "game/game.h"
#include <GL/glew.h>

namespace nyaa {

namespace sys {

void GeometryTransformSystem::Enter2DProjection() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, ThisGame->fb_w(), 0, ThisGame->fb_h(), -1, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
}
void GeometryTransformSystem::Exit2DProjection() {
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

Vector4f GeometryTransformSystem::TransformToScreen(const Matrix<float> &projection, const Matrix<float> &view,
                                                    const Matrix<float> &model, const Vector4f &coord,
                                                    const Vector2i viewport) {
    Vector4f result;
    Matrix<float>::Multiply(model, coord, &result);
    Matrix<float>::Multiply(view, result, &result);
    Matrix<float>::Multiply(projection, result, &result);

    result.x /= result.w;
    result.y /= result.w;
    result.z /= result.w;

    result.x = viewport.x / 2 + result.x + viewport.x / 2 * result.x;
    result.y = viewport.y / 2 + result.y + viewport.y / 2 * result.y;

    return result;
}

}  // namespace sys

}  // namespace nyaa
