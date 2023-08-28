#ifndef BIT_FIRMWARE_MAINMENU_H
#define BIT_FIRMWARE_MAINMENU_H

#include "MenuItem.h"
#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVModal.h"
#include "Util/Events.h"
#include "Services/Robots.h"
#include "Services/GameManager.h"
#include "Devices/Input.h"
#include "BatteryElement.h"

class MainMenu : public LVScreen {
public:
	MainMenu();
	virtual ~MainMenu();

private:
	void buildUI();

	BatteryElement* batt;
	std::vector<MenuItem*> items;
	std::unordered_map<Robot, MenuItem*> robGames;
	lv_obj_t* itemCont;
	LVGIF* bg;
	lv_obj_t* padTop = nullptr;

	void onStarting() override;
	void onStart() override;
	void onStop() override;
	static void onScrollEnd(lv_event_t*);
	bool loopBlocked = true;

	static std::string imgUnl(const char* game);
	static std::string imgLoc(const char* game);

	EventQueue events;
	void loop() override;

	void handleInsert(const GameManager::Event& evt);
	void handleInput(const Input::Data& evt);

	void launch(Games game);

};


#endif //BIT_FIRMWARE_MAINMENU_H
