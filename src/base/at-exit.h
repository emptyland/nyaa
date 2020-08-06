#pragma once
#ifndef NYAA_AT_EXIT_H_
#define NYAA_AT_EXIT_H_

#include <mutex>

namespace nyaa {

class AtExit {
public:
    enum Linker {
        INITIALIZER
    };
    using Callback = void (*)(void *);
    
    explicit AtExit(Linker);
    ~AtExit();

    static AtExit *This();
    
    void Register(Callback callback, void *params);
private:
    struct Hook;
    
    AtExit *prev_ = nullptr;
    Hook *hook_ = nullptr;
    std::mutex mutex_;
}; // class AtExit

} // namespace nyaa

#endif // NYAA_AT_EXIT_H_
