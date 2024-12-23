#pragma once

#include <iostream>
#include <vector>
#include <complex>
#include <sndfile.h>
#include <stdexcept>
#include <string>
#include <locale>
#include <codecvt>

// Конвертация UTF-8 -> UTF-16
extern std::wstring utf8_to_wstring(const std::string& utf8);

class WavReader {
public:
    std::vector<std::complex<double>>* read(const std::string& filename) {
#ifdef _WIN32
        // Преобразуем UTF-8 путь в wstring для Windows
        std::wstring wide_filename = utf8_to_wstring(filename);
        SF_INFO sfInfo;
        SNDFILE* file = sf_wchar_open(wide_filename.c_str(), SFM_READ, &sfInfo);
#else
        SF_INFO sfInfo;
        SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sfInfo);
#endif
        if (!file) {
            throw std::runtime_error("Cannot open WAV file: " + filename + ". Error: " + sf_strerror(nullptr));
        }

        // Проверка формата файла (должен быть PCM)
        if ((sfInfo.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV ||
            (sfInfo.format & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
            sf_close(file);
            throw std::runtime_error("Unsupported WAV format.");
        }

        // Чтение данных из файла
        auto* audioData = new std::vector<std::complex<double>>(sfInfo.frames);
        std::vector<int16_t> buffer(sfInfo.frames * sfInfo.channels);
        sf_readf_short(file, buffer.data(), sfInfo.frames);

        // Преобразование данных в вектор комплексных чисел
        for (int i = 0; i < sfInfo.frames; ++i) {
            double sample = 0.0;
            for (int ch = 0; ch < sfInfo.channels; ++ch) {
                sample += buffer[i * sfInfo.channels + ch] / 32768.0;  // Нормализация
            }
            sample /= sfInfo.channels;
            (*audioData)[i] = std::complex<double>(sample, 0.0);
        }

        sf_close(file);
        return audioData;
    }
};
