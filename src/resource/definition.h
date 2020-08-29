#pragma once
#ifndef NYAA_RESOURCE_DEFINITION_H_
#define NYAA_RESOURCE_DEFINITION_H_

#include "game/identifiers.h"
#include "game/vector.h"
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
    VECTOR2I,
    VECTOR3F,
    BOUNDI,
    ARRAY_I32,
    ARRAY_U32,
};  // enum class DefValType

class DefinitionReader {
public:
    static constexpr size_t kBufSize = 4096;

    explicit DefinitionReader(FILE *file, bool ownership = false)
        : file_(file), ownership_(ownership), buf_(new char[kBufSize]) {}

    ~DefinitionReader() {
        if (ownership_) { ::fclose(file_); }
    }

    int Read(std::vector<std::string_view> *items);

    DISALLOW_IMPLICIT_CONSTRUCTORS(DefinitionReader);

private:
    void SplitLine(std::vector<std::string_view> *items);

    FILE *                  file_;
    const bool              ownership_;
    std::unique_ptr<char[]> buf_;
};  // class DefinitionReader

template <DefValType Ty>
struct DefValParser {};

template <>
struct DefValParser<DefValType::STRING> {
    using Type = std::string;
    int Parse(std::string_view input, std::string *receive) {
        *receive = input;
        return 0;
    }
};

template <>
struct DefValParser<DefValType::I32> {
    using Type = int32_t;
    int Parse(std::string_view input, int32_t *receive) {
        return base::Slice::ParseI32(input.data(), input.size(), receive);
    }
};

template <>
struct DefValParser<DefValType::U32> {
    using Type = uint32_t;
    int Parse(std::string_view input, uint32_t *receive) {
        return base::Slice::ParseI32(input.data(), input.size(), bit_cast<int32_t *>(receive));
    }
};

template <>
struct DefValParser<DefValType::I64> {
    using Type = int64_t;
    int Parse(std::string_view input, int64_t *receive) {
        return base::Slice::ParseI64(input.data(), input.size(), receive);
    }
};

template <>
struct DefValParser<DefValType::U64> {
    using Type = uint64_t;
    int Parse(std::string_view input, uint64_t *receive) {
        return base::Slice::ParseU64(input.data(), input.size(), receive);
    }
};

template <>
struct DefValParser<DefValType::F32> {
    using Type = float;
    int Parse(std::string_view input, float *receive) {
        std::string tmp(input);
        *receive = ::atof(tmp.c_str());
        return 0;
    }
};

template <>
struct DefValParser<DefValType::F64> {
    using Type = double;
    int Parse(std::string_view input, double *receive) {
        std::string tmp(input);
        *receive = ::atof(tmp.c_str());
        return 0;
    }
};

template <>
struct DefValParser<DefValType::ID> {
    using Type = ResourceId;
    int Parse(std::string_view input, ResourceId *receive) {
        int32_t id = 0;
        if (base::Slice::ParseI32(input.data(), input.size(), &id) == 0) {
            *receive = ResourceId::Of(id);
            return 0;
        }
        return -1;
    }
};

template <class T, int Items = sizeof(T) / sizeof(int)>
int ParseVertex_Ni(std::string_view input, T *vec) {
    int         value[4] = {0}, i = 0;
    const char *start = input.data(), *p = start, *e = start + input.size();
    while (p < e) {
        if (*p == ',') {
            if (i > Items - 1) { return -1; }
            if (int err = base::Slice::ParseI32(start, p - start, &value[i++]); err) { return err; }
            start = p + 1;
        }
        p++;
    }
    if (i > Items - 1) { return -1; }
    if (int err = base::Slice::ParseI32(start, p - start, &value[i++]); err) { return err; }
    ::memcpy(vec, value, sizeof(*vec));
    return 0;
}

template <class T, int Items = sizeof(T) / sizeof(float)>
int ParseVertex_Nf(std::string_view input, T *vec) {
    float       value[4] = {0};
    int         i        = 0;
    const char *start = input.data(), *p = start, *e = start + input.size();
    while (p < e) {
        if (*p == ',') {
            if (i > Items - 1) { return -1; }
            value[i++] = ::atof(std::string(start, p - start).c_str());
            start      = p + 1;
        }
        p++;
    }
    if (i > Items - 1) { return -1; }
    // if (int err = base::Slice::ParseF32(start, p - start, &value[i++]); err) { return err; }
    value[i++] = ::atof(std::string(start, p - start).c_str());
    ::memcpy(vec, value, sizeof(*vec));
    return 0;
}

