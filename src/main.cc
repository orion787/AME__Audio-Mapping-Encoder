#include "./readers/wav_reader.hpp"
#include "./readers/mp3_reader.hpp"
#include "./readers/flac_reader.hpp"
#include "./transforms/recursive_fft.hpp"
#include "./mikuzators/custom_mikuzator.hpp"


// Forward declarations for each component class
class WavReader;
class Mp3Reader;
class FLACReader;
class RecursiveFFT;
class CustomMikuzator;

// Базовый случай шаблона (без реализации)
template <typename R, typename F, typename M>
class AudioProcessor;

// Специализация AudioProcessor для WAV файлов
template <>
class AudioProcessor<WavReader, RecursiveFFT, CustomMikuzator> {
public:
    AudioProcessor(WavReader reader, RecursiveFFT fft, CustomMikuzator mikuzator)
        : reader_(reader), fft_(fft), mikuzator_(mikuzator) {}

    uint64_t process(const std::string& filename) {
        auto* data = reader_.read(filename);
        fft_.apply(*data);
        return mikuzator_.generate(*data);
    }

private:
    WavReader reader_;
    RecursiveFFT fft_;
    CustomMikuzator mikuzator_;
};


// Специализация AudioProcessor для MP3 файлов
template <>
class AudioProcessor<Mp3Reader, RecursiveFFT, CustomMikuzator> {
public:
    AudioProcessor(Mp3Reader reader, RecursiveFFT fft, CustomMikuzator mikuzator)
        : reader_(reader), fft_(fft), mikuzator_(mikuzator) {}

    uint64_t process(const std::string& filename) {
        auto* data = reader_.read(filename);
        fft_.apply(*data);
        return mikuzator_.generate(*data);
    }

private:
    Mp3Reader reader_;
    RecursiveFFT fft_;
    CustomMikuzator mikuzator_;
};


// Специализация AudioProcessor для FLAC файлов
template <>
class AudioProcessor<FLACReader, RecursiveFFT, CustomMikuzator> {
public:
    AudioProcessor(FLACReader reader, RecursiveFFT fft, CustomMikuzator mikuzator)
        : reader_(reader), fft_(fft), mikuzator_(mikuzator) {}

    uint64_t process(const std::string& filename) {
        auto* data = reader_.read(filename);
        fft_.apply(*data);
        return mikuzator_.generate(*data);
    }

private:
    FLACReader reader_;
    RecursiveFFT fft_;
    CustomMikuzator mikuzator_;
};



int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <audio_file>" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    std::string fileFormat = filename.substr(filename.find_last_of('.') + 1); // Функция, которая определяет формат файла


    if (fileFormat == "wav") {
        WavReader wavReader;
        RecursiveFFT fft;
        CustomMikuzator mikuzator;
        AudioProcessor<WavReader, RecursiveFFT, CustomMikuzator> wavProcessor(wavReader, fft, mikuzator);
        uint64_t wavID = wavProcessor.process(filename);
        std::cout << "Generated ID for WAV: " << wavID << std::endl;
    } else if (fileFormat == "mp3") {
        Mp3Reader mp3Reader;
        RecursiveFFT fft;
        CustomMikuzator mikuzator;
        AudioProcessor<Mp3Reader, RecursiveFFT, CustomMikuzator> mp3Processor(mp3Reader, fft, mikuzator);
        uint64_t mp3ID = mp3Processor.process(filename);
        std::cout << "Generated ID for MP3: " << mp3ID << std::endl;
    } else if (fileFormat == "flac") {
        FLACReader flacReader;
        RecursiveFFT fft;
        CustomMikuzator mikuzator;
        AudioProcessor<FLACReader, RecursiveFFT, CustomMikuzator> flacProcessor(flacReader, fft, mikuzator);
        uint64_t flacID = flacProcessor.process(filename);
        std::cout << "Generated ID for FLAC: " << flacID << std::endl;
    } else {
        std::cerr << "Unsupported file format: " << fileFormat << std::endl;
        return 1;
    }

    return 0;
}