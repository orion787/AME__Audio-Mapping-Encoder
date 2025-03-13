#pragma once
#include <string>
#include <vector>
using namespace std;

class PasswordMikuzator {
public:
    virtual ~PasswordMikuzator() {}
    // Генерирует пароль на основе FFT-амплитуд
    virtual std::string generate(const std::vector<float>& fft_amplitudes) = 0;
};
