#pragma once
#ifndef NYAA_UI_COMPONENT_H_
#define NYAA_UI_COMPONENT_H_

#include "ui/ui.h"
#include "resource/text-def-inl.h"
#include "game/vector.h"
#include "game/identifiers.h"
#include "base/queue-macros.h"
#include <string>
#include <string_view>
#include <vector>

namespace nyaa {
namespace res {
class FontFace;
}  // namespace res
namespace ui {

template <class T, class S = int>
class Flags {
public:
    static constexpr int kMaxBits = sizeof(S) * CHAR_BIT;

    constexpr Flags() : mask_(0) {}
    constexpr Flags(S mask) : mask_(mask) {}

    void set(T value) { mask_ |= Mask(value); }
    bool test(T value) const { return (mask_ & Mask(value)) != 0; }
    void unset(T value) { mask_ &= ~Mask(value); }

    static constexpr S Mask(T value) { return 1 << static_cast<S>(value); }

private:
    S mask_ = 0;
};  // class Flags

#define DEFINE_FLAGS_ENUMS(flag) k##flag,

#define DEFINE_FLAGS_METHOD(flag)                          \
    bool Is##flag() const { return flags_.test(k##flag); } \
    void Set##flag(bool value) {                           \
        if (value) {                                       \
            flags_.set(k##flag);                           \
        } else {                                           \
            flags_.unset(k##flag);                         \
        }                                                  \
    }

class Component;

class ComponentDelegate {
public:
    inline ComponentDelegate() = default;
    virtual ~ComponentDelegate();

    virtual void DidForce(Component *sender);
    virtual void OnCommand(Component *sender, UIComponentId id);

    friend class Component;
    DISALLOW_IMPLICIT_CONSTRUCTORS(ComponentDelegate);
};  // class ControllerDelegate

class Component {
public:
    using Id       = UIComponentId;
    using Delegate = ComponentDelegate;

    static constexpr int kMaxDelegates = 8;

    inline explicit Component(Id id) : Component(id, nullptr) {}
    inline Component(Id id, Component *parent) : Component(id, "", parent) {}
    Component(Id id, res::TextID name, Component *parent);
    Component(Id id, std::string_view name, Component *parent);
    virtual ~Component();

    DEF_VAL_GETTER(Id, id);
    DEF_VAL_PROP_RW(std::string, name);
    DEF_PTR_PROP_RW(res::FontFace, font);
    DEF_VAL_PROP_RMW(Boundi, bound);
    DEF_VAL_PROP_RW(int, z_order);
    DEF_PTR_PROP_RW(Component, parent);
    DEF_VAL_PROP_RW(double, last_time);
    DEF_VAL_PROP_RW(Vector4f, bg_color);
    DEF_VAL_PROP_RW(Vector4f, fg_color);

    void SetName(res::TextID text);

    bool DeltaTest(double delta);

    bool is_root() const { return parent() == nullptr; }

#define DECLARE_STATE(V) \
    V(Enable)            \
    V(Visible)           \
    V(Focus)             \
    V(Input)

    enum State { DECLARE_STATE(DEFINE_FLAGS_ENUMS) kMaxFlags };
    DECLARE_STATE(DEFINE_FLAGS_METHOD);

#undef DECLARE_STATE

    class Children {
    public:
        Children() {
            dummy_        = reinterpret_cast<Component *>(&stub_);
            dummy_->next_ = dummy_;
            dummy_->prev_ = dummy_;
        }

        class iterator;

        inline iterator begin();
        inline iterator end();

        class const_iterator;

        inline const_iterator begin() const;
        inline const_iterator end() const;

        bool empty() const { return dummy_->next_ == dummy_; }

        int size() const {
            int i = 0;
            for (Component *node = dummy_->next_; node != dummy_; node = node->next_) { i++; }
            return i;
        }

        void Append(Component *child) { QUEUE_INSERT_TAIL(dummy_, child); }

        void Remove(Component *child) {
            DCHECK(Exist(child));
            QUEUE_REMOVE(child);
        }

        bool Exist(Component *child) const {
            for (Component *i = dummy_->next_; i != dummy_; i = i->next_) {
                if (i == child) { return true; }
            }
            return false;
        }

        DISALLOW_IMPLICIT_CONSTRUCTORS(Children);

    private:
        struct DummyStub {
            intptr_t   padding;
            Component *next;
            Component *prev;
        } stub_;
        Component *dummy_;
    };  // class Children

    class Delegates {
    public:
        struct DelegateBundle {
            Delegate *value;
            bool      ownership;
        };

        Delegates() = default;

        class iterator;

        inline iterator begin();
        inline iterator end();

        class const_iterator;

        inline const_iterator begin() const;
        inline const_iterator end() const;

        bool empty() const { return delegate_size() == 0; }
        DEF_VAL_PROP_RW(int, delegate_size);

        void AddDelegate(Delegate *value, bool ownership = false) {
            DCHECK_LT(delegate_size_, kMaxDelegates);
            delegates_[delegate_size_++] = {value, ownership};
        }

        DISALLOW_IMPLICIT_CONSTRUCTORS(Delegates);

    private:
        DelegateBundle delegates_[kMaxDelegates];
        int            delegate_size_ = 0;
    };  // class Controller::Delegates

    DEF_VAL_PROP_RM(Children, children);
    DEF_VAL_PROP_RM(Delegates, delegates);

    void AddDelegate(Delegate *value, bool ownership = false) { delegates_.AddDelegate(value, ownership); }

    void AddChild(Component *child) { children_.Append(child); }

    virtual void HandleMouseButtonInput(int button, int action, int mods, bool *should_break);
    virtual void HandleKeyInput(int key, int code, int action, int mods, bool *should_break);
    virtual void HandleCharInput(char32_t code, bool *should_break);

    virtual void DidFocus(bool focus);
    virtual void OnMouseMove(double x, double y);

    virtual void OnPaint(double delta) = 0;

    static Vector2i GetCursorPosition();

    DISALLOW_IMPLICIT_CONSTRUCTORS(Component);

protected:
    bool TestKeyPress(int key);

    void DrawBorder(const Vector4f &color, double delta);

private:
    Component *    next_;
    Component *    prev_;
    Id             id_;
    std::string    name_;
    res::FontFace *font_      = nullptr;
    Vector4f       bg_color_  = kBgColor;
    Vector4f       fg_color_  = kFgColor;
    Boundi         bound_     = {0, 0, 0, 0};
    int            z_order_   = 0;
    Component *    parent_    = nullptr;
    double         last_time_ = -1;

    Flags<State, uint32_t> flags_;

    Children  children_;
    Delegates delegates_;
};  // class Controller

class Component::Delegates::iterator {
public:
    iterator(DelegateBundle *delegates, int i) : delegates_(delegates), i_(i) {}

    void operator++(int) { i_++; }
    void operator++() { i_++; }
    void operator--(int) { i_--; }
    void operator--() { i_--; }
    bool operator==(const iterator &other) const { return i_ == other.i_ && delegates_ == other.delegates_; }
    bool operator!=(const iterator &other) const { return i_ != other.i_ || delegates_ != other.delegates_; }

    std::pair<Delegate *, bool> operator*() const {
        return std::make_pair(delegates_[i_].value, delegates_[i_].ownership);
    }

private:
    DelegateBundle *delegates_;
    int             i_;
};  // class Controller::Delegates::iterator

inline Component::Delegates::iterator Component::Delegates::begin() { return iterator(delegates_, 0); }
inline Component::Delegates::iterator Component::Delegates::end() { return iterator(delegates_, delegate_size()); }

class Component::Delegates::const_iterator {
public:
    const_iterator(const DelegateBundle *delegates, int i) : delegates_(delegates), i_(i) {}

    void operator++(int) { i_++; }
    void operator++() { i_++; }
    void operator--(int) { i_--; }
    void operator--() { i_--; }
    bool operator==(const const_iterator &other) const { return i_ == other.i_ && delegates_ == other.delegates_; }
    bool operator!=(const const_iterator &other) const { return i_ != other.i_ && delegates_ != other.delegates_; }

    std::pair<Delegate *, bool> operator*() const {
        return std::make_pair(delegates_[i_].value, delegates_[i_].ownership);
    }

private:
    const DelegateBundle *delegates_;
    int                   i_;
};  // class Controller::Delegates::const_iterator

inline Component::Delegates::const_iterator Component::Delegates::begin() const {
    return const_iterator(&delegates_[0], 0);
}
inline Component::Delegates::const_iterator Component::Delegates::end() const {
    return const_iterator(&delegates_[0], delegate_size());
}

class Component::Children::iterator {
public:
    iterator(Component *cursor) : cursor_(cursor) {}

    void operator++(int) { cursor_ = cursor_->next_; }
    void operator++() { cursor_ = cursor_->next_; }
    void operator--(int) { cursor_ = cursor_->prev_; }
    void operator--() { cursor_ = cursor_->prev_; }
    bool operator==(const iterator &other) const { return cursor_ == other.cursor_; }
    bool operator!=(const iterator &other) const { return cursor_ != other.cursor_; }

    Component *operator*() const { return cursor_; }

private:
    Component *cursor_;
};  // class Controller::Delegates::iterator

inline Component::Children::iterator Component::Children::begin() { return iterator(dummy_->next_); }
inline Component::Children::iterator Component::Children::end() { return iterator(dummy_); }

class Component::Children::const_iterator {
public:
    const_iterator(const Component *cursor) : cursor_(cursor) {}

    void operator++(int) { cursor_ = cursor_->next_; }
    void operator++() { cursor_ = cursor_->next_; }
    void operator--(int) { cursor_ = cursor_->prev_; }
    void operator--() { cursor_ = cursor_->prev_; }
    bool operator==(const const_iterator &other) const { return cursor_ == other.cursor_; }
    bool operator!=(const const_iterator &other) const { return cursor_ != other.cursor_; }

    const Component *operator*() const { return cursor_; }

private:
    const Component *cursor_;
};  // class Controller::Delegates::const_iterator

inline Component::Children::const_iterator Component::Children::begin() const { return const_iterator(dummy_->next_); }

inline Component::Children::const_iterator Component::Children::end() const { return const_iterator(dummy_); }

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_COMPONENT_H_