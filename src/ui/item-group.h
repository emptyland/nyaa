#pragma once
#ifndef NYAA_UI_ITEM_GROUP_H_
#define NYAA_UI_ITEM_GROUP_H_

#include "ui/component.h"
#include "ui/ui.h"

namespace nyaa {
namespace res {
class Texture;
}  // namespace res
namespace ui {

class ItemGroup : public Component {
public:
    class Delegate : public Component::Delegate {
    public:
        inline Delegate() = default;

        virtual void DidItemDispose(Vector2i grid, EntityId id, bool *dropped) {}
        virtual void OnItemDrop(Vector2i src, Vector2i dst, EntityId id, bool *dropped) {}

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
        DEF_VAL_PROP_RW(std::string, name);
        DEF_PTR_PROP_RW(void, extra);
        DEF_VAL_PROP_RW(bool, full);

        DISALLOW_IMPLICIT_CONSTRUCTORS(Item);

    private:
        EntityId      id_;
        res::Texture *icon_ = nullptr;
        std::string   name_;
        void *        extra_ = nullptr;
        bool          full_  = false;
    };  // class Item

    ItemGroup(Id id, int column_count, int row_count, Component *parent);
    ~ItemGroup() override;

    DEF_VAL_GETTER(int, column_count);
    DEF_VAL_GETTER(int, row_count);

    void AddProducer(Producer *data) { data_ = data; }

    DISALLOW_IMPLICIT_CONSTRUCTORS(ItemGroup);

private:
    void HandleMouseButtonInput(int button, int action, int mods, bool *should_break) override;
    // void OnMouseMove(double x, double y) override;
    void OnPaint(double delta) override;
    void DidFocus(bool focus) override;

    Item *item(int i, int j) {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, column_count_);
        DCHECK_GE(j, 0);
        DCHECK_LT(j, row_count_);
        return &items_[j * column_count_ + i];
    }

    bool is_drag() const { return drag_.x != column_count_ && drag_.y != row_count_; }

    void Drop() { drag_ = {column_count_, row_count_}; }

    Vector2i ToUserGrid(Vector2i grid) const { return {grid.x, row_count_ - 1 - grid.y}; }

    // Drag and drop

    const int column_count_;
    const int row_count_;
    Vector2i  cursor_;
    Vector2i  drag_;
    Vector4f  border_color_ = kBorderColor;

    std::unique_ptr<Item[]> items_;
    Producer *              data_ = nullptr;
};  // class ItemGroup

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_ITEM_GROUP_H_