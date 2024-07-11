#include <iostream>
#include <cctype>
#include "ConsoleProcessor.h"
#include <limits>

using std::cout;
using std::cin;
using std::endl;


char* Console::ReadConsole() {
    cout << "Enter a string: ";

    char ch;
    inputText_ = new char[inputTextLen_ + 1]; // Extra space for '\0'
    int size = 0;

    cin.clear();
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while ((ch = cin.get()) != '\n') {
        if (size == inputTextLen_) {
            // Resize the array
            dynamicResize(size);
        }
        inputText_[size++] = ch;
    }

    if (size == inputTextLen_) {
        dynamicResize(size);
    }
    // Add the null terminator
    inputText_[size] = '\0';

    cout << "String " << inputText_ << endl; // Print inputText_
    return inputText_;
}

int Console::ReadCommand() {
    while (true) {
        cout << "Enter a command: ";
        cin >> command_;
        if (!cin.fail()) {
            break;
        }
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a valid integer." << endl;
    }

    return command_;
}

Coordinates Console::ReadCoordinates() {
    bool invalid = true;
            
    while (invalid) {
        cout << "Enter coordinates(fromat: (x, y) - (row, line)) : ";
        cin >> coords_.row >> coords_.index;
                
        if (!cin.fail()) {
            invalid = false;
        } else {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid input. Please enter valid integers." << endl;
        }
    }

    return coords_;
}

void Console::Clear() {
#ifdef _WIN32
	system("cls");
#else
	(void)system("clear");
#endif
}

void Console::dynamicResize(int size) {
    inputTextLen_ *= 2;
    char* temp = new char[inputTextLen_ + 1]; // Extra space for '\0'
    std::copy(inputText_, inputText_ + size, temp);
    delete[] inputText_;
    inputText_ = temp;
}