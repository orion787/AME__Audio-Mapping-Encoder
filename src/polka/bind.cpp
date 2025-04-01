//-----------------------------------------------------------------------------
//
// Project: AME - POLKA realization (thrird)
// Author: orion787 
// License: GNU GPL v3 (see the LICENSE in the root of the repository)
//
//-----------------------------------------------------------------------------
#include <iostream>
#include "SHA512ID.cpp"
#include "FilePasswordProcessor.cpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


std::string fileSHAPassword(std::wstring filePath) {
    try {
        size_t blockSize = 16384;
        size_t numThreads = 8;
        size_t passwordLength = 16;

        FilePasswordProcessor processor(filePath, blockSize, numThreads);
        std::string password = processor.processFile(passwordLength);

        return password;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

std::string fileSHAID(std::string filePath){
    try{
        std::string hash = computeFileHash(filePath);
        return hash;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


PYBIND11_MODULE(polka, m) {
    m.def("filePassword", &fileSHAPassword, "Convert file to password");
    m.def("fileID", &fileSHAID, "Convert file to ID");
}
