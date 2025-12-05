#pragma once
#include "VolumeDisc.h"
class VolumeISO : public VolumeDisc {
public:
	VolumeISO(std::filesystem::path filename, uint8_t track, int sectorsize, bool mode2, unsigned long startSector, std::string disctype, bool contiguous, bool duallayer);
	bool readandCompare(unsigned long long sector, unsigned long count, size_t start, size_t end, uint8_t* magic) override;
	void extractAudio(std::filesystem::path path, int index) override;
	uint8_t* readSectors(unsigned long long int start, unsigned long count, bool ignore = false, bool xa = false) override;
	uint8_t* readSectorsByAddress(unsigned long start, unsigned long bytecount) override;
	unsigned long long setEndSector() override;
	void getTrackType() override;
	uint8_t* readFileToMemory(unsigned long long int start, double size, bool ignore = false) override;
	FileTree* generateDirectories(Directory* dir, std::wstring path) override;
	bool operator>(unsigned long& sector) const;
protected:
	bool setMode2() override;
	int setSectorSize() override;
};