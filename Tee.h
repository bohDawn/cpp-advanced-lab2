#pragma once

#include <streambuf>

// Stream buffer that duplicates everything written to it into two buffers
// (used to print to the console and to a result file at the same time).
class TeeStreambuf : public std::streambuf {
public:
    TeeStreambuf(std::streambuf* first, std::streambuf* second)
        : first_(first), second_(second) {}

protected:
    int_type overflow(int_type ch) override {
        if (traits_type::eq_int_type(ch, traits_type::eof())) {
            return traits_type::not_eof(ch);
        }
        const auto c = traits_type::to_char_type(ch);
        const auto r1 = first_->sputc(c);
        const auto r2 = second_->sputc(c);
        if (traits_type::eq_int_type(r1, traits_type::eof()) ||
            traits_type::eq_int_type(r2, traits_type::eof())) {
            return traits_type::eof();
        }
        return ch;
    }

    int sync() override {
        const int r1 = first_->pubsync();
        const int r2 = second_->pubsync();
        return (r1 == 0 && r2 == 0) ? 0 : -1;
    }

private:
    std::streambuf* first_;
    std::streambuf* second_;
};
