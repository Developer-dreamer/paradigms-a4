#define CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>

#include "TextProcessor.h"
#include "ProgramBackup.h"

class BackupHistory;


TextProcessor::TextProcessor(int rows, int lineChars) : rows_(rows), lineChars_(lineChars), coords_{ 0, 0 }, buffer_{}, cursorPos_ {0, 0} {
	text_ = new char* [rows_];
	std::fill_n(text_, rows_, nullptr);
};


void TextProcessor::EndInsert(const char* userStr) {
	if (cursorPos_.row != 0 || cursorPos_.index != 0) {
		if (text_[cursorPos_.row][0] == '\n')
		{
			cursorPos_.index = 0;
		}
		IndexInput(userStr, cursorPos_);
		return;
	}
	int i;
	for (i = 0; i < rows_; i++) {
		if (text_[i] == nullptr) {
			text_[i] = new char[lineChars_];
			if (lineChars_ <= strlen(userStr)) {
				ResizeLine_();
			}
			strcpy(text_[i], userStr);
			break;
		}
		if (text_[i][0] == '\n' && i+1 < lineChars_ && text_[i + 1] == nullptr) {
			strcpy(text_[i], userStr);
			break;
		}
		if (text_[i + 1] == nullptr) {
			if (lineChars_ <= strlen(userStr) || lineChars_ <= strlen(text_[i])) {
				ResizeLine_();
			}
			strcat(text_[i], userStr);
			break;
		}
		if (rows_ <= i) {
			ResizeRows_();
		}
		
	}

	UpdateCursor_(i);
}
void TextProcessor::StartNewLine() {
	for (int i = 0; i < rows_; i++) {
		if (text_[i] == nullptr) {
			text_[i] = new char[lineChars_];
			text_[i][0] = '\n';
			break;
		}
		if (rows_ <= i) {
			ResizeRows_();
		}
	}
	cursorPos_.row ++;
}

void TextProcessor::IndexInput(const char* userInput, Coordinates coords) {
	this->coords_ = coords;

	if (coords_.row < 0 || coords_.row >= rows_ || coords_.index < 0 || coords_.index >= lineChars_) {
		exit(1);
	}

	if (text_ == nullptr) {
		text_ = new char* [rows_];
		std::fill_n(text_, rows_, nullptr);
	}

	if (text_[coords_.row] == nullptr) {
		text_[coords_.row] = new char[lineChars_];
		std::fill_n(text_[coords_.row], lineChars_, '\0');
	}

	size_t new_length = coords_.index + strlen(userInput) + strlen(text_[coords_.row] + coords_.index);
	
	if (lineChars_ < new_length) {
		lineChars_ = new_length;
		char* temp = new char[lineChars_];
		strcpy(temp, text_[coords_.row]);
		delete[] text_[coords_.row];
		text_[coords_.row] = temp;
	}

	if (text_[coords_.row][coords_.index] == '\n') {
		text_[coords_.row][coords_.index] = '\0';
	}

	// Move existing elements to the right to make space for the new text
	memmove(text_[coords_.row] + coords_.index + strlen(userInput), text_[coords_.row] + coords_.index, strlen(text_[coords_.row] + coords_.index) + 1);

	// Insert the new text
	memcpy(text_[coords_.row] + coords_.index, userInput, strlen(userInput));

	UpdateCursor_(userInput);

	// Free the user input
	delete[] userInput;
	userInput = nullptr;
}

void TextProcessor::IndexInputReplacement(const char* userInput, const Coordinates coords, const size_t charsToReplace) {
	this->coords_ = coords;

	if (coords_.row < 0 || coords_.row >= rows_ || coords_.index < 0 || coords_.index >= lineChars_) {
		exit(1);
	}

	if (text_ == nullptr) {
		text_ = new char* [rows_];
		std::fill_n(text_, rows_, nullptr);
	}

	if (text_[coords_.row] == nullptr) {
		text_[coords_.row] = new char[lineChars_];
		std::fill_n(text_[coords_.row], lineChars_, '\0');
	}

	const size_t new_length = coords_.index + strlen(userInput) + strlen(text_[coords_.row] + coords_.index) - charsToReplace;

	if (lineChars_ < new_length) {
		lineChars_ = new_length;
		char* temp = new char[lineChars_];
		strcpy(temp, text_[coords_.row]);
		delete[] text_[coords_.row];
		text_[coords_.row] = temp;
	}

	if (text_[coords_.row][coords_.index] == '\n') {
		text_[coords_.row][coords_.index] = '\0';
	}

	// Move existing elements to the right to make space for the new text
	memmove(text_[coords_.row] + coords_.index + strlen(userInput), text_[coords_.row] + coords_.index + charsToReplace, strlen(text_[coords_.row] + coords_.index + charsToReplace) + 1);

	// Insert the new text
	memcpy(text_[coords_.row] + coords_.index, userInput, strlen(userInput));

	UpdateCursor_(userInput);

	// Free the user input
	delete[] userInput;
	userInput = nullptr;
}


