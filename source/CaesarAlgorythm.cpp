#include <iostream>
#include "CaesarAlgorythm.h"

#include <fstream>
#include <cstring>

#include <dlfcn.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>


using namespace std;

CaesarAlgorythm::CaesarAlgorythm(): rows_(0), lineChars_(0)
{
    key_ = 0;
    encryptedText_ = nullptr;
    decryptedText_ = nullptr;
}

void CaesarAlgorythm::setKey(int key)
{
    key_ = key;
}

void CaesarAlgorythm::setOutPath(const char* fileName) {
    try {
        // Get the current working directory
        std::filesystem::path currentPath = std::filesystem::current_path();

        // Construct the full path
        std::filesystem::path fullPath = currentPath / fileName;

        // Check if the file exists
        if (std::filesystem::exists(fullPath)) {
            // Get the canonical path
            outPath_ = std::filesystem::canonical(fullPath).string();
        } else {
            std::cerr << "File does not exist: " << fullPath << std::endl;
            outPath_ = ""; // Set to empty string on failure
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error resolving path: " << e.what() << std::endl;
        outPath_ = ""; // Set to empty string on failure
    }
}



void CaesarAlgorythm::encrypt(TextProcessor &localText)
{
    typedef char*(*CaesarAlgorythmEncrypt)(const char*, const int);
    void* handle = dlopen("./source/libCAlib.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot open library: " << dlerror() << '\n';
        return;
    }

    dlerror();

    auto encryptFunc = (CaesarAlgorythmEncrypt)(dlsym(handle, "CaesarAlgorythmEncrypt"));
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol 'CaesarAlgorythmEncrypt': " << dlsym_error << '\n';
        dlclose(handle);
        return;
    }


    rows_ = localText.rows_;
    lineChars_ = localText.lineChars_;

    encryptedText_ = new char*[rows_];
    for(int i = 0; i < localText.rows_; i++)
    {
        if(localText.text_[i] == nullptr)
        {
            break;
        }
        encryptedText_[i] = new char[lineChars_];
        strcpy(encryptedText_[i], encryptFunc(localText.text_[i], key_));
        delete[]localText.text_[i];
        localText.text_[i] = nullptr;
    }

    writeEncryptedToFile_();
    localText.ResetCursor();
}

void CaesarAlgorythm::decrypt(TextProcessor &localText)
{
    openFile_();
    if (!file_.is_open()) {
        throw new runtime_error("Unable to open file for reading.");
    }
    typedef char*(*CaesarAlgorythmDecrypt)(const char*, const int);
    void* handle = dlopen("./source/libCAlib.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot open library: " << dlerror() << '\n';
        return;
    }

    dlerror();

    auto decryptFunc = (CaesarAlgorythmDecrypt)(dlsym(handle, "CaesarAlgorythmDecrypt"));
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol 'CaesarAlgorythmDecrypt': " << dlsym_error << '\n';
        dlclose(handle);
        return;
    }

    string line = readFromFile_();

    if (localText.text_[0] != nullptr)
    {
        for(int i = 0; i < localText.rows_; i++)
        {
            delete[]localText.text_[i];
            localText.text_[i] = nullptr;
        }
    }

    while(!line.empty())
    {
        char* decryptedLine = decryptFunc(line.c_str(), key_);
        localText.EndInsert(decryptedLine);
        localText.StartNewLine();

        line = readFromFile_();
    }

    closeFile_();
    // dlclose(handle);

}

CaesarAlgorythm::~CaesarAlgorythm() {
    delete[] encryptedText_;
    delete[] decryptedText_;
}


void CaesarAlgorythm::openFile_() {
    file_.open(outPath_);
    if (!file_.is_open()) {
        throw std::runtime_error("Unable to open file for reading.");
    }
}

void CaesarAlgorythm::closeFile_() {
    if (file_.is_open()) {
        file_.close();
    }
}

void CaesarAlgorythm::writeEncryptedToFile_()
{
    if (std::ofstream file(outPath_); file.is_open()) {
        for (int i = 0; i < rows_; i++) {
            if (encryptedText_[i] == nullptr) {
                break;
            }
            file << encryptedText_[i] << std::endl;
        }
        file.close();
    }
    else {
        std::cout << "Unable to open file" << std::endl;
    }
}

std::string CaesarAlgorythm::readFromFile_() {
    std::string line;
    if (std::getline(file_, line)) {
        return line;
    } else {
        closeFile_();
        return "";
    }
}

