#ifndef BIT_FIRMWARE_GAMESPLASHSCREEN_H
#define BIT_FIRMWARE_GAMESPLASHSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVStyle.h"
#include "Util/Events.h"
#include "Screens/Settings/BoolElement.h"
#include "Screens/Settings/SliderElement.h"
#include "Services/RobotManager.h"
#include "Screens/BatteryElement.h"

class GameSplashScreen : public LVScreen {
public:
	explicit GameSplashScreen(Games current);

private:
	Games currentGame;

	std::string gameUIPath = "S:/Splash/";

	void buildUI();
	void loop() override;
};

#endif //BIT_FIRMWARE_GAMESPLASHSCREEN_H