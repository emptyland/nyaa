#ifndef NYAA_ALLOCATOR_H_
#define NYAA_ALLOCATOR_H_

#include <stddef.h>

namespace nyaa {
    
class Allocator {
public:
    enum Access : int {
        kWr = 1,
        kRd = 1 << 1,
        kEx = 1 << 2,
        kRdWr = kRd | kWr,
    };
    
    Allocator() {}
    virtual ~Allocator() {}
    
    virtual void *Allocate(size_t size, size_t alignment = sizeof(max_align_t)) = 0;
    
    virtual void Free(const void *chunk, size_t size = 0) = 0;
    
    virtual size_t granularity() = 0;
}; // class Allocator
    
} // namespace nyaa

#endif // NYAA_ALLOCATOR_H_
