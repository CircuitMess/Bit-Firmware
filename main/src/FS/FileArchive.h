#ifndef BIT_FIRMWARE_FILEARCHIVE_H
#define BIT_FIRMWARE_FILEARCHIVE_H

#include <vector>
#include <unordered_map>
#include <string>
#include "FS/File.h"

class FileArchive {
public:
	FileArchive(File file);

	File get(const char* file, const char* name = nullptr);

private:
	struct Entry {
		const std::string name;
		size_t size;
		size_t offset;
	};

	std::unordered_map<std::string, const Entry> entries;
	std::vector<uint8_t> data;

};


#endif //BIT_FIRMWARE_FILEARCHIVE_H
