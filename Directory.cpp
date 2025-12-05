#include "Directory.h"
Directory::Directory(unsigned long location, uint8_t track, uint8_t* data, std::string type, bool root) {
	this->track = track;
	if (type == "cd/dvd" || type == "joliet") {
		memcpy_s(&length, sizeof(length), (data + location), sizeof(length));
		memcpy_s(&ealength, sizeof(ealength), (data + location) + 1, sizeof(ealength));
		memcpy_s(&startSector, sizeof(startSector), (data + location) + 2, sizeof(startSector));
		memcpy_s(&fileSize, 4, (data + location) + 10, 4);
		date = new uint8_t[7];
		memcpy_s(date, 7, (data + location) + 18, 7);
		memcpy_s(&flags, 1, (data + location) + 25, 1);
		folder = isBitSet(flags, 1);
		memcpy_s(&interleavedSize, sizeof(interleavedSize), (data + location) + 26, sizeof(interleavedSize));
		memcpy_s(&gapSize, sizeof(gapSize), (data + location) + 27, sizeof(gapSize));
		memcpy_s(&seqNumber, sizeof(seqNumber), (data + location) + 28, sizeof(seqNumber));
		memcpy_s(&idLength, sizeof(idLength), (data + location) + 32, sizeof(idLength));
		id.assign(&data[location + 33], &data[location + 33] + idLength);
		if (type == "joliet") {
			id = bigEndiantoLittleEndianUTF16(id);
		}
		id = id.substr(0, id.find(L";", 0));
	}
	else if (type == "xbox") {
		if (root) {
			memcpy_s(&startSector, sizeof(startSector), data + 20, sizeof(startSector));
			memcpy_s(&fileSize, 4, data + 24, 4);
		}
		else {
			memcpy_s(&tree_left, sizeof(tree_left), (data + location), sizeof(tree_left));
			memcpy_s(&tree_right, sizeof(tree_right), (data + location) + 2, sizeof(tree_right));
			memcpy_s(&startSector, sizeof(startSector), (data + location) + 4, sizeof(startSector));
			memcpy_s(&fileSize, 4, (data + location) + 8, 4);
			memcpy_s(&flags, 1, (data + location) + 12, 1);
			memcpy_s(&idLength, sizeof(idLength), (data + location) + 13, sizeof(idLength));
			id.assign(&data[location + 14], &data[location + 14] + idLength);
			if (isBitSet(flags, 4)) {
				folder = true;
			}
		}
		
	}
}

Directory::Directory(unsigned long location, uint8_t track, uint8_t* data, std::string type, bool root, uint32_t stringTable) {
	if (type == "gamecube") {
		if (root) {
			this->track = track;
			memcpy_s(&startSector, sizeof(startSector), (data + location), sizeof(startSector));
			uint32_t size;
			memcpy_s(&size, sizeof(size), (data + location) + 4, sizeof(size));
			fileSize = std::byteswap(size);
			startSector = std::byteswap(startSector);
		}
		else {
			this->track = track;
			memcpy_s(&flags, 2, (data + location), 2);
			flags = std::byteswap(flags);
			folder = isBitSet(flags, 8);
			uint16_t stringOffset;
			memcpy_s(&stringOffset, sizeof(stringOffset), (data + location) + 2, sizeof(stringOffset));
			memcpy_s(&startSector, sizeof(startSector), (data + location) + 4, sizeof(startSector));
			uint32_t size;
			memcpy_s(&size, sizeof(size), (data + location) + 8, sizeof(size));
			stringOffset = std::byteswap(stringOffset);
			fileSize = std::byteswap(size);
			startSector = std::byteswap(startSector);
			uint16_t current = 0;
			id = L"";
			if (stringTable != NULL) {
				while (data[(stringTable + stringOffset) + current] != 0x00) {
					id += data[(stringTable + stringOffset) + current];
					current++;
				}
			}
			
		}
	}
}

Directory::~Directory() {
	if (date != nullptr) {
		delete[] date;
	}
}

bool Directory::findFileExtension(std::string filename) {
	return filename.rfind(".");
}