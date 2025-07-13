#pragma once

// Lightweight core functionality for enchantum
// This header provides essential enum reflection with minimal compile-time overhead
// 
// Features included:
// - to_string: Convert enum to string
// - cast: Convert string/value to enum  
// - contains: Check if value/string is valid enum
// - Basic enum metadata (min, max, count)
//
// Features NOT included (use all.hpp if needed):
// - iostream operators
// - formatting support  
// - bitset/array containers
// - iteration generators

#include "common.hpp"
#include "enchantum.hpp"