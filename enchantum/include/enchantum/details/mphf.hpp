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

/// Hash table result structure for optimal string lookup
template<std::size_t N>
struct hash_table_result {
    std::array<std::size_t, N> table{};
    std::uint32_t seed = 0;
    bool is_perfect = false;
    
    constexpr hash_table_result() {
        for (std::size_t i = 0; i < N; ++i) {
            table[i] = N; // N means "empty"
        }
    }
};

/// Generate optimal hash table - tries to find perfect hash, falls back to linear probing
template<std::size_t N>
constexpr hash_table_result<N> generate_optimal_hash_table(const std::array<string_view, N>& strings) noexcept
{
    hash_table_result<N> result;
    
    // Try to find a perfect hash function (no collisions)
    for (std::uint32_t seed = 0; seed < 200; ++seed) {
        std::array<std::size_t, N> temp_table{};
        std::array<bool, N> used{};
        bool is_perfect = true;
        
        // Initialize temp table
        for (std::size_t i = 0; i < N; ++i) {
            temp_table[i] = N; // N means "empty"
        }
        
        // Try to place all strings with this seed
        for (std::size_t i = 0; i < N && is_perfect; ++i) {
            const auto hash_val = hash_string(strings[i], seed) % N;
            if (used[hash_val]) {
                is_perfect = false; // Collision detected
            } else {
                temp_table[hash_val] = i;
                used[hash_val] = true;
            }
        }
        
        if (is_perfect) {
            result.table = temp_table;
            result.seed = seed;
            result.is_perfect = true;
            return result;
        }
    }
    
    // If no perfect hash found, create hash table with linear probing
    result.seed = 17; // Fixed seed for deterministic behavior
    
    // Initialize table
    for (std::size_t i = 0; i < N; ++i) {
        result.table[i] = N; // N means "empty"
    }
    
    // Place strings using linear probing
    for (std::size_t i = 0; i < N; ++i) {
        const auto hash_val = hash_string(strings[i], result.seed) % N;
        
        // Find next available slot using linear probing
        for (std::size_t probe = 0; probe < N; ++probe) {
            const auto pos = (hash_val + probe) % N;
            if (result.table[pos] == N) {
                result.table[pos] = i;
                break;
            }
        }
    }
    
    result.is_perfect = false;
    return result;
}

/// Optimized string lookup using hash table
template<std::size_t N>
constexpr std::size_t optimized_string_lookup(const std::array<string_view, N>& names_array, string_view name) noexcept
{
    if constexpr (N == 0) {
        return N;
    }
    
    // For very small enums, linear search is already optimal
    if constexpr (N <= 5) {
        for (std::size_t i = 0; i < N; ++i) {
            if (names_array[i] == name) {
                return i;
            }
        }
        return N;
    } else {
        // For larger enums, use hash table with perfect hash if possible
        constexpr auto hash_table_result = generate_optimal_hash_table<N>(names_array);
        
        if constexpr (hash_table_result.is_perfect) {
            // Perfect hash table - single lookup, no collisions
            const auto hash_val = hash_string(name, hash_table_result.seed) % N;
            const auto string_index = hash_table_result.table[hash_val];
            
            if (string_index < N && names_array[string_index] == name) {
                return string_index;
            }
            return N; // Not found
        } else {
            // Fallback hash table with linear probing  
            const auto hash_val = hash_string(name, hash_table_result.seed) % N;
            
            // Linear probing for collision resolution
            for (std::size_t probe = 0; probe < N; ++probe) {
                const auto pos = (hash_val + probe) % N;
                const auto string_index = hash_table_result.table[pos];
                
                if (string_index == N) {
                    return N; // Not found (empty slot)
                }
                
                if (names_array[string_index] == name) {
                    return string_index;
                }
            }
            
            return N; // Not found
        }
    }
}

} // namespace enchantum::details