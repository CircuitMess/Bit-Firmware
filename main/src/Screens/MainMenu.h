#ifndef BIT_FIRMWARE_MAINMENU_H
#define BIT_FIRMWARE_MAINMENU_H

#include "MenuItem.h"
#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVModal.h"
#include "Util/Events.h"
#include "Services/Robots.h"
#include "Services/GameManager.h"

class MainMenu : public LVScreen {
public:
	MainMenu();
	virtual ~MainMenu();

	static void loadCache();

private:
	void buildUI();

	std::vector<MenuItem*> items;
	std::unordered_map<Robot, MenuItem*> robGames;
	lv_obj_t* itemCont;
	LVGIF* bg;
	lv_obj_t* padTop = nullptr;

	void onStarting() override;
	void onStart() override;
	void onStop() override;

	static constexpr const char* BgImgs[] = { "/bg/0.bin", "/bg/1.bin", "/bg/2.bin", "/bg/3.bin", };
	static void unloadCache();

	static std::string imgUnl(const char* game);
	static std::string imgLoc(const char* game);

	EventQueue events;
	void loop() override;

	void handleInsert(const GameManager::Event& evt);

	void launch(Games game);

	std::unique_ptr<LVModal> modal;

};


#endif //BIT_FIRMWARE_MAINMENU_H
