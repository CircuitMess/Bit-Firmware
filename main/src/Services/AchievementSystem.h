#ifndef BIT_FIRMWARE_ACHIEVEMENTSYSTEM_H
#define BIT_FIRMWARE_ACHIEVEMENTSYSTEM_H

#include <cstdint>
#include <vector>
#include <cstdint>
#include <array>
#include <unordered_map>

enum class Achievement : uint32_t {
	Artemis_friendly,
	Artemis_sharp,
	Bee_b,
	Bee_s,
	Bee_g,
	Blocks_clear,
	Blocks_b,
	Blocks_s,
	Blocks_g,
	Bob_bombs,
	Bonk_5,
	Bonk_comeback,
	Buttons_win,
	Capacitron_b,
	Capacitron_s,
	Capacitron_g,
	Capacitron_fire,
	Dusty_rat,
	Hertz_3,
	Marv_5,
	Marv_life,
	Planck_b,
	Planck_g,
	Planck_juice,
	Planck_s,
	Resistron_b,
	Resistron_s,
	Resistron_g,
	Resistron_deflect,
	Robby_asteroid,
	Robby_sharp,
	Snake_b,
	Snake_s,
	Snake_g,
	Snake_fill,
	Sparkly_nobrake,
	Sparkly_road,
	Stacky_5,
	Stacky_highrise,
	Stacky_skyscraper,
	Stacky_super,
	// Bee_crow,
	// Blocks_4lines,
	// Bob_green,
	// Bob_yellow,
	// Buttons_triple,
	// Charlie_nomercy,
	// Charlie_yesmercy,
	// Dusty_shovel,
	// Fred_copper,
	// Fred_detective,
	// Fred_veteran,
	// Marv_newspapers,
	// Planck_chauffeur,
	// Planck_nobrake,
	// Sparkly_overtake,
	COUNT
};

struct AchievementText {
	const char* Title;
	const char* Description;
};

extern const std::unordered_map<Achievement, AchievementText> AchievementTextData;

struct AchievementData {
	inline constexpr bool unlocked() const{
		return goal != 0 && progress >= goal;
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

	bool isUnlocked(Achievement achi);
	void getAll(std::vector<AchievementData>& unlockedList) const;
	const AchievementData& get(Achievement ID);
	void reset(Achievement ID);
	void increment(Achievement ID, int32_t increment);
	void startSession();
	void endSession(std::vector<AchievementData>& unlockedList);

private:
	std::unordered_map<Achievement, AchievementData> achievementProgress; //global achievement progress
	std::unordered_map<Achievement, AchievementData> previousState; //achivement state before a new session is started
	bool inSession = false;
	inline static constexpr const char* Blob = "Achievements";

private:
	bool load();
	void store();
};

#endif //BIT_FIRMWARE_ACHIEVEMENTSYSTEM_H