#ifndef CUE_SHEET_H
#define CUE_SHEET_H
#include <vector>
#include "Track.h"
class CueSheet {
public:
	static std::vector<Track*> parseSheet(std::filesystem::path sheet);
private:
	static unsigned long msftolba(std::string msf);
	static void split(std::string _string, std::string delimiter, std::vector<std::string>& list);
};



#endif
