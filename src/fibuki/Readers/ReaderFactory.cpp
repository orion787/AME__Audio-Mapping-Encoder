#pragma once
#include "ReaderBase.cpp"

// Объявления функций создания конкретных Reader-ов
extern Reader* createWavReader();
extern Reader* createFlacReader();
extern Reader* createMp3Reader();

#include <string>
#include <memory>
#include <algorithm>
using namespace std;

class ReaderFactory {
public:
    static unique_ptr<Reader> createReader(const string& path) {
        string ext;
        size_t pos = path.find_last_of('.');
        if (pos != string::npos) {
            ext = path.substr(pos + 1);
            transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        }
        if(ext == "wav")
            return unique_ptr<Reader>(createWavReader());
        else if(ext == "flac")
            return unique_ptr<Reader>(createFlacReader());
        else if(ext == "mp3")
            return unique_ptr<Reader>(createMp3Reader());
        return nullptr;
    }
};
