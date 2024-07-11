#ifndef CAESARALGORYTHM_H
#define CAESARALGORYTHM_H

#include <fstream>

#include "TextProcessor.h"

class CaesarAlgorythm {
public:
    explicit CaesarAlgorythm();
    void setOutPath(const char* fileName);
    void setKey(int key);
    void encrypt(TextProcessor &localText);
    void decrypt(TextProcessor &localText);
    ~CaesarAlgorythm();
private:
    int key_;
    char** encryptedText_;
    char** decryptedText_;

    int rows_;
    int lineChars_;

    const char* fileName_;

    std::ifstream file_;
    void openFile_();
    void closeFile_();

    void writeEncryptedToFile_();
    std::string readFromFile_();
};



#endif //CAESARALGORYTHM_H
