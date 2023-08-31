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
#include "Games/Asteroids/Asteroids.h"

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
		{ Games::Robby,      [](Sprite& canvas){ return std::make_unique<Asteroids::Asteroids>(canvas); } }
};

GameRunner::GameRunner(Display& display) : display(display){

}

Games GameRunner::getCurrent(){
	return currentGameEnum;
}

void GameRunner::startGame(Games game){
	endGame();

	if(!Launcher.contains(game)) return;

	auto icon = GameIcons[(int) game];
	std::string splash("/spiffs/Splash/"); splash += icon; splash += "_splash.bmp";
	std::string instr("/spiffs/Splash/"); instr += icon; instr += "_instr.bmp";

	auto& canvas = display.getCanvas();
	auto& lgfx = display.getLGFX();

	lgfx.drawBmpFile(splash.c_str());
	const auto startTime = millis();

	auto launcher = Launcher.at(game);

	srand(micros());

	auto inst = launcher(display.getCanvas());

	EventQueue evts(12);
	Events::listen(Facility::Battery, &evts);

	inst->load();
	while(!inst->isLoaded() || (millis() - startTime) < 2000){
		delayMillis(1);
	}
	lgfx.drawBmpFile(instr.c_str());

	Events::listen(Facility::Input, &evts);
	for(;;){
		Event evt{};
		if(!evts.get(evt, portMAX_DELAY)) continue;

		if(evt.facility == Facility::Input){
			auto data = (Input::Data*) evt.data;
			if(data->btn != Input::Menu && data->action == Input::Data::Release){
				free(evt.data);
				break;
			}
		}else if(evt.facility == Facility::Battery){
			auto data = (Battery::Event*) evt.data;
			if(data->action == Battery::Event::LevelChange && data->level == Battery::Critical){
				free(evt.data);
				Events::unlisten(&evts);
				return;
			}
		}
		free(evt.data);
	}
	Events::unlisten(&evts);

	srand(micros());

	currentGameEnum = game;
	currentGame = std::move(inst);
	currentGame->start();
	lastMicros = micros();
}

void GameRunner::endGame(){
	if(!currentGame) return;
	currentGame->stop();
	currentGame.reset();
	currentGameEnum = Games::COUNT;
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
