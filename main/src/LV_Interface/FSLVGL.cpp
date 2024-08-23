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

std::initializer_list<std::string> Cached = {
		"/bg/0.bin",
		"/bg/1.bin",
		"/bg/2.bin",
		"/bg/3.bin",
		// "/Profile/bg.bin",
		// "/Profile/barOutline.bin",
		// "/Profile/arrowUp.bin",
		// "/Profile/arrowDown.bin",
		// "/Profile/arrowLeft.bin",
		// "/Profile/arrowRight.bin",
		// "/Profile/lock.bin",
		// "/Profile/theme-outline.bin",
		// "/Profile/ach-border.bin",
		// "/Profile/theme-01-icon.bin",
		// "/Profile/theme-02-icon.bin",
		// "/Profile/theme-03-icon.bin",
		// "/Profile/theme-04-icon.bin",
		// "/Theme1/bg.bin",
		// "/Theme1/header.bin",
		// "/Theme1/paused.bin",
		// "/Theme1/settings.bin",
		// "/Theme2/bg.bin",
		// "/Theme2/header.bin",
		// "/Theme2/paused.bin",
		// "/Theme2/settings.bin",
		// "/Theme3/bg.bin",
		// "/Theme3/header.bin",
		// "/Theme3/paused.bin",
		// "/Theme3/settings.bin",
		// "/Theme4/bg.bin",
		// "/Theme4/header.bin",
		// "/Theme4/paused.bin",
		// "/Theme4/settings.bin",
};

std::initializer_list<std::string> Archives = {
		"/Achi",
		"/AchiBW",
		"/Menu",
		"/MenuBW"
};

const char* TAG = "FSLVGL";

FSLVGL::FSLVGL(char letter) : cache(Cached), archive(Archives){
	lv_fs_drv_init(&drv);

	drv.letter = letter;
	drv.ready_cb = [](struct _lv_fs_drv_t*){ return true; }; // Is the drive ready to use?
	drv.open_cb = [](struct _lv_fs_drv_t* drv, const char* path, lv_fs_mode_t mode){ return ((FSLVGL*) drv->user_data)->lvOpen(path, mode);  };
	drv.close_cb = [](struct _lv_fs_drv_t* drv, void* file){ return ((FSLVGL*) drv->user_data)->lvClose(file);  };
	drv.read_cb = [](struct _lv_fs_drv_t* drv, void* file, void* buf, uint32_t btr, uint32_t* br){ return ((FSLVGL*) drv->user_data)->lvRead(file, buf, btr, br); };
	drv.write_cb = [](struct _lv_fs_drv_t* drv, void* file, const void* buf, uint32_t btw, uint32_t* bw){ return ((FSLVGL*) drv->user_data)->lvWrite(file, buf, btw, bw); };
	drv.seek_cb = [](struct _lv_fs_drv_t* drv, void* file, uint32_t pos, lv_fs_whence_t whence){ return ((FSLVGL*) drv->user_data)->lvSeek(file, pos, whence); };
	drv.tell_cb = [](struct _lv_fs_drv_t* drv, void* file, uint32_t* pos){ return ((FSLVGL*) drv->user_data)->lvTell(file, pos); };

	drv.dir_open_cb = [](struct _lv_fs_drv_t* drv, const char* path){ return ((FSLVGL*) drv->user_data)->lvDirOpen(path); };
	drv.dir_read_cb = [](struct _lv_fs_drv_t* drv, void* dir, char* fn){ return ((FSLVGL*) drv->user_data)->lvDirRead(dir, fn); };
	drv.dir_close_cb = [](struct _lv_fs_drv_t* drv, void* dir){ return ((FSLVGL*) drv->user_data)->lvDirClose(dir); };

	drv.user_data = this;

	lv_fs_drv_register(&drv);
}

FSLVGL::~FSLVGL(){
	esp_vfs_spiffs_unregister("storage");
}

void FSLVGL::loadArchives(){
	archive.load();
}

void FSLVGL::loadCache(){
	cache.load();
}

void FSLVGL::unloadCache(){
	cache.unload();
}

void* FSLVGL::lvOpen(const char* path, lv_fs_mode_t mode){
	File file;

	static const auto mkPtr = [](File& file){
		file.seek(0);
		File* filePtr = new File();
		*filePtr = std::move(file);
		return filePtr;
	};

	file = archive.open(path);
	if(file) return mkPtr(file);

	file = cache.open(path);
	if(file) return mkPtr(file);

	static const std::unordered_map<lv_fs_mode_t, const char*> Map = {
			{ LV_FS_MODE_WR, "w" },
			{ LV_FS_MODE_RD, "r" }
	};

	file = SPIFFS::open(path, Map.at(mode));
	if(file) return mkPtr(file);

	return nullptr;
}

lv_fs_res_t FSLVGL::lvClose(void* file){
	File* filePtr = (File*) file;
	delete filePtr;
	return 0;
}

lv_fs_res_t FSLVGL::lvRead(void* fp, void* buf, uint32_t btr, uint32_t* br){
	File* file = getFile(fp);
	if(!*file) return LV_FS_RES_NOT_EX;

	*br = file->read((uint8_t*) buf, btr);

	return 0;
}

lv_fs_res_t FSLVGL::lvWrite(void* fp, const void* buf, uint32_t btw, uint32_t* bw){
	File* file = getFile(fp);
	if(!*file) return LV_FS_RES_NOT_EX;

	*bw = file->write((const uint8_t*) buf, btw);

	return 0;
}

lv_fs_res_t FSLVGL::lvSeek(void* fp, uint32_t pos, lv_fs_whence_t whence){
	File* file = getFile(fp);
	if(!*file) return LV_FS_RES_NOT_EX;

	static const std::unordered_map<lv_fs_whence_t, SeekMode> Map = {
			{ LV_FS_SEEK_SET, SeekMode::SeekSet },
			{ LV_FS_SEEK_CUR, SeekMode::SeekCur },
			{ LV_FS_SEEK_END, SeekMode::SeekEnd }
	};
	if(!Map.contains(whence)) return LV_FS_RES_INV_PARAM;

	const auto success = file->seek(pos, Map.at(whence));
	if(!success) return LV_FS_RES_INV_PARAM;

	return 0;
}

lv_fs_res_t FSLVGL::lvTell(void* fp, uint32_t* pos){
	File* file = getFile(fp);
	if(!*file) return LV_FS_RES_NOT_EX;

	printf("pos\n");

	*pos = file->position();
	return 0;
}

void* FSLVGL::lvDirOpen(const char* path){
	std::string p = Root + std::string(path);
	DIR* dir = opendir(p.c_str());
	return dir;
}

lv_fs_res_t FSLVGL::lvDirRead(void* dir, char* fn){
	auto entry = readdir((DIR*) dir);
	if(!entry) return LV_FS_RES_NOT_EX;
	strncpy(fn, entry->d_name, 256);
	return 0;
}

lv_fs_res_t FSLVGL::lvDirClose(void* dir){
	closedir((DIR*) dir);
	return 0;
}
