#include "ReaderBase.cpp"
#include <memory>
#include <vector>
#include <string>

#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>

using namespace std;

class Mp3Reader : public Reader {
public:
    virtual unique_ptr<vector<float>> read(const string& path) override {
        unique_ptr<vector<float>> samples = make_unique<vector<float>>();
        drmp3 mp3;
        if (drmp3_init_file(&mp3, path.c_str(), nullptr)) {
            size_t frameCount = drmp3_get_pcm_frame_count(&mp3);
            samples->resize(frameCount * mp3.channels);
            drmp3_read_pcm_frames_f32(&mp3, frameCount, samples->data());
            drmp3_uninit(&mp3);
            return samples;
        }
        return nullptr;
    }
};

Reader* createMp3Reader() {
    return new Mp3Reader();
}
