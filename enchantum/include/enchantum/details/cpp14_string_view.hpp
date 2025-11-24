#pragma once

// Minimal C++14-compatible string_view implementation for enchantum
// Only implements the features needed by enchantum

#include <algorithm>
#include <cstring>
#include <iosfwd>
#include <iterator>
#include <stdexcept>
#include <string>

namespace enchantum {
namespace detail {

template<typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_view {
public:
    using traits_type = Traits;
    using value_type = CharT;
    using pointer = CharT*;
    using const_pointer = const CharT*;
    using reference = CharT&;
    using const_reference = const CharT&;
    using const_iterator = const CharT*;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static constexpr size_type npos = size_type(-1);

private:
    const_pointer data_;
    size_type size_;

public:
    constexpr basic_string_view() noexcept : data_(nullptr), size_(0) {}

    constexpr basic_string_view(const basic_string_view&) noexcept = default;

    constexpr basic_string_view(const CharT* s, size_type count) noexcept
        : data_(s), size_(count) {}

    constexpr basic_string_view(const CharT* s) noexcept
        : data_(s), size_(s ? Traits::length(s) : 0) {}

    basic_string_view& operator=(const basic_string_view&) noexcept = default;

    constexpr const_iterator begin() const noexcept { return data_; }
    constexpr const_iterator end() const noexcept { return data_ + size_; }
    constexpr const_iterator cbegin() const noexcept { return begin(); }
    constexpr const_iterator cend() const noexcept { return end(); }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator crend() const noexcept { return rend(); }

    constexpr const_reference operator[](size_type pos) const {
        return data_[pos];
    }

    constexpr const_reference at(size_type pos) const {
        return pos < size_ ? data_[pos] : 
            (throw std::out_of_range("basic_string_view::at"), data_[0]);
    }

    constexpr const_reference front() const { return data_[0]; }
    constexpr const_reference back() const { return data_[size_ - 1]; }
    constexpr const_pointer data() const noexcept { return data_; }

    constexpr size_type size() const noexcept { return size_; }
    constexpr size_type length() const noexcept { return size_; }
    constexpr size_type max_size() const noexcept { return npos - 1; }
    constexpr bool empty() const noexcept { return size_ == 0; }

    constexpr void remove_prefix(size_type n) {
        data_ += n;
        size_ -= n;
    }

    constexpr void remove_suffix(size_type n) {
        size_ -= n;
    }

    constexpr void swap(basic_string_view& other) noexcept {
        const_pointer tmp_data = data_;
        size_type tmp_size = size_;
        data_ = other.data_;
        size_ = other.size_;
        other.data_ = tmp_data;
        other.size_ = tmp_size;
    }

    size_type copy(CharT* dest, size_type count, size_type pos = 0) const {
        if (pos > size_) {
            throw std::out_of_range("basic_string_view::copy");
        }
        const size_type rcount = std::min(count, size_ - pos);
        Traits::copy(dest, data_ + pos, rcount);
        return rcount;
    }

    constexpr basic_string_view substr(size_type pos = 0, size_type count = npos) const {
        return pos > size_ ? 
            (throw std::out_of_range("basic_string_view::substr"), basic_string_view{}) :
            basic_string_view{data_ + pos, std::min(count, size_ - pos)};
    }

    constexpr int compare(basic_string_view other) const noexcept {
        const size_type rlen = std::min(size_, other.size_);
        const int cmp = rlen == 0 ? 0 : Traits::compare(data_, other.data_, rlen);
        return cmp != 0 ? cmp : (size_ < other.size_ ? -1 : (size_ > other.size_ ? 1 : 0));
    }

    constexpr int compare(size_type pos1, size_type count1, basic_string_view other) const {
        return substr(pos1, count1).compare(other);
    }

    constexpr int compare(size_type pos1, size_type count1,
                         basic_string_view other,
                         size_type pos2, size_type count2) const {
        return substr(pos1, count1).compare(other.substr(pos2, count2));
    }

    constexpr int compare(const CharT* s) const {
        return compare(basic_string_view{s});
    }

    constexpr int compare(size_type pos1, size_type count1, const CharT* s) const {
        return substr(pos1, count1).compare(basic_string_view{s});
    }

    constexpr int compare(size_type pos1, size_type count1,
                         const CharT* s, size_type count2) const {
        return substr(pos1, count1).compare(basic_string_view{s, count2});
    }

    constexpr size_type find(basic_string_view str, size_type pos = 0) const noexcept {
        if (pos > size_ || str.size_ > size_ - pos) {
            return npos;
        }
        if (str.size_ == 0) {
            return pos;
        }
        const_pointer found = std::search(begin() + pos, end(), str.begin(), str.end(), Traits::eq);
        return found == end() ? npos : static_cast<size_type>(found - begin());
    }

    constexpr size_type find(CharT ch, size_type pos = 0) const noexcept {
        return find(basic_string_view{&ch, 1}, pos);
    }

    constexpr size_type find(const CharT* s, size_type pos, size_type count) const {
        return find(basic_string_view{s, count}, pos);
    }

    constexpr size_type find(const CharT* s, size_type pos = 0) const {
        return find(basic_string_view{s}, pos);
    }

    constexpr size_type rfind(basic_string_view str, size_type pos = npos) const noexcept {
        if (str.size_ > size_) {
            return npos;
        }
        if (str.size_ == 0) {
            return std::min(pos, size_);
        }
        pos = std::min(pos, size_ - str.size_);
        for (size_type i = pos + 1; i > 0; --i) {
            if (Traits::compare(data_ + i - 1, str.data_, str.size_) == 0) {
                return i - 1;
            }
        }
        return npos;
    }

    constexpr size_type rfind(CharT ch, size_type pos = npos) const noexcept {
        return rfind(basic_string_view{&ch, 1}, pos);
    }

    constexpr size_type rfind(const CharT* s, size_type pos, size_type count) const {
        return rfind(basic_string_view{s, count}, pos);
    }

    constexpr size_type rfind(const CharT* s, size_type pos = npos) const {
        return rfind(basic_string_view{s}, pos);
    }

    // Helper for conversion to std::string
    std::string to_string() const {
        return std::string(data_, size_);
    }

    explicit operator std::string() const {
        return to_string();
    }
};

// Comparison operators
template<typename CharT, typename Traits>
constexpr bool operator==(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept {
    return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
}

template<typename CharT, typename Traits>
constexpr bool operator!=(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept {
    return !(lhs == rhs);
}

template<typename CharT, typename Traits>
constexpr bool operator<(basic_string_view<CharT, Traits> lhs,
                         basic_string_view<CharT, Traits> rhs) noexcept {
    return lhs.compare(rhs) < 0;
}

template<typename CharT, typename Traits>
constexpr bool operator<=(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept {
    return lhs.compare(rhs) <= 0;
}

template<typename CharT, typename Traits>
constexpr bool operator>(basic_string_view<CharT, Traits> lhs,
                         basic_string_view<CharT, Traits> rhs) noexcept {
    return lhs.compare(rhs) > 0;
}

template<typename CharT, typename Traits>
constexpr bool operator>=(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept {
    return lhs.compare(rhs) >= 0;
}

// Stream output
template<typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
                                               basic_string_view<CharT, Traits> str) {
    return os.write(str.data(), str.size());
}

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;

} // namespace detail
} // namespace enchantum
