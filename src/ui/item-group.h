#pragma once
#ifndef NYAA_UI_ITEM_GROUP_H_
#define NYAA_UI_ITEM_GROUP_H_

#include "ui/component.h"
#include "ui/ui.h"

namespace nyaa {
namespace res {
class Texture;
class FontFace;
}  // namespace res
namespace ui {

class ItemGroup : public Component {
public:
    class Delegate : public Component::Delegate {
    public:
        inline Delegate() = default;

        virtual void DidItemDispose(Vector2i grid, EntityId id, bool *dropped) {}
        virtual void OnItemDrop(Vector2i src, Vector2i dst, int stack, bool *dropped) {}

        DISALLOW_IMPLICIT_CONSTRUCTORS(Delegate);
    };  // class Delegate

    class Item;

    class Producer {
    public:
        inline Producer() = default;
        virtual ~Producer() {}
        virtual void OnItemProduce(int x, int y, Item *item) = 0;

        DISALLOW_IMPLICIT_CONSTRUCTORS(Producer);
    };  // class Producer

    class Item {
    public:
        inline Item() = default;

        DEF_VAL_PROP_RW(EntityId, id);
        DEF_PTR_PROP_RW(res::Texture, icon);
        DEF_PTR_PROP_RW(void, extra);
        DEF_VAL_PROP_RW(int, stack);

        DISALLOW_IMPLICIT_CONSTRUCTORS(Item);

    private:
        EntityId      id_;
        res::Texture *icon_  = nullptr;
        void *        extra_ = nullptr;
        int           stack_ = 0;
    };  // class Item

    ItemGroup(Id id, int column_count, int row_count, Component *parent);
    ~ItemGroup() override;

    DEF_VAL_GETTER(int, column_count);
    DEF_VAL_GETTER(int, row_count);
    DEF_PTR_PROP_RW(res::FontFace, font);
    DEF_VAL_PROP_RW(Vector4f, font_color);
    DEF_VAL_PROP_RW(Vector4f, border_color);

    const Vector2i &drag_grid() const { return drag_; }

    void AddProducer(Producer *data) { data_ = data; }

    DISALLOW_IMPLICIT_CONSTRUCTORS(ItemGroup);

private:
    void HandleMouseButtonInput(int button, int action, int mods, bool *should_break) override;
    void OnPaint(double delta) override;
    void DidFocus(bool focus) override;

    void DrawIcon(const Boundf &bound, res::Texture *tex, int stack);

    Item *item(int i, int j) {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, column_count_);
        DCHECK_GE(j, 0);
        DCHECK_LT(j, row_count_);
        return &items_[j * column_count_ + i];
    }

    bool is_drag() const { return drag_.x != column_count_ && drag_.y != row_count_; }

    void Drag(Vector2i src, int stack) {
        drag_       = src;
        drag_stack_ = stack;
    }

    void Drop() {
        drag_       = {column_count_, row_count_};
        drag_stack_ = 0;
    }

    Vector2i ToUserGrid(Vector2i grid) const { return {grid.x, row_count_ - 1 - grid.y}; }

    const int      column_count_;
    const int      row_count_;
    res::FontFace *font_;
    Vector2i       cursor_;
    Vector2i       drag_;
    int            drag_stack_   = 0;
    Vector4f       font_color_   = kFontColor;
    Vector4f       border_color_ = kBorderColor;

    std::unique_ptr<Item[]> items_;
    Producer *              data_ = nullptr;
};  // class ItemGroup

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_ITEM_GROUP_H_