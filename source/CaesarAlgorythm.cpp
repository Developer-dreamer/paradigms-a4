#include <iostream>
#include "CaesarAlgorythm.h"

#include <fstream>
#include <cstring>

#include <dlfcn.h>


using namespace std;

CaesarAlgorythm::CaesarAlgorythm(): rows_(0), lineChars_(0)
{
    key_ = 0;
    encryptedText_ = nullptr;
    decryptedText_ = nullptr;
    fileName_ = nullptr;
}

void CaesarAlgorythm::setKey(int key)
{
    key_ = key;
}

void CaesarAlgorythm::setOutPath(const char* fileName)
{
    fileName_ = fileName;
}



void CaesarAlgorythm::encrypt(TextProcessor &localText)
{
    typedef char*(*CaesarAlgorythmEncrypt)(const char*, const int);
    void* handle = dlopen("/home/serafym/CLionProjects/paradigms-a4/source/CAlib.so", RTLD_LAZY);
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
        encryptedText_[i] = new char[lineChars_];
        strcpy(encryptedText_[i],encryptFunc(localText.text_[i], key_));
        delete[]localText.text_[i];
        localText.text_[i] = nullptr;
    }

    writeEncryptedToFile_();
}

void CaesarAlgorythm::decrypt(TextProcessor &localText)
{
    typedef char*(*CaesarAlgorythmDecrypt)(const char*, const int);
    void* handle = dlopen("/home/serafym/CLionProjects/paradigms-a4/source/CAlib.so", RTLD_LAZY);
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

    dlclose(handle);

}

CaesarAlgorythm::~CaesarAlgorythm() {
    delete[] encryptedText_;
    delete[] decryptedText_;
    delete[] fileName_;
}

void CaesarAlgorythm::openFile_() {
    file_.open(fileName_);
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
    std::ofstream file(fileName_);
    if (file.is_open()) {
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

