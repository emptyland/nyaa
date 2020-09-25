#include "scene/scene.h"
#include "game/game.h"
#include "game/vector.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

Scene::Scene(Game *game) : id_(game->NextEntityId()), game_(DCHECK_NOTNULL(game)) {}

/*virtual*/ Scene::~Scene() {}

void Scene::SwitchTo(Scene *prev/* = nullptr*/) {
    set_prev(prev);
    Reset();
    game()->set_scene(this);
}

void Scene::DelayDispose() { game()->DelayDeleteScene(this); }

/*virtual*/ void Scene::Reset() {}

/*virtual*/ void Scene::OnKeyInput(int key, int code, int action, int mods) {}

/*virtual*/ void Scene::Render(double delta) {}

bool Scene::TestKeyPressed(int key) {
    return ::glfwGetKey(game()->window(), key) == GLFW_PRESS;
}

}  // namespace nyaa
