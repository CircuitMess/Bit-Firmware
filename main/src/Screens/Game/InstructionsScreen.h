#ifndef BIT_FIRMWARE_INSTRUCTIONSSCREEN_H
#define BIT_FIRMWARE_INSTRUCTIONSSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVStyle.h"
#include "Util/Events.h"
#include "Screens/Settings/BoolElement.h"
#include "Screens/Settings/SliderElement.h"
#include "Services/RobotManager.h"
#include "Screens/BatteryElement.h"
#include "Screens/InstructionsElement.h"

class InstructionsScreen : public LVScreen {
public:
	explicit InstructionsScreen(Games current, bool launch);

private:
	bool ignoreFirstInput = false;
	EventQueue evts;
	Games currentGame;

	std::string gameUIPath = "S:/Splash/";

	InstructionsElement instrElement;

	void onStart() override;
	void onStop() override;
	void loop() override;

	void exit();

	bool launch;
};

#endif //BIT_FIRMWARE_INSTRUCTIONSSCREEN_H