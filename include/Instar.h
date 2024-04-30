#ifndef INSTAR_H_
#define INSTAR_H_

#include "Output.h"

class Instar {
private:
    unsigned int value;

public:
    Instar();
    explicit Instar(unsigned int value);
    inline const unsigned int& getValue() const { return value; }
    inline void moveOnPreviousInstar() { value--; }
    inline void moveOnNextInstar() { value++; }
    friend Instar operator+(const Instar& lhs, const int& rhs);
    friend Instar operator+(const int& lhs, const Instar& rhs);
    friend bool operator<(const Instar& lhs, const Instar& rhs);
    friend bool operator<(const Instar& lhs, const int& rhs);
    friend bool operator<(const int& lhs, const Instar& rhs);
    friend bool operator<=(const Instar& lhs, const Instar& rhs);
    friend bool operator<=(const Instar& lhs, const int& rhs);
    friend bool operator<=(const int& lhs, const Instar& rhs);
    friend bool operator>(const Instar& lhs, const Instar& rhs);
    friend bool operator>(const Instar& lhs, const int& rhs);
    friend bool operator>(const int& lhs, const Instar& rhs);
    friend bool operator>=(const Instar& lhs, const Instar& rhs);
    friend bool operator>=(const Instar& lhs, const int& rhs);
    friend bool operator>=(const int& lhs, const Instar& rhs);
    friend bool operator==(const Instar& lhs, const Instar& rhs);
    friend bool operator==(const Instar& lhs, const int& rhs);
    friend bool operator==(const int& lhs, const Instar& rhs);
    friend bool operator!=(const Instar& lhs, const Instar& rhs);
    friend bool operator!=(const Instar& lhs, const int& rhs);
    friend bool operator!=(const int& lhs, const Instar& rhs);
    friend std::ostream& operator<<(std::ostream& os, Instar const& instar);
    std::string format() const {
        return fmt::format("{}", value+1);
    }
};

template <> struct fmt::formatter<Instar> {
    // Presentation format: 'f' - fixed, 'e' - exponential.
    char presentation = 'f';

    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // [ctx.begin(), ctx.end()) is a character range that contains a part of
        // the format string starting from the format specifications to be parsed,
        // e.g. in
        //
        //   fmt::format("{:f} - point of interest", point{1, 2});
        //
        // the range will contain "f} - point of interest". The formatter should
        // parse specifiers until '}' or the end of the range. In this example
        // the formatter should parse the 'f' specifier and return an iterator
        // pointing to '}'.

        // Please also note that this character range may be empty, in case of
        // the "{}" format string, so therefore you should check ctx.begin()
        // for equality with ctx.end().

        // Parse the presentation format and store it in the formatter:
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

        // Check if reached the end of the range:
        if (it != end && *it != '}') throw format_error("invalid format");

        // Return an iterator past the end of the parsed range:
        return ctx.begin();
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const Instar& instar, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "{}", instar.getValue()+1);
    }
};

#endif /* INSTAR_H_ */