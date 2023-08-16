#include "GameRunner.h"
#include "Util/stdafx.h"

GameRunner::GameRunner(Display& display) : display(display){

}

void GameRunner::startGame(std::function<std::unique_ptr<Game>(Sprite&)> launcher){
	endGame();

	auto game = launcher(display.getCanvas());
	game->load();
	while(!game->isLoaded()){
		delayMillis(1);
	}

	currentGame = std::move(game);
	currentGame->start();
	lastMicros = micros();
}

void GameRunner::endGame(){
	if(!currentGame) return;
	currentGame->stop();
	currentGame.reset();
}

void GameRunner::resume(){
	lastMicros = micros();
}

void GameRunner::loop(){
	if(!currentGame){
		delayMillis(FrameTime);
		return;
	}

	const auto currMicros = esp_timer_get_time();
	const auto delta = currMicros - lastMicros;
	lastMicros = currMicros;

	currentGame->loop(delta);
	if(!currentGame) return; // in case the game exited in its loop

	display.getCanvas().pushSprite(0, 0);
	display.getLGFX().display();

	const auto loopTime = (micros() - currMicros) / 1000;
	if(loopTime > FrameTime){
		delayMillis(1);
	}else{
		delayMillis(FrameTime - loopTime);
	}
}
