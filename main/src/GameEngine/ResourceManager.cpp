#include "ResourceManager.h"
#include "FS/RamFile.h"
#include "FS/SPIFFS.h"
#include "Util/stdafx.h"
#include <esp_log.h>
#include <esp_attr.h>

ResourceManager::ResourceManager(const char* root) : root(root){}

void IRAM_ATTR ResourceManager::load(const std::vector<ResDescriptor>& descriptors, Allocator* alloc){
//	uint8_t copyBuffer[1024];

	for(auto descriptor : descriptors){
		delayMillis(1);

		std::string path;

		if(descriptor.path[0] == 'c'){
			descriptor.path = descriptor.path.substr(1);
			path = "/Games/Common" + descriptor.path;
		}else{
			path = root + descriptor.path;
		}

		auto cPath = path.c_str();

		File original = SPIFFS::open(cPath);
		if(!original){
			ESP_LOGE("ResMan", "Failed to load resource %s", cPath);
			continue;
		}

		if(descriptor.inRam){
			if(descriptor.compressParams){
				//TODO - implement compressed files if necessary
/*				//decode compressed file from SPIFFS to decoded RAMFile
				original = CompressedFile::open(original, descriptor.compressParams.expansion, descriptor.compressParams.lookahead);
				original.seek(0);
				File output = RamFile::create();

				while(size_t readBytes = original.read(copyBuffer, sizeof(copyBuffer))){
					output.write(copyBuffer, readBytes);
				}
				resources[descriptor.path] = output;*/
			}else{
				//copy file from SPIFFS to RAMFile
				File file;

				if(alloc){
					auto buf = (uint8_t*) alloc->malloc(original.size());
					if(buf == nullptr){
						ESP_LOGW("GameLoader", "Failed allocating %zu B from allocator. Reverting to RamFile", original.size());
						file = RamFile::open(original);
					}else{
						original.read(buf, original.size());
						file = { std::make_shared<RamFile>(buf, original.size(), original.name()) };
					}
				}else{
					file = RamFile::open(original);
				}

				resources[descriptor.path] = file;
			}

		}else{
			//use file from SPIFFS, not from RAM
			resources[descriptor.path] = original;
		}
	}
}

File ResourceManager::getResource(std::string path){
	auto file = resources.find(path);
	if(file == resources.end()) return {};
	file->second.seek(0);
	return file->second;
}

