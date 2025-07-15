#pragma once

#include "string_view.hpp"
#include <array>
#include <cstdint>
#include <limits>

namespace enchantum::details {

/// Simple hash function optimized for compile-time evaluation
constexpr std::uint32_t hash_string(string_view str, std::uint32_t seed = 0) noexcept
{
    std::uint32_t hash = seed ^ 2166136261u; // FNV offset basis
    
    for (char c : str) {
        hash = (hash ^ static_cast<std::uint8_t>(c)) * 16777619u; // FNV prime
    }
    return hash;
}

/// For small enums, a simple hash table with linear probing often works well
template<std::size_t N>
constexpr std::array<std::size_t, N> generate_hash_table(const std::array<string_view, N>& strings) noexcept
{
    std::array<std::size_t, N> hash_table{};
    
    // Initialize with invalid index
    for (std::size_t i = 0; i < N; ++i) {
        hash_table[i] = N; // N means "not found"
    }
    
    // Try different seeds until we find one that works (no collisions)
    for (std::uint32_t seed = 0; seed < 100; ++seed) {
        bool success = true;
        std::array<std::size_t, N> temp_table{};
        
        // Initialize temp table
        for (std::size_t i = 0; i < N; ++i) {
            temp_table[i] = N;
        }
        
        // Try to place all strings
        for (std::size_t i = 0; i < N && success; ++i) {
            const auto hash_val = hash_string(strings[i], seed) % N;
            
            // Linear probing to find empty slot
            for (std::size_t probe = 0; probe < N; ++probe) {
                const auto pos = (hash_val + probe) % N;
                if (temp_table[pos] == N) {
                    temp_table[pos] = i;
                    break;
                }
                if (probe == N - 1) {
                    success = false; // Could not place this string
                }
            }
        }
        
        if (success) {
            return temp_table;
        }
    }
    
    // Fallback: identity mapping
    for (std::size_t i = 0; i < N; ++i) {
        hash_table[i] = i;
    }
    return hash_table;
}

/// Simple hash-based lookup with linear probing
template<std::size_t N>
constexpr std::size_t hash_lookup(const std::array<string_view, N>& strings, 
                                  const std::array<std::size_t, N>& hash_table,
                                  string_view target,
                                  std::uint32_t seed = 0) noexcept
{
    if constexpr (N == 0) {
        return 0;
    }
    
    const auto hash_val = hash_string(target, seed) % N;
    
    // Linear probing
    for (std::size_t probe = 0; probe < N; ++probe) {
        const auto pos = (hash_val + probe) % N;
        const auto string_index = hash_table[pos];
        
        if (string_index == N) {
            return N; // Not found (empty slot)
        }
        
        if (strings[string_index] == target) {
            return string_index;
        }
    }
    
    return N; // Not found
}

} // namespace enchantum::details