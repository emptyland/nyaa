#ifndef NYAA_GAME_IDENTIFIERS_H_
#define NYAA_GAME_IDENTIFIERS_H_

#include "base/arena-utils.h"
#include "base/base.h"

namespace nyaa {

template <class T>
class Identifier {
public:
    // break
    constexpr bool operator==(const Identifier &other) const { return value_ == other.value(); }
    constexpr bool operator!=(const Identifier &other) const { return value_ != other.value(); }
    constexpr bool operator<(const Identifier &other) const { return value_ < other.value(); }
    constexpr T value() const { return value_; }

protected:
    constexpr Identifier(T value) : value_(value) {}

private:
    T value_;
};  // class Identifier

class EntityId final : public Identifier<uint64_t> {
public:
    constexpr EntityId() : EntityId(0) {}
    static EntityId Next();
    static EntityId New();
    static constexpr EntityId Of(uint64_t value) { return EntityId(value); }

private:
    constexpr EntityId(uint64_t value) : Identifier<uint64_t>(value) {}
};  // class EntityId

class ResourceId final : public Identifier<uint32_t> {
public:
    constexpr ResourceId() : ResourceId(0) {}
    static constexpr ResourceId Of(uint32_t value) { return ResourceId(value); }

private:
    constexpr ResourceId(uint64_t value) : Identifier<uint32_t>(value) {}
};  // class ResourceId

class UIComponentId final : public Identifier<uint32_t> {
public:
    constexpr UIComponentId() : UIComponentId(0) {}
    static constexpr UIComponentId Of(uint32_t value) { return UIComponentId(value); }

private:
    constexpr UIComponentId(uint64_t value) : Identifier<uint32_t>(value) {}
}; // class ControllerId

struct EntityHash : public std::unary_function<EntityId, size_t> {
    inline size_t operator()(EntityId value) const { return value.value(); }
};

struct ControllerHash : public std::unary_function<UIComponentId, size_t> {
    inline size_t operator()(UIComponentId value) const { return value.value(); }
};

namespace base {

template <>
struct ArenaHash<ResourceId> : public std::unary_function<ResourceId, size_t> {
    inline size_t operator()(ResourceId value) const { return value.value(); }
};  // template <> struct ArenaHash<ArenaString *>

}  // namespace base

}  // namespace nyaa

#endif  // NYAA_GAME_IDENTIFIERS_H_