#include "scene/world-creation-scene.h"
#include "scene/controller.h"
#include "resource/avatar-library.h"
#include "resource/text-formatter.h"
#include "ui/message-box.h"
#include "ui/button-group.h"
#include "ui/input-box.h"
#include "ui/property-box-group.h"
#include "ui/property-box.h"
#include "ui/check-box-group.h"
#include "ui/avatar-selector.h"
#include "ui/label-component.h"
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
                                         public ui::AvatarSelector::Producer,
                                         public ui::PropertyBox::Delegate,
                                         public ui::PropertyBoxGroup::Producer {
public:
    static constexpr auto kBackId   = UIComponentId::Of(0);
    static constexpr auto kResetId  = UIComponentId::Of(1);
    static constexpr auto kCreateId = UIComponentId::Of(2);

    static constexpr auto kSmallMapId  = UIComponentId::Of(3);
    static constexpr auto kNormalMapId = UIComponentId::Of(4);
    static constexpr auto kLargeMapId  = UIComponentId::Of(5);

    static constexpr auto kAttPropId = UIComponentId::Of(6);
    static constexpr auto kDefPropId = UIComponentId::Of(7);
    static constexpr auto kStgPropId = UIComponentId::Of(8);
    static constexpr auto kAglPropId = UIComponentId::Of(9);

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
        map_seed_->Child()->set_number_only(true);
        map_seed_->Child()->SetUtf8Text(base::Sprintf("%d", game()->random()->NextI32()));

        map_size_ = ui->New<ui::CheckBoxGroup>(res::LABEL_MAP_SIZE, nullptr);
        map_size_->set_font_scale(0.8f);
        map_size_->AddCheckBox(kSmallMapId, res::LABEL_MAP_SMALL);
        map_size_->AddCheckBox(kNormalMapId, res::LABEL_MAP_NORMAL);
        map_size_->AddCheckBox(kLargeMapId, res::LABEL_MAP_LARGE);
        map_size_->AddProducer(static_cast<ui::CheckBoxGroup::Producer *>(this));
        map_size_->AddDelegate(static_cast<ui::LabelCheckBox::Delegate *>(this));

        player_name_ = ui->New<ui::LabelInputBox>(res::LABEL_NAME, nullptr);

        avatar_selector_ = ui->New<ui::AvatarSelector>(nullptr);
        avatar_selector_->AddDelegate(static_cast<ui::AvatarSelector::Delegate *>(this));
        avatar_selector_->AddProducer(static_cast<ui::AvatarSelector::Producer *>(this));

        player_props_ = ui->New<ui::PropertyBoxGroup>(res::LABEL_INIT_PROPS, nullptr);
        player_props_->AddPropertyBox(kAttPropId, res::LABEL_PROP_ATT);
        player_props_->AddPropertyBox(kDefPropId, res::LABEL_PROP_DEF);
        player_props_->AddPropertyBox(kStgPropId, res::LABEL_PROP_STG);
        player_props_->AddPropertyBox(kAglPropId, res::LABEL_PROP_AGL);
        player_props_->AddDelegate(static_cast<ui::PropertyBox::Delegate *>(this));
        player_props_->AddProducer(static_cast<ui::PropertyBoxGroup::Producer *>(this));
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

        player_props_->set_bound({
            column_x,
            avatar_selector_->bound().y - (ui::kScreenBorder + ui::kButtonH) * 4,
            column_w,
            (ui::kScreenBorder + ui::kButtonH) * 4,
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

            case kCreateId.value(): PrepareCreation(); break;

            case ui::MessageBox::kIdOk.value():
            case ui::MessageBox::kIdCancel.value():
            case ui::MessageBox::kIdDone.value():
                if (sender->parent()->name() == "[error]") { service()->Modaless(sender->parent()); }
                if (sender->parent()->name() == "[warn]") { service()->Modaless(sender->parent()); }
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

    void OnIncreaseProperty(ui::PropertyBox *sender) override {
        int *value = nullptr;
        switch (sender->id().value()) {
            case kAttPropId.value(): value = &prop_att_; break;
            case kDefPropId.value(): value = &prop_def_; break;
            case kStgPropId.value(): value = &prop_stg_; break;
            case kAglPropId.value(): value = &prop_agl_; break;
            default: break;
        }
        if (value) {
            if (prop_oth_ > 0) {
                (*value)++;
                prop_oth_--;
            }
        }
    }

    void OnDecreaseProperty(ui::PropertyBox *sender) override {
        int *value = nullptr;
        switch (sender->id().value()) {
            case kAttPropId.value(): value = &prop_att_; break;
            case kDefPropId.value(): value = &prop_def_; break;
            case kStgPropId.value(): value = &prop_stg_; break;
            case kAglPropId.value(): value = &prop_agl_; break;
            default: break;
        }
        if (value) {
            if (*value > 0) {
                (*value)--;
                prop_oth_++;
            }
        }
    }

    void OnPropertyBoxProduce(ui::PropertyBoxGroup *sender, UIComponentId id, int *value) override {
        switch (id.value()) {
            case kAttPropId.value(): *value = prop_att_; break;
            case kDefPropId.value(): *value = prop_def_; break;
            case kStgPropId.value(): *value = prop_stg_; break;
            case kAglPropId.value(): *value = prop_agl_; break;
            default: break;
        }
    }

    void PrepareCreation() {
        // if ()

        std::string role_name = player_name_->Child()->Utf8Text();
        if (role_name.empty()) {
            ui::MessageBox *box = service()->Modal<ui::MessageBox>(ui::kDone, nullptr);
            box->set_name("[error]");
            box->Add(Vec3(1, 0, 0), res::LABEL_ERRORS);
            box->Add(Vec3(1, 0, 0), res::HINT_ROLE_NAME_EMPTY);
            box->AddDelegate(static_cast<ui::PropertyBox::Delegate *>(this));
            return;
        }

        if (prop_oth_ > 0) {
            ui::MessageBox *box = service()->Modal<ui::MessageBox>(ui::kOk|ui::kCancel, nullptr);
            box->set_name("[warn]");
            box->Add(Vec3(1, 1, 0), res::LABEL_NOTICE);
            box->Add(Vec3(1, 1, 0), res::Format(res::HINT_ROLE_PP_LEFT, prop_oth_));
            box->Add(Vec3(1, 1, 0), res::HINT_CONTINUE_OR_NOT);
            box->AddDelegate(static_cast<ui::PropertyBox::Delegate *>(this));
            return;
        }
    }

private:
    ui::ButtonGroup *     btn_group_       = nullptr;
    ui::LabelInputBox *   map_seed_        = nullptr;
    ui::CheckBoxGroup *   map_size_        = nullptr;
    ui::LabelInputBox *   player_name_     = nullptr;
    ui::AvatarSelector *  avatar_selector_ = nullptr;
    ui::PropertyBoxGroup *player_props_    = nullptr;
    int                   avatar_index_    = 0;
    int                   prop_att_        = 0;
    int                   prop_def_        = 0;
    int                   prop_stg_        = 0;
    int                   prop_agl_        = 0;
    int                   prop_oth_        = 30;
    MapSize               map_kind_        = kNormal;
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
