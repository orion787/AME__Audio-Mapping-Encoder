#pragma once
#include "IDMikuzatorBase.cpp"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>
using namespace std;

static uint64_t spectral_hash(const vector<float>& amplitudes) {
    constexpr uint64_t prime = 0x100000001B3;
    uint64_t hash = 0x811C9DC5;
    for (size_t i = 0; i < amplitudes.size(); i += 5) {
        float val = fabs(amplitudes[i]);
        uint32_t bits = *reinterpret_cast<const uint32_t*>(&val);
        hash = (hash ^ bits) * prime;
    }
    return hash;
}

class DefaultIDMikuzator : public IDMikuzator {
public:
    virtual string generate(const vector<float>& fft_amplitudes) override {
        if(fft_amplitudes.empty()) return "";
        uint64_t hash = spectral_hash(fft_amplitudes);
        string id;
        id.reserve(20);
        while(hash > 0 && id.size() < 20) {
            id.push_back('0' + (hash % 10));
            hash /= 10;
        }
        while(id.size() < 20)
            id.push_back('0');
        reverse(id.begin(), id.end());
        return id;
    }
};

// Функция создания объекта DefaultIDMikuzator
IDMikuzator* createDefaultIDMikuzator() {
    return new DefaultIDMikuzator();
}
