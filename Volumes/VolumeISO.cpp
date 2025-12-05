#include "VolumeISO.h"

VolumeISO::VolumeISO(std::filesystem::path filename, uint8_t track, int sectorsize, bool mode2, unsigned long startSector, std::string disctype, bool contiguous, bool duallayer) : VolumeDisc(filename, track, sectorsize, mode2, startSector, disctype, contiguous, duallayer) {
	this->file.open(filename, std::ios::in | std::ios::binary);
	if (sectorsize == 0) {
		this->sectorsize = 2048;
		this->sectorsize = setSectorSize();
		this->contiguous = contiguous;
		contiguous = true;
	}
	endSector = setEndSector();
	if (mode2 == false) {
		this->mode2 = setMode2();
	}
	if (disctype != "audio") {
		try {
			getTrackType();
		}
		catch (std::invalid_argument& e) {
			this->disctype = "unknown";
		}
	}
	if (contiguous) {
		endSector = setEndSector();
	}
}

void VolumeISO::getTrackType() {
	uint8_t gcmagicword[4] = { 0xc2, 0x33, 0x9f, 0x3d };
	uint8_t isomagicword[5] = { 0x43, 0x44, 0x30, 0x30, 0x31 };
	uint8_t jolietmagicword[6] = { 0x02 ,0x43, 0x44, 0x30, 0x30, 0x31 };
	uint8_t xboxmagicword[20] = { 0x4d, 0x49, 0x43, 0x52, 0x4F, 0x53, 0x4f, 0x46, 0x54, 0x2a, 0x58, 0x42, 0x4f, 0x58, 0x2a, 0x4d, 0x45, 0x44, 0x49, 0x41 };
	uint8_t* sector;
	sector = readSectors(contiguous ? 16 : startSector + 16, 1, true);
	if (compareBytes(1, 6, sector, isomagicword))
	{
		delete[] sector;
		sector = readSectors(contiguous ? 17 : startSector + 17, 1, true);
		uint8_t* sector1;
		sector1 = readSectors(contiguous ? 18 : startSector + 18, 1, true);
		if (compareBytes(0, 6, sector, jolietmagicword) || compareBytes(0, 6, sector1, jolietmagicword)) {
			disctype = "joliet";
			pvd = new PVD(sector, track, "joliet");
			delete[] sector;
			delete[] sector1;
			filetree = generateDirectories(pvd->rootDir, L"");
			return;
		}

		delete[] sector;
		delete[] sector1;
		sector = readSectors(contiguous ? 16 : startSector + 16, 1, true);
		disctype = "cd/dvd";
		pvd = new PVD(sector, track, "cd/dvd");
		filetree = generateDirectories(pvd->rootDir, L"");
		delete[] sector;

		return;
	}
	delete[] sector;
	sector = readSectors(contiguous ? 0 : startSector + 0, 1, true);
	if (compareBytes(28, 32, sector, gcmagicword))
	{
		disctype = "gamecube";
		pvd = new PVD(sector, track, "gamecube");
		delete[] sector;
		filetree = generateDirectories(pvd->rootDir, L"");
		return;
	}
	delete[] sector;
	sector = readSectors((disctype == "xbox" || contiguous) ? 32 : startSector + 32, 1, true);
	if (compareBytes(0, 20, sector, xboxmagicword))
	{
		disctype = "xbox";
		pvd = new PVD(sector, track, "xbox");
		pvd->spaceSize = endSector - startSector;
		delete[] sector;
		filetree = generateDirectories(pvd->rootDir, L"");
		return;
	}
	else {
		disctype = "unknown";
		delete[] sector;
		return;
	}
}

uint8_t* VolumeISO::readFileToMemory(unsigned long long int start, double size, bool ignore) {
	uint32_t sectorcount = 0;
	if (size == 0) {
		size = 2048;
	}
	if (disctype != "gamecube") {
		sectorcount = ceil(size / 2048);
	}

	uint8_t* data;
	data = readSectors(start, sectorcount, ignore);

	return data;
}

FileTree* VolumeISO::generateDirectories(Directory* dir, std::wstring path) {
	if (disctype == "cd/dvd" || disctype == "joliet") {
		uint8_t* sector = readFileToMemory(contiguous && !duallayer ? dir->startSector - startSector : dir->startSector, dir->fileSize);
		FileTree* tree = new FileTree(0, track, nullptr, sector, disctype, path);
		delete[] sector;
		for (int i = 0; i < tree->dirs->size(); i++) {
			if (tree->dirs->at(i)->folder == true) {
				tree->dirs->at(i)->next = generateDirectories(tree->dirs->at(i), path + tree->dirs->at(i)->id + L"\\");
				tree->dirs->at(i)->next->parent = tree;
			}
		}
		tree->sort(tree->dirs);
		return tree;
	}
	else if (disctype == "gamecube") {
		uint8_t* sector = readSectorsByAddress(dir->startSector, dir->fileSize);
		uint32_t dirCount;
		uint16_t stringTable;
		memcpy_s(&dirCount, sizeof(dirCount), sector + 8, sizeof(dirCount));
		dirCount = std::byteswap(dirCount);
		stringTable = dirCount * 12;
		FileTree* tree = new FileTree(0, track, nullptr, sector, disctype, stringTable, path);
		delete[] sector;
		tree->sort(tree->dirs);
		return tree;
	}
	else if (disctype == "xbox") {
		uint8_t* sector = readFileToMemory(dir->startSector, dir->fileSize);
		FileTree* tree = new FileTree(0, track, nullptr, sector, disctype, path);
		delete[] sector;
		for (size_t i = 0; i < tree->dirs->size(); i++) {
			if (tree->dirs->at(i)->folder == true) {
				tree->dirs->at(i)->next = generateDirectories(tree->dirs->at(i), path + tree->dirs->at(i)->id + L"\\");
				tree->dirs->at(i)->next->parent = tree;
			}
		}
		tree->sort(tree->dirs);
		return tree;
	}
	return nullptr;
}

