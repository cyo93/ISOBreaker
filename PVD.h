#ifndef PVD_H
#define PVD_H
#include <string>
#include "Directory.h"
#include "DataLogic.h"

class PVD {
public:
	PVD(uint8_t* data, uint8_t track, std::string type);
	~PVD();
	std::wstring sysIdentifier = L"";
	std::wstring volIdentifier = L"";
	uint32_t rootDirStart;
	uint32_t rootDirSize;
	uint32_t spaceSize;
	uint16_t setSize;
	uint16_t seqNumber;
	uint16_t lbaSize;
	uint16_t pathTableSize;
	uint16_t pathTableLocation;
	uint16_t optionalPathTableLocation;
	Directory* rootDir;
	char volSetIdentifier[128];
	char publisherIdentifier[128];
	char dataPrepararIdentifier[128];
	char applicationIdentifier[128];
	char copyrightIdentifier[37];
	char abstractIdentifier[37];
	char bibIdentifier[37];
	char volCreationTime[17];
	char volModificationTime[17];
	char volExpirationTime[17];
	char volEffectiveDate[17];
private:
	
};
#endif