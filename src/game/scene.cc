#include "game/scene.h"
#include "game/game.h"
#include "glog/logging.h"

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

/*virtual*/ void Scene::OnMouseInput(double x, double y) {}

/*virtual*/ void Scene::Render(double delta) {}

}  // namespace nyaa