bool VolumeISO::readandCompare(unsigned long long sector, unsigned long count, size_t start, size_t end, uint8_t* magic) {
	uint8_t* data = readSectors(sector, count, true);
	if (compareBytes(start, end, data, magic)) {
		delete[] data;
		return true;
	}
	delete[] data;
	return false;
}

uint8_t* VolumeISO::readSectors(unsigned long long int start, unsigned long count, bool ignore, bool xa) {
	file.clear();
	if ((disctype == "xbox" || contiguous) && (start + startSector < startSector || start >= endSector) && !ignore) {
		throw std::invalid_argument("LBA Address out of range.");
	}
	else if ((disctype != "xbox" && !contiguous) && (start < startSector || start >= endSector) && !ignore) {
		throw std::invalid_argument("LBA Address out of range.");
	}
	unsigned long bytes_read = 0;
	uint8_t* data = new uint8_t[count * 2048];
	if (disctype == "xbox" || contiguous) {
		start += startSector;
	}
	else {
		start -= startSector;
	}
	start *= sectorsize;
	for (size_t i = 0; i < count; i++) {
		switch (sectorsize) {
		case 2352:
			if (mode2) {
				start += 24;
			}
			else {
				start += 16;
			}
			file.seekg(start);
			file.read(data + bytes_read, 2048);
			if (mode2) {
				start += 2328;
			}
			else {
				start += 2336;
			}
			bytes_read += 2048;
			break;
		default:
			file.seekg(start);
			file.read(data + bytes_read, 2048);
			start += sectorsize;
			bytes_read += 2048;
		}
	}
	return data;
}

void VolumeISO::extractAudio(std::filesystem::path path, int index) {
	file.clear();
	uint8_t* buffer = new uint8_t[4];
	uint8_t wavheader[40] = { 0x52, 0x49, 0x46, 0x46, 0xF4, 0x55, 0x98, 0x02, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02,
		0x00, 0x44, 0xAC, 0x00, 0x00, 0x10, 0xB1, 0x02, 0x00, 0x04, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61 };
	uint32_t size = ((getEndSector() - getStartSector())) * 2352;
	unsigned long long location = 0;
	if (contiguous) {
		location = getStartSector() * 2352;
	}
	location += (getIndex(index) * 2352);
	size -= (getIndex(index) * 2352);
	std::string track_num = "Track " + std::to_string(track + 1) + ".wav";
	std::basic_ofstream<uint8_t> output(path/track_num, std::ios::out | std::ios::binary);
	output.write(wavheader, 40);
	memcpy_s(buffer, 4, &size, 4);
	output.write(buffer, 4);
	delete[] buffer;
	while (size > 0) {
		if (size >= 18816) {
			buffer = new uint8_t[18816];
			file.seekg(location);
			file.read(buffer, 18816);
			output.write(buffer, 18816);
			location += 18816;
			size -= 18816;
			delete[] buffer;
		}
		else {
			buffer = new uint8_t[2352];
			file.seekg(location);
			file.read(buffer, 2352);
			output.write(buffer, (size >= 2352) ? 2352 : (size));
			location += 2352;
			size -= 2352;
			delete[] buffer;
		}
	}
	output.close();
}

uint8_t* VolumeISO::readSectorsByAddress(unsigned long start, unsigned long bytecount) {
	file.clear();
	uint8_t* data = new uint8_t[bytecount];
	start -= startSector * sectorsize;
	file.seekg(start);
	file.read(data, bytecount);
	return data;
}

unsigned long long VolumeISO::setEndSector() {
	if (pvd != nullptr && pvd->spaceSize != NULL) {
		return pvd->spaceSize + startSector;
	}
	if (disctype == "xbox") {
		return std::filesystem::file_size(filename) / sectorsize;
	}
	else {
		return std::filesystem::file_size(filename)/sectorsize + startSector;
	}
}

int VolumeISO::setSectorSize() {
	uint8_t* buf = nullptr;
	buf = readSectors(startSector, 1, true);
	uint8_t data[11] = { 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }; // CD-ROM Sync Pattern
	if (compareBytes(0, 11, buf, data)) {
		delete[] buf;
		return 2352;
	}
	delete[] buf;
	return 2048;
}

bool VolumeISO::setMode2() {
	uint8_t* buf = nullptr;
	buf = new uint8_t[2352];
	file.seekg(0);
	file.read(buf, 2352);
	uint8_t data[11] = { 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }; // CD-ROM Sync Pattern
	if (compareBytes(0, 11, buf, data)) {
		if (buf[15] == 0x02) {
			delete[] buf;
			return true;
		}
		else {
			delete[] buf;
			return false;
		}
	}
	delete[] buf;
	return false;
}

bool VolumeISO::operator>(unsigned long& sector) const {
	return sector >= startSector && sector < endSector;
}