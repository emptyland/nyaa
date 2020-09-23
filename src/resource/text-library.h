#pragma once
#ifndef NYAA_RESOURCE_TEXT_LIBRARY_H_
#define NYAA_RESOURCE_TEXT_LIBRARY_H_

#include "resource/text-def-inl.h"
#include "base/arena-utils.h"
#include "base/slice.h"
#include "glog/logging.h"
#include <stdarg.h>

namespace nyaa {

namespace res {

struct NRStrHeader {
    int  len;
    char data[0];
};  // struct NRStrHeader

using NRStr = const char *;

class TextLibrary final {
public:
    TextLibrary(base::Arena *arena);

    std::string_view Load(TextID id) const {
        int index = static_cast<int>(id);
        DCHECK_GE(index, 0);
        DCHECK_LT(index, MAX_TEXT_ID);
        return From(text_[index]);
    }

    std::string Clone(TextID id) const {
        std::string_view slice = Load(id);
        return std::string(slice.data(), slice.size());
    }

    std::string Vsprintf(TextID fmt, va_list ap) const {
        std::string_view text = Load(fmt);
        return base::Vsprintf(text.data(), ap);
    }

    TextID FindID(std::string_view name) const {
        auto iter = text_name_to_id_.find(name);
        return iter == text_name_to_id_.end() ? MAX_TEXT_ID : iter->second;
    }

    bool Prepare(const std::string &file_name);

private:
    NRStr NewString(std::string_view input);

    static std::string_view From(NRStr s) {
        size_t len = *reinterpret_cast<const int *>(s - sizeof(int));
        return std::string_view(s, len);
    }

    base::Arena *const                                arena_;
    base::ArenaUnorderedMap<std::string_view, TextID> text_name_to_id_;
    NRStr                                             text_[MAX_TEXT_ID];

    static const char *kTextIDName[MAX_TEXT_ID];
};  // class TextLibrary

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_TEXT_LIBRARY_H_