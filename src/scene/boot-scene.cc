#include "scene/boot-scene.h"
#include "scene/test-scene.h"
#include "ui/ui-service.h"
#include "ui/flat-menu.h"
#include "resource/texture-library.h"
#include "game/game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

class BootScene::UIController : public ui::Component::Delegate {
public:
    static constexpr auto kExitId      = UIComponentId::Of(0);
    static constexpr auto kNewGameId   = UIComponentId::Of(1);
    static constexpr auto kTestSceneId = UIComponentId::Of(2);

    static constexpr int kBtnGroupW = 500;

    UIController(BootScene *owns) : owns_(owns), service_(new ui::UIService(1)) {}

    ~UIController() { owns_->game()->RemoveUIService(service_.get()); }

    void OnCommand(ui::Component *sender, UIComponentId id) override {
        switch (id.value()) {
            case kExitId.value(): owns_->game()->Exit(); break;

            case kTestSceneId.value(): {
                Detach();
                TestScene *test_scene = new TestScene(owns_->game());
                test_scene->SwitchTo(owns_);
            } break;

            default: break;
        }
    }

    void Prepare() {
        if (initialized_) { return; }
        service_->set_dpi_factor(owns_->game()->dpi_factor());

        main_menu_ = service_->New<ui::FlatMenu>(nullptr);
        main_menu_->AddItem("New Game", kNewGameId);
        main_menu_->AddItem("Test Scene", kTestSceneId);
        main_menu_->AddItem("Exit", kExitId);
        main_menu_->AddDelegate(this);
        initialized_ = true;
    }

    void Attach() { owns_->game()->AddUIService(service_.get()); }

    void Detach() { owns_->game()->RemoveUIService(service_.get()); }

    void Render(double delta) {
        int w = owns_->game()->fb_w();
        int h = owns_->game()->fb_h();

        main_menu_->set_bound({
            (w - 300) / 2,
            (h - 400) / 2,
            300,
            400,
        });
        service_->Render(delta);
    }

    bool has_focus() const { return service_->focus() != nullptr; }

private:
    BootScene *                    owns_;
    std::unique_ptr<ui::UIService> service_;

    ui::FlatMenu *main_menu_   = nullptr;
    bool          initialized_ = false;
};  // class BootScene::UIController

BootScene::BootScene(Game *game) : Scene(game), ui_(new UIController(this)) {}

BootScene::~BootScene() {}

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
