#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <cmath>
#include <string>
#include <cstring>
#include <concepts>
#include <type_traits>
#include <cstdint>


class WavReader{
private:
    struct RIFFHeader {
        char chunk_id[4];
        uint32_t chunk_size;
        char format[4];
    };

    struct ChunkInfo {
        char chunk_id[4];
        uint32_t chunk_size;
    };

    struct FmtChunk {
        uint16_t audio_format;
        uint16_t num_channels;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t block_align;
        uint16_t bits_per_sample;
    };

    struct DataChunk {
        int16_t* data;
        int nb_of_samples;
        DataChunk(int s) : nb_of_samples{ s }, data{ new int16_t[s] } {}
        ~DataChunk() { delete[] data; }
    };

public:
    std::vector<std::complex<double>>* read(const std::string& filename) {
        constexpr char riff_id[4] = { 'R', 'I', 'F', 'F' };
        constexpr char format[4] = { 'W', 'A', 'V', 'E' };
        constexpr char fmt_id[4] = { 'f', 'm', 't', ' ' };
        constexpr char data_id[4] = { 'd', 'a', 't', 'a' };

        std::ifstream ifs{ filename, std::ios_base::binary };
        if (!ifs) {
            throw std::runtime_error("Cannot open file");
        }

        // Чтение заголовка RIFF
        RIFFHeader h;
        ifs.read(reinterpret_cast<char*>(&h), sizeof(h));
        if (!ifs || std::memcmp(h.chunk_id, riff_id, 4) || std::memcmp(h.format, format, 4)) {
            throw std::runtime_error("Bad formatting");
        }

        // Чтение chunkов
        ChunkInfo ch;
        FmtChunk fmt;
        DataChunk* data_chunk = nullptr;
        bool fmt_read = false;
        bool data_read = false;

        while (ifs.read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
            // Читает fmt chunk
            if (std::memcmp(ch.chunk_id, fmt_id, 4) == 0) {
                ifs.read(reinterpret_cast<char*>(&fmt), ch.chunk_size);
                fmt_read = true;
            }
            // Читает data chunk
            else if (std::memcmp(ch.chunk_id, data_id, 4) == 0) {
                data_chunk = new DataChunk(ch.chunk_size / sizeof(int16_t));
                ifs.read(reinterpret_cast<char*>(data_chunk->data), ch.chunk_size);
                data_read = true;
            }
            else {
                ifs.seekg(ch.chunk_size, std::ios_base::cur);
            }
        }

        if (!data_read || !fmt_read) {
            delete data_chunk;
            throw std::runtime_error("Problem when reading data");
        }

        // Преобразует данные в массив комплексных чисел
        auto* complex_data = new std::vector<std::complex<double>>(data_chunk->nb_of_samples);
        for (int i = 0; i < data_chunk->nb_of_samples; ++i) {
            (*complex_data)[i] = std::complex<double>(data_chunk->data[i], 0.0);
        }

        delete data_chunk;
        return complex_data;
    }
};