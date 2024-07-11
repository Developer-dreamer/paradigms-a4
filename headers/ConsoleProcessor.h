#pragma once

#ifndef CONSOLEPROCESSOR
struct Coordinates {
    size_t row;
    ptrdiff_t index;
};

class Console {
public:
    char* ReadConsole();
    int ReadCommand();
    Coordinates ReadCoordinates();
    void Clear();
private:
    char* inputText_;
    int inputTextLen_ = 64;
    int command_;
    Coordinates coords_;
    void dynamicResize(int size);
};
#endif // !PROCESSOR
