#include "GameRunner.h"
#include "Util/stdafx.h"
#include "Devices/Battery.h"
#include "Games/Flappy/Flappy.h"
#include "Games/Pong/Pong.h"
#include "Games/Snake/Snake.h"
#include "Games/Blocks/Blocks.h"
#include "Games/MarvGame/MarvGame.h"
#include "Games/Hertz/HertzGame.h"
#include "Games/BobGame/BobGame.h"
#include "Games/CapacitronGame/CapacitronGame.h"
#include "Games/Invaders/Invaders.h"
#include "Games/Dance/Dance.h"
#include "Games/Asteroids/Asteroids.h"
#include "Games/Artemis/Game.h"
#include "Games/Sparkly/Sparkly.h"
#include "Games/Harald/Harald.h"
#include "Games/Planck/Planck.h"
#include "Games/WackyStacky/WackyStacky.h"
#include "Games/Charlie/CharlieGame.h"
#include "Games/Dusty/DustyGame.h"

static const std::unordered_map<Games, std::function<std::unique_ptr<Game>(Sprite& canvas)>> Launcher{
		{ Games::MrBee,      [](Sprite& canvas){ return std::make_unique<Flappy>(canvas); } },
		{ Games::Pong,       [](Sprite& canvas){ return std::make_unique<Pong>(canvas); } },
		{ Games::Snake,      [](Sprite& canvas){ return std::make_unique<Snake>(canvas); } },
		{ Games::Blocks,     [](Sprite& canvas){ return std::make_unique<Blocks>(canvas); } },
		{ Games::Marv,       [](Sprite& canvas){ return std::make_unique<MarvGame::MarvGame>(canvas); } },
		{ Games::Hertz,      [](Sprite& canvas){ return std::make_unique<HertzGame>(canvas); } },
		{ Games::Bob,        [](Sprite& canvas){ return std::make_unique<BobGame::BobGame>(canvas); } },
		{ Games::Capacitron, [](Sprite& canvas){ return std::make_unique<CapacitronGame::CapacitronGame>(canvas); } },
		{ Games::Resistron,  [](Sprite& canvas){ return std::make_unique<Invaders::Invaders>(canvas); } },
		{ Games::Buttons,	 [](Sprite& canvas){ return std::make_unique<Dance>(canvas); } },
		{ Games::Robby,      [](Sprite& canvas){ return std::make_unique<Asteroids::Asteroids>(canvas); } },
		{ Games::Artemis,    [](Sprite& canvas){ return std::make_unique<ArtemisGame::PewPew>(canvas); } },
		{ Games::Harald,    [](Sprite& canvas){ return std::make_unique<Harald::Harald>(canvas); } },
		{ Games::Planck,    [](Sprite& canvas){ return std::make_unique<Planck::Planck>(canvas); } },
		{ Games::WackyStacky,   [](Sprite& canvas){ return std::make_unique<WackyStacky::WackyStacky>(canvas); } },
		{ Games::Sparkly,	 [](Sprite& canvas){ return std::make_unique<Sparkly::Sparkly>(canvas); } },
		{ Games::Charlie,   [](Sprite& canvas){ return std::make_unique<CharlieGame::CharlieGame>(canvas); } },
		{ Games::Dusty,   [](Sprite& canvas){ return std::make_unique<DustyGame::DustyGame>(canvas); } }
};

GameRunner::GameRunner(Display& display, Allocator* alloc) : display(display), alloc(alloc){

}

Games GameRunner::getCurrent(){
	return currentGameEnum;
}

void GameRunner::startGame(Games game){
	endGame();

	if(!Launcher.contains(game)) return;

	const auto startTime = millis();

	auto launcher = Launcher.at(game);

	auto inst = launcher(display.getCanvas());

	inst->load(alloc);
	while(!inst->isLoaded() || (millis() - startTime) < 2000){
		delayMillis(100);
	}

	currentGameEnum = game;
	currentGame = std::move(inst);
	currentGame->start();
	lastMicros = micros();
}

void GameRunner::endGame(){
	if(!currentGame) return;
	currentGame->stop();
	currentGame.reset();
	if(alloc){
		alloc->reset();
	}
	currentGameEnum = Games::COUNT;
}

void GameRunner::exitGame(){
	if(!currentGame){
		return;
	}

	currentGame->exit();
}

void GameRunner::resume(){
	lastMicros = micros();
	currentGame->start();
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
