#include "XPSystem.h"
#include "Util/Services.h"

XPSystem::XPSystem() : nvs(*(NVSFlash*) Services.get(Service::NVS)){
	load();
}

uint32_t XPSystem::getXP() const{
	return xp;
}

uint8_t XPSystem::getLevel() const{
	return MapXPToLevel(xp).nextLvl - 1;
}

void XPSystem::increment(uint32_t xpGain){
	xp += xpGain;
	store();
}

constexpr LevelProgress XPSystem::MapXPToLevel(uint32_t xp){
	uint8_t levelupsNum = sizeof(LevelupThresholds) / sizeof(uint32_t);
	uint32_t requiredXP = 0;

	for(uint8_t i = 0; i < levelupsNum; i++){
		requiredXP += LevelupThresholds[i];
		if(xp < requiredXP){
			return { (uint8_t) (i + 2), (float) xp / LevelupThresholds[i] };
		}
	}
	return { MaxLevel, 1.0f };
}

void XPSystem::load(){
	if(!nvs.get(BlobName, xp)){
		store();
	}
}

void XPSystem::store(){
	nvs.set(BlobName, xp);
}
