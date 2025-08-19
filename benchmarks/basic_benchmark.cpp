#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <enchantum/enchantum.hpp>

enum class TestEnum {
    A, B, C, D, E, F, G, H, I, J
};

TEST_CASE("Basic enum reflection benchmark", "[benchmark]") {
    BENCHMARK("to_string") {
        return enchantum::to_string(TestEnum::E);
    };
    
    BENCHMARK("cast") {
        return enchantum::cast<TestEnum>("E");
    };
    
    BENCHMARK("values") {
        return enchantum::values<TestEnum>;
    };
}