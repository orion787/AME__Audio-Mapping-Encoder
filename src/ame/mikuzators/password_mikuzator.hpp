#pragma once

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <numeric>

class PasswordMikuzator {
public:
    std::string generate(const std::vector<std::complex<double>>& data, size_t password_length = 16) {
        return generatePassword(data, password_length);
    }

private:
    std::string generatePassword(const std::vector<std::complex<double>>& data, size_t password_length) {
        // Предварительная обработка данных
        size_t seed = processAudioData(data);

        // Генерация пароля
        std::string password(password_length, ' ');
        const std::string chars = "abcdefghijklmnopqrstuvwxyz_-+!#*.ABCDEFGHIJKL()^%$?:;/=MNOPQRSTUVWXYZ";

        for (size_t i = 0; i < password_length; ++i) {
            password[i] = chars[seed % chars.size()];
            seed = rotateHash(seed); // Ротируем seed для нового символа
        }

        return password;
    }

    size_t processAudioData(const std::vector<std::complex<double>>& data) {
        // Простая хэш-функция, комбинирующая амплитуды
        return std::accumulate(data.begin(), data.end(), static_cast<size_t>(0),
            [](size_t hash, const std::complex<double>& value) {
                // Используем амплитуду и фазу для комбинированного хэша
                double amplitude = std::abs(value);
                double phase = std::arg(value);

                // Умножение и XOR для хэширования
                return hash ^ (static_cast<size_t>(amplitude * 1000) * 31) ^ (static_cast<size_t>(phase * 1000) * 17);
            });
    }

    size_t rotateHash(size_t hash_value) {
        // Упрощённая ротация
        return (hash_value >> 3) | (hash_value << (sizeof(size_t) * 8 - 3));
    }
};
