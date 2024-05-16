#ifndef BIT_FIRMWARE_PROFILESCREEN_H
#define BIT_FIRMWARE_PROFILESCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVModal.h"
#include "Util/Events.h"
#include "Services/Robots.h"
#include "Services/GameManager.h"
#include "Devices/Input.h"
#include <optional>
#include "Services/ChirpSystem.h"

class ProfileScreen : public LVScreen {
public:
	ProfileScreen();
	virtual ~ProfileScreen() override;

private:
	void buildUI();

	void onStarting() override;
	void onStart() override;
	void onStop() override;

	EventQueue events;
	void loop() override;

	void handleInput(const Input::Data& evt);

	void launch(Games game);

	ChirpSystem* audio;

	lv_obj_t* achievementSection;
	lv_obj_t* themeSection;
	lv_obj_t* characterSection;

	class XPBar* xpBar = nullptr;
};

#endif //BIT_FIRMWARE_PROFILESCREEN_H