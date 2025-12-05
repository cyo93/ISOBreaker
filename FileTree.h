#ifndef FILE_TREE_H
#define FILE_TREE_H
#include <iostream>
#include <vector>
#include <algorithm>
#include "Directory.h"
#include "DataLogic.h"
class FileTree {
public:
	FileTree(uint32_t location, uint8_t track, FileTree* parent, uint8_t* data, std::string type, std::wstring path);
	FileTree(uint32_t location, uint8_t track, FileTree* parent, uint8_t* data, std::string type, uint16_t stringTable, std::wstring path);
	~FileTree();
	void recurse_xbox(uint32_t location, uint8_t track, uint8_t* data, std::vector<Directory*> *filelist);
	uint32_t recurse_gamecube(FileTree* next, uint32_t location, uint8_t track, FileTree* parent, uint8_t* data, std::string type, uint16_t stringTable, std::wstring path);
	void sort(std::vector<Directory*>* dirs);
	static bool sortByFolder(Directory* a, Directory* b) { return (b->folder < a->folder); }
	static bool sortByName(Directory* a, Directory* b) {return (lower(a->id) < lower(b->id));}
	void merge(Directory* dir1, Directory* dir2);
	std::vector<Directory*>* dirs = nullptr;
	Directory* cur = nullptr;
	Directory* prev = nullptr;
	FileTree* parent = nullptr;
	std::wstring path = L"";
	uint16_t stringTable = 0;
};

#endif