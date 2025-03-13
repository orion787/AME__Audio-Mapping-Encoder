#include "ReaderBase.cpp"
#include <memory>
#include <vector>
#include <string>

#define DR_FLAC_IMPLEMENTATION
#include <dr_flac.h>

using namespace std;

class FlacReader : public Reader {
public:
    virtual unique_ptr<vector<float>> read(const string& path) override {
        unique_ptr<vector<float>> samples = make_unique<vector<float>>();
        drflac* flac = drflac_open_file(path.c_str(), nullptr);
        if (flac) {
            samples->resize(flac->totalPCMFrameCount * flac->channels);
            drflac_read_pcm_frames_f32(flac, flac->totalPCMFrameCount, samples->data());
            drflac_close(flac);
            return samples;
        }
        return nullptr;
    }
};

Reader* createFlacReader() {
    return new FlacReader();
}
