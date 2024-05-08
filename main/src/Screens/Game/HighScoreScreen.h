#ifndef BIT_FIRMWARE_HIGHSCORESCREEN_H
#define BIT_FIRMWARE_HIGHSCORESCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVStyle.h"
#include "Util/Events.h"
#include "Screens/Settings/BoolElement.h"
#include "Screens/Settings/SliderElement.h"
#include "Services/GameManager.h"
#include "Screens/BatteryElement.h"

class HighScoreScreen : public LVScreen {
public:
	explicit HighScoreScreen(Games current);

private:
	EventQueue evts;
	Games currentGame;

	BatteryElement* batt;

	LVStyle itemStyle;

	void buildUI();

	void onStart() override;
	void onStop() override;
	void loop() override;

	void exit();
};

#endif //BIT_FIRMWARE_HIGHSCORESCREEN_H