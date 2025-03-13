#pragma once
#include <string>
#include <vector>
using namespace std;

// Предполагается, что IDMikuzator и PasswordMikuzator определены в файлах генераторов

class IDMikuzator; 
class PasswordMikuzator;

// Процессор для генерации ID
class IDProcessor {
    IDMikuzator* idGenerator;
public:
    IDProcessor(IDMikuzator* generator) : idGenerator(generator) {}
    string process(const vector<float>& fft_amplitudes) {
        return idGenerator->generate(fft_amplitudes);
    }
};

// Процессор для генерации пароля
class PasswordProcessor {
    PasswordMikuzator* pwdGenerator;
public:
    PasswordProcessor(PasswordMikuzator* generator) : pwdGenerator(generator) {}
    string process(const vector<float>& fft_amplitudes) {
        return pwdGenerator->generate(fft_amplitudes);
    }
};
