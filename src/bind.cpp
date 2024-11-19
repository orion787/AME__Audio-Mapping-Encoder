#include "./readers/wav_reader.hpp"
#include "./readers/mp3_reader.hpp"
#include "./readers/flac_reader.hpp"
#include "./transforms/recursive_fft.hpp"
#include "./mikuzators/custom_mikuzator.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>

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



template <Reader R, FourierTransform F, Mikuzator M>
class AudioProcessor {
    R reader;
    F transform;
    M mikuzator;

public:
    AudioProcessor(R r, F f, M m) : reader(r), transform(f), mikuzator(m) {}

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



uint64_t audioID(std::string filename){  
    std::string fileFormat = filename.substr(filename.find_last_of('.') + 1); // Функция, которая определяет формат файла

    constexpr WavReader wavReader;
    constexpr Mp3Reader mp3Reader;
    constexpr FLACReader flacReader;
    constexpr RecursiveFFT fft;
    constexpr CustomMikuzator mikuzator;
    uint64_t audioID;
    
    if (fileFormat == "wav") {
        AudioProcessor<WavReader, RecursiveFFT, CustomMikuzator> wavProcessor(wavReader, fft, mikuzator);
         audioID = wavProcessor.process(filename);
    } else if (fileFormat == "mp3") {
        AudioProcessor<Mp3Reader, RecursiveFFT, CustomMikuzator> mp3Processor(mp3Reader, fft, mikuzator);
        audioID = mp3Processor.process(filename);
    } else if (fileFormat == "flac") {
        AudioProcessor<FLACReader, RecursiveFFT, CustomMikuzator> flacProcessor(flacReader, fft, mikuzator);
        audioID = flacProcessor.process(filename);
    } else {
        std::cerr << "Unsupported file format: " << fileFormat << std::endl;
        return 1;
    }
    return audioID;
}



PYBIND11_MODULE(ame, m) {
    m.def("audioID", &audioID, "Generate audio file ID");
}
