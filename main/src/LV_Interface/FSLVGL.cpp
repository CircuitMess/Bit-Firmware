#include "FSLVGL.h"
#include "FS/FSFileImpl.h"
#include "FS/SPIFFS.h"
#include <cstdio>
#include <dirent.h>
#include <esp_spiffs.h>
#include <esp_log.h>
#include <string>
#include <algorithm>
#include <unordered_map>

static const char* Cached[] = {
		"/bg/0.bin",
		"/bg/1.bin",
		"/bg/2.bin",
		"/bg/3.bin",
		"/Profile/bg.bin",
		"/Profile/barOutline.bin",
		"/Profile/arrowUp.bin",
		"/Profile/arrowDown.bin",
		"/Profile/arrowLeft.bin",
		"/Profile/arrowRight.bin",
		"/Profile/lock.bin",
		"/Profile/theme-outline.bin",
		"/Profile/ach-border.bin",
		"/Profile/theme-01-icon.bin",
		"/Profile/theme-02-icon.bin",
		"/Profile/theme-03-icon.bin",
		"/Profile/theme-04-icon.bin",
		"/Theme1/bg.bin",
		"/Theme1/header.bin",
		"/Theme1/paused.bin",
		"/Theme1/settings.bin",
		"/Theme2/bg.bin",
		"/Theme2/header.bin",
		"/Theme2/paused.bin",
		"/Theme2/settings.bin",
		"/Theme3/bg.bin",
		"/Theme3/header.bin",
		"/Theme3/paused.bin",
		"/Theme3/settings.bin",
		"/Theme4/bg.bin",
		"/Theme4/header.bin",
		"/Theme4/paused.bin",
		"/Theme4/settings.bin",
};

static const char* Archives[] = {
		"/Achi",
		"/AchiBW",
		"/Menu",
		"/MenuBW"
};

const char* TAG = "FSLVGL";
std::unordered_set<FSLVGL::FileResource, std::hash<File*>> FSLVGL::cache;
std::unordered_map<std::string, FileArchive*> FSLVGL::archives;
std::unordered_set<FSLVGL::FileResource, std::hash<File*>> FSLVGL::archiveOpen;

FSLVGL::FSLVGL(char letter){
	cache.reserve(sizeof(Cached) / sizeof(Cached[0]) + 16);

	/*esp_vfs_spiffs_conf_t conf = {
			.base_path = "/spiffs",
			.partition_label = "storage",
			.max_files = 8,
			.format_if_mount_failed = false
	};

	auto ret = esp_vfs_spiffs_register(&conf);
	if(ret != ESP_OK){
		if(ret == ESP_FAIL){
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		}else if(ret == ESP_ERR_NOT_FOUND){
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		}else{
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
		}

		return;
	}*/


	lv_fs_drv_init(&drv);                     /*Basic initialization*/

	drv.letter = letter;                         /*An uppercase letter to identify the drive */
	drv.ready_cb = ready_cb;               /*Callback to tell if the drive is ready to use */
	drv.open_cb = open_cb;                 /*Callback to open a file */
	drv.close_cb = close_cb;               /*Callback to close a file */
	drv.read_cb = read_cb;                 /*Callback to read a file */
	drv.write_cb = write_cb;               /*Callback to write a file */
	drv.seek_cb = seek_cb;                 /*Callback to seek in a file (Move cursor) */
	drv.tell_cb = tell_cb;                 /*Callback to tell the cursor position  */

	drv.dir_open_cb = dir_open_cb;         /*Callback to open directory to read its content */
	drv.dir_read_cb = dir_read_cb;         /*Callback to read a directory's content */
	drv.dir_close_cb = dir_close_cb;       /*Callback to close a directory */

	drv.user_data = this;             /*Any custom data if required*/

	lv_fs_drv_register(&drv);                 /*Finally register the drive*/
}

FSLVGL::~FSLVGL(){
	esp_vfs_spiffs_unregister("storage");
}

auto FSLVGL::findCache(const std::string& lvPath){
	std::string path("/spiffs");
	path.append(lvPath);

	return std::find_if(cache.begin(), cache.end(), [&path](auto fileResource){
		return fileResource.ramFile->name() == path;
	});
}

