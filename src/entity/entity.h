#ifndef NYAA_ENTITY_ENTITY_H_
#define NYAA_ENTITY_ENTITY_H_

#include "base/base.h"

namespace nyaa {

namespace entity {

template<class T>
class Entity {
public:
    using ClassId = uintptr_t;

    Entity() = default;
    ~Entity() = default;

    ClassId class_id() const { return reinterpret_cast<ClassId>(&class_); }
private:
    inline static int class_;
}; // class Entity

    
} // namespace entity

} // namespace nyaa

#endif // NYAA_ENTITY_ENTITY_H_