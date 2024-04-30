#ifndef BIT_FIRMWARE_AWARDSSCREEN_H
#define BIT_FIRMWARE_AWARDSSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVStyle.h"
#include "Util/Events.h"
#include "Screens/Settings/BoolElement.h"
#include "Screens/Settings/SliderElement.h"
#include "Services/GameManager.h"
#include "BatteryElement.h"

class AwardsScreen : public LVScreen {
public:
	AwardsScreen(Games current, std::array<HighScore, 5> highScores);

private:
	std::array<HighScore, 5> highScores;
	EventQueue evts;
	Games currentGame;

	lv_obj_t* name[3];
	lv_obj_t* item;
	lv_obj_t* rest;
	lv_obj_t* value;

	BatteryElement* batt;

	LVStyle itemStyle;

	uint8_t activeIndex = 0;
	int8_t labelChanged = 0;
	uint64_t lastChanged = 0;

	uint64_t start;
	uint64_t blinkTime = 500;

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

	void buildUI();

	void onStart() override;
	void onStop() override;
	void loop() override;

	void exit();
};

#endif //BIT_FIRMWARE_AWARDSSCREEN_H