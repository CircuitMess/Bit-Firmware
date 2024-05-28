#ifndef BIT_FIRMWARE_PROFILESCREEN_H
#define BIT_FIRMWARE_PROFILESCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVModal.h"
#include "Util/Events.h"
#include "Services/Robots.h"
#include "Devices/Input.h"
#include <optional>
#include "Services/ChirpSystem.h"
#include "Services/AchievementSystem.h"
#include "Settings/Settings.h"
#include "LV_Interface/LVStyle.h"
#include "CharacterPicker.h"
#include "AchievementView.h"
#include "ThemePicker.h"

class ProfileScreen : public LVScreen {
public:
	ProfileScreen();
	virtual ~ProfileScreen() override;

private:
	void setupThemes();
	void buildUI();

	void onStart() override;
	void onStop() override;

	void loop() override;

	void handleInput(const Input::Data& evt);

	EventQueue events;
	ChirpSystem* audio;

	LVStyle unfocusedSection;
	LVStyle focusedSection;

	AchievementView achievementSection = AchievementView(*this);
	ThemePicker themeSection = ThemePicker(*this);
	CharacterPicker characterSection = CharacterPicker(*this);

	lv_obj_t* achievementOverlay; //used for focus indication

	class XPBar* xpBar = nullptr;
};

#endif //BIT_FIRMWARE_PROFILESCREEN_H