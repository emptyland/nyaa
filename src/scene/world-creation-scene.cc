#include "scene/world-creation-scene.h"
#include "scene/controller.h"
#include "resource/avatar-library.h"
#include "ui/button-group.h"
#include "ui/label-component.h"
#include "ui/input-box.h"
#include "ui/check-box-group.h"
#include "ui/avatar-selector.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

static constexpr ResourceId kPlayerAvatars[] = {
    ResourceId::Of(100000), ResourceId::Of(100010), ResourceId::Of(100020), ResourceId::Of(100030),
    ResourceId::Of(100040), ResourceId::Of(100050), ResourceId::Of(100060), ResourceId::Of(100070),
    ResourceId::Of(100080), ResourceId::Of(100090), ResourceId::Of(100100), ResourceId::Of(100110),
};

class WorldCreationScene::UIController : public UIControllerT<WorldCreationScene>,
                                         public ui::LabelCheckBox::Delegate,
                                         public ui::CheckBoxGroup::Producer,
                                         public ui::AvatarSelector::Delegate,
                                         public ui::AvatarSelector::Producer {
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
        btn_group_->AddDelegate(static_cast<ui::LabelCheckBox::Delegate *>(this));

        ui::ButtonGroup::Button *btn = btn_group_->AddButton(kBackId, 0, 0);
        btn->SetName(res::LABEL_BACK);
        btn = btn_group_->AddButton(kResetId, 1, 0);
        btn->SetName(res::LABEL_RESET);
        btn = btn_group_->AddButton(kCreateId, 2, 0);
        btn->SetName(res::LABEL_CREATE);

        map_seed_ = ui->New<ui::LabelInputBox>(res::LABEL_MAP_SEED, nullptr);

        map_size_ = ui->New<ui::CheckBoxGroup>(res::LABEL_MAP_SIZE, nullptr);
        map_size_->set_font_scale(0.8f);
        map_size_->AddCheckBox(kSmallMapId, res::LABEL_MAP_SMALL);
        map_size_->AddCheckBox(kNormalMapId, res::LABEL_MAP_NORMAL);
        map_size_->AddCheckBox(kLargeMapId, res::LABEL_MAP_LARGE);
        map_size_->AddProducer(static_cast<ui::CheckBoxGroup::Producer *>(this));
        map_size_->AddDelegate(static_cast<ui::LabelCheckBox::Delegate *>(this));

        player_name_ = ui->New<ui::LabelInputBox>("Name:", nullptr);

        avatar_selector_ = ui->New<ui::AvatarSelector>(nullptr);
        avatar_selector_->AddDelegate(static_cast<ui::AvatarSelector::Delegate *>(this));
        avatar_selector_->AddProducer(static_cast<ui::AvatarSelector::Producer *>(this));
    }

    void DoLayout(const Boundi &view) {
        static constexpr int column_w = 500;
        static constexpr int total_w  = column_w * 2 + ui::kScreenBorder;

        int column_x = (view.w - total_w) / 2;
        map_seed_->set_bound({
            column_x,
            view.h - 2 * ui::kScreenBorder - ui::kButtonH,
            column_w,
            ui::kButtonH,
        });

        map_size_->set_bound({
            column_x,
            map_seed_->bound().y - 2 * ui::kScreenBorder - (ui::kScreenBorder + ui::kButtonH) * 3,
            column_w,
            (ui::kScreenBorder + ui::kButtonH) * 3,
        });

        column_x = (view.w - total_w) / 2 + column_w + ui::kScreenBorder;
        player_name_->set_bound({
            column_x,
            view.h - 2 * ui::kScreenBorder - ui::kButtonH,
            column_w,
            ui::kButtonH,
        });

        avatar_selector_->set_bound({
            column_x + (column_w - ui::kButtonH * 6) / 2,
            player_name_->bound().y - (ui::kScreenBorder + ui::kButtonH * 2),
            ui::kButtonH * 6,
            ui::kButtonH * 2,
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

    void OnAvatarProduce(ui::AvatarSelector *sender, res::Avatar **value) override {
        *value = game()->avatar_lib()->FindOrNull(kPlayerAvatars[avatar_index_]);
    }

    void OnNextAvatar(ui::AvatarSelector *sender) override {
        avatar_index_ = (avatar_index_ + 1) % arraysize(kPlayerAvatars);
    }

    void OnPrevAvatar(ui::AvatarSelector *sender) override {
        if (avatar_index_ == 0) {
            avatar_index_ = arraysize(kPlayerAvatars) - 1;
        } else {
            avatar_index_--;
        }
    }

private:
    ui::ButtonGroup *   btn_group_       = nullptr;
    ui::LabelInputBox * map_seed_        = nullptr;
    ui::CheckBoxGroup * map_size_        = nullptr;
    ui::LabelInputBox * player_name_     = nullptr;
    ui::AvatarSelector *avatar_selector_ = nullptr;
    int                 avatar_index_    = 0;
    MapSize             map_kind_        = kNormal;
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
