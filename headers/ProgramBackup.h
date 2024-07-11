#pragma once

#include "TextProcessor.h"

class TextProcessor;

#ifndef BACKUP
struct Backup {
public:
	Backup();
	Backup(const TextProcessor& localText);
	char** GetText() const;
	int GetRows() const;
	int GetLineChars() const;
	
private:
	char** localText_;
	int lineChars_;
	int rows_;
	friend class BackupHistory;
};

class BackupHistory {
public:
	BackupHistory();
	void AddToHistory(Backup backup);
	void AddToHistory_redo(Backup backup);
	void Undo(TextProcessor& localText);
	void Redo(TextProcessor& localText);
private:
	Backup backup_;
	int backupSize_;
	int undo_pointer_;
	int redo_pointer_;
	Backup* backupHistory_;
	Backup* redoHistory_;
	void RewriteBackupHistory();
};
#endif // !BACKUP