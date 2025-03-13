#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

#define M_PI 3.1415

class FourierTransform {
public:
    virtual ~FourierTransform() {}
    // Вычисляет FFT: принимает входной вектор и возвращает спектральные амплитуды
    virtual void compute(vector<float>& input, vector<float>& out_amplitudes) = 0;
};
