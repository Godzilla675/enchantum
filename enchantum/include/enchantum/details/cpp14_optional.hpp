#pragma once

// Minimal C++14-compatible optional implementation for enchantum
// Only implements the features needed by enchantum

#include <stdexcept>
#include <utility>
#include <type_traits>

namespace enchantum {
namespace detail {

struct nullopt_t {
    constexpr explicit nullopt_t(int) {}
};

constexpr nullopt_t nullopt{0};

template<typename T>
class optional {
private:
    alignas(T) unsigned char storage_[sizeof(T)];
    bool has_value_;

    T* ptr() { return reinterpret_cast<T*>(storage_); }
    const T* ptr() const { return reinterpret_cast<const T*>(storage_); }

public:
    using value_type = T;

    constexpr optional() noexcept : has_value_(false) {}
    
    constexpr optional(nullopt_t) noexcept : has_value_(false) {}

    optional(const optional& other) : has_value_(other.has_value_) {
        if (has_value_) {
            new (storage_) T(*other.ptr());
        }
    }

    optional(optional&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
        : has_value_(other.has_value_) {
        if (has_value_) {
            new (storage_) T(std::move(*other.ptr()));
        }
    }

    template<typename U = T,
             typename = typename std::enable_if<
                 std::is_constructible<T, U&&>::value &&
                 !std::is_same<typename std::decay<U>::type, optional<T>>::value>::type>
    constexpr optional(U&& value)
        : has_value_(true) {
        new (storage_) T(std::forward<U>(value));
    }

    ~optional() {
        if (has_value_) {
            ptr()->~T();
        }
    }

    optional& operator=(nullopt_t) noexcept {
        reset();
        return *this;
    }

    optional& operator=(const optional& other) {
        if (this != &other) {
            if (has_value_ && other.has_value_) {
                *ptr() = *other.ptr();
            } else if (other.has_value_) {
                new (storage_) T(*other.ptr());
                has_value_ = true;
            } else if (has_value_) {
                reset();
            }
        }
        return *this;
    }

    optional& operator=(optional&& other)
        noexcept(std::is_nothrow_move_assignable<T>::value &&
                 std::is_nothrow_move_constructible<T>::value) {
        if (this != &other) {
            if (has_value_ && other.has_value_) {
                *ptr() = std::move(*other.ptr());
            } else if (other.has_value_) {
                new (storage_) T(std::move(*other.ptr()));
                has_value_ = true;
            } else if (has_value_) {
                reset();
            }
        }
        return *this;
    }

    template<typename U = T>
    typename std::enable_if<
        std::is_constructible<T, U&&>::value &&
        std::is_assignable<T&, U&&>::value &&
        !std::is_same<typename std::decay<U>::type, optional<T>>::value,
        optional&>::type
    operator=(U&& value) {
        if (has_value_) {
            *ptr() = std::forward<U>(value);
        } else {
            new (storage_) T(std::forward<U>(value));
            has_value_ = true;
        }
        return *this;
    }

    constexpr const T* operator->() const { return ptr(); }
    T* operator->() { return ptr(); }

    constexpr const T& operator*() const& { return *ptr(); }
    T& operator*() & { return *ptr(); }
    constexpr const T&& operator*() const&& { return std::move(*ptr()); }
    T&& operator*() && { return std::move(*ptr()); }

    constexpr explicit operator bool() const noexcept { return has_value_; }
    constexpr bool has_value() const noexcept { return has_value_; }

    T& value() & {
        if (!has_value_) {
            throw std::runtime_error("bad optional access");
        }
        return *ptr();
    }

    const T& value() const& {
        if (!has_value_) {
            throw std::runtime_error("bad optional access");
        }
        return *ptr();
    }

    T&& value() && {
        if (!has_value_) {
            throw std::runtime_error("bad optional access");
        }
        return std::move(*ptr());
    }

    const T&& value() const&& {
        if (!has_value_) {
            throw std::runtime_error("bad optional access");
        }
        return std::move(*ptr());
    }

    template<typename U>
    constexpr T value_or(U&& default_value) const& {
        return has_value_ ? *ptr() : static_cast<T>(std::forward<U>(default_value));
    }

    template<typename U>
    T value_or(U&& default_value) && {
        return has_value_ ? std::move(*ptr()) : static_cast<T>(std::forward<U>(default_value));
    }

    void reset() noexcept {
        if (has_value_) {
            ptr()->~T();
            has_value_ = false;
        }
    }

    template<typename... Args>
    T& emplace(Args&&... args) {
        reset();
        new (storage_) T(std::forward<Args>(args)...);
        has_value_ = true;
        return *ptr();
    }
};

template<typename T>
constexpr bool operator==(const optional<T>& lhs, const optional<T>& rhs) {
    return lhs.has_value() == rhs.has_value() && (!lhs.has_value() || *lhs == *rhs);
}

template<typename T>
constexpr bool operator!=(const optional<T>& lhs, const optional<T>& rhs) {
    return !(lhs == rhs);
}

template<typename T>
constexpr bool operator<(const optional<T>& lhs, const optional<T>& rhs) {
    return rhs.has_value() && (!lhs.has_value() || *lhs < *rhs);
}

template<typename T>
constexpr bool operator<=(const optional<T>& lhs, const optional<T>& rhs) {
    return !(rhs < lhs);
}

template<typename T>
constexpr bool operator>(const optional<T>& lhs, const optional<T>& rhs) {
    return rhs < lhs;
}

template<typename T>
constexpr bool operator>=(const optional<T>& lhs, const optional<T>& rhs) {
    return !(lhs < rhs);
}

// Comparison with nullopt
template<typename T>
constexpr bool operator==(const optional<T>& opt, nullopt_t) noexcept {
    return !opt.has_value();
}

template<typename T>
constexpr bool operator==(nullopt_t, const optional<T>& opt) noexcept {
    return !opt.has_value();
}

template<typename T>
constexpr bool operator!=(const optional<T>& opt, nullopt_t) noexcept {
    return opt.has_value();
}

template<typename T>
constexpr bool operator!=(nullopt_t, const optional<T>& opt) noexcept {
    return opt.has_value();
}

// Comparison with value
template<typename T, typename U>
constexpr bool operator==(const optional<T>& opt, const U& value) {
    return opt.has_value() && *opt == value;
}

template<typename T, typename U>
constexpr bool operator==(const U& value, const optional<T>& opt) {
    return opt.has_value() && value == *opt;
}

template<typename T, typename U>
constexpr bool operator!=(const optional<T>& opt, const U& value) {
    return !opt.has_value() || *opt != value;
}

template<typename T, typename U>
constexpr bool operator!=(const U& value, const optional<T>& opt) {
    return !opt.has_value() || value != *opt;
}

} // namespace detail
} // namespace enchantum
