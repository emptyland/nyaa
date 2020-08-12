#ifndef NYAA_GAME_IDENTIFIERS_H_
#define NYAA_GAME_IDENTIFIERS_H_

#include "base/arena-utils.h"
#include "base/base.h"

namespace nyaa {

template <class T>
class Identifier {
public:
    // break
    bool operator==(const Identifier &other) const { return value_ == other.value(); }
    bool operator<(const Identifier &other) const { return value_ < other.value(); }
    DEF_VAL_GETTER(T, value);

protected:
    Identifier(T value) : value_(value) {}

private:
    T value_;
};  // class Identifier

class EntityId final : public Identifier<uint64_t> {
public:
    static EntityId Next();
    static EntityId New();
    static EntityId Of(uint64_t value) { return EntityId(value); }

private:
    EntityId(uint64_t value) : Identifier<uint64_t>(value) {}
};  // class EntityId

class ResourceId final : public Identifier<uint32_t> {
public:
    ResourceId() : ResourceId(0) {}
    static ResourceId Of(uint32_t value) { return ResourceId(value); }

private:
    ResourceId(uint64_t value) : Identifier<uint32_t>(value) {}
};  // class ResourceId

namespace base {

template <>
struct ArenaHash<ResourceId> : public std::unary_function<ResourceId, size_t> {
    inline size_t operator()(ResourceId value) const { return value.value(); }
};  // template <> struct ArenaHash<ArenaString *>

}  // namespace base

}  // namespace nyaa

#endif  // NYAA_GAME_IDENTIFIERS_H_