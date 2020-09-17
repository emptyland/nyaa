#include "scene/boot-scene.h"
#include "scene/test-scene.h"
#include "scene/world-creation-scene.h"
#include "scene/controller.h"
#include "ui/ui-service.h"
#include "ui/flat-menu.h"
#include "resource/texture-library.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

class BootScene::UIController : public UIControllerT<BootScene>, public ui::Component::Delegate {
public:
    static constexpr auto kExitId      = UIComponentId::Of(0);
    static constexpr auto kNewGameId   = UIComponentId::Of(1);
    static constexpr auto kLoadGameId  = UIComponentId::Of(2);
    static constexpr auto kTestSceneId = UIComponentId::Of(3);

    static constexpr int kBtnGroupW = 500;

    UIController(BootScene *owns) : UIControllerT<BootScene>(owns) {}

    ~UIController() {}

    void OnCommand(ui::Component *sender, UIComponentId id) override {
        switch (id.value()) {
            case kExitId.value(): game()->Exit(); break;

            case kNewGameId.value(): {
                Detach();
                Scene *scene = new WorldCreationScene(game());
                scene->SwitchTo(owns());
            } break;

            case kTestSceneId.value(): {
                Detach();
                TestScene *scene = new TestScene(game());
                scene->SwitchTo(owns());
            } break;

            default: break;
        }
    }

    void DoInitialize(ui::UIService *service) {
        service->set_dpi_factor(game()->dpi_factor());

        main_menu_ = service->New<ui::FlatMenu>(nullptr);
        main_menu_->AddItem(res::TITLE_MENU_NEW_WORLD, kNewGameId);
        main_menu_->AddItem(res::TITLE_MENU_LOAD_WORLD, kLoadGameId);
        main_menu_->AddItem(res::TITLE_MENU_TEST_SCENE, kTestSceneId);
        main_menu_->AddItem(res::TITLE_MENU_EXIT, kExitId);
        main_menu_->AddDelegate(this);
    }

    void DoLayout(const Boundi &view) {
        main_menu_->set_bound({
            (view.w - 300) / 2,
            (view.h - 500) / 2,
            300,
            500,
        });
    }

private:
    ui::FlatMenu *main_menu_ = nullptr;
};  // class BootScene::UIController

BootScene::BootScene(Game *game) : Scene(game), ui_(new UIController(this)) {}

BootScene::~BootScene() { ui_->Detach(); }

void BootScene::Reset() {
    ui_->Prepare();
    ui_->Attach();
}

void BootScene::OnKeyInput(int key, int code, int action, int mods) {}

void BootScene::Render(double delta) {
    if (!game()->break_input() && !ui_->has_focus()) {
        if (glfwGetKey(game()->window(), GLFW_KEY_T) == GLFW_PRESS) {
            ui_->Detach();

            TestScene *test_scene = new TestScene(game());
            test_scene->SwitchTo(this);
        }

        if (glfwGetKey(game()->window(), GLFW_KEY_ESCAPE) == GLFW_PRESS) { game()->Exit(); }
    }

    ui_->Render(delta);
}

}  // namespace nyaa
