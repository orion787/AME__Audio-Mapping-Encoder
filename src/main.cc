#include "./readers/wav_reader.hpp"
#include "./readers/mp3_reader.hpp"
#include "./readers/flac_reader.hpp"
#include "./transforms/recursive_fft.hpp"
#include "./mikuzators/custom_mikuzator.hpp"


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



int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <audio_file>" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    std::string fileFormat = filename.substr(filename.find_last_of('.') + 1); // Функция, которая определяет формат файла

    constexpr WavReader wavReader;
    constexpr Mp3Reader mp3Reader;
    constexpr FLACReader flacReader;
    constexpr RecursiveFFT fft;
    constexpr CustomMikuzator mikuzator;

    if (fileFormat == "wav") {
        AudioProcessor<WavReader, RecursiveFFT, CustomMikuzator> wavProcessor(wavReader, fft, mikuzator);
        uint64_t wavID = wavProcessor.process(filename);
        std::cout << "Generated ID for WAV: " << wavID << std::endl;
    } else if (fileFormat == "mp3") {
        AudioProcessor<Mp3Reader, RecursiveFFT, CustomMikuzator> mp3Processor(mp3Reader, fft, mikuzator);
        uint64_t mp3ID = mp3Processor.process(filename);
        std::cout << "Generated ID for MP3: " << mp3ID << std::endl;
    } else if (fileFormat == "flac") {
        AudioProcessor<FLACReader, RecursiveFFT, CustomMikuzator> flacProcessor(flacReader, fft, mikuzator);
        uint64_t flacID = flacProcessor.process(filename);
        std::cout << "Generated ID for FLAC: " << flacID << std::endl;
    } else {
        std::cerr << "Unsupported file format: " << fileFormat << std::endl;
        return 1;
    }

    return 0;
}
