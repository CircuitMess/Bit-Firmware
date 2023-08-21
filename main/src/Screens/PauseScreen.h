#ifndef BIT_FIRMWARE_PAUSESCREEN_H
#define BIT_FIRMWARE_PAUSESCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"
#include "LV_Interface/LVStyle.h"

class PauseScreen : public LVScreen {
public:
	PauseScreen();

private:

	LVGIF* bg;

	LVStyle itemStyle;
	LVStyle focusStyle;

	void buildUI();

	void onStart() override;
	void onStop() override;

};


#endif //BIT_FIRMWARE_PAUSESCREEN_H
