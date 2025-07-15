#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>

enum class TestEnum { First, Second, Third };

TEST_CASE("MPHF functionality", "[mphf]")
{
    SECTION("MPHF generation and lookup")
    {
        // Test basic string-to-enum conversion still works
        auto result = enchantum::cast<TestEnum>("First");
        REQUIRE(result.has_value());
        REQUIRE(*result == TestEnum::First);
        
        result = enchantum::cast<TestEnum>("Second");
        REQUIRE(result.has_value());
        REQUIRE(*result == TestEnum::Second);
        
        result = enchantum::cast<TestEnum>("Third");
        REQUIRE(result.has_value());
        REQUIRE(*result == TestEnum::Third);
        
        // Test invalid string
        result = enchantum::cast<TestEnum>("Invalid");
        REQUIRE(!result.has_value());
    }
    
    SECTION("Contains functionality")
    {
        REQUIRE(enchantum::contains<TestEnum>("First"));
        REQUIRE(enchantum::contains<TestEnum>("Second"));
        REQUIRE(enchantum::contains<TestEnum>("Third"));
        REQUIRE(!enchantum::contains<TestEnum>("Invalid"));
    }
}