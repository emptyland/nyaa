#include "resource/definition.h"

namespace nyaa {

namespace res {

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