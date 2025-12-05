#ifndef VOLUME_DISC_H
#define VOLUME_DISC_H
#include <cstdint>
#include <fstream>
#include "PVD.h"
#include "FileTree.h"
#include <filesystem>
class VolumeDisc {
public:
	bool contiguous;
	bool duallayer;
	VolumeDisc(std::filesystem::path filename, uint8_t track, int sectorsize, bool mode2, unsigned long startSector, std::string disctype, bool contiguous, bool duallayer);
	~VolumeDisc();
	virtual bool readandCompare(unsigned long long sector, unsigned long count, size_t start, size_t end, uint8_t* magic) = 0;
	virtual uint8_t* readSectors(unsigned long long int start, unsigned long count, bool ignore = false, bool xa = false) = 0;
	virtual void extractAudio(std::filesystem::path path, int index) = 0;
	virtual uint8_t* readSectorsByAddress(unsigned long start, unsigned long bytecount) = 0;
	virtual uint8_t* readFileToMemory(unsigned long long int start, double size, bool ignore = false) = 0;
	virtual unsigned long long setEndSector() = 0;
	virtual void getTrackType() = 0;
	std::filesystem::path getFileName() { return filename; }
	bool getContiguous() { return contiguous;  }
	unsigned long getStartSector() { return startSector; }
	unsigned long getEndSector() { return endSector; }
	int getSectorSize() { return sectorsize;  }
	void setIndex(int index, unsigned long lba);
	unsigned long getIndex(int index) { return indexes[index]; }
	void setStartSector(unsigned long sector) { startSector = sector; }
	void setEndSector(int end) { endSector = end; }
	virtual FileTree* generateDirectories(Directory* dir, std::wstring path) = 0;
	std::string disctype;
	FileTree* filetree = nullptr;
	PVD* pvd = nullptr;
	uint8_t track;
protected:
	virtual bool setMode2() = 0;
	virtual int setSectorSize() = 0;
	bool mode2;
	std::basic_ifstream<uint8_t> file;
	unsigned long startSector;
	unsigned long endSector = NULL;
	std::filesystem::path filename;
	int sectorsize;
	unsigned long indexes[99];
};
#endif