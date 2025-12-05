#include "Track.h"
#include "DataLogic.h"
#include "Directory.h"
namespace fs = std::filesystem;
using namespace std;

Track::Track(std::filesystem::path _filename, uint8_t track, int _sectorsize, bool _mode2, unsigned long _startSector, std::string disctype, bool contiguous, bool duallayer) {
	disc = new VolumeISO(_filename, track, _sectorsize, _mode2, _startSector, disctype, contiguous, duallayer);
	disc->setIndex(0, 0);
	disc->setIndex(1, 0);
}

Track::~Track() {
	delete disc;
}
