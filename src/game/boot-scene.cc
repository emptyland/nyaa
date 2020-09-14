#include "game/boot-scene.h"
#include "ui/ui-service.h"
#include "ui/item-grid-view.h"
#include "ui/button-group.h"
#include "resource/texture-library.h"
#include "game/test-scene.h"
#include "game/game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

class BootScene::UIController : public ui::ButtonGroup::Delegate,
                                public ui::ItemGridView::Producer,
                                public ui::ItemGridView::Delegate {
public:
    static constexpr auto kExitId      = UIComponentId::Of(0);
    static constexpr auto kNewGameId   = UIComponentId::Of(1);
    static constexpr auto kTestSceneId = UIComponentId::Of(2);

    static constexpr int kBtnGroupW = 500;

    struct ItemGrid {
        res::Texture *tex;
        int           stack;
    };  // struct ItemGrid

    UIController(BootScene *owns) : owns_(owns), service_(new ui::UIService(1)) {
        ::memset(items_, 0, sizeof(items_));
        ::memset(slots_, 0, sizeof(slots_));
    }

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

    void OnItemProduce(ui::ItemGridView *sender, Vector2i grid, ui::ItemGridView::Item *item) override {
        if (sender == item_group_) {
            item->set_icon(items_[grid.x][grid.y].tex);
            item->set_stack(items_[grid.x][grid.y].stack);
        } else {
            item->set_icon(slots_[grid.y].tex);
            item->set_stack(slots_[grid.y].stack);
        }
    }

    void DidItemDispose(ui::ItemGridView *sender, Vector2i grid, bool *dropped) override {
        if (sender == item_group_) {
            if (items_[grid.x][grid.y].tex) {
                if (--items_[grid.x][grid.y].stack <= 0) {
                    items_[grid.x][grid.y].tex = nullptr;
                    *dropped                   = true;
                }
            }
        } else {
            if (slots_[grid.y].tex) {
                if (--slots_[grid.y].stack <= 0) {
                    slots_[grid.y].tex = nullptr;
                    *dropped           = true;
                }
            }
        }
    }

    void OnItemDrop(ui::ItemGridView *sender, Vector2i src, int stack, bool *dropped) override {
        // TODO:
        if (sender == item_group_) {
            item_group2_->Drop(item_group_, src, stack, dropped);

            if (!*dropped) {
                if (items_[src.x][src.y].stack -= stack; items_[src.x][src.y].stack <= 0) {
                    items_[src.x][src.y].tex = nullptr;
                    *dropped                 = true;
                }
            }
        } else {
            item_group_->Drop(item_group2_, src, stack, dropped);

            if (!*dropped) {
                if (slots_[src.y].stack -= stack; slots_[src.y].stack <= 0) {
                    slots_[src.y].tex = nullptr;
                    *dropped          = true;
                }
            }
        }
    }

    void OnItemDrop(ui::ItemGridView *sender, Vector2i src, ui::ItemGridView *receiver, Vector2i dst, int stack,
                    bool *dropped) override {
        DLOG(INFO) << "src: " << src.x << ", " << src.y << ":dst:" << dst.x << ", " << dst.y;

        ItemGrid *src_grid, *dst_grid;
        if (sender == item_group_) {
            src_grid = &items_[src.x][src.y];
        } else {
            src_grid = &slots_[src.y];
        }
        if (receiver == item_group_) {
            dst_grid = &items_[dst.x][dst.y];
        } else {
            dst_grid = &slots_[dst.y];
        }
        ItemGrid grid = *dst_grid;

        if (grid.tex == src_grid->tex) {
            dst_grid->stack += stack;
            if (src_grid->stack -= stack; src_grid->stack == 0) { src_grid->tex = nullptr; }
        } else if (!grid.tex) {
            dst_grid->stack = stack;
            dst_grid->tex   = src_grid->tex;
            if (src_grid->stack -= stack; src_grid->stack == 0) { src_grid->tex = nullptr; }
        } else if (grid.tex != src_grid->tex && stack == src_grid->stack) {
            *dst_grid = *src_grid;
            *src_grid = grid;
        } else if (grid.tex != src_grid->tex && stack != src_grid->stack) {
            *dropped = false;
            return;
        }
        *dropped = true;
    }

    void Prepare() {
        if (initialized_) { return; }

        items_[0][0] = {owns_->game()->texture_lib()->FindOrNull(ResourceId::Of(400480)), 1};
        items_[1][0] = {owns_->game()->texture_lib()->FindOrNull(ResourceId::Of(400490)), 2};
        items_[1][1] = {owns_->game()->texture_lib()->FindOrNull(ResourceId::Of(400020)), 99};

        service_->set_dpi_factor(owns_->game()->dpi_factor());

        item_group_ = service_->New<ui::ItemGridView>(4, 2, nullptr);  // NewItemGroup(4, 2, nullptr);
        item_group_->AddDelegate(static_cast<ui::ItemGridView::Delegate *>(this));
        item_group_->AddProducer(this);
        item_group_->SetVisible(false);

        item_group2_ = service_->New<ui::ItemGridView>(1, 4, nullptr);  // NewItemGroup(1, 4, nullptr);
        item_group2_->AddDelegate(static_cast<ui::ItemGridView::Delegate *>(this));
        item_group2_->AddProducer(this);
        item_group2_->SetVisible(false);

        btn_group_ = service_->New<ui::ButtonGroup>(3, 1, nullptr);  // NewButtonGroup(3, 1, nullptr);
        btn_group_->AddDelegate(static_cast<ui::ButtonGroup::Delegate *>(this));

        ui::ButtonGroup::Button *btn = btn_group_->AddButton(kNewGameId, 0, 0);
        btn->set_name("New");
        btn = btn_group_->AddButton(kTestSceneId, 1, 0);
        btn->set_name("Test");
        btn->set_font_color(Vec4(0, 1, 0, 1));
        btn = btn_group_->AddButton(kExitId, 2, 0);
        btn->set_name("Exit");
        btn->set_font_color(Vec4(1, 1, 0, 1));

        btn_group_->SetVisible(false);
        initialized_ = true;
    }

    void Attach() { owns_->game()->AddUIService(service_.get()); }

    void Detach() { owns_->game()->RemoveUIService(service_.get()); }

    void Render(double delta) {
        int w = owns_->game()->fb_w();
        int h = owns_->game()->fb_h();

        btn_group_->set_bound({
            (w - kBtnGroupW) / 2,
            100,
            kBtnGroupW,
            80,
        });

        item_group_->set_bound({(w - 65 * item_group_->column_count() + 4) / 2,
                                (h - 65 * item_group_->row_count() + 4) / 2, 65 * item_group_->column_count() + 4,
                                65 * item_group_->row_count() + 4});

        item_group2_->set_bound({(w - 65 * item_group2_->column_count() - 16),
                                 (h - 65 * item_group2_->row_count() + 4) / 2, 65 * item_group2_->column_count() + 4,
                                 65 * item_group2_->row_count() + 4});

        service_->Render(delta);
    }

    bool has_focus() const { return service_->focus() != nullptr; }

private:
    BootScene *                    owns_;
    std::unique_ptr<ui::UIService> service_;

    ui::ButtonGroup * btn_group_   = nullptr;
    ui::ItemGridView *item_group_  = nullptr;
    ui::ItemGridView *item_group2_ = nullptr;

    ItemGrid items_[4][2];
    ItemGrid slots_[4];

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
