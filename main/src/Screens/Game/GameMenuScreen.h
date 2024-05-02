#ifndef BIT_FIRMWARE_GAMEMENUSCREEN_H
#define BIT_FIRMWARE_GAMEMENUSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVStyle.h"
#include "Util/Events.h"
#include "Screens/Settings/BoolElement.h"
#include "Screens/Settings/SliderElement.h"
#include "Services/GameManager.h"
#include "Screens/BatteryElement.h"

class GameMenuScreen : public LVScreen {
public:
	explicit GameMenuScreen(Games current);

private:
	EventQueue evts;
	Games currentGame;

	BatteryElement* batt;

	LVStyle itemStyle;
	LVStyle focusStyle;

	void buildUI();

	void onStart() override;
	void onStop() override;
	void loop() override;

	void exit();
};

#endif //BIT_FIRMWARE_GAMEMENUSCREEN_H