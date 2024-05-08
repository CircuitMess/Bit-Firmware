#include "XPSystem.h"
#include "Util/Services.h"

XPSystem::XPSystem() : nvs(*(NVSFlash*) Services.get(Service::NVS)){
	load();
}

uint32_t XPSystem::getXP() const{
	return xp;
}

uint8_t XPSystem::getLevel() const{
	LevelProgress lp = MapXPToLevel(xp);
	if(lp.nextLvl == MaxLevel && lp.progress == 1.0f) return MaxLevel;

	return lp.nextLvl - 1;
}

void XPSystem::increment(uint32_t xpGain){
	xp = std::min(xp + xpGain, LevelupThresholds[MaxLevel-2]);
	store();
}

void XPSystem::load(){
	if(!nvs.get(BlobName, xp)){
		store();
	}

	xp = 0;
}

void XPSystem::store(){
	nvs.set(BlobName, xp);
}