auto FSLVGL::findCache(void* ptr){
	return std::find_if(cache.begin(), cache.end(), [ptr](auto fileResource){
		return fileResource.ramFile == ptr;
	});
}

void FSLVGL::addToCache(const char* path, bool use32bAligned){
	std::string p;
	if(strchr(path, DriveSeparator) != nullptr){
		p = std::string(path + 2);
	}else{
		p = path;
	}

	auto found = findCache(p);
	if(found != cache.end()) return;

	auto file = SPIFFS::open(p.c_str());
	auto ram = new File();
	*ram = RamFile::open(file, use32bAligned);
	if(ram->size() == 0){
		delete ram;
		return;
	}

	FileResource r = { ram };
	cache.insert(r);
}

void FSLVGL::removeFromCache(const char* path){
	std::string p;
	if(strchr(path, DriveSeparator) != nullptr){
		p = std::string(path + 2);
	}else{
		p = path;
	}

	auto it = findCache(p);
	if(it == cache.end()) return;

	delete it->ramFile;
	cache.erase(it);
}

void FSLVGL::loadCache(){
	for(const auto& path : Cached){
		addToCache(path);
	}
}

void FSLVGL::unloadCache(){
	for(const auto& path : Cached){
		removeFromCache(path);
	}
}

void FSLVGL::loadArchives(){
	for(const auto& archive : Archives){
		const auto filename = std::string(archive) + ".sz";
		archives.insert(std::make_pair(archive, new FileArchive(SPIFFS::open(filename.c_str()))));
	}
}

bool FSLVGL::ready_cb(struct _lv_fs_drv_t* drv){
	return true;
}

void* FSLVGL::open_cb(struct _lv_fs_drv_t* drv, const char* path, lv_fs_mode_t mode){
	const int len = strnlen(path, 32);
	int slashIndex = 0;
	for(int i = 0; i < len; i++){
		if(path[i] == '/' && i != 0){
			slashIndex = i;
			break;
		}
	}

	const auto archive = archives.find(std::string(path, path+slashIndex));
	if(archive != archives.end()){
		auto file = new File();
		*file = archive->second->get(path + slashIndex + 1);
		if(!*file){
			delete file;
			return nullptr;
		}

		archiveOpen.insert(FileResource{ file });
		return file;
	}

	auto cached = findCache(path);
	if(cached != cache.end()){
		(*cached).ramFile->seek(0);
		return (*cached).ramFile;
	}

	const char* fsMode;

	if(mode == LV_FS_MODE_WR){
		fsMode = "w";
	}else{
		fsMode = "r";
	}

	auto fslvgl = (FSLVGL*) drv->user_data;
	std::string p = fslvgl->Root + std::string(path);

	return (void*) fopen(p.c_str(), fsMode);
}

lv_fs_res_t FSLVGL::close_cb(struct _lv_fs_drv_t* drv, void* file_p){
	auto archIt = std::find_if(archiveOpen.begin(), archiveOpen.end(), [file_p](auto fileResource){
		return fileResource.ramFile == file_p;
	});
	if(archIt != archiveOpen.end()){
		delete archIt->ramFile;
		archiveOpen.erase(archIt);
		return 0;
	}

	auto it = findCache(file_p);
	if(it != cache.end()) return 0;

	fclose((FILE*) file_p);
	return 0;
}

lv_fs_res_t FSLVGL::read_cb(struct _lv_fs_drv_t* drv, void* file_p, void* buf, uint32_t btr, uint32_t* br){
	auto archIt = std::find_if(archiveOpen.begin(), archiveOpen.end(), [file_p](auto fileResource){
		return fileResource.ramFile == file_p;
	});
	if(archIt != archiveOpen.end()){
		*br = (*archIt).ramFile->read((uint8_t*) buf, btr);
		return 0;
	}

	auto cached = findCache(file_p);
	if(cached != cache.end()){
		*br = (*cached).ramFile->read((uint8_t*) buf, btr);
		return 0;
	}

	if(ferror((FILE*) file_p)) return LV_FS_RES_NOT_EX;
	*br = fread(buf, 1, btr, (FILE*) file_p);
	return 0;
}

