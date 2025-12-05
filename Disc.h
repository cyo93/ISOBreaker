#ifndef DISC_H
#define DISC_H
#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cmath>
#include "Track.h"
#include "CueSheet.h"
#include "DataLogic.h"
class Disc {
public:
	Disc(std::wstring _filename);
	~Disc();
	Track* operator[](int index);
	size_t getTracksSize();
	std::vector<Track*> tracks;
	void extractAll(Track* track, std::filesystem::path path = std::filesystem::current_path());
	void extractFile(Directory* dir, std::filesystem::path path = std::filesystem::current_path());
	std::wstring filename;
private:
	std::wstring getFileExtension(std::wstring file);
	unsigned long long getXboxTrack(Track* track);
	Track* lbaToTrack(Directory* dir);
};
#endif