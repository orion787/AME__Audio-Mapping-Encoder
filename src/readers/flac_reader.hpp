#pragma once

#include <vector>
#include <complex>
#include <fstream>
#include <stdexcept>
#include <sndfile.h>


class FLACReader {
public:
    std::vector<std::complex<double>>* read(const std::string& filename) {
        // Открываем файл с помощью libsndfile
        SF_INFO sfInfo;
        SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sfInfo);
        if (!file) {
            throw std::runtime_error("Не удалось открыть FLAC файл с помощью libsndfile.");
        }

        // Проверка формата файла (должен быть PCM)
        if (sfInfo.format & SF_FORMAT_TYPEMASK != SF_FORMAT_FLAC || (sfInfo.format & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
            sf_close(file);
            throw std::runtime_error("Неподдерживаемый формат файла.");
        }

        // Чтение данных из файла
        auto* audioData = new std::vector<std::complex<double>>(sfInfo.frames);
        std::vector<int16_t> buffer(sfInfo.frames * sfInfo.channels);
        sf_readf_short(file, buffer.data(), sfInfo.frames);

        // Преобразование данных в вектор комплексных чисел
        for (int i = 0; i < sfInfo.frames; ++i) {
            double sample = 0.0;
            for (int ch = 0; ch < sfInfo.channels; ++ch) {
                sample += buffer[i * sfInfo.channels + ch] / 32768.0; // нормализация
            }
            sample /= sfInfo.channels;
            (*audioData)[i] = std::complex<double>(sample, 0.0);
        }

        sf_close(file);
        return audioData;
    }
};