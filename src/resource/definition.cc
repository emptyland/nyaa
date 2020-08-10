#include "resource/definition.h"

namespace nyaa {

namespace res {

int ParseArray_i32(std::string_view input, int32_t *receive) {
        const char *start = input.data(), *p = start, *e = start + input.size();
    int i = 1;
    while (p < e) {
        if (*p == ',') {
            if (int err = base::Slice::ParseI32(start, p - start, &receive[i++]); err) {
                return err;
            }
            start = p + 1;
        }
        p++;
    }
    if (int err = base::Slice::ParseI32(start, p - start, &receive[i++]); err) {
        return err;
    }
    *receive = i - 1;
    return 0;
}

int DefinitionReader::Read(std::vector<std::string_view> *items) {
    if (::feof(file_)) {
        return EOF;
    }
    buf_[0] = 0;
    ::fgets(buf_.get(), kBufSize, file_);
    if (buf_[0] == '#' || buf_[0] == '-' || buf_[0] == 0) { // Comment
        return 0;
    }
    SplitLine(items);
    return 1;
}

void DefinitionReader::SplitLine(std::vector<std::string_view> *items) {
    const char *p = buf_.get();
    const char *start = p;
    while (*p) {
        if (*p == '|' || *p == '\n') {
            if (p - start > 0) {
                while ((*start == ' ' || *start == '\t') && start < p) { start++; }
                const char *e = p;
                while ((*(e - 1) == ' ' || *(e - 1) == '\t') && e > start) { e--; }
                items->push_back(std::string_view(start, e - start));
            }
            start = ++p;
        } else {
            p++;
        }
    }
}

} // namespace res

} // namespace nyaa