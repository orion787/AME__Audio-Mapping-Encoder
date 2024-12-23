#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>

class PasswordGenerator {
private:
    std::vector<std::string> hashes;

    // Шаблонная функция для преобразования хэша в символы из заданного диапазона
    template <typename T>
    char mapToCharacter(T value, const std::string& charset) {
        return charset[value % charset.size()];
    }

public:
    void addHash(const std::string& hash) {
        hashes.push_back(hash);
    }

    std::string generatePassword(size_t length) {
        if (hashes.empty()) return "";

        std::string charsetUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        std::string charsetLower = "abcdefghijklmnopqrstuvwxyz";
        std::string charsetDigits = "0123456789";
        std::string charsetSpecial = "!@#$%^&*()-_=+";

        std::ostringstream passwordStream;
        size_t totalHashes = hashes.size();

        for (size_t i = 0; i < length; ++i) {
            size_t hashIndex = i % totalHashes;
            char hashChar = hashes[hashIndex][i % hashes[hashIndex].size()];

            switch (i % 4) {
                case 0:
                    passwordStream << mapToCharacter(hashChar, charsetUpper);
                    break;
                case 1:
                    passwordStream << mapToCharacter(hashChar, charsetLower);
                    break;
                case 2:
                    passwordStream << mapToCharacter(hashChar, charsetDigits);
                    break;
                case 3:
                    passwordStream << mapToCharacter(hashChar, charsetSpecial);
                    break;
            }
        }

        return passwordStream.str();
    }
};

