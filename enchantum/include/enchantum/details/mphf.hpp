#pragma once

#include "../common.hpp"
#include "string_view.hpp"
#include <array>
#include <cstdint>
#include <limits>

namespace enchantum {
namespace details {

// FNV-1a hash function - simple and constexpr-friendly
constexpr std::uint32_t fnv1a_hash(string_view str, std::uint32_t seed = 0) noexcept {
    constexpr std::uint32_t FNV_OFFSET_BASIS = 2166136261u;
    constexpr std::uint32_t FNV_PRIME = 16777619u;
    
    std::uint32_t hash = FNV_OFFSET_BASIS ^ seed;
    for (char c : str) {
        hash ^= static_cast<std::uint32_t>(static_cast<unsigned char>(c));
        hash *= FNV_PRIME;
    }
    return hash;
}

// MPHF data structure
template<typename E, std::size_t N>
struct mphf_data {
    std::uint32_t seed;
    std::array<E, N> lookup_table;
    
    constexpr mphf_data() : seed(0), lookup_table{} {}
    constexpr mphf_data(std::uint32_t s, const std::array<E, N>& table) 
        : seed(s), lookup_table(table) {}
};

// Generate MPHF at compile time with reasonable limits
template<typename E, std::size_t N>
consteval mphf_data<E, N> generate_mphf(const std::array<string_view, N>& names, 
                                         const std::array<E, N>& values) {
    if constexpr (N == 0) {
        return mphf_data<E, N>{};
    }
    
    // For very small enums, just use seed 1 and try a few seeds only
    constexpr std::uint32_t max_attempts = N <= 4 ? 100 : (N <= 16 ? 500 : 1000);
    
    // Try different seeds until we find one that creates a perfect hash
    for (std::uint32_t seed = 1; seed <= max_attempts; ++seed) {
        std::array<bool, N> used{};
        std::array<E, N> lookup_table{};
        bool collision = false;
        
        // Test if this seed creates a collision-free mapping
        for (std::size_t i = 0; i < N; ++i) {
            std::uint32_t hash = fnv1a_hash(names[i], seed);
            std::size_t index = hash % N;
            
            if (used[index]) {
                collision = true;
                break;
            }
            
            used[index] = true;
            lookup_table[index] = values[i];
        }
        
        if (!collision) {
            return mphf_data<E, N>{seed, lookup_table};
        }
    }
    
    // If we couldn't find a good hash, return invalid (seed 0)
    return mphf_data<E, N>{};
}

// MPHF lookup function
template<typename E, std::size_t N>
constexpr std::size_t mphf_lookup_index(const mphf_data<E, N>& mphf, string_view name) noexcept {
    if constexpr (N == 0) {
        return N; // Invalid index
    }
    
    std::uint32_t hash = fnv1a_hash(name, mphf.seed);
    return hash % N;
}

// Check if the MPHF was successfully generated (non-zero seed or empty enum)
template<typename E, std::size_t N>
constexpr bool is_mphf_valid(const mphf_data<E, N>& mphf) noexcept {
    return N == 0 || mphf.seed != 0;
}

} // namespace details
} // namespace enchantum