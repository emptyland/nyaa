#include "scene/world-creation-scene.h"
#include "scene/controller.h"
#include "ui/button-group.h"
#include "ui/label-input-box.h"
#include "ui/label-check-box.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

class WorldCreationScene::UIController : public UIControllerT<WorldCreationScene>, public ui::Component::Delegate {
public:
    static constexpr auto kBackId   = UIComponentId::Of(0);
    static constexpr auto kResetId  = UIComponentId::Of(1);
    static constexpr auto kCreateId = UIComponentId::Of(2);

    UIController(WorldCreationScene *owns) : UIControllerT<WorldCreationScene>(owns) {}

    void DoInitialize(ui::UIService *ui) {
        btn_group_ = ui->New<ui::ButtonGroup>(3, 1, nullptr);
        btn_group_->AddDelegate(this);

        ui::ButtonGroup::Button *btn = btn_group_->AddButton(kBackId, 0, 0);
        btn->set_name("Back");
        btn = btn_group_->AddButton(kResetId, 1, 0);
        btn->set_name("Reset");
        btn = btn_group_->AddButton(kCreateId, 2, 0);
        btn->set_name("Create");

        map_seed_ = ui->New<ui::LabelInputBox>(nullptr);
        map_seed_->set_name("Seed:");

        cb1_ = ui->New<ui::LabelCheckBox>("Small", nullptr);
        cb1_->set_checked(true);

        cb2_ = ui->New<ui::LabelCheckBox>("Normal", nullptr);

        cb3_ = ui->New<ui::LabelCheckBox>("Large", nullptr);
    }

    void DoLayout(const Boundi &view) {
        map_seed_->set_bound({
            ui::kScreenBorder,
            view.h - ui::kScreenBorder - ui::kButtonH,
            500,
            ui::kButtonH,
        });

        cb1_->set_bound({
            ui::kScreenBorder,
            view.h - (ui::kScreenBorder + ui::kButtonH) * 2,
            500,
            ui::kButtonH,
        });

        cb2_->set_bound({
            ui::kScreenBorder,
            view.h - (ui::kScreenBorder + ui::kButtonH) * 3,
            500,
            ui::kButtonH,
        });

        cb3_->set_bound({
            ui::kScreenBorder,
            view.h - (ui::kScreenBorder + ui::kButtonH) * 4,
            500,
            ui::kButtonH,
        });

        btn_group_->set_bound({
            (view.w - 500) / 2,
            ui::kButtonH + ui::kScreenBorder,
            500,
            ui::kButtonH,
        });
    }

    void OnCommand(ui::Component *sender, UIComponentId id) {
        switch (id.value()) {
            case kBackId.value():
                owns()->DelayDispose();
                DCHECK_NOTNULL(owns()->prev())->SwitchTo(nullptr);
                break;

            default: break;
        }
    }

private:
    ui::ButtonGroup *btn_group_ = nullptr;
    ui::LabelInputBox *map_seed_ = nullptr;
    ui::LabelCheckBox *cb1_ = nullptr;
    ui::LabelCheckBox *cb2_ = nullptr;
    ui::LabelCheckBox *cb3_ = nullptr;
};  // class WorldCreationScene::UIController

WorldCreationScene::WorldCreationScene(Game *game) : Scene(game), ui_(new UIController(this)) {}

WorldCreationScene::~WorldCreationScene() { ui_->Detach(); }

void WorldCreationScene::Reset() {
    ui_->Prepare();
    ui_->Attach();
}

void WorldCreationScene::OnKeyInput(int key, int code, int action, int mods) {}

void WorldCreationScene::Render(double delta) { ui_->Render(delta); }

}  // namespace nyaa