void TextProcessor::SubstrSearch(const char* userInput) const
{
	if (text_ == nullptr || userInput == nullptr) {
		return;
	}

	Coordinates coords[256];
	int numMatches = 0;

	for (int i = 0; i < rows_; i++) {
		if (text_[i] == nullptr) {
			continue;
		}

		char* found = strstr(text_[i], userInput);
		while (found != nullptr) {

			coords[numMatches].row = i;
			coords[numMatches].index = found - text_[i];
			numMatches++;

			if (found + 1 < text_[i] + strlen(text_[i]) && found != nullptr) {
				found = strstr(found + 1, userInput);
			}
			else {
				break;
			}
		}
	}

    std::cout << "Coordinates of the found substrings:" << std::endl;
    for (int i = 0; i < numMatches; i++) {
        std::cout << "(" << coords[i].row << ", " << coords[i].index << ")" << std::endl;
    }

}

void TextProcessor::IndexDelete(const Coordinates coords, const size_t charsToDelete) {
	this->coords_ = coords;
	this->cursorPos_ = coords;

	if (coords_.row < 0 || coords_.row >= rows_ || coords_.index < 0 || coords_.index >= lineChars_) {
		exit(1); // Better to handle error more gracefully in real applications
	}

	if (text_ == nullptr || text_[coords_.row] == nullptr) {
		return; // nothing to delete
	}

	size_t lineLength = strlen(text_[coords_.row]);
	if (coords_.index >= lineLength) {
		return; // Index beyond the current line length
	}

	size_t charsAvailable = lineLength - coords_.index;
	size_t charsToDeleteHere = std::min(charsToDelete, charsAvailable);

	// Move existing elements to the left to fill the gap
	memmove(text_[coords_.row] + coords_.index,
			text_[coords_.row] + coords_.index + charsToDeleteHere,
			charsAvailable - charsToDeleteHere + 1); // +1 for null terminator


}

void TextProcessor::Copy(const size_t charsToCopy, Coordinates coords) {
	this->coords_ = coords;

	if (coords_.row < 0 || coords_.row >= rows_ || coords_.index < 0 || coords_.index >= lineChars_) {
		exit(1);
	}

	if (text_ == nullptr || text_[coords_.row] == nullptr) {
		buffer_[0] = '\0';
		return;
	}
	buffer_ = new char[charsToCopy + 1];
	size_t charsAvailable = strlen(text_[coords_.row] + coords_.index);
	size_t charsToCopyHere = std::min(charsToCopy, charsAvailable);

	// Copy the specified number of characters to the buffer
	if (buffer_ != nullptr && text_[coords_.row] != nullptr && coords_.index < strlen(text_[coords_.row]) && charsToCopyHere <= sizeof(buffer_) && charsToCopyHere <= strlen(text_[coords_.row] + coords_.index)) {
		memcpy(buffer_, text_[coords_.row] + coords_.index, charsToCopyHere);
	}


	// Null-terminate the buffer
	buffer_[charsToCopyHere] = '\0';
}

void TextProcessor::Paste(Coordinates coords) {
	this->coords_ = coords;
	IndexInput(buffer_, coords);
}

void TextProcessor::Cut(size_t charsToCut, Coordinates coords) {
	Copy(charsToCut, coords);
	IndexDelete(coords, charsToCut);
}

void TextProcessor::PrintText() const
{
	for(int i = 0; i < rows_; i++) {
		if (text_[i] == nullptr) {
			break;
		}
		if (text_[i][0] == '\n') {
			std::fill_n(text_[i], lineChars_, '\0');
		}
		std::cout << text_[i] << std::endl;
	}
}

void TextProcessor::ShowCursor() const
{
	std::cout << "Cursor position: (" << cursorPos_.row << ", " << cursorPos_.index << ")" << std::endl;
}

void TextProcessor::ResetCursor()
{
	ResetCursor_();
}

void TextProcessor::SetCursor(const Coordinates coords)
{
	this->cursorPos_ = coords;
}

TextProcessor::~TextProcessor()
{
	for (int i = 0; i < rows_; i++) {
		delete[] text_[i];
	}
	delete[] text_;
	delete[] buffer_;
}

void TextProcessor::ResetCursor_() {
	cursorPos_ = { 0, 0 };
}

void TextProcessor::UpdateCursor_(const int row) {
	const int currentChar = strlen(text_[row]);
	cursorPos_ = { static_cast<size_t>(row), static_cast<ptrdiff_t>(currentChar) };
}

void TextProcessor::UpdateCursor_(const char* userInput) {
	const ptrdiff_t currentChar = strlen(userInput) + coords_.index;
	cursorPos_ = { coords_.row, currentChar };
}

void TextProcessor::ResizeRows_() {
	const auto temp = new char*[rows_ * 2];
	for (int i = 0; i < rows_; i++) {
		temp[i] = text_[i];
	}
	delete[] text_;
	text_ = temp;
	rows_ *= 2;
}

void TextProcessor::ResizeLine_() {
	for (int i = 0; i < rows_; i++) {
		if (text_[i] == nullptr) {
			break;
		}
		auto temp = new char[lineChars_ * 2];
		if (text_[i] != nullptr) {
			strcpy(temp, text_[i]);
		}
		delete[] text_[i];
		text_[i] = temp;
	}
	lineChars_ *= 2;
}