int ParseArray_i32(std::string_view input, int32_t *receive);

template <>
struct DefValParser<DefValType::VECTOR2I> {
    using Type = Vector2i;
    int Parse(std::string_view input, Vector2i *receive) { return ParseVertex_Ni(input, receive); }
};

template <>
struct DefValParser<DefValType::VECTOR3F> {
    using Type = Vector3f;
    int Parse(std::string_view input, Vector3f *receive) { return ParseVertex_Nf(input, receive); }
};

template <>
struct DefValParser<DefValType::BOUNDI> {
    using Type = Vector4i;
    int Parse(std::string_view input, Boundi *receive) { return ParseVertex_Ni(input, receive); }
};

template <>
struct DefValParser<DefValType::ARRAY_I32> {
    using Type = int32_t;
    int Parse(std::string_view input, int32_t *receive) { return ParseArray_i32(input, receive); }
};

template <>
struct DefValParser<DefValType::ARRAY_U32> {
    using Type = int32_t;
    int Parse(std::string_view input, int32_t *receive) { return ParseArray_i32(input, receive); }
};

template <class T>
class Definition {
public:
    int Read(DefinitionReader *reader) {
        std::vector<std::string_view> items;
        int                           rv = 0;
        while ((rv = reader->Read(&items)) == 0) {}
        if (rv > 0) { static_cast<T *>(this)->Parse(items); }
        return rv;
    }

    using ClassId = uintptr_t;
    static ClassId class_id() { return reinterpret_cast<uintptr_t>(&class_); }

protected:
    template <DefValType Ty, class S = typename DefValParser<Ty>::Type>
    inline int ParseValue(std::string_view input, S *receiver) {
        return DefValParser<Ty>{}.Parse(input, receiver);
    }

    int ParseValue(std::string_view input, DefValType type, void *receiver);

    static const int class_;
};  // template<class T> class DefinitionRow

template <class T>
int Definition<T>::ParseValue(std::string_view input, DefValType type, void *receiver) {
    switch (type) {
        case DefValType::STRING:
            return DefValParser<DefValType::STRING>{}.Parse(input, static_cast<std::string *>(receiver));
        case DefValType::I32: return DefValParser<DefValType::I32>{}.Parse(input, static_cast<int32_t *>(receiver));
        case DefValType::U32: return DefValParser<DefValType::U32>{}.Parse(input, static_cast<uint32_t *>(receiver));
        case DefValType::I64: return DefValParser<DefValType::I64>{}.Parse(input, static_cast<int64_t *>(receiver));
        case DefValType::U64: return DefValParser<DefValType::U64>{}.Parse(input, static_cast<uint64_t *>(receiver));
        case DefValType::F32: return DefValParser<DefValType::F32>{}.Parse(input, static_cast<float *>(receiver));
        case DefValType::F64: return DefValParser<DefValType::F64>{}.Parse(input, static_cast<double *>(receiver));
        case DefValType::ID: return DefValParser<DefValType::ID>{}.Parse(input, static_cast<ResourceId *>(receiver));
        case DefValType::VECTOR2I:
            return DefValParser<DefValType::VECTOR2I>{}.Parse(input, static_cast<Vector2i *>(receiver));
        case DefValType::BOUNDI:
            return DefValParser<DefValType::BOUNDI>{}.Parse(input, static_cast<Boundi *>(receiver));
        case DefValType::ARRAY_I32:
            return DefValParser<DefValType::ARRAY_I32>{}.Parse(input, static_cast<int32_t *>(receiver));
        case DefValType::ARRAY_U32:
            return DefValParser<DefValType::ARRAY_U32>{}.Parse(input, static_cast<int32_t *>(receiver));
        default: NOREACHED(); break;
    }
    return 0;
}

template <class T> /*static*/ const int Definition<T>::class_ = 0;

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_DEFINITION_H_