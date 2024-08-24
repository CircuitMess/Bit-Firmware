#ifndef BIT_FIRMWARE_MAINMENU_H
#define BIT_FIRMWARE_MAINMENU_H

#include "MenuItem.h"
#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVModal.h"
#include "Util/Events.h"
#include "Services/Robots.h"
#include "Services/RobotManager.h"
#include "Devices/Input.h"
#include "Screens/BatteryElement.h"
#include <optional>
#include "Services/ChirpSystem.h"

class MainMenu : public LVScreen {
public:
	MainMenu(bool delayed = false);
	virtual ~MainMenu();

	static void gameEvent(RobotManager::Event evt);

private:
	void buildUI();
	static constexpr uint8_t RowWidth = 3;

	BatteryElement* batt;
	class MenuHeader* menuHeader;
	std::vector<MenuItem*> items;
	std::unordered_map<uint8_t , MenuItem*> robGames;
	lv_obj_t* itemCont;
	lv_obj_t* padTop = nullptr;

	void onStarting() override;
	void onStart() override;
	void onStop() override;
	static void onScrollEnd(lv_event_t*);
	bool loopBlocked = true;
	bool delayed = false;
	uint32_t startTime;

	static std::string imgFullPath(const char* game);
	static std::string imgGrayscalePath(const char* game);

	EventQueue events;
	void loop() override;

	void handleGameInsert(const RobotManager::Event& evt);
	void handleThemeInsert(const RobotManager::Event& evt);
	void handlePetInsert(const RobotManager::Event& evt);
	void handleInput(const Input::Data& evt);

	void launch(Games game);

	static std::optional<RobotManager::Event> gmEvt;
	static std::atomic<bool> running;

	ChirpSystem* audio;

	static uint8_t lastCursor;
};

#endif //BIT_FIRMWARE_MAINMENU_H
