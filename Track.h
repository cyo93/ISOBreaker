#ifndef TRACK_H
#define TRACK_H
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>
#include "FileTree.h"
#include "PVD.h"
#include "Volumes\VolumeDisc.h"
#include "Volumes\VolumeISO.h"

class Track {
public:
	VolumeDisc* disc = nullptr;
	Track(std::filesystem::path _filename, uint8_t track, int _sectorsize, bool _mode2, unsigned long _startSector, std::string disctype = "", bool contiguous = false, bool duallayer = false);
	~Track();
	FileTree* generateDirectories(Directory* dir, std::wstring path);
	bool operator>(unsigned long &sector) const;
};



#endif