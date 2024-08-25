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
#include "Services/MelodyPlayer.h"

class GameSplashScreen : public LVScreen {
public:
	explicit GameSplashScreen(Games current);

private:
	Games currentGame;

	std::string gameUIPath = "S:/Splash/";

	void buildUI();
	void loop() override;
	void onStart() override;
	void onStop() override;

	uint32_t startTime;
	static constexpr uint32_t HoldTime = 2000;

	std::unique_ptr<MelodyPlayer> melody;

};

#endif //BIT_FIRMWARE_GAMESPLASHSCREEN_H