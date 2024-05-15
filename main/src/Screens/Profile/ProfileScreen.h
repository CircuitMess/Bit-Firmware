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
#include "Screens/MainMenu/MenuItem.h"

class ProfileScreen : public LVScreen {
public:
	ProfileScreen();
	virtual ~ProfileScreen();

private:
	void buildUI();

	void onStarting() override;
	void onStart() override;
	void onStop() override;
	static void onScrollEnd(lv_event_t*);
	bool loopBlocked = true;

	EventQueue events;
	void loop() override;

	void handleInput(const Input::Data& evt);

	void launch(Games game);

	static std::atomic<bool> running;

	ChirpSystem* audio;
};


#endif //BIT_FIRMWARE_PROFILESCREEN_H