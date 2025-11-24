#pragma once


#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

#ifndef ENCHANTUM_ALIAS_STRING_VIEW
  #if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    // C++17 or later - use std::string_view
    #include <string_view>
  #else
    // C++14 - need to provide a polyfill or use a different type
    // For minimal changes, we'll use const char* as a stand-in
    // but ideally a proper string_view implementation would be used
    #include <cstring>
    #include <string>
  #endif
#endif


namespace enchantum {
#ifdef ENCHANTUM_ALIAS_STRING_VIEW
ENCHANTUM_ALIAS_STRING_VIEW;
#else
  #if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    using ::std::string_view;
  #else
    // C++14 polyfill: A minimal string_view implementation
    class string_view {
    public:
      using value_type = char;
      using size_type = std::size_t;
      using const_pointer = const char*;
      using const_reference = const char&;
      using const_iterator = const char*;
      
      static constexpr size_type npos = static_cast<size_type>(-1);
      
      constexpr string_view() noexcept : data_(nullptr), size_(0) {}
      
      constexpr string_view(const char* str, size_type len) noexcept 
        : data_(str), size_(len) {}
      
      constexpr string_view(const char* str) noexcept 
        : data_(str), size_(str ? std::char_traits<char>::length(str) : 0) {}
      
      string_view(const std::string& str) noexcept 
        : data_(str.data()), size_(str.size()) {}
      
      constexpr const_iterator begin() const noexcept { return data_; }
      constexpr const_iterator end() const noexcept { return data_ + size_; }
      
      constexpr const_reference operator[](size_type pos) const noexcept { return data_[pos]; }
      
      constexpr const char* data() const noexcept { return data_; }
      constexpr size_type size() const noexcept { return size_; }
      constexpr size_type length() const noexcept { return size_; }
      constexpr bool empty() const noexcept { return size_ == 0; }
      
      constexpr const_reference front() const noexcept { return data_[0]; }
      constexpr const_reference back() const noexcept { return data_[size_ - 1]; }
      
      constexpr void remove_prefix(size_type n) noexcept { data_ += n; size_ -= n; }
      constexpr void remove_suffix(size_type n) noexcept { size_ -= n; }
      
      constexpr string_view substr(size_type pos = 0, size_type count = npos) const {
        return string_view(data_ + pos, (count == npos || count > size_ - pos) ? size_ - pos : count);
      }
      
      constexpr int compare(string_view other) const noexcept {
        const size_type rlen = (size_ < other.size_) ? size_ : other.size_;
        int result = std::char_traits<char>::compare(data_, other.data_, rlen);
        if (result == 0) {
          if (size_ < other.size_) return -1;
          if (size_ > other.size_) return 1;
        }
        return result;
      }
      
    private:
      const char* data_;
      size_type size_;
    };
    
    constexpr bool operator==(string_view lhs, string_view rhs) noexcept {
      return lhs.compare(rhs) == 0;
    }
    
    constexpr bool operator!=(string_view lhs, string_view rhs) noexcept {
      return lhs.compare(rhs) != 0;
    }
    
    constexpr bool operator<(string_view lhs, string_view rhs) noexcept {
      return lhs.compare(rhs) < 0;
    }
    
    constexpr bool operator<=(string_view lhs, string_view rhs) noexcept {
      return lhs.compare(rhs) <= 0;
    }
    
    constexpr bool operator>(string_view lhs, string_view rhs) noexcept {
      return lhs.compare(rhs) > 0;
    }
    
    constexpr bool operator>=(string_view lhs, string_view rhs) noexcept {
      return lhs.compare(rhs) >= 0;
    }
  #endif
#endif

} // namespace enchantum