#pragma once

#ifndef __cplusplus
#error "This file must be compiled as C++."
#endif

#include <functional>
#include <type_traits>

namespace Arbiter {

template<typename T>
size_t hashOf (const T &value)
{
  return std::hash<T>()(value);
}

} // namespace Arbiter
