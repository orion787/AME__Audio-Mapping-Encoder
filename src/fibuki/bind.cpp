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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "Readers/ReaderFactory.cpp"
#include "FourierTransforms/FFTBasic.cpp"
#include "FourierTransforms/FFT_AVX256.cpp"
#include "FourierTransforms/FFT_ExtraOptimized.cpp"
#include "Generators/DefaultIDMikuzator.cpp"
#include "Generators/DefaultPasswordMikuzator.cpp"
#include "Processors/Processors.cpp"

using namespace std;
namespace py = pybind11;


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


PYBIND11_MODULE(fibuki, m) {
    m.doc() = "Модуль для генерации ID и пароля из аудиофайлов";
    m.def("audioID", &generateID, "Генерация ID по аудиофайлу", py::arg("file_path"));
    m.def("audioPassword", &generatePassword, "Генерация пароля по аудиофайлу", py::arg("file_path"));
}
