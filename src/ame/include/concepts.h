#pragma once
#include <vector>
#include <complex>
#include <string>
#include <concepts>
#include <type_traits>
#include <cstdint>

// Reader concept
template <typename T>
concept Reader = requires(T reader, const std::string & filename) {
    { reader.read(filename) } -> std::same_as<std::vector<std::complex<double>>*>;
};

// FourierTransform concept
template <typename T>
concept FourierTransform = requires(T transform, std::vector<std::complex<double>>&data) {
    { transform.apply(data) } -> std::same_as<void>;
};

// Mikuzator concept
template <typename T>
concept Mikuzator = requires(T mikuzator, const std::vector<std::complex<double>>&data) {
    { mikuzator.generate(data) } -> std::same_as<uint64_t>;
};