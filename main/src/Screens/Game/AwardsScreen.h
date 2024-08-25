#ifndef BIT_FIRMWARE_AWARDSSCREEN_H
#define BIT_FIRMWARE_AWARDSSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVStyle.h"
#include "Util/Events.h"
#include "Screens/Settings/BoolElement.h"
#include "Screens/Settings/SliderElement.h"
#include "Services/HighScoreManager.h"
#include "Screens/BatteryElement.h"

class AwardsScreen : public LVScreen {
public:
	AwardsScreen(Games current, uint32_t highScore, uint32_t xp, std::vector<AchievementData>& achievements);

private:
	enum class Award : uint8_t {
		None,
		XP,
		LevelUp,
		Achievement,
		HighScore
	};

	uint32_t highScore;
	uint32_t xp;
	std::vector<AchievementData> achievements;

	EventQueue evts;
	Games currentGame;

	Award awardMode = Award::None;

	char characters[3] {'A', 'A', 'A'};

	lv_obj_t* name[3];
	lv_obj_t* item;
	lv_obj_t* rest;
	lv_obj_t* value;

	class XPBar* xpBar = nullptr;

	LVStyle itemStyle;

	uint8_t activeIndex = 0;
	int8_t labelChanged = 0;
	uint64_t lastChanged = 0;

	uint64_t start;
	uint64_t lastChange = 0;
	uint64_t blinkTime = 500;
	bool chirped = false;
	uint32_t levelSet = 0;

	inline static constexpr const uint32_t AnimLength = 500;

	static constexpr char getChar(char c, int8_t direction) {
		if(c < ' '){
			return 'z';
		}

		if(c == ' ' || c > 'z'){
			return ' ';
		}

		if(c < '0'){
			if(direction > 0){
				return '0';
			}else if(direction < 0){
				return ' ';
			}
		}

		if(c > '9' && c < 'A'){
			if(direction > 0){
				return 'A';
			}else if(direction < 0){
				return '9';
			}
		}

		if(c > 'Z' && c < 'a'){
			if(direction > 0){
				return 'a';
			}else if(direction < 0){
				return 'Z';
			}
		}

		return c;
	}

	void setAwardMode(Award award);

	void onStart() override;
	void onStop() override;
	void loop() override;

	void exit();
};

#endif //BIT_FIRMWARE_AWARDSSCREEN_H