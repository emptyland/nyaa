#include "game/boot-scene.h"
#include "ui/ui-service.h"
#include "ui/button-group.h"
#include "game/test-scene.h"
#include "game/game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

class BootScene::UIController : public ui::ButtonGroup::Delegate {
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

        btn_group_ = service_->NewButtonGroup(3, 1, nullptr);
        btn_group_->AddDelegate(this);

        ui::ButtonGroup::Button *btn = btn_group_->AddButton(kNewGameId, 0, 0);
        btn->set_name("New");
        btn = btn_group_->AddButton(kTestSceneId, 1, 0);
        btn->set_name("Test");
        btn->set_font_color(Vec4(0, 1, 0, 1));
        btn = btn_group_->AddButton(kExitId, 2, 0);
        btn->set_name("Exit");
        btn->set_font_color(Vec4(1, 1, 0, 1));

        initialized_ = true;
    }

    void Attach() { owns_->game()->AddUIService(service_.get()); }

    void Detach() { owns_->game()->RemoveUIService(service_.get()); }

    void Render(double delta) {
        int w = owns_->game()->fb_w();
        // int h = owns_->game()->fb_h();

        btn_group_->set_bound({
            (w - kBtnGroupW) / 2,
            100,
            kBtnGroupW,
            80,
        });

        service_->Render(delta);
    }

    bool has_focus() const { return service_->focus() != nullptr; }

private:
    BootScene *                    owns_;
    std::unique_ptr<ui::UIService> service_;

    ui::ButtonGroup *btn_group_ = nullptr;

    bool initialized_ = false;
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
