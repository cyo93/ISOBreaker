#include "Disc.h"
using namespace std;
Disc::Disc(wstring filename) {
	uint8_t isomagicword[5] = { 0x43, 0x44, 0x30, 0x30, 0x31 };
	if (getFileExtension(filename) == L".cue") {
		tracks = CueSheet::parseSheet(filename);
	}
	else {
		tracks.push_back(new Track(filename, tracks.size(), 0, false, 0, ""));
		if ((tracks[0]->disc->disctype != "unknown" && tracks[0]->disc->disctype != "audio") && tracks[0]->disc->pvd->spaceSize < std::filesystem::file_size(tracks[0]->disc->getFileName())/tracks[0]->disc->getSectorSize()) {
			if (tracks[0]->disc->readandCompare(tracks[0]->disc->pvd->spaceSize, 1, 1, 6, isomagicword)) {
				tracks.push_back(new Track(filename, tracks.size(), 0, false, tracks[0]->disc->pvd->spaceSize - 16, "", true, true));
			}
		}
		if ((tracks[0]->disc->disctype != "unknown" && tracks[0]->disc->disctype != "audio") && std::filesystem::file_size(tracks[0]->disc->getFileName())/tracks[0]->disc->getSectorSize() > 198176) {
			unsigned long long xboxSector = getXboxTrack(tracks[0]);
			if (xboxSector != 0) {
				tracks.push_back(new Track(filename, tracks.size(), 0, false, xboxSector, "xbox", true));
			}
		}
	}
	this->filename = filename;
}

Disc::~Disc() {
	for (unsigned long i = 0; i < tracks.size(); i++) {
		delete tracks[i];
	}
}

void Disc::extractAll(Track* track, std::filesystem::path path) {
	std::filesystem::create_directories(path);
	for (uint32_t i = 0; i < track->disc->filetree->dirs->size(); i++) {
		extractFile(track->disc->filetree->dirs->at(i), path);
	}
}

size_t Disc::getTracksSize() {
	return tracks.size();
}

unsigned long long Disc::getXboxTrack(Track* track) {
	uint8_t xboxmagicword[20] = { 0x4d, 0x49, 0x43, 0x52, 0x4F, 0x53, 0x4f, 0x46, 0x54, 0x2a, 0x58, 0x42, 0x4f, 0x58, 0x2a, 0x4d, 0x45, 0x44, 0x49, 0x41 };
	if (track->disc->readandCompare(198176, 1, 0, 20, xboxmagicword)) {
		return 198176-32;
	}
	else  if (track->disc->readandCompare(129856, 1, 0, 20, xboxmagicword)) {
		return 129856-32;
	}
	else if (track->disc->readandCompare(16672, 1, 0, 20, xboxmagicword)) {
		return 16672 - 32;
	}
	return 0;
}

void Disc::extractFile(Directory* dir, std::filesystem::path path) {
	uint8_t* buffer;
	unsigned long long size = dir->fileSize;
	unsigned long long bytesremaining = size;
	unsigned long long location = 0;

	Track* track = lbaToTrack(dir);
	unsigned long long byteswritten = 0;
	if (dir->folder) {
		std::filesystem::path directory(path/dir->id);
		std::filesystem::create_directories(directory);
		for (unsigned long i = 0; i < dir->next->dirs->size(); i++) {
			extractFile(dir->next->dirs->at(i), directory);
		}
	}
	else {
		std::basic_ofstream<uint8_t> file(path/dir->id, std::ios::out | std::ios::binary);

		while (location < size) {
			if (track->disc->disctype == "gamecube") {
				if (size - location >= 16384) {
					buffer = track->disc->readSectorsByAddress(dir->startSector + location, 16384);
					file.write(buffer, 16384);
					location += 16384;
					bytesremaining -= 16384;
					delete[] buffer;
				}
				else {
					buffer = track->disc->readSectorsByAddress(dir->startSector + location, (bytesremaining >= 2048) ? 2048 : bytesremaining);
					file.write(buffer, (bytesremaining >= 2048) ? 2048 : bytesremaining);
					location += 2048;
					bytesremaining -= 2048;
					delete[] buffer;
				}
			}
			else {
				if (size - location >= 16384) {
					buffer = track->disc->readFileToMemory((track->disc->disctype == "cd/dvd" || track->disc->disctype == "joliet") && track->disc->contiguous && !track->disc->duallayer ? (dir->startSector - track->disc->getStartSector()) + ceil(static_cast<double>(location) / 2048) : dir->startSector + ceil(static_cast<double>(location) / 2048), 16384);
					file.write(buffer, 16384);
					location += 16384;
					bytesremaining -= 16384;
					delete[] buffer;
				}
				else {
					buffer = track->disc->readFileToMemory((track->disc->disctype == "cd/dvd" || track->disc->disctype == "joliet") && track->disc->contiguous && !track->disc->duallayer ? (dir->startSector - track->disc->getStartSector()) + ceil(static_cast<double>(location) / 2048) : dir->startSector + ceil(static_cast<double>(location) / 2048), 2048);
					file.write(buffer, (bytesremaining >= 2048) ? 2048 : bytesremaining);
					location += 2048;
					bytesremaining -= 2048;
					delete[] buffer;
				}
			}
			
		}
		file.close();
	}
	
}
Track* Disc::lbaToTrack(Directory* dir) {
	if (tracks[dir->track]->disc->disctype == "gamecube") {
		return tracks[dir->track];
	}
	if ((tracks[dir->track]->disc->getContiguous()) && dir->startSector + tracks[dir->track]->disc->getStartSector() >= tracks[dir->track]->disc->getStartSector() && dir->startSector + tracks[dir->track]->disc->getStartSector() < tracks[dir->track]->disc->getEndSector()) {
		return tracks[dir->track];
	}
	if (dir->startSector >= tracks[dir->track]->disc->getStartSector() && dir->startSector < tracks[dir->track]->disc->getEndSector()) {
		return tracks[dir->track];
	}
	for (unsigned long i = 0; i < tracks.size(); i++) {
		if (dir->startSector >= tracks[i]->disc->getStartSector() && dir->startSector < tracks[i]->disc->getEndSector()) {
			return tracks[i];
		}
	}
	return 0;
}

wstring Disc::getFileExtension(wstring filename) {
	wstring ext = L"";
	size_t index = filename.rfind(L".");
	for (index; index < filename.size(); index++) {
		ext += filename[index];
	}
	return ext;
}

Track* Disc::operator[](int index) {
	return tracks[index - 1];
}