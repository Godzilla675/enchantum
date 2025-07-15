#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace enchantum::details::mphf {

// Simple but effective FNV-1a hash for compile-time use.
struct fnv1a {
    static constexpr std::uint32_t basis = 0x811C9DC5;
    static constexpr std::uint32_t prime = 0x01000193;

    [[nodiscard]] static constexpr std::uint32_t hash(std::string_view str) noexcept {
        std::uint32_t h = basis;
        for (const char c : str) {
            h ^= static_cast<std::uint8_t>(c);
            h *= prime;
        }
        return h;
    }
};

// Data structure to hold the generated MPHF data.
template <std::size_t N>
struct perfect_hash_data {
    std::array<std::int32_t, N> displacement_map{};
};

// Simplified MPHF generator that works in consteval context
// This creates a hash table that maps names back to their original indices
template <std::size_t N>
[[nodiscard]] consteval auto make_perfect_hash_data(const std::array<std::string_view, N>& keys) noexcept -> perfect_hash_data<N> {
    if constexpr (N == 0) {
        return {};
    }

    perfect_hash_data<N> data{};
    std::array<std::int32_t, N> table{}; // -1 means empty, >=0 means original index
    std::fill(table.begin(), table.end(), -1);
    
    // Try different displacement values until we find one that works for all keys
    for (std::int32_t displacement = 0; displacement < static_cast<std::int32_t>(N * 2); ++displacement) {
        std::fill(table.begin(), table.end(), -1);
        bool success = true;
        
        // Try to place all keys in the table
        for (std::size_t i = 0; i < N && success; ++i) {
            std::size_t hash_val = (fnv1a::hash(keys[i]) + static_cast<std::size_t>(displacement)) % N;
            if (table[hash_val] != -1) {
                success = false; // collision
            } else {
                table[hash_val] = static_cast<std::int32_t>(i); // store original index
            }
        }
        
        if (success) {
            // Found a working displacement
            for (std::size_t i = 0; i < N; ++i) {
                data.displacement_map[i] = displacement;
            }
            break;
        }
    }
    
    return data;
}

// Function to calculate the final perfect hash value at runtime.
template <std::size_t N>
[[nodiscard]] constexpr std::size_t calculate_hash(std::string_view key, const perfect_hash_data<N>& data) noexcept {
    const std::int32_t displacement = data.displacement_map[0]; // All entries have same displacement
    return (fnv1a::hash(key) + static_cast<std::size_t>(displacement)) % N;
}

} // namespace enchantum::details::mphf