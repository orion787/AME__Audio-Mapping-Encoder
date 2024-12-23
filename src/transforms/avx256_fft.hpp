#pragma once

#include <immintrin.h>
#include <vector>
#include <complex>
#include <cmath>
#include <cstdint>


#define M_PI 3.14159265

class AVX256FFT{
public:
    void apply(std::vector<std::complex<double>>& data) {
        size_t N = data.size();

        // Проверка: если размер не кратен 2, дополняем до следующей степени двойки
        if (N & (N - 1)) {
            size_t next_power_of_two = 1;
            while (next_power_of_two < N) {
                next_power_of_two <<= 1;
            }

            // Дополняем данные нулями
            data.resize(next_power_of_two, std::complex<double>(0.0, 0.0));
            N = next_power_of_two;  // Обновляем размер
        }

        // Битовая развёртка
        bit_reversal(data);

        // Этапы "бабочки"
        for (size_t len = 2; len <= N; len <<= 1) {
            double angle = -2 * M_PI / len;
            std::complex<double> wlen(cos(angle), sin(angle));

            for (size_t i = 0; i < N; i += len) {
                __m256d w_re = _mm256_set1_pd(1.0);
                __m256d w_im = _mm256_set1_pd(0.0);
                __m256d wlen_re = _mm256_set1_pd(wlen.real());
                __m256d wlen_im = _mm256_set1_pd(wlen.imag());

                for (size_t j = 0; j < len / 2; j += 4) {
                    // Получаем указатели на реальные и мнимые части данных
                    double* u_re_ptr = reinterpret_cast<double*>(&data[i + j]);
                    double* u_im_ptr = reinterpret_cast<double*>(&data[i + j]) + 1;

                    double* v_re_ptr = reinterpret_cast<double*>(&data[i + j + len / 2]);
                    double* v_im_ptr = reinterpret_cast<double*>(&data[i + j + len / 2]) + 1;

                    // Загружаем данные
                    __m256d u_re = _mm256_loadu_pd(u_re_ptr);
                    __m256d u_im = _mm256_loadu_pd(u_im_ptr);
                    __m256d v_re = _mm256_loadu_pd(v_re_ptr);
                    __m256d v_im = _mm256_loadu_pd(v_im_ptr);

                    // Комплексное умножение v * w
                    __m256d temp_re = _mm256_sub_pd(
                        _mm256_mul_pd(v_re, w_re),
                        _mm256_mul_pd(v_im, w_im)
                    );
                    __m256d temp_im = _mm256_add_pd(
                        _mm256_mul_pd(v_re, w_im),
                        _mm256_mul_pd(v_im, w_re)
                    );

                    // Комплексное сложение
                    __m256d new_u_re = _mm256_add_pd(u_re, temp_re);
                    __m256d new_u_im = _mm256_add_pd(u_im, temp_im);

                    // Комплексное вычитание
                    __m256d new_v_re = _mm256_sub_pd(u_re, temp_re);
                    __m256d new_v_im = _mm256_sub_pd(u_im, temp_im);

                    // Сохраняем данные
                    _mm256_storeu_pd(u_re_ptr, new_u_re);
                    _mm256_storeu_pd(u_im_ptr, new_u_im);
                    _mm256_storeu_pd(v_re_ptr, new_v_re);
                    _mm256_storeu_pd(v_im_ptr, new_v_im);

                    // Умножение w *= wlen
                    __m256d temp_w_re = _mm256_sub_pd(
                        _mm256_mul_pd(w_re, wlen_re),
                        _mm256_mul_pd(w_im, wlen_im)
                    );
                    w_im = _mm256_add_pd(
                        _mm256_mul_pd(w_re, wlen_im),
                        _mm256_mul_pd(w_im, wlen_re)
                    );
                    w_re = temp_w_re;
                }
            }
        }
    }

private:
    void bit_reversal(std::vector<std::complex<double>>& data) {
        size_t N = data.size();
        for (size_t i = 1, j = 0; i < N; ++i) {
            size_t bit = N >> 1;
            while (j >= bit) {
                j -= bit;
                bit >>= 1;
            }
            j += bit;
            if (i < j) std::swap(data[i], data[j]);
        }
    }
};
