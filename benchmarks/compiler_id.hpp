#pragma once

#include <iostream>
#include <string>

namespace benchmark_utils {

// Get normalized compiler identification string
inline std::string get_compiler_id() {
    std::string compiler;
    std::string version;
    
#if defined(_MSC_VER)
    compiler = "msvc";
    version = std::to_string(_MSC_VER);
#elif defined(__clang__)
    compiler = "clang";
    version = std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__);
#elif defined(__GNUC__)
    compiler = "gcc";
    version = std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__);
#else
    compiler = "unknown";
    version = "unknown";
#endif
    
    return compiler + "-" + version;
}

// Print standardized compiler identification line
inline void print_compiler_id() {
    std::cout << "COMPILER_ID:" << get_compiler_id() << std::endl;
}

} // namespace benchmark_utils