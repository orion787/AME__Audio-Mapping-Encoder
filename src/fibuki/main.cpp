//-----------------------------------------------------------------------------
//
// Project: AME - FIBUKI realization (second)
// Author: orion787 
// License: GNU GPL v3 (see the LICENSE in the root of the repository)
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "Readers/ReaderFactory.cpp"
#include "FourierTransforms/FFTBasic.cpp"
#include "FourierTransforms/FFT_AVX256.cpp"
#include "FourierTransforms/FFT_ExtraOptimized.cpp"
#include "Generators/DefaultIDMikuzator.cpp"
#include "Generators/DefaultPasswordMikuzator.cpp"
#include "Processors/Processors.cpp"

using namespace std;
/*
int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <audio_file>\n";
        return 1;
    }
    
    string file_path = argv[1];
    
    // Создаем нужный Reader через фабрику
    unique_ptr<Reader> reader = ReaderFactory::createReader(file_path);
    if (!reader) {
        std::cerr << "Unsupported file format\n";
        return 2;
    }
    
    // Читаем аудиоданные
    unique_ptr<vector<float>> audioData = reader->read(file_path);
    if (!audioData || audioData->empty()) {
        std::cerr << "Error loading audio file or file is empty\n";
        return 3;
    }
    
    // Выбираем реализацию FFT.
    // Можно экспериментировать: FFTBasic, FFT_AVX256, FFT_ExtraOptimized.
    // Здесь для демонстрации выбрана более оптимизированная реализация.
    unique_ptr<FourierTransform> fft = std::unique_ptr<FourierTransform>(createFFT_ExtraOptimized());
    
    vector<float> fftAmplitudes;
    fft->compute(*audioData, fftAmplitudes);
    
    // Генерация ID и пароля
    DefaultIDMikuzator idGen;
    DefaultPasswordMikuzator pwdGen;
    
    IDProcessor idProc(&idGen);
    PasswordProcessor pwdProc(&pwdGen);
    
    cout << "File ID:   " << idProc.process(fftAmplitudes) << "\n";
    cout << "Password:  " << pwdProc.process(fftAmplitudes) << "\n";
    
    return 0;
}
*/


string generateID(string file_path) {
    unique_ptr<Reader> reader = ReaderFactory::createReader(string(file_path));
    if (!reader) {
        throw runtime_error("Unsupported file format");
    }

    unique_ptr<vector<float>> audioData = reader->read(string(file_path));
    if (!audioData || audioData->empty()) {
        throw runtime_error("Error loading audio file or file is empty");
    }

    unique_ptr<FourierTransform> fft = unique_ptr<FourierTransform>(createFFTBasic());
    vector<float> fftAmplitudes;
    fft->compute(*audioData, fftAmplitudes);

    DefaultIDMikuzator idGen;
    IDProcessor idProc(&idGen);
    return idProc.process(fftAmplitudes);
}

string generatePassword(string file_path) {
    unique_ptr<Reader> reader = ReaderFactory::createReader(string(file_path));
    if (!reader) {
        throw runtime_error("Unsupported file format");
    }

    unique_ptr<vector<float>> audioData = reader->read(string(file_path));
    if (!audioData || audioData->empty()) {
        throw runtime_error("Error loading audio file or file is empty");
    }

    unique_ptr<FourierTransform> fft = unique_ptr<FourierTransform>(createFFTBasic());
    vector<float> fftAmplitudes;
    fft->compute(*audioData, fftAmplitudes);

    DefaultPasswordMikuzator pwdGen;
    PasswordProcessor pwdProc(&pwdGen);
    return pwdProc.process(fftAmplitudes);
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <audio_file>\n";
        return 1;
    }

    string file_path = argv[1];

    try {
        cout << "File ID:   " << generateID(file_path) << "\n";
        cout << "Password:  " << generatePassword(file_path) << "\n";
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 2;
    }

    return 0;
}
