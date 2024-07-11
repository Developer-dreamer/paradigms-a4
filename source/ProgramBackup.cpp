#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "ProgramBackup.h"

#include <cstring>

Backup::Backup() {
	localText_ = nullptr;
	lineChars_ = 0;
	rows_ = 0;
};

Backup::Backup(const TextProcessor& localText) {
	rows_ = localText.rows_;
	lineChars_ = localText.lineChars_;
	localText_ = new char* [rows_];
	for (int i = 0; i < rows_; i++) {
		localText_[i] = new char[lineChars_ + 1];
		if (localText.text_[i] != NULL) {// +1 for null terminator
			strcpy(localText_[i], localText.text_[i]);
		}
		else {
			localText_[i] = nullptr;
		}
	}
}

char** Backup::GetText() const
{
	return localText_;
};

int Backup::GetRows() const
{
	return rows_;
};

int Backup::GetLineChars() const
{
	return lineChars_;
};

BackupHistory::BackupHistory() {
	backupSize_ = 10;
	backupHistory_ = new Backup[backupSize_];
	redoHistory_ = new Backup[backupSize_];
	undo_pointer_ = -1;
	redo_pointer_ = -1;
};

void BackupHistory::AddToHistory(Backup programState) {
	undo_pointer_++;
	backupHistory_[undo_pointer_] = programState;
	
};

void BackupHistory::AddToHistory_redo(Backup programState) {
	redo_pointer_++;
	redoHistory_[redo_pointer_] = programState;
};

void BackupHistory::Undo(TextProcessor& localText) {
	if (redo_pointer_ == -1) {
		std::cout << "No undo available" << std::endl;
		return;
	}
	Backup previousState = backupHistory_[undo_pointer_];
	int rowsTemp = localText.rows_;

	localText.rows_ = previousState.GetRows();
	localText.lineChars_ = previousState.GetLineChars();
	char** temp = previousState.GetText();

	if (rowsTemp < localText.rows_) {
		localText.text_ = new char*[localText.rows_];
		std::fill_n(localText.text_, localText.rows_, nullptr);
	}

	if (localText.text_ == NULL) {
		localText.text_ = new char*[localText.rows_];
		for (int i = 0; i < localText.rows_; i++) {
			localText.text_[i] = nullptr;
		}
	}

	for (int i = 0; i < localText.rows_; i++) {
		if (localText.text_[i] == NULL) {
			localText.text_[i] = new char[localText.lineChars_];
		}
		if (temp[i] != NULL){
			strcpy(localText.text_[i], temp[i]);
		}
		else {
			localText.text_[i] = nullptr;
		}
	}

	undo_pointer_--;

}

void BackupHistory::Redo(TextProcessor& localText) {
	if (redo_pointer_ == -1) {
		std::cout << "No redo available" << std::endl;
		return;
	}
	Backup previousState = redoHistory_[redo_pointer_];
	int rowsTemp = localText.rows_;

	localText.rows_ = previousState.GetRows();
	localText.lineChars_ = previousState.GetLineChars();
	char** temp = previousState.GetText();

	if (rowsTemp < localText.rows_) {
		localText.text_ = new char* [localText.rows_];
		std::fill_n(localText.text_, localText.rows_, nullptr);
	}

	if (localText.text_ == NULL) {
		localText.text_ = new char* [localText.rows_];
		for (int i = 0; i < localText.rows_; i++) {
			localText.text_[i] = nullptr;
		}
	}

	for (int i = 0; i < localText.rows_; i++) {
		if (localText.text_[i] == NULL) {
			localText.text_[i] = new char[localText.lineChars_];
		}
		if (temp[i] != NULL) {
			strcpy(localText.text_[i], temp[i]);
		}
		else {
			localText.text_[i] = nullptr;
		}
	}

	redo_pointer_--;
}

void BackupHistory::RewriteBackupHistory() {
    if (undo_pointer_ < 9 || backupHistory_[9].localText_ == nullptr) {
        
        return; // No 9th backup available
    }
	if (undo_pointer_ == 9) {
		Backup* newBackupHistory = new Backup[backupSize_];
		for (int i = 0; i < 3; i++) {
			newBackupHistory[i] = backupHistory_[i + 7];
		}
		delete[] backupHistory_;
		backupHistory_ = newBackupHistory;
		undo_pointer_ = 2;
	}

	Backup temp = backupHistory_[9];
    // Delete all elements from the array
    for (int i = 0; i < backupSize_; i++) {
        delete[] backupHistory_[i].localText_;
        backupHistory_[i].localText_ = nullptr;
    }

    // Set the 9th backup to the first position
    
    backupHistory_[0] = temp;
    undo_pointer_ = 0;
}