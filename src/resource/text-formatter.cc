#include "resource/text-formatter.h"

namespace nyaa {

namespace res {

namespace detail {

bool FormatScanner::Next() {
    latest_segment_ = std::string_view();
    while (iter_ < end_) {
        if (*iter_ == '\\') {
            if (iter_ + 1 < end_ && iter_[1] == '{') {
                iter_ += 2;
                continue;
            }
        } else if (*iter_ == '{') {
            if (iter_ + 1 < end_ && iter_[1] == '}') {
                latest_segment_ = std::string_view(start_, iter_ - start_);
                iter_ += 2;
                start_ = iter_;
                return true;
            }
        }
        iter_++;
    }
    return false;
}

}  // namespace detail

}  // namespace res

}  // namespace nyaa
