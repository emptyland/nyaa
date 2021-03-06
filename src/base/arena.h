#ifndef NYAA_BASE_ARENA_H_
#define NYAA_BASE_ARENA_H_

#include "base/base.h"
#include "base/allocator.h"

namespace nyaa {

namespace base {

class Arena : public Allocator {
public:
    static const uint32_t kInitZag;
    static const uint32_t kFreeZag;

    Arena() {}
    virtual ~Arena() override;

    virtual void Purge(bool reinit) = 0;

    virtual size_t granularity() override { return 4; }

    virtual size_t memory_usage() const = 0;

    template <class T, class... Args>
    T *New(Args &&... args) {
        return new (Allocate(sizeof(T))) T(std::forward(args)...);
    }

    template <class T>
    T *NewArray(size_t n) {
        return static_cast<T *>(Allocate(sizeof(T) * n));
    }

    DISALLOW_IMPLICIT_CONSTRUCTORS(Arena);

private:
    virtual void Free(const void * /*chunk*/, size_t /*size*/) override {}
};  // class Arena

class ArenaObject {
public:
    void *operator new(size_t n, Arena *arena) { return arena->Allocate(n); }
};  // class ArenaObject

}  // namespace base

}  // namespace nyaa

#endif  // NYAA_BASE_ARENA_H_
