#pragma once

#ifndef  FILEPROCESSOR
#include <iostream>
#include "ConsoleProcessor.h"
#include "ProgramBackup.h"

class TextProcessor {
public:
	TextProcessor(int rows, int lineChars);
	void EndInsert(const char* userStr);
	void StartNewLine();
	void IndexInput(const char* userInput, Coordinates coords);
	void IndexInputReplacement(const char* userInput, Coordinates coords, size_t charsToReplace);
	void IndexDelete(Coordinates coords, size_t length);
	void SubstrSearch(const char* userInput) const;
	void PrintText() const;
	void Copy(size_t charsToCopy, Coordinates coords);
	void Paste(Coordinates coords);
	void Cut(size_t charsToCut, Coordinates coords);
	void ShowCursor() const;
	void ResetCursor();
	void SetCursor(Coordinates coords);
	~TextProcessor();
private:
	char** text_;
	char* buffer_;
	size_t rows_;
	size_t lineChars_;
	Coordinates coords_;
	Coordinates cursorPos_;

	friend class CaesarAlgorythm;
	friend struct Backup;
	friend class BackupHistory;


	void ResetCursor_();
	void UpdateCursor_(int row);
	void UpdateCursor_(const char* userInput);
	void ResizeLine_();
	void ResizeRows_();
};

#endif // ! FILEPROCESSOR
