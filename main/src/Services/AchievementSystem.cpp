#include "AchievementSystem.h"
#include "Util/Services.h"
#include "Periph/NVSFlash.h"

static constexpr const std::array<AchievementData, (size_t) Achievement::COUNT> AchievementInfo = {/*TODO - fill out with AchievementData*/};

AchievementSystem::AchievementSystem(): achievementProgress((size_t) Achievement::COUNT), previousState((size_t) Achievement::COUNT){
	load();
}

void AchievementSystem::getAll(std::vector<AchievementData>& unlockedList) const{
	unlockedList.resize((size_t) Achievement::COUNT);

	for(size_t i = 0; i < (size_t) Achievement::COUNT; ++i){
		unlockedList[i] = achievementProgress[i];
	}
}

void AchievementSystem::reset(Achievement ID){
	achievementProgress[(size_t) ID].progress = 0;
}

void AchievementSystem::increment(Achievement ID, int32_t increment){
	if(!inSession){
		return;
	}

	achievementProgress[(size_t) ID].progress = std::min(achievementProgress[(size_t) ID].goal, achievementProgress[(size_t) ID].progress + increment);
}

void AchievementSystem::startSession(){
	inSession = true;
	previousState = achievementProgress;
}

void AchievementSystem::endSession(std::vector<AchievementData>& unlockedList){
	if(!inSession){
		return;
	}

	unlockedList.clear();

	inSession = false;

	for(size_t i = 0; i < (size_t) Achievement::COUNT; ++i){
		if(!previousState[i] && achievementProgress[i]){
			unlockedList.emplace_back(achievementProgress[i]);
		}
	}
}

bool AchievementSystem::load(){
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return false;
	}

	std::array<std::pair<int32_t, int32_t>, (size_t) Achievement::COUNT> rawData{};
	if(!nvs->get(Blob, rawData)){
		for(size_t i = 0; i < (size_t) Achievement::COUNT; ++i){
			achievementProgress[i] = AchievementInfo[i];
		}

		store();
		return true;
	}

	for(size_t i = 0; i < (size_t) Achievement::COUNT; ++i){
		achievementProgress[i] = AchievementData((Achievement) i, rawData[i].first, rawData[i].second);
	}

	return true;
}

void AchievementSystem::store(){
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return;
	}

	std::array<std::pair<int32_t, int32_t>, (size_t) Achievement::COUNT> rawData{};
	for(size_t i = 0; i < (size_t) Achievement::COUNT; ++i){
		rawData[i] = { achievementProgress[i].goal, achievementProgress[i].progress };
	}

	nvs->set(Blob, rawData);
}
