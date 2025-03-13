#pragma once
#include "PasswordMikuzatorBase.cpp"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <random>
using namespace std;

static uint64_t spectral__hash(const vector<float>& amplitudes) {
    constexpr uint64_t prime = 0x100000001B3;
    uint64_t hash = 0x811C9DC5;
    for (size_t i = 0; i < amplitudes.size(); i += 5) {
        float val = fabs(amplitudes[i]);
        uint32_t bits = *reinterpret_cast<const uint32_t*>(&val);
        hash = (hash ^ bits) * prime;
    }
    return hash;
}

class DefaultPasswordMikuzator : public PasswordMikuzator {
public:
    virtual string generate(const vector<float>& fft_amplitudes) override {
        const string charset[] = {
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
            "abcdefghijklmnopqrstuvwxyz",
            "0123456789",
            "!@#$%^&*()_+-=[]{}|;:,.<>?"
        };
        uint64_t hash = spectral__hash(fft_amplitudes);
        string password;
        password.reserve(16);
        for (int i = 0; i < 4; i++) {
            uint32_t segment = (hash >> (i * 16)) & 0xFFFF;
            password += charset[i][segment % charset[i].size()];
        }
        for (int i = 4; i < 16; i++) {
            uint32_t bits = (hash >> ((i % 4) * 16)) & 0xFFFF;
            const auto& group = charset[(i + bits) % 4];
            password += group[bits % group.size()];
        }
        mt19937 rng(static_cast<unsigned int>(hash));
        shuffle(password.begin(), password.end(), rng);
        return password;
    }
};

// Функция создания объекта DefaultPasswordMikuzator
PasswordMikuzator* createDefaultPasswordMikuzator() {
    return new DefaultPasswordMikuzator();
}
