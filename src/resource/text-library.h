#pragma once
#ifndef NYAA_RESOURCE_TEXT_LIBRARY_H_
#define NYAA_RESOURCE_TEXT_LIBRARY_H_

#include "resource/text-id.h"
#include "base/arena.h"
#include "glog/logging.h"

namespace nyaa {

namespace res {

struct NRStrHeader {
    int  len;
    char data[0];
};  // struct NRStrHeader

using NRStr = const char *;

class TextLibrary final {
public:
    TextLibrary(base::Arena *arena) : arena_(arena) {}

    std::string_view Load(TextID id) const {
        int index = static_cast<int>(id);
        DCHECK_GE(index, 0);
        DCHECK_LT(index, MAX_TEXT_ID);
        return From(text_[index]);
    }

    bool Prepare(const std::string &file_name);

private:
    NRStr NewString(std::string_view input);

    static std::string_view From(NRStr s) {
        size_t len = *reinterpret_cast<const int *>(s - sizeof(int));
        return std::string_view(s, len);
    }

    base::Arena *const arena_;
    NRStr              text_[MAX_TEXT_ID];
};  // class TextLibrary

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_TEXT_LIBRARY_H_