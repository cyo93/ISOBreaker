#include "CueSheet.h"
using namespace std;
vector<Track*> CueSheet::parseSheet(std::filesystem::path sheet) {
	vector<Track*> tracks;
	ifstream file;
	file.open(sheet);
	string item;
	std::filesystem::path filename;
	unsigned long startsector = 0;
	bool multiSession = false;
	bool contiguous = false;
	while (getline(file, item)) {
		vector<string> items;
		split(item, " ", items);
		if (items.size() != 0) {
			if (tracks.size() > 0 && !contiguous && !multiSession) {
				startsector = (tracks[tracks.size() - 1]->disc->getEndSector());
			}
			if (items[0] == "REM") {
				if (items[1] == "HIGH-DENSITY") {
					startsector = (startsector < 45000 ? 45000 : startsector); // Dreamcast GD-ROM's, which are really just multi-Sesssion CD-ROM's always start at sector 45000.
					multiSession = true; // We set a multi session flag to prevent the program from using the previous file to calculate the start sector of the next track.
					continue;
				}
				else if (items[1] == "SESSION" && stoi(items[2]) > 1) {
					startsector += 11400; // A new session always adds this amount of sectors to the start of the next track.
					multiSession = true; // ditto.
				}
			}
			else if (items[0] == "FILE") {
				filename = sheet.remove_filename() / items[1];
				
			}
			else if (items[0] == "TRACK") {
				bool mode2 = false;
				if (items[2] == "MODE2/2352") { // Mode 2 has a slightly different sector format; the data is shifted right by eight bytes.
					mode2 = true;
					tracks.push_back(new Track(filename, tracks.size(), 2352, mode2, startsector, "", contiguous));
				}
				else if (items[2] == "MODE1/2048") {
					tracks.push_back(new Track(filename, tracks.size(), 2048, mode2, startsector, "", contiguous));
				}
				else if (items[2] == "AUDIO") {
					tracks.push_back(new Track(filename, tracks.size(), 2352, mode2, startsector, "audio"));
				}
				else {
					tracks.push_back(new Track(filename, tracks.size(), 2352, mode2, startsector, "", contiguous));
				}
				if (tracks.size() > 1 && tracks[tracks.size() - 2]->disc->getFileName() == filename) {
					contiguous = true;
					tracks[tracks.size() - 1]->disc->contiguous = true;
					continue;
				}
				multiSession == true ? multiSession = false : multiSession;
				contiguous == true ? contiguous = false : contiguous;
			}
			else if (items[0] == "INDEX") {
				if (contiguous) {
					if (stoi(items[1]) < 1) {
						tracks[tracks.size() - 2]->disc->setEndSector(msftolba(items[2]));
						tracks[tracks.size() - 1]->disc->setIndex(stoi(items[1]), msftolba(items[2]));
						continue;
					}
					else {
						tracks[tracks.size() - 1]->disc->getIndex(0) == 0 ? tracks[tracks.size() - 2]->disc->setEndSector(msftolba(items[2])) : tracks[tracks.size() - 2]->disc->setIndex(0, 0);
						tracks[tracks.size() - 1]->disc->setStartSector(msftolba(items[2]));
						if (tracks[tracks.size() - 1]->disc->disctype == "unknown") {
							tracks[tracks.size() - 1]->disc->getTrackType();
						}
						tracks[tracks.size() - 1]->disc->setIndex(stoi(items[1]), msftolba(items[2]));
					}
				}
				else {
					tracks[tracks.size() - 1]->disc->setIndex(stoi(items[1]), msftolba(items[2]));
				}
			}
			else {
				continue;
			}
		}
	}
	file.close();
	return tracks;
}

unsigned long CueSheet::msftolba(string msf) {
	vector<string> msfSplit;
	split(msf, ":", msfSplit);
	unsigned long sectors = ((stol(msfSplit[0]) * 60)*75) + (stol(msfSplit[1])*75) + stol(msfSplit[2]);
	return sectors;
}


void CueSheet::split(string _string, string delimiter, vector<string>& list) {
	while (_string.size() != 0) {
		size_t token = _string.find(delimiter);
		if (token == -1) {
			token = _string.size();
		}
		string newString;
		for (size_t i = 0; i < token; i++) {
			newString += _string[0];
			_string.erase(0, 1);
		}
		_string.erase(0, delimiter.size());
		list.push_back(newString);

		// check for quotes to ignore delimiters inside them.
		if (_string[0] == '"') {
			newString.clear();
			_string.erase(0, 1);
			token = _string.find("\"");
			for (size_t i = 0; i < token; i++) {
				newString += _string[0];
				_string.erase(0, 1);
			}
			if (token != -1) {
				_string.erase(0, delimiter.size() + 1);
				list.push_back(newString);
			}
		}
		for (size_t i = 0; i < list.size(); i++) {
			if (list[i] == "") {
				list.erase(list.begin() + i);
			}
		}
	}
}