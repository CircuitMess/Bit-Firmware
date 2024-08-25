#include "AchievementSystem.h"
#include "Util/Services.h"
#include "Periph/NVSFlash.h"

//Starting data for every achievement
//TODO - adjust bronze/silver/gold (_b/s/g) achievement goals according to game
static const std::unordered_map<Achievement, AchievementData> DefaultAchievementData = {
		{ Achievement::Artemis_friendly,  { Achievement::Artemis_friendly,  100,  0 }},
		{ Achievement::Artemis_sharp,     { Achievement::Artemis_sharp,     1,    0 }},
		{ Achievement::Bee_b,             { Achievement::Bee_b,             50,   0 }},
		{ Achievement::Bee_g,             { Achievement::Bee_g,             200,  0 }},
		{ Achievement::Bee_s,             { Achievement::Bee_s,             100,  0 }},
		{ Achievement::Blocks_clear,      { Achievement::Blocks_clear,      1,    0 }},
		{ Achievement::Blocks_b,          { Achievement::Blocks_b,          500,  0 }},
		{ Achievement::Blocks_g,          { Achievement::Blocks_g,          2000, 0 }},
		{ Achievement::Blocks_s,          { Achievement::Blocks_s,          1000, 0 }},
		{ Achievement::Bob_bombs,         { Achievement::Bob_bombs,         1,    0 }},
		{ Achievement::Bonk_5,            { Achievement::Bonk_5,            5,    0 }},
		{ Achievement::Bonk_comeback,     { Achievement::Bonk_comeback,     1,    0 }},
		{ Achievement::Buttons_win,       { Achievement::Buttons_win,       1,    0 }},
		{ Achievement::Capacitron_b,      { Achievement::Capacitron_b,      50,   0 }},
		{ Achievement::Capacitron_fire,   { Achievement::Capacitron_fire,   1,    0 }},
		{ Achievement::Capacitron_g,      { Achievement::Capacitron_g,      200,  0 }},
		{ Achievement::Capacitron_s,      { Achievement::Capacitron_s,      100,  0 }},
		{ Achievement::Dusty_rat,         { Achievement::Dusty_rat,         1,    0 }},
		{ Achievement::Hertz_3,           { Achievement::Hertz_3,           1,    0 }},
		{ Achievement::Marv_5,            { Achievement::Marv_5,            5,    0 }},
		{ Achievement::Marv_life,         { Achievement::Marv_life,         1,    0 }},
		{ Achievement::Planck_b,          { Achievement::Planck_b,          10,   0 }},
		{ Achievement::Planck_g,          { Achievement::Planck_g,          50,   0 }},
		{ Achievement::Planck_juice,      { Achievement::Planck_juice,      10,   0 }},
		{ Achievement::Planck_s,          { Achievement::Planck_s,          20,   0 }},
		{ Achievement::Resistron_b,       { Achievement::Resistron_b,       50,   0 }},
		{ Achievement::Resistron_deflect, { Achievement::Resistron_deflect, 1,    0 }},
		{ Achievement::Resistron_g,       { Achievement::Resistron_g,       200,  0 }},
		{ Achievement::Resistron_s,       { Achievement::Resistron_s,       100,  0 }},
		{ Achievement::Robby_asteroid,    { Achievement::Robby_asteroid,    1,    0 }},
		{ Achievement::Robby_sharp,       { Achievement::Robby_sharp,       1,    0 }},
		{ Achievement::Snake_b,           { Achievement::Snake_b,           50,   0 }},
		{ Achievement::Snake_fill,        { Achievement::Snake_fill,        1,    0 }},
		{ Achievement::Snake_g,           { Achievement::Snake_g,           200,  0 }},
		{ Achievement::Snake_s,           { Achievement::Snake_s,           100,  0 }},
		{ Achievement::Sparkly_nobrake,   { Achievement::Sparkly_nobrake,   1,    0 }},
		{ Achievement::Sparkly_road,      { Achievement::Sparkly_road,      1,    0 }},
		{ Achievement::Stacky_5,          { Achievement::Stacky_5,          5,    0 }},
		{ Achievement::Stacky_highrise,   { Achievement::Stacky_highrise,   50,   0 }},
		{ Achievement::Stacky_skyscraper, { Achievement::Stacky_skyscraper, 100,  0 }},
		{ Achievement::Stacky_super,      { Achievement::Stacky_super,      200,  0 }},
		// { Achievement::Bee_crow,         1,    0 },
		// { Achievement::Blocks_4lines,    1,    0 },
		// { Achievement::Bob_green,        1,    0 },
		// { Achievement::Bob_yellow,       1,    0 },
		// { Achievement::Buttons_triple,   1,    0 },
		// { Achievement::Charlie_nomercy,   1,    0 },
		// { Achievement::Charlie_yesmercy,  1,    0 },
		// { Achievement::Dusty_shovel,      1,    0 },
		// { Achievement::Fred_copper,       1,    0 },
		// { Achievement::Fred_detective,    1,    0 },
		// { Achievement::Fred_veteran,      100,  0 },
		// { Achievement::Marv_newspapers,   100,  0 },
		// { Achievement::Planck_chauffeur,  1,    0 },
		// { Achievement::Planck_nobrake,    1,    0 },
		// { Achievement::Sparkly_overtake,  1,    0 },
};

static const char* TAG = "AchievementSystem";

AchievementSystem::AchievementSystem() : achievementProgress(DefaultAchievementData){
	load();
}

bool AchievementSystem::isUnlocked(Achievement achi){
	const auto& data = achievementProgress[achi];
	return data.goal != 0 && data.progress >= data.goal;
}

void AchievementSystem::getAll(std::vector<AchievementData>& unlockedList) const{
	unlockedList.clear();
	unlockedList.reserve((size_t) Achievement::COUNT);

	for(size_t i = 0; i < (size_t) Achievement::COUNT; ++i){
		const auto& achi = achievementProgress.at((Achievement) i);
		if(!achi.unlocked()) continue;
		unlockedList.emplace_back(achi);
	}

	for(size_t i = 0; i < (size_t) Achievement::COUNT; ++i){
		const auto& achi = achievementProgress.at((Achievement) i);
		if(achi.unlocked()) continue;
		unlockedList.emplace_back(achi);
	}
}

const AchievementData& AchievementSystem::get(Achievement ID){
	return achievementProgress[ID];
}

void AchievementSystem::reset(Achievement ID){
	if(!inSession){
		return;
	}

	// allow reset if achievement was won this session
	if(previousState[ID].progress >= achievementProgress[ID].goal){
		return;
	}

	achievementProgress[ID].progress = 0;
}

void AchievementSystem::increment(Achievement ID, int32_t increment){
	if(!inSession){
		return;
	}

	achievementProgress[ID].progress = std::min(achievementProgress[ID].goal, achievementProgress[ID].progress + increment);
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
		if(!previousState[(Achievement) i].unlocked() && achievementProgress[(Achievement)  i].unlocked()){
			unlockedList.emplace_back(achievementProgress[(Achievement)  i]);
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
		achievementProgress = DefaultAchievementData;
		store();
		return true;
	}

	for(size_t i = 0; i < (size_t) Achievement::COUNT; ++i){
		achievementProgress[(Achievement)  i].progress = data[i];
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
		data[i] = achievementProgress[(Achievement) i].progress;
	}

	nvs->set(Blob, data);
}
