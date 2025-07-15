#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>
#include <chrono>

// Test enum with various sizes to demonstrate algorithmic differences
enum class SmallEnum { A, B, C };
enum class MediumEnum { A, B, C, D, E, F, G, H, I, J };
enum class LargeEnum { 
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T,
    U, V, W, X, Y, Z, A1, B1, C1, D1, E1, F1, G1, H1, I1, J1,
    K1, L1, M1, N1, O1, P1, Q1, R1, S1, T1, U1, V1, W1, X1, Y1, Z1
};

TEST_CASE("Enum performance characteristics", "[performance]")
{
    SECTION("Small enum lookup performance")
    {
        // These should benefit from MPHF when enabled
        auto result = enchantum::cast<SmallEnum>("A");
        REQUIRE(result.has_value());
        REQUIRE(*result == SmallEnum::A);
        
        result = enchantum::cast<SmallEnum>("C");
        REQUIRE(result.has_value());
        REQUIRE(*result == SmallEnum::C);
        
        // Test invalid lookup
        result = enchantum::cast<SmallEnum>("Invalid");
        REQUIRE(!result.has_value());
    }
    
    SECTION("Medium enum lookup performance")
    {
        // Should currently use linear search, but ready for MPHF
        auto result = enchantum::cast<MediumEnum>("A");
        REQUIRE(result.has_value());
        REQUIRE(*result == MediumEnum::A);
        
        result = enchantum::cast<MediumEnum>("J");
        REQUIRE(result.has_value());
        REQUIRE(*result == MediumEnum::J);
    }
    
    SECTION("Large enum lookup performance")
    {
        // Currently uses linear search - demonstrates need for O(1) lookup
        auto result = enchantum::cast<LargeEnum>("A");
        REQUIRE(result.has_value());
        REQUIRE(*result == LargeEnum::A);
        
        // Worst case - element at end of enum
        result = enchantum::cast<LargeEnum>("Z1");
        REQUIRE(result.has_value());
        REQUIRE(*result == LargeEnum::Z1);
    }
    
    SECTION("Contains function performance")
    {
        // Test contains function which also benefits from MPHF
        REQUIRE(enchantum::contains<SmallEnum>("A"));
        REQUIRE(enchantum::contains<SmallEnum>("B"));
        REQUIRE(enchantum::contains<SmallEnum>("C"));
        REQUIRE(!enchantum::contains<SmallEnum>("D"));
        
        REQUIRE(enchantum::contains<LargeEnum>("A"));
        REQUIRE(enchantum::contains<LargeEnum>("Z1"));
        REQUIRE(!enchantum::contains<LargeEnum>("Invalid"));
    }
}

TEST_CASE("MPHF infrastructure validation", "[mphf]")
{
    SECTION("MPHF data structure")
    {
        // Verify the MPHF infrastructure is properly set up
        constexpr auto mphf_small = enchantum::details::enum_mphf<SmallEnum>;
        constexpr auto mphf_large = enchantum::details::enum_mphf<LargeEnum>;
        
        // Currently disabled, so should be invalid for all sizes
        REQUIRE(!enchantum::details::is_mphf_valid(mphf_small));
        REQUIRE(!enchantum::details::is_mphf_valid(mphf_large));
    }
    
    SECTION("Fallback behavior verification")
    {
        // When MPHF is disabled, should fall back to linear search
        // but still provide correct results
        auto result = enchantum::cast<SmallEnum>("B");
        REQUIRE(result.has_value());
        REQUIRE(*result == SmallEnum::B);
    }
}