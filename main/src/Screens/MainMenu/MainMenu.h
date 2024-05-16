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
#include "Screens/BatteryElement.h"
#include <optional>
#include "Services/ChirpSystem.h"

class MainMenu : public LVScreen {
public:
	MainMenu();
	virtual ~MainMenu();

	static void gameEvent(GameManager::Event evt);

private:
	void buildUI();
	static constexpr uint8_t RowWidth = 3;

	BatteryElement* batt;
	class MenuHeader* menuHeader;
	std::vector<MenuItem*> items;
	std::unordered_map<Robot, MenuItem*> robGames;
	lv_obj_t* itemCont;
	LVGIF* bg = nullptr;
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

	static std::optional<GameManager::Event> gmEvt;
	static std::atomic<bool> running;

	ChirpSystem* audio;
};


#endif //BIT_FIRMWARE_MAINMENU_H
