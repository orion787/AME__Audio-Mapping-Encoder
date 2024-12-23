#pragma once

#include <vector>
#include <complex>
#include <cmath>
#include <cstdint>



#define M_PI 3.14159265

class RecursiveFFT{
public:
    void apply(std::vector<std::complex<double>>& data) {
        size_t N = data.size();
        if (N <= 1) return;

        // Разделяем массив на чётные и нечётные части
        std::vector<std::complex<double>> even(N / 2);
        std::vector<std::complex<double>> odd(N / 2);
        for (size_t i = 0; i < N / 2; ++i) {
            even[i] = data[i * 2];
            odd[i] = data[i * 2 + 1];
        }

        // Рекурсивные вызовы для чётных и нечётных
        apply(even);
        apply(odd);

        // Комбинирует результаты
        for (size_t k = 0; k < N / 2; ++k) {
            std::complex<double> t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
            data[k] = even[k] + t;
            data[k + N / 2] = even[k] - t;
        }
    }
};
