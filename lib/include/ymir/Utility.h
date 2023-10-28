#ifndef YMIR_UTILITY_HPP
#define YMIR_UTILITY_HPP

#include <functional>

namespace ymir {

/// Measures runtime the execution of Func and return duration in milliseconds
double measureRuntime(std::function<void()> Func);

} // namespace ymir

#endif // #ifndef YMIR_UTILITY_HPP