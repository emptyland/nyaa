#pragma once
#ifndef NYAA_RESOURCE_RESOURCE_LIBRARY_H_
#define NYAA_RESOURCE_RESOURCE_LIBRARY_H_

#include "resource/definition.h"
#include "game/identifiers.h"
#include "base/arena-utils.h"
#include "base/base.h"
#include <type_traits>

namespace nyaa {

namespace res {

template <class T, class D, class = std::enable_if_t<std::is_base_of<base::ArenaObject, T>::value>>
class ResourceLibrary {
public:
    ResourceLibrary(base::Arena *arena) : arena_(arena), resource_(arena) {}

    DEF_PTR_GETTER(base::Arena, arena);
    DEF_VAL_GETTER(bool, initialized);

    const base::ArenaUnorderedMap<ResourceId, T *> &resource() const { return resource_; }

    bool Prepare(const std::string &file_name) {
        FILE *fp = ::fopen(file_name.c_str(), "rb");
        if (!fp) {
            DLOG(ERROR) << "can not open actor definition file: " << file_name;
            return false;
        }
        DefinitionReader rd(fp, true /*ownership*/);
        if (!static_cast<D *>(this)->Load(&rd)) { return false; }

        initialized_ = true;
        return true;
    }

    T *FindOrNull(ResourceId id) const {
        // DCHECK(initialized());
        auto iter = resource_.find(id);
        return iter == resource_.end() ? nullptr : iter->second;
    }

    DISALLOW_IMPLICIT_CONSTRUCTORS(ResourceLibrary);

protected:
    void Put(ResourceId id, T *value) {
        DCHECK(resource_.find(id) == resource_.end());
        resource_[id] = value;
    }

private:
    base::Arena *const                       arena_;
    base::ArenaUnorderedMap<ResourceId, T *> resource_;
    bool                                     initialized_ = false;
};  // template<class T> class ResourceLibrary

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_RESOURCE_LIBRARY_H_