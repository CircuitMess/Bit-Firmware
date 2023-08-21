#ifndef BIT_FIRMWARE_UITHREAD_H
#define BIT_FIRMWARE_UITHREAD_H

#include "LV_Interface/LVGL.h"
#include "GameEngine/GameRunner.h"
#include "Util/Threaded.h"
#include "GameEngine/Game.h"
#include "Devices/Display.h"

class UIThread : public Threaded {
public:
	UIThread(LVGL& lvgl, GameRunner& gameRunner);
	~UIThread() override;

	void startGame(Games game);
	void startScreen(std::function<std::unique_ptr<LVScreen>()> create);

	void pauseGame();
	void resumeGame();

private:
	LVGL& lvgl;
	GameRunner& gamer;

	enum class Src { LVGL, Game, None } active = Src::None;

	void loop() override;

};


#endif //BIT_FIRMWARE_UITHREAD_H
