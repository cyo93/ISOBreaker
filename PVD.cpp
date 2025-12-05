#include "PVD.h"

PVD::PVD(uint8_t* data, uint8_t track, std::string type) {
	if (type == "cd/dvd" || type == "joliet") {
		uint8_t* rootdir[34];
		sysIdentifier.assign(&data[8], &data[8] + 32);
		volIdentifier.assign(&data[40], &data[40] + 32);
		if (type == "joliet") {
			sysIdentifier = bigEndiantoLittleEndianUTF16(sysIdentifier);
			volIdentifier = bigEndiantoLittleEndianUTF16(volIdentifier);
		}
		sysIdentifier = nullTerminatedStr(sysIdentifier);
		volIdentifier = nullTerminatedStr(volIdentifier);
		memcpy_s(&spaceSize, sizeof(uint32_t), (data + 80), sizeof(uint32_t));
		memcpy_s(&setSize, sizeof(uint16_t), (data + 120), sizeof(uint16_t));
		memcpy_s(&seqNumber, sizeof(uint16_t), (data + 124), sizeof(uint16_t));
		memcpy_s(&lbaSize, sizeof(uint16_t), (data + 128), sizeof(uint16_t));
		memcpy_s(&pathTableSize, sizeof(uint32_t), (data + 132), sizeof(uint32_t));
		memcpy_s(&pathTableLocation, sizeof(uint32_t), (data + 140), sizeof(uint32_t));
		memcpy_s(&optionalPathTableLocation, sizeof(uint32_t), (data + 144), sizeof(uint32_t));
		rootDir = new Directory(156, track, data, type, false);
		memcpy_s(&volSetIdentifier, 128, (data + 190), 128);
		memcpy_s(&publisherIdentifier, 128, (data + 318), 128);
		memcpy_s(&dataPrepararIdentifier, 128, (data + 446), 128);
		memcpy_s(&applicationIdentifier, 128, (data + 574), 128);
		memcpy_s(&copyrightIdentifier, 37, (data + 702), 37);
		memcpy_s(&abstractIdentifier, 37, (data + 739), 37);
		memcpy_s(&bibIdentifier, 37, (data + 776), 37);
		memcpy_s(&volCreationTime, 17, (data + 813), 17);
		memcpy_s(&volModificationTime, 17, (data + 830), 17);
		memcpy_s(&volExpirationTime, 17, (data + 847), 17);
		memcpy_s(&volEffectiveDate, 17, (data + 864), 17);
	}
	else if (type == "gamecube") {
		sysIdentifier.assign(&data[0], &data[0] + 6);
		size_t index = 32;
		while (data[index] != 0x00) {
			volIdentifier += data[index];
			index++;
		}
		lbaSize = 2048;
		spaceSize = 712880;
		rootDir = new Directory(1060, track, data, "gamecube", true, NULL);

	}
	else if (type == "xbox") {

		rootDir = new Directory(0, track, data, "xbox", true);
	}
}

PVD::~PVD() {
	delete rootDir;
}