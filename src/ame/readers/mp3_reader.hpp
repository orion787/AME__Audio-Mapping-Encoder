#pragma once

#include <vector>
#include <complex>
#include <stdexcept>
#include <mpg123.h>


class Mp3Reader {
public:
    // Метод read, реализующий чтение MP3 файла и возвращающий указатель на вектор комплексных чисел
    std::vector<std::complex<double>>* read(const std::string& filename) {
        // Инициализация библиотеки mpg123
        if (mpg123_init() != MPG123_OK) {
            throw std::runtime_error("Ошибка инициализации mpg123.");
        }

        // Создание mpg123 хэндла
        mpg123_handle* mh = mpg123_new(nullptr, nullptr);
        if (!mh) {
            mpg123_exit();
            throw std::runtime_error("Не удалось создать mpg123 хэндл.");
        }

        // Открытие MP3 файла
        if (mpg123_open(mh, filename.c_str()) != MPG123_OK) {
            mpg123_delete(mh);
            mpg123_exit();
            throw std::runtime_error("Не удалось открыть MP3 файл.");
        }

        // Получение информации о формате
        long rate;
        int channels, encoding;
        if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK) {
            mpg123_close(mh);
            mpg123_delete(mh);
            mpg123_exit();
            throw std::runtime_error("Не удалось получить информацию о формате.");
        }

        // Вектор для хранения аудиоданных
        auto* audioData = new std::vector<std::complex<double>>;

        // Буфер для декодированных данных
        const size_t bufferSize = 8192;
        unsigned char buffer[bufferSize];
        size_t bytesRead;

        // Чтение данных из MP3 файла и декодирование в PCM
        while (mpg123_read(mh, buffer, bufferSize, &bytesRead) == MPG123_OK) {
            // Преобразование байтов в значения амплитуды (16-bit PCM)
            size_t samplesRead = bytesRead / sizeof(int16_t);
            for (size_t i = 0; i < samplesRead; i += channels) {
                // Чтение 16-битного значения
                int16_t sample = buffer[i] | (buffer[i + 1] << 8);
                // Нормализация значений в диапазон от -1 до 1
                double normalizedSample = static_cast<double>(sample) / 32768.0;
                // Добавление векторного значения с реальной частью, равной нормализованному значению, и мнимой частью, равной нулю
                audioData->emplace_back(normalizedSample, 0.0);
            }
        }

        // Закрытие файла и очистка ресурсов
        mpg123_close(mh);
        mpg123_delete(mh);
        mpg123_exit();

        // Возвращаем указатель на вектор
        return audioData;
    }
};