#pragma once
#ifndef NYAA_RESOURCE_DEFINITION_H_
#define NYAA_RESOURCE_DEFINITION_H_

#include "base/base.h"
#include <stdio.h>
#include <memory>
#include <string_view>
#include <vector>

namespace nyaa {

namespace res {

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
        while ((rv = reader->Read(&items)) == 0) {
            items.clear();
        }
        if (rv > 0) {
            static_cast<T *>(this)->Parse(items);
        }
        return rv;
    }

}; // template<class T> class DefinitionRow

    
} // namespace res

} // namespace nyaa


#endif // NYAA_RESOURCE_DEFINITION_H_