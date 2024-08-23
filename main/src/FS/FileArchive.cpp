#include <esp_log.h>
#include "FileArchive.h"
#include "FS/RamFile.h"

FileArchive::FileArchive(File file){
	file.seek(0);
	uint32_t count = 0;
	file.read((uint8_t*) &count, 4);

	std::vector<Entry> tmp;
	tmp.reserve(count);

	size_t totalSize = 0;

	while(file.available()){
		std::string name;
		name.reserve(32);
		while(file.available()){
			if(name.length() >= 32){
				ESP_LOGE("FileArchive", "Too big filename; %s...", name.c_str());
				abort();
			}

			char c = 0;
			file.read((uint8_t*) &c, 1);
			if(c == 0) break;

			name.append(1, c);
		}

		if(name.empty()) break;

		uint32_t size = 0;
		file.read((uint8_t*) &size, 4);

		totalSize += size;
		tmp.emplace_back(Entry { name, size, 0 });
	}

	data.resize(totalSize, 0);
	size_t offset = 0;

	for(auto& entry : tmp){
		file.read(data.data() + offset, entry.size);
		entry.offset = offset;

		offset += entry.size;

		entries.insert(std::make_pair(entry.name, std::move(entry)));
	}
}

File FileArchive::get(const char* file, const char* name){
	auto it = entries.find(file);
	if(it == entries.end()) return File();

	if(!name){
		name = file;
	}

	auto f = std::make_shared<RamFile>(data.data() + it->second.offset, it->second.size, name);
	return File(f);
}