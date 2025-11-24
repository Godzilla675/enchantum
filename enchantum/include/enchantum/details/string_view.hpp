#pragma once

#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

namespace enchantum {

#ifdef ENCHANTUM_ALIAS_STRING_VIEW
ENCHANTUM_ALIAS_STRING_VIEW;
#elif __cplusplus >= 201703L
  #include <string_view>
  using ::std::string_view;
#else
  // Minimal C++14 string_view polyfill
  #include <algorithm>
  #include <cstddef>
  #include <cstring>
  #include <ostream>
  #include <stdexcept>
  #include <string>

  class string_view {
  public:
    using traits_type = std::char_traits<char>;
    using value_type = char;
    using pointer = char*;
    using const_pointer = const char*;
    using reference = char&;
    using const_reference = const char&;
    using const_iterator = const char*;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static constexpr size_type npos = size_type(-1);

    // Helper for constexpr length
    static constexpr size_type length(const char* str) {
        size_type len = 0;
        while (str[len] != '\0') ++len;
        return len;
    }

    constexpr string_view() noexcept : data_(nullptr), size_(0) {}
    constexpr string_view(const char* str, size_type len) : data_(str), size_(len) {}
    constexpr string_view(const char* str) : data_(str), size_(str ? length(str) : 0) {}
    string_view(const std::string& str) noexcept : data_(str.data()), size_(str.size()) {}

    constexpr const_iterator begin() const noexcept { return data_; }
    constexpr const_iterator cbegin() const noexcept { return data_; }
    constexpr const_iterator end() const noexcept { return data_ + size_; }
    constexpr const_iterator cend() const noexcept { return data_ + size_; }

    constexpr size_type size() const noexcept { return size_; }
    constexpr size_type length() const noexcept { return size_; }
    constexpr bool empty() const noexcept { return size_ == 0; }

    constexpr const char* data() const noexcept { return data_; }

    constexpr const char& operator[](size_type pos) const { return data_[pos]; }

    constexpr const char& at(size_type pos) const {
      if (pos >= size_) throw std::out_of_range("string_view::at");
      return data_[pos];
    }

    constexpr void remove_prefix(size_type n) {
      if (n > size_) n = size_;
      data_ += n;
      size_ -= n;
    }

    constexpr void remove_suffix(size_type n) {
        if (n > size_) n = size_;
        size_ -= n;
    }

    constexpr string_view substr(size_type pos = 0, size_type count = npos) const {
       if (pos > size_) throw std::out_of_range("string_view::substr");
       const size_type rcount = std::min(count, size_ - pos);
       return string_view(data_ + pos, rcount);
    }

    constexpr int compare(string_view v) const noexcept {
      const size_type len = std::min(size_, v.size_);
      for(size_type i = 0; i < len; ++i) {
        if (data_[i] < v.data_[i]) return -1;
        if (data_[i] > v.data_[i]) return 1;
      }
      if (size_ < v.size_) return -1;
      if (size_ > v.size_) return 1;
      return 0;
    }

    constexpr int compare(const char* v) const { return compare(string_view(v)); }

    constexpr size_type find(char ch, size_type pos = 0) const noexcept {
        if (pos >= size_) return npos;
        for (size_type i = pos; i < size_; ++i) {
            if (data_[i] == ch) return i;
        }
        return npos;
    }

    constexpr size_type find(string_view v, size_type pos = 0) const noexcept {
        if (pos > size_) return npos;
        if (v.empty()) return pos;
        if (v.size() > size_ - pos) return npos;

        // Naive search
        for (size_type i = pos; i <= size_ - v.size(); ++i) {
             bool match = true;
             for (size_type j = 0; j < v.size(); ++j) {
                 if (data_[i + j] != v[j]) {
                     match = false;
                     break;
                 }
             }
             if (match) return i;
        }
        return npos;
    }

    constexpr size_type find(const char* s, size_type pos, size_type count) const {
        return find(string_view(s, count), pos);
    }
    constexpr size_type find(const char* s, size_type pos = 0) const {
        return find(string_view(s), pos);
    }

    constexpr size_type rfind(char ch, size_type pos = npos) const noexcept {
        if (size_ == 0) return npos;
        if (pos >= size_) pos = size_ - 1;

        for (difference_type i = static_cast<difference_type>(pos); i >= 0; --i) {
            if (data_[i] == ch) return static_cast<size_type>(i);
        }
        return npos;
    }

    constexpr size_type rfind(string_view v, size_type pos = npos) const noexcept {
        if (size_ < v.size()) return npos;
        if (pos > size_ - v.size()) pos = size_ - v.size();

        for (difference_type i = static_cast<difference_type>(pos); i >= 0; --i) {
            bool match = true;
            for (size_type j = 0; j < v.size(); ++j) {
                if (data_[i + j] != v[j]) {
                    match = false;
                    break;
                }
            }
            if (match) return static_cast<size_type>(i);
        }
        return npos;
    }

    constexpr size_type rfind(const char* s, size_type pos = npos) const {
        return rfind(string_view(s), pos);
    }

    friend constexpr bool operator==(string_view lhs, string_view rhs) noexcept {
      return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
    }
    friend constexpr bool operator!=(string_view lhs, string_view rhs) noexcept { return !(lhs == rhs); }
    friend constexpr bool operator<(string_view lhs, string_view rhs) noexcept { return lhs.compare(rhs) < 0; }
    friend constexpr bool operator<=(string_view lhs, string_view rhs) noexcept { return lhs.compare(rhs) <= 0; }
    friend constexpr bool operator>(string_view lhs, string_view rhs) noexcept { return lhs.compare(rhs) > 0; }
    friend constexpr bool operator>=(string_view lhs, string_view rhs) noexcept { return lhs.compare(rhs) >= 0; }

    friend std::ostream& operator<<(std::ostream& os, string_view v) {
      return os.write(v.data(), static_cast<std::streamsize>(v.size()));
    }

  private:
    const char* data_;
    size_type size_;
  };

#endif

} // namespace enchantum
