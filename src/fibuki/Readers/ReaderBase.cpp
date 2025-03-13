#pragma once
#include <memory>
#include <vector>
#include <string>
using namespace std;

class Reader {
public:
    virtual ~Reader() {}
    // Читает аудиофайл и возвращает вектор сэмплов
    virtual std::unique_ptr<std::vector<float>> read(const std::string& path) = 0;
};
