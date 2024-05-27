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

	static void onClick(lv_event_t * e);

	struct Achievement {
		::Achievement achievement = ::Achievement::COUNT;
		lv_obj_t* icon;
	};

	struct Theme {
		::Theme theme;
		lv_obj_t* icon;
	};

	EventQueue events;
	ChirpSystem* audio;

	LVStyle unfocusedSection;
	LVStyle focusedSection;

	lv_obj_t* achievementSection;
	lv_obj_t* themeSection;
	CharacterPicker characterSection;

	lv_group_t* themeGroup;
	lv_obj_t* achievementOverlay; //used for focus indication

	bool pickingCharacter = false;
	bool pickingTheme = false;

	uint8_t focusedIndex = 6;

	inline static constexpr const uint8_t AchievementColumns = 4;
	inline static constexpr const uint8_t ThemeColumns = 2;
	inline static constexpr const uint8_t CharacterInputIndex = 0;
	inline static constexpr const uint8_t ThemeInputIndex = 1;
	inline static constexpr const uint8_t AchievementInputIndex = 6;
	inline static constexpr const uint8_t AchievementCount = 32; // TODO

	class XPBar* xpBar = nullptr;
};

#endif //BIT_FIRMWARE_PROFILESCREEN_H