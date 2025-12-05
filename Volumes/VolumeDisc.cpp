#include "VolumeDisc.h"

VolumeDisc::VolumeDisc(std::filesystem::path filename, uint8_t track, int sectorsize, bool mode2, unsigned long startSector, std::string disctype, bool contiguous, bool duallayer) {
	this->sectorsize = sectorsize;
	this->mode2 = mode2;
	this->startSector = startSector;
	this->filetree = filetree;
	this->disctype = disctype;
	this->contiguous = contiguous;
	this->duallayer = duallayer;
	this->filename = filename;
	this->track = track;
}

void VolumeDisc::setIndex(int index, unsigned long lba) {
	if (contiguous) {
		indexes[index] = lba - startSector;
	}
	else {
		indexes[index] = lba;
	}
}


VolumeDisc::~VolumeDisc() {
	file.close();
	delete pvd;
	delete filetree;
}