#ifndef CLOCKSTAR_FIRMWARE_SETTINGSSCREEN_H
#define CLOCKSTAR_FIRMWARE_SETTINGSSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "Settings/Settings.h"
#include "Util/Events.h"
#include "Services/BacklightBrightness.h"
#include "Services/ChirpSystem.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVStyle.h"

class BoolElement;
class SliderElement;
class DiscreteSliderElement;

class SettingsScreen : public LVScreen {
public:
	SettingsScreen();

private:
	EventQueue evts;
	Settings& settings;

	LVStyle itemStyle;
	LVStyle focusStyle;

	LVGIF* bg = nullptr;
	BoolElement* audioSwitch;
	SliderElement* blSlider;
	DiscreteSliderElement* sleepSlider;

	void buildUI();

	void onStart() override;
	void onStop() override;
	void loop() override;

};


#endif //CLOCKSTAR_FIRMWARE_SETTINGSSCREEN_H
