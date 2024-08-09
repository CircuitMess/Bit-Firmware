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
#include "Games/Harald/Harald.h"
#include "Games/Planck/Planck.h"
#include "Games/WackyStacky/WackyStacky.h"
#include "Services/MelodyPlayer.h"
#include "Util/Notes.h"

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
		{ Games::Buttons,  [](Sprite& canvas){ return std::make_unique<Dance>(canvas); } },
		{ Games::Robby,      [](Sprite& canvas){ return std::make_unique<Asteroids::Asteroids>(canvas); } },
		{ Games::Artemis,    [](Sprite& canvas){ return std::make_unique<ArtemisGame::PewPew>(canvas); } },
		{ Games::Harald,    [](Sprite& canvas){ return std::make_unique<Harald::Harald>(canvas); } },
		{ Games::Planck,    [](Sprite& canvas){ return std::make_unique<Planck::Planck>(canvas); } },
		{ Games::WackyStacky,   [](Sprite& canvas){ return std::make_unique<WackyStacky::WackyStacky>(canvas); } }
};

extern const std::unordered_map<Games, std::function<MelodyPlayer*()>> IntroSounds;

GameRunner::GameRunner(Display& display) : display(display){

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

	EventQueue evts(12);
	Events::listen(Facility::Battery, &evts);

	MelodyPlayer* melody = nullptr;
	if(IntroSounds.contains(game)){
		melody = IntroSounds.at(game)();
		if(melody){
			melody->play();
		}
	}

	inst->load();
	while(!inst->isLoaded() || (melody && melody->isPlaying()) || (millis() - startTime) < 2000){
		delayMillis(100);
	}
	delete melody;

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

const std::unordered_map<Games, std::function<MelodyPlayer*()>> IntroSounds = {
		{ Games::Blocks, [](){
			return new MelodyPlayer(130, {
					Tone { NOTE_B3, 2 },
					Tone { 0, 2 },
					Tone { NOTE_F3, 2 },
					Tone { NOTE_G3, 2 },
					Tone { NOTE_A3, 2 },
					Tone { 0, 2 },
					Tone { NOTE_G3, 2 },
					Tone { NOTE_FS3, 2 },
					Tone { NOTE_E3, 2 }
			});
		} },
		{ Games::Pong, [](){
			return new MelodyPlayer(130, {
					Tone { NOTE_C4, 2 },
					Tone { NOTE_E4, 1 },
					Tone { 0, 1 },
					Tone { NOTE_G4, 1 },
					Tone { 0, 1 },
					Tone { NOTE_E4, 1 },
					Tone { 0, 1 },
					Tone { NOTE_D4, 1 },
					Tone { 0, 1 },
					Tone { NOTE_E4, 1 }
			});
		} },
		{ Games::Snake, [](){
			return new MelodyPlayer(130, {
					Tone { NOTE_FS4, 1 },
					Tone { NOTE_D4, 1 },
					Tone { NOTE_G4, 4 },
					Tone { 0, 1 },
					Tone { NOTE_F4, 1 },
					Tone { NOTE_A4, 1 },
					Tone { NOTE_G4, 1 },
					Tone { NOTE_AS4, 4 },
					Tone { 0, 1 },
					Tone { NOTE_GS4, 1 },
					Tone { NOTE_FS4, 1 },
					Tone { NOTE_D4, 1 },
					Tone { NOTE_G4, 4 },
			});
		} },
		{ Games::MrBee, [](){
				return new MelodyPlayer(130, {
						Tone { NOTE_C5, 1 },
						Tone { NOTE_F4, 1 },
						Tone { NOTE_C4, 1 },
						Tone { NOTE_F4, 1 },
						Tone { NOTE_C5, 1 },
						Tone { NOTE_F4, 1 },
						Tone { NOTE_C4, 1 },
						Tone { NOTE_E4, 1 },
						Tone { NOTE_C5, 1 },
				});
		} },
		{ Games::Bob, [](){
			return new MelodyPlayer(130, {
					Tone { NOTE_C4, 2 },
					Tone { NOTE_F4, 2 },
					Tone { 0, 4 },
					Tone { NOTE_D4, 2 },
					Tone { NOTE_A4, 2 },
					Tone { 0, 2 },
					Tone { NOTE_C5, 2 },
					Tone { NOTE_A4, 2 },
					Tone { NOTE_F4, 2 },
			});
		} },
		{ Games::Capacitron, [](){
			return new MelodyPlayer(130, {
					Tone { NOTE_E4, 6 },
					Tone { 0, 1 },
					Tone { NOTE_B4, 1 },
					Tone { 0, 1 },
					Tone { NOTE_G4, 6 },
					Tone { 0, 1 },
					Tone { NOTE_A4, 1 },
					Tone { 0, 1 },
					Tone { NOTE_E4, 6 },
			});
		} },
		{ Games::Hertz, [](){
			return new MelodyPlayer(130, {
					Tone { NOTE_C5, 1 },
					Tone { NOTE_F5, 1 },
					Tone { 0, 1 },
					Tone { NOTE_FS4, 1 },
					Tone { NOTE_G4, 1 },
					Tone { NOTE_E4, 1 },
					Tone { 0, 1 },
					Tone { NOTE_E5, 1 },
					Tone { NOTE_B4, 1 },
					Tone { 0, 2 },
					Tone { NOTE_F5, 1 },
					Tone { 0, 1 },
					Tone { NOTE_C6, 1 },
			});
		} },
		{ Games::Marv, [](){
			return new MelodyPlayer(130, {
					Tone { NOTE_FS3, 6 },
					Tone { 0, 1.5 },
					Tone { NOTE_E4, 6 },
					Tone { 0, 1.5 },
					Tone { NOTE_FS4, 6 },
			});
		} },
		{ Games::Resistron, [](){
			return new MelodyPlayer(130, {
					Tone { NOTE_FS4, 2 },
					Tone { 0, 1 },
					Tone { NOTE_GS4, 2 },
					Tone { 0, 1 },
					Tone { NOTE_FS4, 2 },
					Tone { 0, 1 },
					Tone { NOTE_E4, 2 },
					Tone { 0, 1 },
					Tone { NOTE_FS4, 2 },
			});
		} },
		{ Games::Buttons, [](){
			return new MelodyPlayer(130, {
					Tone { NOTE_D4, 1 },
					Tone { NOTE_E4, 1 },
					Tone { 0, 1 },
					Tone { NOTE_G4, 1 },
					Tone { 0, 1.5 },
					Tone { NOTE_D4, 0.5 },
					Tone { NOTE_E4, 1 },
					Tone { 0, 1 },
					Tone { NOTE_G4, 1 },
					Tone { 0, 1 },
					Tone { NOTE_B3, 1 },
					Tone { 0, 1 },
					Tone { NOTE_A3, 1 },
					Tone { 0, 1 },
					Tone { NOTE_D4, 1 },
			});
		} },
		{ Games::Artemis, [](){
			return new MelodyPlayer(130, {
					Tone { NOTE_E4, 1 },
					Tone { 0, 1 },
					Tone { NOTE_A4, 1 },
					Tone { 0, 1 },
					Tone { NOTE_B4, 1 },
					Tone { NOTE_A4, 1 },
					Tone { NOTE_E5, 2 },
					Tone { NOTE_G5, 2 },
					Tone { 0, 2 },
					Tone { NOTE_B5, 1 },
					Tone { 0, 1 },
					Tone { NOTE_A5, 1 },
					Tone { 0, 1 },
					Tone { NOTE_E5, 2 },
			});
		} },
		{ Games::Robby, [](){
			return new MelodyPlayer(130, {
					Tone { NOTE_E3, 4 },
					Tone { 0, 2 },
					Tone { NOTE_A4, 1 },
					Tone { NOTE_F4, 1 },
					Tone { NOTE_A3, 4 },
					Tone { 0, 2 },
					Tone { NOTE_C4, 1 },
					Tone { NOTE_E4, 4 },
			});
		} },
		// TODO intro sounds for the new games
		{ Games::WackyStacky, [](){
			return new MelodyPlayer(130, {});
		} },
		{ Games::Harald, [](){
			return new MelodyPlayer(130, {});
		} },
		{ Games::Frank, [](){
			return new MelodyPlayer(130, {});
		} },
		{ Games::Charlie, [](){
			return new MelodyPlayer(130, {});
		} },
		{ Games::Fred, [](){
			return new MelodyPlayer(130, {});
		} },
		{ Games::Planck, [](){
			return new MelodyPlayer(130, {});
		} },
		{ Games::Dusty, [](){
			return new MelodyPlayer(130, {});
		} },
		{ Games::Sparkly, [](){
			return new MelodyPlayer(130, {});
		} },
};
