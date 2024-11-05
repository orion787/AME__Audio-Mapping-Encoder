#pragma once

#include <vector>
#include <complex>
#include <cmath>
#include <cstdint>

class CustomMikuzator{
public:
    uint64_t generate(const std::vector<std::complex<double>>& data) {
        uint64_t random_number = 0;
        for (const auto& value : data) {
            // Извлекаем амплитуду и фазу
            double amplitude = std::abs(value);
            double phase = std::arg(value);

            // Комбинируем амплитуду и фазу в целое число
            uint64_t amplitude_part = static_cast<uint64_t>(amplitude * 1000);
            uint64_t phase_part = static_cast<uint64_t>((phase + M_PI) * 1000); // Фаза в диапазоне [0, 2*M_PI]

            // Обновляем итоговое значение
            random_number ^= (amplitude_part << 32) | phase_part;
            random_number = (random_number << 1) | (random_number >> (64 - 1)); // Ротирование
        }
        return random_number;
    }
};