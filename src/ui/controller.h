#pragma once
#ifndef NYAA_UI_CONTROLLER_H_
#define NYAA_UI_CONTROLLER_H_

#include "game/vector.h"
#include "game/identifiers.h"
#include <string>
#include <string_view>
#include <vector>

namespace nyaa {

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

class Controller;

class ControllerDelegate {
public:
    inline ControllerDelegate() = default;
    inline ControllerDelegate(Controller *owns) : owns_(owns) {}
    virtual ~ControllerDelegate();

    DEF_PTR_GETTER(Controller, owns);

    virtual void DidForce(Controller *sender);

    friend class Controller;
    DISALLOW_IMPLICIT_CONSTRUCTORS(ControllerDelegate);

private:
    Controller *owns_ = nullptr;
};  // class ControllerDelegate

class Controller {
public:
    using Id       = ControllerId;
    using Delegate = ControllerDelegate;

    static constexpr int kMaxDelegates = 8;

    inline Controller() : Controller(nullptr) {}
    inline Controller(Controller *parent) : next_(this), prev_(this), parent_(parent) {}
    virtual ~Controller();

    DEF_VAL_GETTER(Id, id);
    DEF_VAL_PROP_RW(std::string, name);
    DEF_VAL_PROP_RMW(Boundi, bound);
    DEF_VAL_PROP_RW(int, z_order);
    DEF_PTR_PROP_RW(Controller, parent);

#define DECLARE_STATE(V) \
    V(Enable)            \
    V(Visible)           \
    V(Force)             \
    V(Input)

    enum State { DECLARE_STATE(DEFINE_FLAGS_ENUMS) kMaxFlags };
    DECLARE_STATE(DEFINE_FLAGS_METHOD);

#undef DECLARE_STATE

    class Children {

    };

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

    DEF_VAL_PROP_RM(Delegates, delegates);

    void AddDelegate(Delegate *value, bool ownership = false) { delegates_.AddDelegate(value, ownership); }

    virtual void Render(double delta);

    DISALLOW_IMPLICIT_CONSTRUCTORS(Controller);

private:
    Controller *           next_;
    Controller *           prev_;
    Id                     id_;
    std::string            name_;
    Boundi                 bound_   = {0, 0, 0, 0};
    int                    z_order_ = 0;
    Controller *           parent_  = nullptr;
    Flags<State, uint32_t> flags_;

    Delegates delegates_;
};  // class Controller

class Controller::Delegates::iterator {
public:
    iterator(DelegateBundle *delegates, int i) : delegates_(delegates), i_(i) {}

    void operator++(int) { i_++; }
    void operator++() { i_++; }
    void operator--(int) { i_--; }
    void operator--() { i_--; }
    bool operator==(const iterator &other) const { return i_ == other.i_ && delegates_ == other.delegates_; }
    bool operator!=(const iterator &other) const { return i_ != other.i_ && delegates_ != other.delegates_; }

    std::pair<Delegate *, bool> operator*() const {
        return std::make_pair(delegates_[i_].value, delegates_[i_].ownership);
    }

private:
    DelegateBundle *delegates_;
    int             i_;
};  // class Controller::Delegates::iterator

inline Controller::Delegates::iterator Controller::Delegates::begin() { return iterator(delegates_, 0); }
inline Controller::Delegates::iterator Controller::Delegates::end() { return iterator(delegates_, delegate_size()); }

class Controller::Delegates::const_iterator {
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

inline Controller::Delegates::const_iterator Controller::Delegates::begin() const {
    return const_iterator(&delegates_[0], 0);
}
inline Controller::Delegates::const_iterator Controller::Delegates::end() const {
    return const_iterator(&delegates_[0], delegate_size());
}

}  // namespace ui

}  // namespace nyaa

#endif  // NYAA_UI_CONTROLLER_H_