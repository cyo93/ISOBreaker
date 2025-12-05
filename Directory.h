#ifndef DIRECTORY_H
#define DIRECTORY_H
#include <bit>
#include <fstream>
#include <string>
#include "DataLogic.h"

class FileTree;

class Directory {
public:
	Directory(unsigned long location, uint8_t track, uint8_t* data, std::string type, bool root);
	Directory(unsigned long location, uint8_t track, uint8_t* data, std::string type, bool root, uint32_t stringTable);
	~Directory();
	uint8_t track;
	uint16_t tree_left = NULL;
	uint16_t tree_right = NULL;
	uint8_t length;
	uint8_t ealength;
	uint32_t startSector;
	unsigned long long fileSize = 0;
	uint8_t* date = nullptr;
	uint16_t flags;
	uint8_t interleavedSize;
	uint8_t gapSize;
	uint16_t seqNumber;
	uint8_t idLength;
	std::wstring id;
	bool folder = false;
	FileTree* next = nullptr;
private:
	bool findFileExtension(std::string filename);
};




#endif
