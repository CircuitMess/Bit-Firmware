#ifndef BIT_FIRMWARE_GAMEINFOSCREEN_H
#define BIT_FIRMWARE_GAMEINFOSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVStyle.h"
#include "Util/Events.h"
#include "Screens/Settings/BoolElement.h"
#include "Screens/Settings/SliderElement.h"
#include "Services/GameManager.h"
#include "BatteryElement.h"

enum class InfoType : uint8_t {
	HighScore,
	Instructions
};

class GameInfoScreen : public LVScreen {
public:
	GameInfoScreen(Games current, InfoType type);

private:
	EventQueue evts;
	Games currentGame;
	InfoType type;

	BatteryElement* batt;

	LVStyle itemStyle;
	LVStyle focusStyle;

	void buildUI();

	void onStart() override;
	void onStop() override;
	void loop() override;

	void exit();
};

#endif //BIT_FIRMWARE_GAMEINFOSCREEN_H