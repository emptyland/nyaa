#pragma once
#ifndef NYAA_RESOURCE_TEXT_FORMATTER_H_
#define NYAA_RESOURCE_TEXT_FORMATTER_H_

#include "resource/text-library.h"
#include "game/game.h"
#include "base/slice.h"

namespace nyaa {

namespace res {

template <class T>
struct Textualizer {
    static inline std::string Of(const T &value) { return ""; }
};

template <>
struct Textualizer<int> {
    static inline std::string Of(const int &value) { return base::Sprintf("%d", value); }
};

template <>
struct Textualizer<unsigned> {
    static inline std::string Of(const unsigned &value) { return base::Sprintf("%u", value); }
};

template <>
struct Textualizer<long> {
    static inline std::string Of(const long &value) { return base::Sprintf("%ld", value); }
};

template <>
struct Textualizer<unsigned long> {
    static inline std::string Of(const unsigned long &value) { return base::Sprintf("%lu", value); }
};

template <>
struct Textualizer<float> {
    static inline std::string Of(const float &value) { return base::Sprintf("%0.2f", value); }
};

template <>
struct Textualizer<double> {
    static inline std::string Of(const double &value) { return base::Sprintf("%0.2f", value); }
};

template <>
struct Textualizer<const char *> {
    static inline std::string Of(const char *value) { return value; }
};

template <>
struct Textualizer<std::string> {
    static inline std::string Of(const std::string &value) { return value; }
};

template <>
struct Textualizer<std::string_view> {
    static inline std::string Of(const std::string_view &value) { return std::string(value.data(), value.size()); }
};

template <>
struct Textualizer<TextID> {
    static inline std::string Of(const TextID &value) { return Game::This()->text_lib()->Clone(value); }
};

class FormatScanner {
public:
    FormatScanner(std::string_view slice)
        : iter_(slice.data()), start_(slice.data()), end_(slice.data() + slice.size()) {}

    bool Valid() { return iter_ < end_; }

    bool Next();

    std::string Segment() const { return std::string(latest_segment_.data(), latest_segment_.size()); }

    std::string Left() const { return std::string(start_, end_ - start_); }

private:
    const char *      iter_;
    const char *      start_;
    const char *const end_;
    std::string_view  latest_segment_;
};  // class FormatScanner

inline std::string Format(TextID fmt) {
    std::string_view slice = Game::This()->text_lib()->Load(fmt);
    return std::string(slice.data(), slice.size());
}

// res::Format("a={}, b={}, c={}", 1, 2, 3)
template <class A>
inline std::string Format(TextID fmt, const A &a) {
    FormatScanner scanner(Game::This()->text_lib()->Load(fmt));

    std::string text;
    if (!scanner.Valid()) { return text; }
    if (scanner.Next()) {
        text.append(scanner.Segment());
        text.append(Textualizer<A>::Of(a));
    }
    text.append(scanner.Left());
    return text;
}

template <class A, class B>
inline std::string Format(TextID fmt, const A &a, const B &b) {
    FormatScanner scanner(Game::This()->text_lib()->Load(fmt));

    std::string text;
    if (!scanner.Valid()) { return text; }
    if (scanner.Next()) {
        text.append(scanner.Segment());
        text.append(Textualizer<A>::Of(a));
    } else {
        text.append(scanner.Left());
        return text;
    }
    if (scanner.Next()) {
        text.append(scanner.Segment());
        text.append(Textualizer<B>::Of(b));
    }
    text.append(scanner.Left());
    return text;
}

template <class A, class B, class C>
inline std::string Format(TextID fmt, const A &a, const B &b, const C &c) {
    FormatScanner scanner(Game::This()->text_lib()->Load(fmt));

    std::string text;
    if (!scanner.Valid()) { return text; }
    if (scanner.Next()) {
        text.append(scanner.Segment());
        text.append(Textualizer<A>::Of(a));
    } else {
        text.append(scanner.Left());
        return text;
    }
    if (scanner.Next()) {
        text.append(scanner.Segment());
        text.append(Textualizer<B>::Of(b));
    } else {
        text.append(scanner.Left());
        return text;
    }
    if (scanner.Next()) {
        text.append(scanner.Segment());
        text.append(Textualizer<C>::Of(c));
    }
    text.append(scanner.Left());
    return text;
}

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_TEXT_FORMATTER_H_