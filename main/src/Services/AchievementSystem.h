#ifndef BIT_FIRMWARE_ACHIEVEMENTSYSTEM_H
#define BIT_FIRMWARE_ACHIEVEMENTSYSTEM_H

#include <cstdint>
#include <vector>
#include <cstdint>
#include <array>

// TODO needs to be filled out
enum class Achievement : uint32_t {
	Artemis_friendly,
	Artemis_sharp,
	Bee_b,
	Bee_crow,
	Bee_g,
	Bee_s,
	Blocks_4lines,
	Blocks_b,
	Blocks_clear,
	Blocks_g,
	Blocks_s,
	Bob_bombs,
	Bob_green,
	Bob_yellow,
	Bonk_5,
	Bonk_comeback,
	Buttons_triple,
	Buttons_win,
	Capacitron_b,
	Capacitron_fire,
	Capacitron_g,
	Capacitron_s,
	Charlie_nomercy,
	Charlie_yesmercy,
	Dusty_shovel,
	Dusty_rat,
	Fred_copper,
	Fred_detective,
	Fred_veteran,
	Hertz_3,
	Marv_5,
	Marv_life,
	Marv_newspapers,
	Planck_b,
	Planck_chauffeur,
	Planck_g,
	Planck_juice,
	Planck_nobrake,
	Planck_s,
	Resistron_b,
	Resistron_deflect,
	Resistron_g,
	Resistron_s,
	Robby_asteroid,
	Robby_sharp,
	Snake_b,
	Snake_fill,
	Snake_g,
	Snake_s,
	Sparkly_nobrake,
	Sparkly_overtake,
	Sparkly_road,
	Stacky_5,
	Stacky_highrise,
	Stacky_skyscraper,
	Stacky_super,
	COUNT
};

struct AchievementData {
	inline constexpr bool unlocked() const{
		return goal != 0 && progress == goal;
	}

	inline constexpr operator bool() const{
		return unlocked();
	}

	Achievement achievementID;
	uint32_t goal;
	uint32_t progress;
};

class AchievementSystem {
public:
	AchievementSystem();
	virtual ~AchievementSystem() = default;

	void getAll(std::vector<AchievementData>& unlockedList) const;
	void reset(Achievement ID);
	void increment(Achievement ID, int32_t increment);
	void startSession();
	void endSession(std::vector<AchievementData>& unlockedList);

private:
	std::vector<AchievementData> achievementProgress; //global achievement progress
	std::vector<AchievementData> previousState; //achivement state before a new session is started
	bool inSession = false;
	inline static constexpr const char* Blob = "Achievements";

private:
	bool load();
	void store();
};

#endif //BIT_FIRMWARE_ACHIEVEMENTSYSTEM_H