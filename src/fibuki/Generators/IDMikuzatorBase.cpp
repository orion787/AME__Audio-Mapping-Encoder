#pragma once
#include <string>
#include <vector>
using namespace std;

class IDMikuzator {
public:
    virtual ~IDMikuzator() {}
    // Генерирует строковый идентификатор на основе FFT-амплитуд
    virtual std::string generate(const std::vector<float>& fft_amplitudes) = 0;
};
