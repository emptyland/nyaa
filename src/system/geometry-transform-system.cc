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
    glDisable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT);
}
void GeometryTransformSystem::Exit2DProjection() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void GeometryTransformSystem::EnterRotatedProjection() {
    //------------------------------------------------------------------------------------------------------------------
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    //------------------------------------------------------------------------------------------------------------------

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glFrustum(0, Game::This()->fb_w(), 0, Game::This()->fb_h(), -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    glRotated(rotate_angle_y(), -1, 0, 0);

    glPushMatrix();
    glRotated(rotate_angle_z(), 0.0, 1.0, 0.0);

    glPushMatrix();
    glScaled(scale(), scale(), scale());
}

void GeometryTransformSystem::ExitRotatedProjection() {
    glPopMatrix();  // glScaled
    glPopMatrix();  // glRotated
    glPopMatrix();  // glRotated

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    //------------------------------------------------------------------------------------------------------------------
    glDisable(GL_CULL_FACE);
    //------------------------------------------------------------------------------------------------------------------
}

}  // namespace sys

}  // namespace nyaa
