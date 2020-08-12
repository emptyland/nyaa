#include "game/scene.h"
#include "game/game.h"
#include "glog/logging.h"

namespace nyaa {

Scene::Scene(Game *game) : game_(DCHECK_NOTNULL(game)), id_(game->NextEntityId()) {}

/*virtual*/ Scene::~Scene() {}

/*virtual*/ void Scene::Reset() {}

/*virtual*/ void Scene::OnKeyInput(int key, int code, int action, int mods) {}

/*virtual*/ void Scene::OnMouseInput(double x, double y) {}

/*virtual*/ void Scene::Render(double delta) {}

}  // namespace nyaa