lv_fs_res_t FSLVGL::write_cb(struct _lv_fs_drv_t* drv, void* file_p, const void* buf, uint32_t btw, uint32_t* bw){
	auto archIt = std::find_if(archiveOpen.begin(), archiveOpen.end(), [file_p](auto fileResource){
		return fileResource.ramFile == file_p;
	});
	if(archIt != archiveOpen.end()){
		*bw = 0;
		return 0;
	}

	auto cached = findCache(file_p);
	if(cached != cache.end()){
		*bw = 0;
		return 0;
	}

	if(ferror((FILE*) file_p)) return LV_FS_RES_NOT_EX;

	*bw = fwrite((uint8_t*) buf, 1, btw, (FILE*) file_p);
	return 0;
}

lv_fs_res_t FSLVGL::seek_cb(struct _lv_fs_drv_t* drv, void* file_p, uint32_t pos, lv_fs_whence_t whence){
	auto archIt = std::find_if(archiveOpen.begin(), archiveOpen.end(), [file_p](auto fileResource){
		return fileResource.ramFile == file_p;
	});
	if(archIt != archiveOpen.end()){
		static const std::unordered_map<lv_fs_whence_t, SeekMode> SeekMap = {
				{ LV_FS_SEEK_SET, SeekMode::SeekSet },
				{ LV_FS_SEEK_CUR, SeekMode::SeekCur },
				{ LV_FS_SEEK_END, SeekMode::SeekEnd },
		};
		(*archIt).ramFile->seek(pos, SeekMap.at(whence));
		return 0;
	}

	auto cached = findCache(file_p);
	if(cached != cache.end()){
		static const std::unordered_map<lv_fs_whence_t, SeekMode> SeekMap = {
				{ LV_FS_SEEK_SET, SeekMode::SeekSet },
				{ LV_FS_SEEK_CUR, SeekMode::SeekCur },
				{ LV_FS_SEEK_END, SeekMode::SeekEnd },
		};
		(*cached).ramFile->seek(pos, SeekMap.at(whence));
		return 0;
	}

	if(ferror((FILE*) file_p)){
		return LV_FS_RES_NOT_EX;
	}

	int mode;
	switch(whence){
		case LV_FS_SEEK_SET:
			mode = SEEK_SET;
			break;
		case LV_FS_SEEK_CUR:
			mode = SEEK_CUR;
			break;
		case LV_FS_SEEK_END:
			mode = SEEK_END;
			break;
		default:
			mode = SEEK_SET;
	}
	if(fseek((FILE*) file_p, pos, mode) != 0){
		return LV_FS_RES_INV_PARAM;
	}
	return 0;

}

lv_fs_res_t FSLVGL::tell_cb(struct _lv_fs_drv_t* drv, void* file_p, uint32_t* pos_p){
	auto archIt = std::find_if(archiveOpen.begin(), archiveOpen.end(), [file_p](auto fileResource){
		return fileResource.ramFile == file_p;
	});
	if(archIt != archiveOpen.end()){
		*pos_p = (*archIt).ramFile->position();
		return 0;
	}

	auto cached = findCache(file_p);
	if(cached != cache.end()){
		*pos_p = (*cached).ramFile->position();
		return 0;
	}

	if(ferror((FILE*) file_p)) return LV_FS_RES_NOT_EX;
	auto val = ftell((FILE*) file_p);
	if(val == -1) return LV_FS_RES_UNKNOWN;
	*pos_p = val;
	return 0;
}

void* FSLVGL::dir_open_cb(struct _lv_fs_drv_t* drv, const char* path){
	auto fslvgl = (FSLVGL*) drv->user_data;
	std::string p = fslvgl->Root + std::string(path);
	DIR* dir = opendir(p.c_str());
	return dir;

}

lv_fs_res_t FSLVGL::dir_read_cb(struct _lv_fs_drv_t* drv, void* rddir_p, char* fn){
	auto entry = readdir((DIR*) rddir_p);
	if(!entry) return LV_FS_RES_NOT_EX;
	strncpy(fn, entry->d_name, 256);
	return 0;
}

lv_fs_res_t FSLVGL::dir_close_cb(struct _lv_fs_drv_t* drv, void* rddir_p){
	closedir((DIR*) rddir_p);
	return 0;
}
