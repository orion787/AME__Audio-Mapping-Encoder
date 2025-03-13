#include "FourierTransformBase.cpp"
#include <vector>
#include <cmath>
#include <immintrin.h>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <algorithm>

using namespace std;

class FFT_ExtraOptimized : public FourierTransform {
public:
    virtual void compute(vector<float>& input, vector<float>& out_amplitudes) override {
        // Определяем новый размер как степень двойки
        size_t n = input.size();
        size_t new_size = 1;
        while(new_size < n) new_size <<= 1;
        input.resize(new_size, 0.0f);

        // Выделяем выровненную память для массивов действительной и мнимой частей
        float* real = (float*) _mm_malloc(new_size * sizeof(float), 32);
        float* imag = (float*) _mm_malloc(new_size * sizeof(float), 32);

        memcpy(real, input.data(), new_size * sizeof(float));
        memset(imag, 0, new_size * sizeof(float));

        // Битовый реверс перестановки (скалярная реализация для выровненных массивов)
        bit_reverse_permute_aligned(real, imag, new_size);

        // Определяем число потоков (если не определено, используем 2)
        unsigned int num_threads = std::thread::hardware_concurrency();
        if(num_threads == 0) num_threads = 2;

        // Основной итеративный цикл FFT
        for (size_t len = 2; len <= new_size; len <<= 1) {
            int half = int(len >> 1);
            float theta = 2.0f * float(M_PI) / float(len);

            // Выделяем выровненные таблицы для твиддловых коэффициентов
            float* cos_table = (float*) _mm_malloc(half * sizeof(float), 32);
            float* sin_table = (float*) _mm_malloc(half * sizeof(float), 32);
            for (int j = 0; j < half; j++) {
                cos_table[j] = cosf(j * theta);
                sin_table[j] = sinf(j * theta);
            }

            // Разбиваем обработку блоков на независимые задания.
            size_t num_blocks = new_size / len; // число блоков для текущего этапа
            size_t blocks_per_thread = (num_blocks + num_threads - 1) / num_threads;

            vector<thread> threads;
            for (unsigned int t = 0; t < num_threads; t++) {
                size_t start_block = t * blocks_per_thread;
                size_t end_block = min(start_block + blocks_per_thread, num_blocks);
                if (start_block >= end_block)
                    break;

                threads.emplace_back([=]() {
                    for (size_t b = start_block; b < end_block; b++) {
                        size_t i = b * len;
                        int j = 0;
                        // Векторизованный цикл по 8 элементов (AVX2, FMA)
                        for (; j <= half - 8; j += 8) {
                            __m256 wcos = _mm256_load_ps(cos_table + j);
                            __m256 wsin = _mm256_load_ps(sin_table + j);
                            
                            __m256 even_re = _mm256_load_ps(real + i + j);
                            __m256 even_im = _mm256_load_ps(imag + i + j);
                            __m256 odd_re  = _mm256_load_ps(real + i + j + half);
                            __m256 odd_im  = _mm256_load_ps(imag + i + j + half);
                            
                            // Вычисляем: t_re = wcos * odd_re - wsin * odd_im
                            //              t_im = wcos * odd_im + wsin * odd_re
                            __m256 t_re = _mm256_fmsub_ps(wcos, odd_re, _mm256_mul_ps(wsin, odd_im));
                            __m256 t_im = _mm256_fmadd_ps(wcos, odd_im, _mm256_mul_ps(wsin, odd_re));
                            
                            __m256 new_even_re = _mm256_add_ps(even_re, t_re);
                            __m256 new_even_im = _mm256_add_ps(even_im, t_im);
                            __m256 new_odd_re  = _mm256_sub_ps(even_re, t_re);
                            __m256 new_odd_im  = _mm256_sub_ps(even_im, t_im);
                            
                            _mm256_store_ps(real + i + j, new_even_re);
                            _mm256_store_ps(imag + i + j, new_even_im);
                            _mm256_store_ps(real + i + j + half, new_odd_re);
                            _mm256_store_ps(imag + i + j + half, new_odd_im);
                        }
                        // Скалярная обработка остатка
                        for (; j < half; j++) {
                            float wcos = cos_table[j];
                            float wsin = sin_table[j];
                            float even_r = real[i + j];
                            float even_i = imag[i + j];
                            float odd_r  = real[i + j + half];
                            float odd_i  = imag[i + j + half];
                            float t_re = wcos * odd_r - wsin * odd_i;
                            float t_im = wcos * odd_i + wsin * odd_r;
                            real[i + j] = even_r + t_re;
                            imag[i + j] = even_i + t_im;
                            real[i + j + half] = even_r - t_re;
                            imag[i + j + half] = even_i - t_im;
                        }
                    }
                });
            }
            // Ожидаем завершения всех потоков для данного этапа
            for(auto &th : threads) {
                th.join();
            }
            _mm_free(cos_table);
            _mm_free(sin_table);
        }

        // Вычисляем спектральные амплитуды
        out_amplitudes.resize(new_size);
        for (size_t i = 0; i < new_size; i++) {
            out_amplitudes[i] = sqrtf(real[i] * real[i] + imag[i] * imag[i]);
        }

        _mm_free(real);
        _mm_free(imag);
    }

private:
    // Скалярная функция битового реверс-перестановки для выровненных массивов
    void bit_reverse_permute_aligned(float* real, float* imag, size_t n) {
        int bits = 0;
        for (size_t temp = n; temp > 1; temp >>= 1)
            bits++;
        for (size_t i = 0; i < n; i++) {
            size_t rev = 0;
            size_t x = i;
            for (int j = 0; j < bits; j++) {
                rev = (rev << 1) | (x & 1);
                x >>= 1;
            }
            if (i < rev) {
                float tmp = real[i];
                real[i] = real[rev];
                real[rev] = tmp;
                tmp = imag[i];
                imag[i] = imag[rev];
                imag[rev] = tmp;
            }
        }
    }
};

// Функция-фабрика для создания объекта FFT_ExtraOptimized
FourierTransform* createFFT_ExtraOptimized() {
    return new FFT_ExtraOptimized();
}
