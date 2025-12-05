#include "FileTree.h"
#include <iostream>
FileTree::FileTree(uint32_t location, uint8_t track, FileTree* parent, uint8_t* data, std::string type, std::wstring path) {
	this->path = path;
	unsigned long size = 0;
	dirs = new std::vector<Directory*>;
	if (type == "cd/dvd" || type == "joliet") {
		cur = new Directory(location, track, data, type, false);
		location = (location + cur->length) + cur->length % 2;
		prev = new Directory(location, track, data, type, false);
		location = (location + prev->length) + prev->length % 2;
		size = cur->fileSize;
		unsigned int index = 0;
		bool done = false;
		while (!done) {
			if (data[location] == 0x00) {
				done = true;
				break;
			}
			dirs->push_back(new Directory(location, track, data, type, false));
			location = (location + (dirs->at(index)->length + dirs->at(index)->ealength)) + (dirs->at(index)->length % 2);
			if (isBitSet(dirs->at(index)->flags, 7)) {
				bool bitSet = true;
				while (bitSet) {
					index += 1;
					dirs->push_back(new Directory(location, track, data, type, false));
					bitSet = isBitSet(dirs->at(index)->flags, 7);
					if (!bitSet) {
						bitSet = false;
					}
					merge(dirs->at(index - 1), dirs->at(index));
					location = (location + (dirs->at(index)->length + dirs->at(index)->ealength)) + (dirs->at(index)->length % 2);
					delete dirs->at(index);
					dirs->pop_back();
					index -= 1;
				}
				index += 1;
			}
			else {
				index += 1;
			}
			if (data[location] == 0x00) {
				location = ((location + 2048 - 1) / 2048) * 2048;
				if (location >= size) {
					done = true;
				}
			}
		}
	}
	else if (type == "xbox") {
		recurse_xbox(location, track, data, dirs);
	}
};

FileTree::FileTree(uint32_t location, uint8_t track, FileTree* parent, uint8_t* data, std::string type, uint16_t stringTable, std::wstring path) {
	this->path = path;
	this->parent = parent;
	if (this->parent == nullptr) {
		recurse_gamecube(this, location, track, parent, data, type, stringTable, this->path);
	}
}


uint32_t FileTree::recurse_gamecube(FileTree* tree, uint32_t location, uint8_t track, FileTree* parent, uint8_t* data, std::string type, uint16_t stringTable, std::wstring path) {
		tree->dirs = new std::vector<Directory*>;
		tree->parent = parent;
		tree->cur = new Directory(location, track, data, "gamecube", false, stringTable);
		uint32_t directories = tree->cur->fileSize;
		uint32_t size = directories * 12;
		FileTree* previous = parent;
		location += 12;
		uint32_t start = location;
		while (location < size) {
			tree->dirs->push_back(new Directory(location, track, data, "gamecube", false, stringTable));
			if (tree->dirs->at(tree->dirs->size() - 1)->folder) {
				FileTree* next = new FileTree(location, track, this, data, "gamecube", stringTable, path + tree->dirs->at(tree->dirs->size() - 1)->id + L"\\");
				tree->dirs->at(tree->dirs->size() - 1)->next = next;
				location += (recurse_gamecube(tree->dirs->at(tree->dirs->size() - 1)->next, location, track, tree, data, type, stringTable, path + tree->dirs->at(tree->dirs->size() - 1)->id + L"\\") + 12);
				continue;
			}
			location += 12;
		}
		return tree->cur->fileSize * 12 - start;
}

void FileTree::sort(std::vector<Directory*>* dirs) {
	std::sort(dirs->begin(),dirs->end(), sortByFolder);
	size_t partition = 0;
	for (size_t i = 0; i < dirs->size(); i++) {
		if (dirs->at(i)->folder) {
			partition++;
		}
	}
	std::partial_sort(dirs->begin(), dirs->begin() + partition, dirs->begin() + partition, sortByName);
	std::partial_sort(dirs->begin() + partition, dirs->end(), dirs->end(), sortByName);

}

void FileTree::merge(Directory* dir1, Directory* dir2) {
	dir1->fileSize += dir2->fileSize;
}

FileTree::~FileTree() {
	delete cur;
	delete prev;
	for (unsigned long i = 0; i < dirs->size(); i++) {
		if (dirs->at(i)->next != nullptr) {
			delete dirs->at(i)->next;
		}
		delete dirs->at(i);
	}
	delete dirs;
}

void FileTree::recurse_xbox(uint32_t location, uint8_t track, uint8_t* data, std::vector<Directory*> *filelist) {
	uint16_t lefttree;
	memcpy_s(&lefttree, sizeof(lefttree), data + location, sizeof(lefttree));
	if (lefttree != 0xffff) {
		Directory* dir = new Directory(location, track, data, "xbox", false);
		filelist->push_back(dir);
		if (dir->tree_left != NULL) {
			recurse_xbox((dir->tree_left * 4), track, data, filelist);
		}
		if (dir->tree_right != NULL) {
			recurse_xbox((dir->tree_right * 4), track, data, filelist);
		}
	}
}