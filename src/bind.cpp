#include "./readers/wav_reader.hpp"
#include "./readers/mp3_reader.hpp"
#include "./readers/flac_reader.hpp"
#include "./transforms/recursive_fft.hpp"
#include "./transforms/avx256_fft.hpp"
#include "./mikuzators/custom_mikuzator.hpp"
#include "./mikuzators/password_mikuzator.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <string>

// Reader concept
template <typename T>
concept Reader = requires(T reader, const std::string & filename) {
    { reader.read(filename) } -> std::same_as<std::vector<std::complex<double>>*>;
};

// FourierTransform concept
template <typename T>
concept FourierTransform = requires(T transform, std::vector<std::complex<double>>&data) {
    { transform.apply(data) } -> std::same_as<void>;
};

// Mikuzator concept
template <typename T>
concept Mikuzator = requires(T mikuzator, const std::vector<std::complex<double>>&data) {
    { mikuzator.generate(data) } -> std::same_as<uint64_t>;
};

// PasswordMikuzator concept
template <typename T>
concept StringdMikuzator = requires(T mikuzator, const std::vector<std::complex<double>>&data) {
    { mikuzator.generate(data) } -> std::same_as<std::string>; // Возвращаем строку, а не число
};


template <Reader R, FourierTransform F, StringdMikuzator M>
class AudioPasswordProcessor {
    R reader;
    F transform;
    M mikuzator;

public:
    AudioPasswordProcessor(R r, F f, M m) : reader(r), transform(f), mikuzator(m) {}

    std::string process(const std::string& filename) {
        // Читаем данные
        auto* data = reader.read(filename);
        if (!data) {
            throw std::runtime_error("Failed to read audio data");
        }

        // Применяем преобразование
        transform.apply(*data);

        // Генерируем число
        std::string result = mikuzator.generate(*data);

        // Освобождаем память
        delete data;
        return result;
    }
};





template <Reader R, FourierTransform F, Mikuzator M>
class AudioIDProcessor {
    R reader;
    F transform;
    M mikuzator;

public:
    AudioIDProcessor(R r, F f, M m) : reader(r), transform(f), mikuzator(m) {}

    uint64_t process(const std::string& filename) {
        // Читаем данные
        auto* data = reader.read(filename);
        if (!data) {
            throw std::runtime_error("Failed to read audio data");
        }

        // Применяем преобразование
        transform.apply(*data);

        // Генерируем число
        uint64_t result = mikuzator.generate(*data);

        // Освобождаем память
        delete data;
        return result;
    }
};



std::string audioPasswordAVX(std::string filename) {
    std::string fileFormat = filename.substr(filename.find_last_of('.') + 1); // Определение формата файла

    constexpr WavReader wavReader;
    constexpr Mp3Reader mp3Reader;
    constexpr FLACReader flacReader;
    constexpr AVX256FFT fft;
    constexpr PasswordMikuzator mikuzator;

    std::string audioPassword;

    if (fileFormat == "wav") {
        AudioPasswordProcessor<WavReader, AVX256FFT, PasswordMikuzator> wavProcessor(wavReader, fft, mikuzator);
        audioPassword = wavProcessor.process(filename);
    } else if (fileFormat == "mp3") {
        AudioPasswordProcessor<Mp3Reader, AVX256FFT, PasswordMikuzator> mp3Processor(mp3Reader, fft, mikuzator);
        audioPassword = mp3Processor.process(filename);
    } else if (fileFormat == "flac") {
        AudioPasswordProcessor<FLACReader, AVX256FFT, PasswordMikuzator> flacProcessor(flacReader, fft, mikuzator);
        audioPassword = flacProcessor.process(filename);
    } else {
        std::cerr << "Unsupported file format: " << fileFormat << std::endl;
        return nullptr; // Возвращаем null, если формат не поддерживается
    }

    return audioPassword;
}



