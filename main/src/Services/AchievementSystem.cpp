#include "AchievementSystem.h"
#include "Util/Services.h"
#include "Periph/NVSFlash.h"

//Starting data for every achievement
//TODO - adjust bronze/silver/gold (_b/s/g) achievement goals according to game
static constexpr const AchievementData DefaultAchievementData[(uint32_t) Achievement::COUNT] = {
		{ Achievement::Artemis_friendly, 100,  0 },
		{ Achievement::Artemis_sharp,    1,    0 },
		{ Achievement::Bee_b,            50,   0 },
		// { Achievement::Bee_crow,         1,    0 },
		{ Achievement::Bee_g,            200,  0 },
		{ Achievement::Bee_s,            100,  0 },
		// { Achievement::Blocks_4lines,    1,    0 },
		{ Achievement::Blocks_clear,     1,    0 },
		{ Achievement::Blocks_b,         500,  0 },
		{ Achievement::Blocks_g,         2000, 0 },
		{ Achievement::Blocks_s,         1000, 0 },
		{ Achievement::Bob_bombs,        1,    0 },
		// { Achievement::Bob_green,        1,    0 },
		// { Achievement::Bob_yellow,       1,    0 },
		{ Achievement::Bonk_5,           5,    0 },
		{ Achievement::Bonk_comeback,    1,    0 },
		// { Achievement::Buttons_triple,   1,    0 },
		{ Achievement::Buttons_win,      1,    0 },
		{ Achievement::Capacitron_b,     50,  0 },
		{ Achievement::Capacitron_fire,  1,    0 },
		{ Achievement::Capacitron_g,     200, 0 },
		{ Achievement::Capacitron_s,      100,  0 },
		// { Achievement::Charlie_nomercy,   1,    0 },
		// { Achievement::Charlie_yesmercy,  1,    0 },
		// { Achievement::Dusty_shovel,      1,    0 },
		{ Achievement::Dusty_rat,         1,    0 },
		// { Achievement::Fred_copper,       1,    0 },
		// { Achievement::Fred_detective,    1,    0 },
		// { Achievement::Fred_veteran,      100,  0 },
		{ Achievement::Hertz_3,           1,    0 },
		{ Achievement::Marv_5,            5,    0 },
		{ Achievement::Marv_life,         1,    0 },
		// { Achievement::Marv_newspapers,   100,  0 },
		{ Achievement::Planck_b,          10,   0 },
		// { Achievement::Planck_chauffeur,  1,    0 },
		{ Achievement::Planck_g,          50,  0 },
		{ Achievement::Planck_juice,      10,   0 },
		// { Achievement::Planck_nobrake,    1,    0 },
		{ Achievement::Planck_s,          20,  0 },
		{ Achievement::Resistron_b,       50,   0 },
		{ Achievement::Resistron_deflect, 1,    0 },
		{ Achievement::Resistron_g,       200,  0 },
		{ Achievement::Resistron_s,       100,  0 },
		{ Achievement::Robby_asteroid,    1,    0 },
		{ Achievement::Robby_sharp,       1,    0 },
		{ Achievement::Snake_b,           50,   0 },
		{ Achievement::Snake_fill,        1,    0 },
		{ Achievement::Snake_g,           200,  0 },
		{ Achievement::Snake_s,           100,  0 },
		{ Achievement::Sparkly_nobrake,   1,    0 },
		// { Achievement::Sparkly_overtake,  1,    0 },
		{ Achievement::Sparkly_road,      1,    0 },
		{ Achievement::Stacky_5,          5,    0 },
		{ Achievement::Stacky_highrise,   50,   0 },
		{ Achievement::Stacky_skyscraper, 100,  0 },
		{ Achievement::Stacky_super,      200,  0 }
};

static const char* TAG = "AchievementSystem";

AchievementSystem::AchievementSystem(){
	achievementProgress = { DefaultAchievementData, DefaultAchievementData + sizeof(DefaultAchievementData)/sizeof(DefaultAchievementData[0]) };
	load();
}

bool AchievementSystem::isUnlocked(Achievement achi){
	const auto& data = achievementProgress[(size_t) achi];
	return data.progress >= data.goal;
}

void AchievementSystem::getAll(std::vector<AchievementData>& unlockedList) const{
	unlockedList.resize((size_t) Achievement::COUNT);

	for(size_t i = 0; i < (size_t) Achievement::COUNT; ++i){
		unlockedList[i] = achievementProgress[i];
	}
}

const AchievementData& AchievementSystem::get(Achievement ID){
	return achievementProgress[(int) ID];
}

void AchievementSystem::reset(Achievement ID){
	if(!inSession){
		return;
	}

	// allow reset if achievement was won this session
	if(previousState[(size_t) ID].progress >= achievementProgress[(size_t) ID].goal){
		return;
	}

	achievementProgress[(size_t) ID].progress = 0;
}

void AchievementSystem::increment(Achievement ID, int32_t increment){
	if(!inSession){
		return;
	}

	achievementProgress[(size_t) ID].progress = std::min(achievementProgress[(size_t) ID].goal, achievementProgress[(size_t) ID].progress + increment);
}

void AchievementSystem::startSession(){
	if(inSession){
		ESP_LOGW(TAG, "startSession called but already in session!");
		return;
	}

	inSession = true;
	previousState = achievementProgress;
}

void AchievementSystem::endSession(std::vector<AchievementData>& unlockedList){
	if(!inSession){
		ESP_LOGW(TAG, "endSession called but not in session!");
		return;
	}

	unlockedList.clear();

	inSession = false;

	for(size_t i = 0; i < (size_t) Achievement::COUNT; ++i){
		if(!previousState[i].unlocked() && achievementProgress[i].unlocked()){
			unlockedList.emplace_back(achievementProgress[i]);
		}
	}

	store();
}

bool AchievementSystem::load(){
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return false;
	}

	std::array<uint32_t, (size_t) Achievement::COUNT> data = {};

	if(!nvs->get(Blob, data)){
		store();
		return true;
	}

	for(size_t i = 0; i < (size_t) Achievement::COUNT; ++i){
		achievementProgress[i].progress = data[i];
	}

	return true;
}

void AchievementSystem::store(){
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return;
	}

	std::array<uint32_t, (int) Achievement::COUNT> data = {};

	for(int i = 0; i < (int) Achievement::COUNT; i++){
		data[i] = achievementProgress[i].progress;
	}

	nvs->set(Blob, data);
}
