#include "resource/text-library.h"
#include "resource/definition.h"
#include "glog/logging.h"

namespace nyaa {

namespace res {

bool TextLibrary::Prepare(const std::string &file_name) {
    FILE *fp = ::fopen(file_name.c_str(), "r");
    if (!fp) {
        DLOG(ERROR) << "can not open text resource file: " << file_name;
        return false;
    }
    DefinitionReader rd(fp, true/*ownership*/);
    std::vector<std::string_view> items;
    items.reserve(2);

    int index = 0;
    int rv = 0;
    while ((rv = rd.Read(&items)) != EOF) {
        if (rv == 0) {
            continue;
        }
        if (index >= MAX_TEXT_ID) {
            DLOG(ERROR) << "Text resource file out of bound(" << MAX_TEXT_ID << ")";
            return false;
        }
        text_[index++] = NewString(items[1]);
        items.clear();
    }
    DCHECK_EQ(index, MAX_TEXT_ID);
    return true;
}

const char *TextLibrary::NewString(std::string_view input) {
    size_t request_size = sizeof(NRStrHeader) + input.size() + 1;
    NRStrHeader *header = static_cast<NRStrHeader *>(arena_.Allocate(request_size));
    header->len = static_cast<int>(input.size());
    ::memcpy(header->data, input.data(), input.size());
    header->data[input.size()] = 0;
    return &header->data[0];
}


} // namespace res
    
} // namespace nyaa