#pragma once

#include <fstream>
#include <vector>
#include <optional>
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

class FileReader {
private:
    std::ifstream file;
    size_t blockSize;

public:
    FileReader(const std::wstring& filePath, size_t blockSize) : blockSize(blockSize) {
        fs::path path(filePath);
        file.open(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Unable to open file.");
        }
    }

    std::optional<std::vector<char>> readNextBlock() {
        if (!file || file.eof()) {
            return std::nullopt;
        }

        std::vector<char> buffer(blockSize);
        file.read(buffer.data(), blockSize);

        size_t bytesRead = file.gcount();
        if (bytesRead == 0) {
            return std::nullopt;
        }

        buffer.resize(bytesRead);
        return buffer;
    }
};


