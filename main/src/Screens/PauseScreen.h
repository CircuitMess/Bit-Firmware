#ifndef BIT_FIRMWARE_PAUSESCREEN_H
#define BIT_FIRMWARE_PAUSESCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVStyle.h"
#include "Util/Events.h"
#include "Screens/Settings/BoolElement.h"
#include "Screens/Settings/SliderElement.h"

class PauseScreen : public LVScreen {
public:
	PauseScreen();

private:
	EventQueue evts;

	LVGIF* bg;

	LVStyle itemStyle;
	LVStyle focusStyle;

	BoolElement* audioSwitch;
	SliderElement* blSlider;

	void buildUI();

	void onStart() override;
	void onStop() override;
	void loop() override;

};


#endif //BIT_FIRMWARE_PAUSESCREEN_H