uint64_t audioIDAVX(std::string filename){  
    std::string fileFormat = filename.substr(filename.find_last_of('.') + 1); // Функция, которая определяет формат файла

    constexpr WavReader wavReader;
    constexpr Mp3Reader mp3Reader;
    constexpr FLACReader flacReader;
    constexpr AVX256FFT fft;
    constexpr CustomMikuzator mikuzator;
    uint64_t audioID;
    
    if (fileFormat == "wav") {
        AudioIDProcessor<WavReader, AVX256FFT, CustomMikuzator> wavProcessor(wavReader, fft, mikuzator);
         audioID = wavProcessor.process(filename);
    } else if (fileFormat == "mp3") {
        AudioIDProcessor<Mp3Reader, AVX256FFT, CustomMikuzator> mp3Processor(mp3Reader, fft, mikuzator);
        audioID = mp3Processor.process(filename);
    } else if (fileFormat == "flac") {
        AudioIDProcessor<FLACReader, AVX256FFT, CustomMikuzator> flacProcessor(flacReader, fft, mikuzator);
        audioID = flacProcessor.process(filename);
    } else {
        std::cerr << "Unsupported file format: " << fileFormat << std::endl;
        return 1;
    }
    return audioID;
}





std::string audioPassword(std::string filename) {
    std::string fileFormat = filename.substr(filename.find_last_of('.') + 1); // Определение формата файла

    constexpr WavReader wavReader;
    constexpr Mp3Reader mp3Reader;
    constexpr FLACReader flacReader;
    constexpr RecursiveFFT fft;
    constexpr PasswordMikuzator mikuzator;

    std::string audioPassword;

    if (fileFormat == "wav") {
        AudioPasswordProcessor<WavReader, RecursiveFFT, PasswordMikuzator> wavProcessor(wavReader, fft, mikuzator);
        audioPassword = wavProcessor.process(filename);
    } else if (fileFormat == "mp3") {
        AudioPasswordProcessor<Mp3Reader, RecursiveFFT, PasswordMikuzator> mp3Processor(mp3Reader, fft, mikuzator);
        audioPassword = mp3Processor.process(filename);
    } else if (fileFormat == "flac") {
        AudioPasswordProcessor<FLACReader, RecursiveFFT, PasswordMikuzator> flacProcessor(flacReader, fft, mikuzator);
        audioPassword = flacProcessor.process(filename);
    } else {
        std::cerr << "Unsupported file format: " << fileFormat << std::endl;
        return nullptr; // Возвращаем null, если формат не поддерживается
    }

    return audioPassword;
}



uint64_t audioID(std::string filename){  
    std::string fileFormat = filename.substr(filename.find_last_of('.') + 1); // Функция, которая определяет формат файла

    constexpr WavReader wavReader;
    constexpr Mp3Reader mp3Reader;
    constexpr FLACReader flacReader;
    constexpr RecursiveFFT fft;
    constexpr CustomMikuzator mikuzator;
    uint64_t audioID;
    
    if (fileFormat == "wav") {
        AudioIDProcessor<WavReader, RecursiveFFT, CustomMikuzator> wavProcessor(wavReader, fft, mikuzator);
         audioID = wavProcessor.process(filename);
    } else if (fileFormat == "mp3") {
        AudioIDProcessor<Mp3Reader, RecursiveFFT, CustomMikuzator> mp3Processor(mp3Reader, fft, mikuzator);
        audioID = mp3Processor.process(filename);
    } else if (fileFormat == "flac") {
        AudioIDProcessor<FLACReader, RecursiveFFT, CustomMikuzator> flacProcessor(flacReader, fft, mikuzator);
        audioID = flacProcessor.process(filename);
    } else {
        std::cerr << "Unsupported file format: " << fileFormat << std::endl;
        return 1;
    }
    return audioID;
}







PYBIND11_MODULE(ame, m) {
    m.def("audioIDAVX", &audioIDAVX, "Generate audio file ID");
    m.def("audioPasswordAVX", &audioPasswordAVX, "Convert audio file to password");

    m.def("audioID", &audioID, "Generate audio file ID");
    m.def("audioPassword", &audioPassword, "Convert audio file to password");
}
