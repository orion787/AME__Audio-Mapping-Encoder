#pragma once

#include "FileReader.cpp"
#include "PasswordGenerator.cpp"
#include "BlockHasher.cpp"
#include "ThreadPool.cpp"
#include <map>
#include <mutex>

class FilePasswordProcessor {
private:
    std::wstring filePath;
    size_t blockSize;
    size_t numThreads;
    std::map<size_t, std::string> blockHashes;  // Сохраняем хэши с индексами блоков
    std::mutex mtx;                             // Мьютекс для защиты доступа к blockHashes

public:
    FilePasswordProcessor(const std::wstring& filePath, size_t blockSize, size_t numThreads)
        : filePath(filePath), blockSize(blockSize), numThreads(numThreads) {}

    std::string processFile(size_t passwordLength) {
        FileReader reader(filePath, blockSize);
        ThreadPool threadPool(numThreads);
        size_t blockIndex = 0;

        while (auto block = reader.readNextBlock()) {
            size_t currentIndex = blockIndex++;
            threadPool.enqueueTask([block = std::move(block.value()), currentIndex, this]() {
                BlockHasher hasher;
                std::string hash = hasher.hashBlock(block);

                std::lock_guard<std::mutex> lock(this->mtx);
                this->blockHashes[currentIndex] = hash;
            });
        }

        threadPool.waitForCompletion();

        // Генерация пароля из отсортированных хэшей
        PasswordGenerator passwordGenerator;
        for (const auto& [index, hash] : blockHashes) {
            passwordGenerator.addHash(hash);
        }

        return passwordGenerator.generatePassword(passwordLength);
    }
};





