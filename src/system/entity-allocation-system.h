#pragma once
#ifndef NYAA_SYSTEM_ENTITY_ALLOCATION_SYSTEM_H_
#define NYAA_SYSTEM_ENTITY_ALLOCATION_SYSTEM_H_

#include "system/system.h"
#include "entity/entity.h"
#include <type_traits>

namespace nyaa {

namespace sys {

class EntityAllocationSystem : public base::ArenaObject {
public:
    using ClassId = uintptr_t;

    template <class T>
    struct Class {
        inline static const int id = 0;
    };  // template<class T> class Class

    template <class T>
    ClassId Register() {
        return reinterpret_cast<ClassId>(&Class<T>::id);
    }

    template <class T, class = std::enable_if_t<std::is_base_of<entity::Entity, T>::value>>
    T *Allocate() {
        return new T();
    }

    //_Args&&... __args
    template <class T, class... Args, class = std::enable_if_t<std::is_base_of<entity::Entity, T>::value>>
    T *New(Args&&... args) {
        return new T(std::forward<Args>(args)...);
    }

    template <class T, class = std::enable_if_t<std::is_base_of<entity::Entity, T>::value>>
    void Free(T *entity) {
        delete entity;
    }
};  // class EntityAllocationSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_ENTITY_ALLOCATION_SYSTEM_H_