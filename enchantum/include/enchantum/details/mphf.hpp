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

// Simple hash table approach - use a larger table to reduce collisions
template <std::size_t N>
struct hash_table_data {
    // Use a table size that's a prime number larger than N to reduce collisions
    static constexpr std::size_t table_size = N < 10 ? 23 : (N < 50 ? 101 : (N < 100 ? 211 : 509));
    std::array<std::int32_t, table_size> table{}; // -1 means empty, >=0 means original index
    
    constexpr hash_table_data() {
        // Initialize all entries to -1 (empty)
        for (std::size_t i = 0; i < table_size; ++i) {
            table[i] = -1;
        }
    }
};

// Simple hash table generator 
template <std::size_t N>
[[nodiscard]] constexpr auto make_hash_table(const std::array<std::string_view, N>& keys) noexcept -> hash_table_data<N> {
    if constexpr (N == 0) {
        return {};
    }

    hash_table_data<N> data{};
    constexpr std::size_t table_size = hash_table_data<N>::table_size;
    
    // Insert keys into hash table using linear probing for collision resolution
    for (std::size_t i = 0; i < N; ++i) {
        std::size_t hash_val = fnv1a::hash(keys[i]) % table_size;
        
        // Linear probing to find an empty slot
        while (data.table[hash_val] != -1) {
            hash_val = (hash_val + 1) % table_size;
        }
        
        data.table[hash_val] = static_cast<std::int32_t>(i);
    }
    
    return data;
}

// Function to lookup in the hash table
template <std::size_t N>
[[nodiscard]] constexpr std::int32_t hash_lookup(std::string_view key, const hash_table_data<N>& data, 
                                                  const std::array<std::string_view, N>& keys) noexcept {
    constexpr std::size_t table_size = hash_table_data<N>::table_size;
    std::size_t hash_val = fnv1a::hash(key) % table_size;
    
    // Linear probing to find the key
    for (std::size_t attempts = 0; attempts < table_size; ++attempts) {
        std::int32_t stored_index = data.table[hash_val];
        if (stored_index == -1) {
            return -1; // Not found
        }
        
        if (keys[stored_index] == key) {
            return stored_index; // Found!
        }
        
        hash_val = (hash_val + 1) % table_size;
    }
    
    return -1; // Not found
}

} // namespace enchantum::details::mphf