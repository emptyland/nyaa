#include "resource/definition.h"

namespace nyaa {

namespace res {

int DefinitionReader::Read(std::vector<std::string_view> *items) {
    int rv = ::fscanf(file_, "%s\n", buf_.get());
    if (rv == EOF) {
        return -1;
    }
    buf_[rv] = 0;
    if (buf_[0] == '#' || buf_[0] == '-' || rv == 0) { // Comment
        return 0;
    }
    SplitLine(items);
    return rv;
}

void DefinitionReader::SplitLine(std::vector<std::string_view> *items) {
    const char *p = buf_.get();
    const char *start = p;
    while (*p) {
        if (*p == '|' || *p == '\n') {
            if (p - start > 0) {
                while (*start == ' ' || *start == '\t') { start++; }
                while (*(p - 1) == ' ' || *(p - 1) == '\t') { p--; }
                items->push_back(std::string_view(start, p - start));
            }
            start = ++p;
        } else {
            p++;
        }
    }
}

} // namespace res

} // namespace nyaa