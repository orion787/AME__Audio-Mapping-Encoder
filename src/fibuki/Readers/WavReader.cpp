#include "ReaderBase.cpp"
#include <memory>
#include <vector>
#include <string>

#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

using namespace std;

class WavReader : public Reader {
public:
    virtual unique_ptr<vector<float>> read(const string& path) override {
        unique_ptr<vector<float>> samples = make_unique<vector<float>>();
        drwav wav;
        if (drwav_init_file(&wav, path.c_str(), nullptr)) {
            samples->resize(wav.totalPCMFrameCount * wav.channels);
            drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, samples->data());
            drwav_uninit(&wav);
            return samples;
        }
        return nullptr;
    }
};

// Функция для создания объекта WavReader (используется в фабрике)
Reader* createWavReader() {
    return new WavReader();
}
