#include <esp_log.h>
#include "RawCache.h"
#include "SPIFFS.h"
#include "RamFile.h"

RawCache::RawCache(){ }

RawCache::RawCache(const std::vector<std::string>& paths) : paths(paths){
	files.reserve(paths.size());
}

void RawCache::setPaths(const std::vector<std::string>& paths){
	if(loaded) return;

	this->paths = paths;

	if(files.bucket_count() < paths.size()){
		files.reserve(paths.size());
	}
}

void RawCache::load(Allocator* alloc){
	if(loaded) return;
	loaded = true;

	for(const auto& path : paths){
		File file = SPIFFS::open(path.c_str());
		if(!file){
			ESP_LOGW("RawCache", "Can't open file %s", path.c_str());
			continue;
		}

		File myFile;

		if(alloc){
			auto buf = (uint8_t*) alloc->malloc(file.size());
			if(buf != nullptr){
				file.read(buf, file.size());
				myFile = { std::make_shared<RamFile>(buf, file.size(), file.name()) };
			}else{
				ESP_LOGW("RawCache", "Failed allocating %zu B from allocator. Reverting to RamFile", file.size());
				myFile = RamFile::open(file);
			}
		}else{
			myFile = RamFile::open(file);
		}

		files.emplace(path, myFile);
	}
}

void RawCache::unload(){
	if(!loaded) return;
	loaded = false;

	files.clear();
}

File RawCache::open(const char* path){
	auto entry = files.find(path);
	if(entry == files.end()) return { };
	return entry->second;
}
