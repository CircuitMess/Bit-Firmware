#ifndef BIT_FIRMWARE_ACHIEVEMENTSYSTEM_H
#define BIT_FIRMWARE_ACHIEVEMENTSYSTEM_H

#include <cstdint>
#include <vector>
#include <cstdint>
#include <array>

// TODO needs to be filled out
enum class Achievement : uint32_t {
	COUNT
};

struct AchievementData {
public:
	inline AchievementData(Achievement achievement = (Achievement) 0, uint32_t goal = 0, uint32_t progress = 0) : achievementID(achievement), goal(goal), progress(progress) {}

	inline constexpr bool unlocked() const{
		return goal != 0 && progress == goal;
	}

	inline constexpr operator bool() const{
		return unlocked();
	}

public:
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