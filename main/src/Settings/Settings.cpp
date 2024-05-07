#include "Settings.h"
#include "Periph/NVSFlash.h"
#include "Util/Services.h"

static const char* TAG = "Settings";

Settings::Settings(){
	load();
}

SettingsStruct Settings::get() const{
	return settingsStruct;
}

void Settings::set(SettingsStruct& settings){
	settingsStruct = settings;
}

void Settings::store(){
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return;
	}

	if(!nvs->set(BlobName, settingsStruct)){
		ESP_LOGW(TAG, "NVS settings set failed.");
	}
}

void Settings::load(){
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return;
	}

	if(!nvs->get(BlobName, settingsStruct)){
		store();

		if(!nvs->get(BlobName, settingsStruct)){
			ESP_LOGW(TAG, "NVS settings get failed.");
		}
	}
}
