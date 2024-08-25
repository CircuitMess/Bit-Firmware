#include "FSLVGL.h"
#include "FS/SPIFFS.h"
#include "Util/stdafx.h"
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "Filepaths.hpp"
#include "Services/RobotManager.h"
#include <cstdio>
#include <dirent.h>
#include <esp_spiffs.h>
#include <string>
#include <unordered_map>

std::initializer_list<std::string> GeneralCache = {
		"/Ach/bg.bin",
		"/Ach/bgSelected.bin",
		"/Profile/bg.bin",
		"/Profile/lock.bin",
		"/Profile/barOutline.bin",
		"/Profile/arrowDown.bin",
		"/Profile/arrowUp.bin",
		"/Profile/arrowLeft.bin",
		"/Profile/arrowRight.bin",
		"/Profile/ach-border.bin",
		"/Profile/ach-placeholder.bin",
		"/Profile/theme-01-icon.bin",
		"/Profile/theme-02-icon.bin",
		"/Profile/theme-03-icon.bin",
		"/Profile/theme-04-icon.bin",
		"/Profile/theme-outline.bin"
};

const char* TAG = "FSLVGL";

FSLVGL* FSLVGL::instance = nullptr;
bool FSLVGL::themeChanged = false;
bool FSLVGL::menuChanged = false;

FSLVGL::FSLVGL(char letter, Allocator* alloc) : alloc(alloc){
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

	instance = this;
}

FSLVGL::~FSLVGL(){
	esp_vfs_spiffs_unregister("storage");
}

void FSLVGL::themeChange(){
	themeChanged = true;
}

void FSLVGL::menuChange(){
	menuChanged = true;
}

void FSLVGL::reloadMenu(){
	instance->unloadMenu();
	instance->loadMenu();
}

void FSLVGL::loadCache(){
	if(cacheLoaded && themeChanged){
		unloadTheme();
		loadTheme();
	}

	if(cacheLoaded && menuChanged){
		unloadMenu();
		loadMenu();
	}

	if(cacheLoaded) return;
	cacheLoaded = true;

	if(alloc){
		archives.menu.allocBuf = alloc->malloc(MenuSize);
		archives.theme.allocBuf = alloc->malloc(ThemeSize);
		archives.achis.allocBuf = alloc->malloc(AchisSize);
	}else{
		archives.menu.allocBuf = malloc(MenuSize);
		archives.theme.allocBuf = malloc(ThemeSize);
		archives.achis.allocBuf = malloc(AchisSize);
	}

	cache.setPaths(getCacheFiles());
	cache.load(alloc);

	loadMenu();
	loadTheme();
	loadAchis();
}

void FSLVGL::unloadCache(){
	if(!cacheLoaded) return;
	cacheLoaded = false;

	unloadMenu();
	unloadTheme();
	unloadAchis();

	cache.unload();

	if(alloc){
		alloc->reset();
	}else{
		free(archives.menu.allocBuf);
		free(archives.theme.allocBuf);
		free(archives.achis.allocBuf);
	}

	archives.menu.allocBuf = nullptr;
	archives.theme.allocBuf = nullptr;
	archives.achis.allocBuf = nullptr;
}

void FSLVGL::loadMenu(){
	if(archives.menu) return;
	if(!cacheLoaded) return;
	if(archives.menu.allocBuf == nullptr) return;

	std::unordered_set<std::string> excluded;
	excluded.reserve((int) Games::COUNT);

	auto roboMan = (RobotManager*) Services.get(Service::RobotManager);
	for(int i = 0; i < (int) Games::COUNT; i++){
		if((Games) i == Games::Frank || (Games) i == Games::Fred) continue;

		std::string path = "/";
		if(roboMan->isUnlocked((Games) i)){
			path += "BW/";
		}
		path += GameIcons[i];
		path += ".bin";

		excluded.emplace(std::move(path));
	}

	archives.menu.alloc = new Allocator(archives.menu.allocBuf, MenuSize);
	archives.menu.archive = new FileArchive(SPIFFS::open("/Menu.sz"), archives.menu.alloc, excluded);

	menuChanged = false;
}

void FSLVGL::unloadMenu(){
	if(!archives.menu) return;

	delete archives.menu.archive;
	delete archives.menu.alloc;

	archives.menu.archive = nullptr;
	archives.menu.alloc = nullptr;
}

void FSLVGL::loadTheme(){
	if(archives.theme) return;
	if(!cacheLoaded) return;
	if(archives.theme.allocBuf == nullptr) return;

	auto settings = (Settings*) Services.get(Service::Settings);
	const auto theme = settings ? settings->get().theme : Theme::Theme1;

	static constexpr const char* ThemeArchives[] = { "/Theme1.sz", "/Theme2.sz", "/Theme3.sz", "/Theme4.sz" };

	archives.theme.alloc = new Allocator(archives.theme.allocBuf, ThemeSize);
	archives.theme.archive = new FileArchive(SPIFFS::open(ThemeArchives[(int) theme]), archives.theme.alloc);

	themeChanged = false;
}

void FSLVGL::unloadTheme(){
	if(!archives.theme) return;

	delete archives.theme.archive;
	delete archives.theme.alloc;

	archives.theme.archive = nullptr;
	archives.theme.alloc = nullptr;
}

void FSLVGL::loadAchis(){
	if(archives.achis) return;
	if(!cacheLoaded) return;
	if(archives.achis.allocBuf == nullptr) return;

	std::unordered_set<std::string> excluded;
	excluded.reserve((int) Achievement::COUNT);

	auto achiMan = (AchievementSystem*) Services.get(Service::Achievements);
	for(const auto& files : AchievementFiles){
		if(achiMan->isUnlocked(files.first)){
			excluded.emplace(files.second.locked + 7);
		}else{
			excluded.emplace(files.second.unlocked + 7);
		}
	}

	archives.achis.alloc = new Allocator(archives.achis.allocBuf, AchisSize);
	archives.achis.archive = new FileArchive(SPIFFS::open("/Achi.sz"), archives.achis.alloc, excluded);
}

void FSLVGL::unloadAchis(){
	if(!archives.achis) return;

	delete archives.achis.archive;
	delete archives.achis.alloc;

	archives.achis.archive = nullptr;
	archives.achis.alloc = nullptr;
}

std::vector<std::string> FSLVGL::getCacheFiles() const{
	std::vector<std::string> paths;
	paths.reserve(GeneralCache.size());

	for(const auto& file : GeneralCache){
		paths.emplace_back(file);
	}

	return paths;
}

void* FSLVGL::lvOpen(const char* path, lv_fs_mode_t mode){
	File file;

	static const auto mkPtr = [](File& file){
		file.seek(0);
		File* filePtr = new File();
		*filePtr = std::move(file);
		return filePtr;
	};

	file = cache.open(path);
	if(file) return mkPtr(file);

	if(archives.menu && std::string(path).starts_with("/Menu")){
		file = archives.menu.archive->get(path + 5);
		if(file) return mkPtr(file);
	}

	if(archives.theme && std::string(path).starts_with("/Theme")){
		file = archives.theme.archive->get(path + 7);
		if(file) return mkPtr(file);
	}

	if(archives.achis && std::string(path).starts_with("/Achi")){
		file = archives.achis.archive->get(path + 5);
		if(file) return mkPtr(file);
	}

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
