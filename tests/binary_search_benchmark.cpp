#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>
#include <chrono>
#include <iostream>

// Create a sparse (non-contiguous) enum with many values to test binary search performance
enum class SparseEnum : int {
    V0 = 0,
    V10 = 10,
    V20 = 20,
    V30 = 30,
    V40 = 40,
    V50 = 50,
    V60 = 60,
    V70 = 70,
    V80 = 80,
    V90 = 90,
    V100 = 100,
    V110 = 110,
    V120 = 120,
    V130 = 130,
    V140 = 140,
    V150 = 150,
    V160 = 160,
    V170 = 170,
    V180 = 180,
    V190 = 190,
    V200 = 200,
    V210 = 210,
    V220 = 220,
    V230 = 230,
    V240 = 240,
    V250 = 250,
    V260 = 260,
    V270 = 270,
    V280 = 280,
    V290 = 290,
    V300 = 300,
    V310 = 310,
    V320 = 320,
    V330 = 330,
    V340 = 340,
    V350 = 350,
    V360 = 360,
    V370 = 370,
    V380 = 380,
    V390 = 390,
    V400 = 400,
    V410 = 410,
    V420 = 420,
    V430 = 430,
    V440 = 440,
    V450 = 450,
    V460 = 460,
    V470 = 470,
    V480 = 480,
    V490 = 490
};

// Specialize enum_traits to tell enchantum the proper range
template<>
struct enchantum::enum_traits<SparseEnum> {
    static constexpr int min = 0;
    static constexpr int max = 490;
};

// Small contiguous enum for comparison
enum class ContiguousEnum : int {
    A, B, C, D, E, F, G, H, I, J
};

TEST_CASE("Binary search correctness for sparse enum", "[binary_search][correctness]")
{
    SECTION("contains() should work correctly for sparse enum values")
    {
        // Test valid values
        CHECK(enchantum::contains<SparseEnum>(0));
        CHECK(enchantum::contains<SparseEnum>(100));
        CHECK(enchantum::contains<SparseEnum>(490));
        CHECK(enchantum::contains(SparseEnum::V250));
        
        // Test invalid values (gaps in the sparse enum)
        CHECK_FALSE(enchantum::contains<SparseEnum>(5));
        CHECK_FALSE(enchantum::contains<SparseEnum>(15));
        CHECK_FALSE(enchantum::contains<SparseEnum>(495));
        CHECK_FALSE(enchantum::contains<SparseEnum>(-1));
        CHECK_FALSE(enchantum::contains<SparseEnum>(1000));
    }
    
    SECTION("enum_to_index() should work correctly for sparse enum")
    {
        // Test valid conversions
        auto idx0 = enchantum::enum_to_index(SparseEnum::V0);
        REQUIRE(idx0.has_value());
        CHECK(*idx0 == 0);
        
        auto idx25 = enchantum::enum_to_index(SparseEnum::V250);
        REQUIRE(idx25.has_value());
        CHECK(*idx25 == 25);
        
        auto idx49 = enchantum::enum_to_index(SparseEnum::V490);
        REQUIRE(idx49.has_value());
        CHECK(*idx49 == 49);
        
        // Test invalid value (not in enum)
        auto invalid = enchantum::enum_to_index(static_cast<SparseEnum>(5));
        CHECK_FALSE(invalid.has_value());
    }
    
    SECTION("Verify enum is non-contiguous")
    {
        // This enum should be non-contiguous because of the gaps
        CHECK_FALSE(enchantum::is_contiguous<SparseEnum>);
    }
    
    SECTION("Verify contiguous enum uses fast path")
    {
        // Contiguous enums should use the O(1) fast path
        CHECK(enchantum::is_contiguous<ContiguousEnum>);
        CHECK(enchantum::contains(ContiguousEnum::A));
        CHECK(enchantum::contains(ContiguousEnum::J));
    }
}

TEST_CASE("Binary search edge cases", "[binary_search][edge_cases]")
{
    SECTION("First element")
    {
        CHECK(enchantum::contains(SparseEnum::V0));
        auto idx = enchantum::enum_to_index(SparseEnum::V0);
        REQUIRE(idx.has_value());
        CHECK(*idx == 0);
    }
    
    SECTION("Last element")
    {
        CHECK(enchantum::contains(SparseEnum::V490));
        auto idx = enchantum::enum_to_index(SparseEnum::V490);
        REQUIRE(idx.has_value());
        CHECK(*idx == 49);
    }
    
    SECTION("Middle element")
    {
        CHECK(enchantum::contains(SparseEnum::V250));
        auto idx = enchantum::enum_to_index(SparseEnum::V250);
        REQUIRE(idx.has_value());
        CHECK(*idx == 25);
    }
    
    SECTION("Value just before first")
    {
        CHECK_FALSE(enchantum::contains<SparseEnum>(-1));
    }
    
    SECTION("Value just after last")
    {
        CHECK_FALSE(enchantum::contains<SparseEnum>(491));
    }
    
    SECTION("Value in gap")
    {
        CHECK_FALSE(enchantum::contains<SparseEnum>(255));
        CHECK_FALSE(enchantum::contains<SparseEnum>(1));
        CHECK_FALSE(enchantum::contains<SparseEnum>(99));
    }
}

TEST_CASE("Binary search all values", "[binary_search][comprehensive]")
{
    SECTION("All valid values should be found")
    {
        for (int i = 0; i < 50; ++i) {
            int value = i * 10;
            INFO("Testing value " << value);
            CHECK(enchantum::contains<SparseEnum>(value));
            
            auto idx = enchantum::enum_to_index(static_cast<SparseEnum>(value));
            REQUIRE(idx.has_value());
            CHECK(*idx == static_cast<std::size_t>(i));
        }
    }
    
    SECTION("All invalid values in gaps should not be found")
    {
        for (int i = 0; i < 49; ++i) {
            for (int j = 1; j < 10; ++j) {
                int value = i * 10 + j;
                INFO("Testing gap value " << value);
                CHECK_FALSE(enchantum::contains<SparseEnum>(value));
            }
        }
    }
}

// Performance benchmark (not a unit test, just for manual observation)
TEST_CASE("Binary search performance characteristics", "[binary_search][.performance]")
{
    SECTION("contains() performance for sparse enum")
    {
        const int iterations = 100000;
        volatile bool result = false;
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            result = enchantum::contains(SparseEnum::V490); // Worst case - last element
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        std::cout << "contains() for " << iterations << " iterations: " << duration << " µs" << std::endl;
        std::cout << "Average per call: " << (static_cast<double>(duration) / static_cast<double>(iterations)) << " µs" << std::endl;
        
        // Just to prevent optimization
        CHECK(result);
    }
    
    SECTION("enum_to_index() performance for sparse enum")
    {
        const int iterations = 100000;
        volatile std::size_t result = 0;
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            auto idx = enchantum::enum_to_index(SparseEnum::V490);
            if (idx.has_value()) {
                result = *idx;
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        std::cout << "enum_to_index() for " << iterations << " iterations: " << duration << " µs" << std::endl;
        std::cout << "Average per call: " << (static_cast<double>(duration) / static_cast<double>(iterations)) << " µs" << std::endl;
        
        // Just to prevent optimization
        CHECK(result == 49);
    }
}
