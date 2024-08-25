#ifndef BIT_FIRMWARE_PAUSESCREEN_H
#define BIT_FIRMWARE_PAUSESCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVStyle.h"
#include "Util/Events.h"
#include "Screens/Settings/BoolElement.h"
#include "Screens/Settings/SliderElement.h"
#include "Services/RobotManager.h"
#include "BatteryElement.h"

class PauseScreen : public LVScreen {
public:
	PauseScreen(Games current);

private:
	enum class State {
		Pause, Controls, IgnoreInput
	} state = State::Pause;
	EventQueue evts;
	Games currentGame;

	LVStyle itemStyle;
	LVStyle focusStyle;

	BoolElement* audioSwitch;
	SliderElement* blSlider;

	void buildUI();

	void onStart() override;
	void onStop() override;
	void loop() override;

	void showControls();
	void exitControls();
	void exit();
};

#endif //BIT_FIRMWARE_PAUSESCREEN_H
