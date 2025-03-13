#pragma once
#include "FourierTransformBase.cpp"
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

// Вспомогательная функция: битовый реверс перестановки
static void bit_reverse_permute(vector<float>& real, vector<float>& imag) {
    size_t n = real.size();
    for (size_t i = 0, j = 0; i < n; i++) {
        if (i < j) {
            swap(real[i], real[j]);
            swap(imag[i], imag[j]);
        }
        size_t bit = n >> 1;
        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
    }
}

class FFTBasic : public FourierTransform {
public:
    virtual void compute(vector<float>& input, vector<float>& out_amplitudes) override {
        size_t n = input.size();
        size_t new_size = 1;
        while(new_size < n) new_size <<= 1;
        input.resize(new_size, 0.0f);
        
        vector<float> real = input;
        vector<float> imag(new_size, 0.0f);
        
        fft_inplace(real, imag, false);
        
        out_amplitudes.resize(new_size);
        for (size_t i = 0; i < new_size; i++) {
            out_amplitudes[i] = sqrtf(real[i]*real[i] + imag[i]*imag[i]);
        }
    }
private:
    void fft_inplace(vector<float>& real, vector<float>& imag, bool invert) {
        size_t n = real.size();
        if(n == 0 || (n & (n - 1))) return;
        
        bit_reverse_permute(real, imag);
        
        for (size_t len = 2; len <= n; len <<= 1) {
            const float angle = 2 * M_PI / len * (invert ? -1 : 1);
            const float wlen_real = cos(angle);
            const float wlen_imag = sin(angle);
            
            for (size_t i = 0; i < n; i += len) {
                float w_real = 1.0f, w_imag = 0.0f;
                for (size_t j = 0; j < len/2; j++) {
                    size_t idx_even = i + j;
                    size_t idx_odd = i + j + len/2;
                    
                    float even_real = real[idx_even];
                    float even_imag = imag[idx_even];
                    
                    float odd_real = w_real * real[idx_odd] - w_imag * imag[idx_odd];
                    float odd_imag = w_real * imag[idx_odd] + w_imag * real[idx_odd];
                    
                    real[idx_even] = even_real + odd_real;
                    imag[idx_even] = even_imag + odd_imag;
                    real[idx_odd] = even_real - odd_real;
                    imag[idx_odd] = even_imag - odd_imag;
                    
                    float tmp = w_real * wlen_real - w_imag * wlen_imag;
                    w_imag = w_real * wlen_imag + w_imag * wlen_real;
                    w_real = tmp;
                }
            }
        }
        
        if (invert) {
            float scale = 1.0f / n;
            for (size_t i = 0; i < n; i++) {
                real[i] *= scale;
                imag[i] *= scale;
            }
        }
    }
};

// Функция создания объекта FFTBasic (используется при выборе реализации)
FourierTransform* createFFTBasic() {
    return new FFTBasic();
}
