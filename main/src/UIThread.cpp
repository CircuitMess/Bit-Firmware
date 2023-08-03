#include "UIThread.h"

UIThread::UIThread(Display& display) : Threaded("UIThread", 4 * 1024, 6, 1), display(display){
	start();
}

UIThread::~UIThread(){
	stop();
}

void UIThread::loop(){
	if(!game){
		vTaskDelay(1);
		return;
	}

	auto curr = esp_timer_get_time();
	auto delta = curr - loopTime;
	loopTime = curr;

	game->loop(delta);
	display.getCanvas().pushSprite(0, 0);
	display.getLGFX().display();

	vTaskDelay(1);
}

void UIThread::startGame(std::unique_ptr<Game> game){
	this->game = std::move(game);
	//TODO - rework game loading and starting
	this->game->load();
	while(!this->game->isLoaded()){
		vTaskDelay(1);
	}

	this->game->start();
}
