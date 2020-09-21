#include "scene/world-creation-scene.h"
#include "scene/controller.h"
#include "ui/button-group.h"
#include "ui/label-input-box.h"
#include "ui/check-box-group.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

class WorldCreationScene::UIController : public UIControllerT<WorldCreationScene>,
                                         public ui::LabelCheckBox::Delegate,
                                         public ui::CheckBoxGroup::Producer {
public:
    static constexpr auto kBackId   = UIComponentId::Of(0);
    static constexpr auto kResetId  = UIComponentId::Of(1);
    static constexpr auto kCreateId = UIComponentId::Of(2);

    static constexpr auto kSmallMapId  = UIComponentId::Of(3);
    static constexpr auto kNormalMapId = UIComponentId::Of(4);
    static constexpr auto kLargeMapId  = UIComponentId::Of(5);

    enum MapSize {
        kSmall,
        kNormal,
        kLarge,
    };

    UIController(WorldCreationScene *owns) : UIControllerT<WorldCreationScene>(owns) {}

    void DoInitialize(ui::UIService *ui) {
        btn_group_ = ui->New<ui::ButtonGroup>(3, 1, nullptr);
        btn_group_->AddDelegate(this);

        ui::ButtonGroup::Button *btn = btn_group_->AddButton(kBackId, 0, 0);
        btn->SetName(res::LABEL_BACK);
        btn = btn_group_->AddButton(kResetId, 1, 0);
        btn->SetName(res::LABEL_RESET);
        btn = btn_group_->AddButton(kCreateId, 2, 0);
        btn->SetName(res::LABEL_CREATE);

        map_seed_ = ui->New<ui::LabelInputBox>(nullptr);
        map_seed_->SetName(res::LABEL_MAP_SEED);

        map_size_ = ui->New<ui::CheckBoxGroup>(res::LABEL_MAP_SIZE, nullptr);
        map_size_->set_font_scale(0.8f);
        map_size_->AddCheckBox(kSmallMapId, res::LABEL_MAP_SMALL);
        map_size_->AddCheckBox(kNormalMapId, res::LABEL_MAP_NORMAL);
        map_size_->AddCheckBox(kLargeMapId, res::LABEL_MAP_LARGE);
        map_size_->AddProducer(this);
        map_size_->AddDelegate(this);
    }

    void DoLayout(const Boundi &view) {
        map_seed_->set_bound({
            ui::kScreenBorder,
            view.h - ui::kScreenBorder - ui::kButtonH,
            500,
            ui::kButtonH,
        });

        map_size_->set_bound({
            ui::kScreenBorder,
            map_seed_->bound().y - ui::kScreenBorder - (ui::kScreenBorder + ui::kButtonH) * 3,
            500,
            (ui::kScreenBorder + ui::kButtonH) * 3,
        });

        btn_group_->set_bound({
            (view.w - 500) / 2,
            ui::kButtonH + ui::kScreenBorder,
            500,
            ui::kButtonH,
        });
    }

    void OnCommand(ui::Component *sender, UIComponentId id) override {
        switch (id.value()) {
            case kBackId.value():
                owns()->DelayDispose();
                DCHECK_NOTNULL(owns()->prev())->SwitchTo(nullptr);
                break;

            default: break;
        }
    }

    void OnCheckBoxProduce(ui::CheckBoxGroup *sender, UIComponentId id, bool *value) override {
        switch (id.value()) {
            case kSmallMapId.value(): *value = (map_kind_ == kSmall); break;
            case kNormalMapId.value(): *value = (map_kind_ == kNormal); break;
            case kLargeMapId.value(): *value = (map_kind_ == kLarge); break;
            default: break;
        }
    }

    void OnChecked(ui::LabelCheckBox *sender, bool *checked) override {
        switch (sender->id().value()) {
            case kSmallMapId.value(): map_kind_ = kSmall; break;
            case kNormalMapId.value(): map_kind_ = kNormal; break;
            case kLargeMapId.value(): map_kind_ = kLarge; break;
            default: break;
        }
    }

private:
    ui::ButtonGroup *  btn_group_ = nullptr;
    ui::LabelInputBox *map_seed_  = nullptr;
    ui::CheckBoxGroup *map_size_  = nullptr;
    MapSize            map_kind_  = kNormal;
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
