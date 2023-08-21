#include "UIThread.h"
#include "Util/stdafx.h"
#include "GameEngine/Game.h"
#include "Devices/Display.h"

UIThread::UIThread(LVGL& lvgl, GameRunner& gameRunner) : Threaded("UI", 4 * 1024, 5, 1), lvgl(lvgl), gamer(gameRunner){
	start();
}

UIThread::~UIThread(){
	stop();
}

void UIThread::loop(){
	if(active == Src::LVGL){
		lvgl.loop();
	}else if(active == Src::Game){
		gamer.loop();
	}else{
		delayMillis(100);
		return;
	}
}

void UIThread::startGame(Games game){
	lvgl.stopScreen();
	gamer.startGame(game);
	active = Src::Game;
}

void UIThread::startScreen(std::function<std::unique_ptr<LVScreen>()> create){
	gamer.endGame();
	lvgl.startScreen(std::move(create));
	active = Src::LVGL;
}
