#ifndef BIT_FIRMWARE_UITHREAD_H
#define BIT_FIRMWARE_UITHREAD_H

#include "Util/Threaded.h"
#include "GameEngine/Game.h"
#include "Devices/Display.h"

class UIThread : private Threaded {
public:
	UIThread(Display& display);
	~UIThread() override;

	void startGame(std::unique_ptr<Game> game);

private:
	void loop() override;

	Display& display;
	std::unique_ptr<Game> game;

	int64_t loopTime = 0;
};


#endif //BIT_FIRMWARE_UITHREAD_H
