#ifndef BIT_FIRMWARE_MAINMENU_H
#define BIT_FIRMWARE_MAINMENU_H

#include "LV_Interface/LVScreen.h"
#include "MenuItem.h"

class MainMenu : public LVScreen {
public:
	MainMenu();
	virtual ~MainMenu();

private:
	void buildUI();

	std::vector<MenuItem*> items;

	static constexpr const char* BgImgs[] = { "/bg/0.bin", "/bg/1.bin", "/bg/2.bin", "/bg/3.bin", };
	void loadCache();
	void unloadCache();

};


#endif //BIT_FIRMWARE_MAINMENU_H