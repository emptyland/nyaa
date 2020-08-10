#pragma once
#ifndef NYAA_RESOURCE_DEFINITION_H_
#define NYAA_RESOURCE_DEFINITION_H_

#include "game/identifiers.h"
#include "base/slice.h"
#include "base/base.h"
#include <memory>
#include <string_view>
#include <vector>

namespace nyaa {

namespace res {

enum class DefValType {
    STRING,
    I32,
    I64,
    U32,
    U64,
    F32,
    F64,
    ID,
    VERTEX4I,
}; // enum class DefValType

class DefinitionReader {
public:
    static constexpr size_t kBufSize = 4096;

    explicit DefinitionReader(FILE *file, bool ownership = false)
        : file_(file)
        , ownership_(ownership)
        , buf_(new char[kBufSize]) {}
    
    ~DefinitionReader() {
        if (ownership_) { ::fclose(file_); }
    }

    int Read(std::vector<std::string_view> *items);

    DISALLOW_IMPLICIT_CONSTRUCTORS(DefinitionReader);
private:
    void SplitLine(std::vector<std::string_view> *items);

    FILE *file_;
    const bool ownership_;
    std::unique_ptr<char[]> buf_;
}; // class DefinitionReader

template<class T>
class Definition {
public:
    int Read(DefinitionReader *reader) {
        std::vector<std::string_view> items;
        int rv = 0;
        while ((rv = reader->Read(&items)) == 0) {}
        if (rv > 0) {
            static_cast<T *>(this)->Parse(items);
        }
        return rv;
    }

    using ClassId = uintptr_t;
    static ClassId class_id() { return reinterpret_cast<uintptr_t>(&class_); }

protected:
    int ParseValue(std::string_view src, DefValType type, void *receive) {
        switch (type) {
        case res::DefValType::STRING:
            *static_cast<std::string*>(receive) = src;
            break;
        case res::DefValType::I32:
            return base::Slice::ParseI32(src.data(), src.size(), static_cast<int32_t*>(receive));
        case res::DefValType::U32:
            return base::Slice::ParseI32(src.data(), src.size(), static_cast<int32_t*>(receive));
        case res::DefValType::I64:
            return base::Slice::ParseI64(src.data(), src.size(), static_cast<int64_t*>(receive));
        case res::DefValType::U64:
            return base::Slice::ParseU64(src.data(), src.size(), static_cast<uint64_t*>(receive));
        case res::DefValType::F32:
            TODO();
            break;
        case res::DefValType::F64:
            TODO();
            break;
        case res::DefValType::ID: {
            int32_t id = 0;
            if (base::Slice::ParseI32(src.data(), src.size(), &id) == 0) {
                *static_cast<ResourceId *>(receive) = ResourceId::Of(id);
            }
        } break;
        case res::DefValType::VERTEX4I:
            return ParseVertex4i(src, static_cast<Vertex4i*>(receive));
        default:
            break;
        }
        return 0;
    }

    int ParseVertex4i(std::string_view input, Vertex4i *vec) {
        int value[4] = {0}, i = 0;
        const char *start = input.data(), *p = start, *e = start + input.size();
        while (p < e) {
            if (*p == ',') {
                if (i > 3) {
                    return -1;
                }
                if (int err = base::Slice::ParseI32(start, p - start, &value[i++]); err) {
                    return err;
                }
                start = p + 1;
            }
            p++;
        }
        if (i > 3) {
            return -1;
        }
        if (int err = base::Slice::ParseI32(start, p - start, &value[i++]); err) {
            return err;
        }
        ::memcpy(vec, value, sizeof(*vec));
        return 0;
    }

    static const int class_;
}; // template<class T> class DefinitionRow

template<class T> /*static*/ const int Definition<T>::class_ = 0;
    
} // namespace res

} // namespace nyaa


#endif // NYAA_RESOURCE_DEFINITION_H_