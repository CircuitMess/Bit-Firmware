#include "GameSplashScreen.h"
#include "LV_Interface/InputLVGL.h"
#include "UIThread.h"
#include "Util/Services.h"
#include "Periph/NVSFlash.h"
#include "Services/RobotManager.h"
#include "Settings/Settings.h"
#include "Util/stdafx.h"
#include "Screens/Game/GameMenuScreen.h"
#include "Util/Notes.h"

extern const std::unordered_map<Games, std::function<MelodyPlayer*()>> IntroSounds;

GameSplashScreen::GameSplashScreen(Games current) : currentGame(current){
	switch(currentGame){
		case Games::Artemis:{
			gameUIPath.append("Artemis/");
			break;
		}
		case Games::Blocks:{
			gameUIPath.append("Blocks/");
			break;
		}
		case Games::Pong:{
			gameUIPath.append("Pong/");
			break;
		}
		case Games::Snake:{
			gameUIPath.append("Snake/");
			break;
		}
		case Games::WackyStacky:{
			gameUIPath.append("Stacky/");
			break;
		}
		case Games::MrBee:{
			gameUIPath.append("Bee/");
			break;
		}
		case Games::Bob:{
			gameUIPath.append("Bob/");
			break;
		}
		case Games::Buttons:{
			gameUIPath.append("Buttons/");
			break;
		}
		case Games::Capacitron:{
			gameUIPath.append("Capacitron/");
			break;
		}
		case Games::Hertz:{
			gameUIPath.append("Hertz/");
			break;
		}
		case Games::Marv:{
			gameUIPath.append("Marv/");
			break;
		}
		case Games::Resistron:{
			gameUIPath.append("Resistron/");
			break;
		}
		case Games::Robby:{
			gameUIPath.append("Robby/");
			break;
		}
		case Games::Harald:{
			gameUIPath.append("Harald/");
			break;
		}
		case Games::Frank:{
			gameUIPath.append("Frank/");
			break;
		}
		case Games::Charlie:{
			gameUIPath.append("Charlie/");
			break;
		}
		case Games::Fred:{
			gameUIPath.append("Fred/");
			break;
		}
		case Games::Planck:{
			gameUIPath.append("Planck/");
			break;
		}
		case Games::Dusty:{
			gameUIPath.append("Dusty/");
			break;
		}
		case Games::Sparkly:{
			gameUIPath.append("Sparkly/");
			break;
		}
		case Games::COUNT:
		default:{
			break;
		}
	}

	buildUI();
}

void GameSplashScreen::buildUI(){
	const Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);

	auto bg = lv_img_create(*this);
	lv_obj_set_size(bg, 128, 128);
	lv_obj_add_flag(bg, LV_OBJ_FLAG_FLOATING);
	lv_img_set_src(bg, (gameUIPath + "bg.bin").c_str());

	auto title = lv_img_create(*this);
	lv_obj_set_size(title, 128, 40);
	lv_img_set_src(title, (gameUIPath + "title.bin").c_str());

	auto splash = lv_img_create(*this);
	lv_obj_set_size(splash, 128, 88);
	lv_img_set_src(splash, (gameUIPath + "char.bin").c_str());
}

void GameSplashScreen::loop(){
	if(millis() - startTime < HoldTime || (melody && melody->isPlaying())) return;

	if(auto ui = (UIThread*) Services.get(Service::UI)){
		ui->startScreen([this](){ return std::make_unique<GameMenuScreen>(currentGame); });
	}
}

void GameSplashScreen::onStart(){
	startTime = millis();

	const auto settings = (Settings*) Services.get(Service::Settings);
	if(settings->get().sound && IntroSounds.contains(currentGame)){
		melody = std::unique_ptr<MelodyPlayer>(IntroSounds.at(currentGame)());
		if(melody){
			melody->play();
		}
	}
}

void GameSplashScreen::onStop(){
	melody.reset();
}

const std::unordered_map<Games, std::function<MelodyPlayer*()>> IntroSounds = {
		{ Games::Blocks, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_B3, 2),
					toneNone(2),
					toneFlat(NOTE_F3, 2),
					toneFlat(NOTE_G3, 2),
					toneFlat(NOTE_A3, 2),
					toneNone(2),
					toneFlat(NOTE_G3, 2),
					toneFlat(NOTE_FS3, 2),
					toneFlat(NOTE_E3, 2)
			});
		} },
		{ Games::Pong, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_C4, 2),
					toneFlat(NOTE_E4, 1),
					toneNone(1),
					toneFlat(NOTE_G4, 1),
					toneNone(1),
					toneFlat(NOTE_E4, 1),
					toneNone(1),
					toneFlat(NOTE_D4, 1),
					toneNone(1),
					toneFlat(NOTE_E4, 1)
			});
		} },
		{ Games::Snake, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_FS4, 1),
					toneFlat(NOTE_D4, 1),
					toneFlat(NOTE_G4, 4),
					toneNone(1),
					toneFlat(NOTE_F4, 1),
					toneFlat(NOTE_A4, 1),
					toneFlat(NOTE_G4, 1),
					toneFlat(NOTE_AS4, 4),
					toneNone(1),
					toneFlat(NOTE_GS4, 1),
					toneFlat(NOTE_FS4, 1),
					toneFlat(NOTE_D4, 1),
					toneFlat(NOTE_G4, 4),
			});
		} },
		{ Games::MrBee, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_C5, 1),
					toneFlat(NOTE_F4, 1),
					toneFlat(NOTE_C4, 1),
					toneFlat(NOTE_F4, 1),
					toneFlat(NOTE_C5, 1),
					toneFlat(NOTE_F4, 1),
					toneFlat(NOTE_C4, 1),
					toneFlat(NOTE_E4, 1),
					toneFlat(NOTE_C5, 1),
			});
		} },
		{ Games::Bob, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_C4, 2),
					toneFlat(NOTE_F4, 2),
					toneNone(4),
					toneFlat(NOTE_D4, 2),
					toneFlat(NOTE_A4, 2),
					toneNone(2),
					toneFlat(NOTE_C5, 2),
					toneFlat(NOTE_A4, 2),
					toneFlat(NOTE_F4, 2),
			});
		} },
		{ Games::Capacitron, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_E4, 6),
					toneNone(1),
					toneFlat(NOTE_B4, 1),
					toneNone(1),
					toneFlat(NOTE_G4, 6),
					toneNone(1),
					toneFlat(NOTE_A4, 1),
					toneNone(1),
					toneFlat(NOTE_E4, 6),
			});
		} },
		{ Games::Hertz, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_C5, 1),
					toneFlat(NOTE_F5, 1),
					toneNone(1),
					toneFlat(NOTE_FS4, 1),
					toneFlat(NOTE_G4, 1),
					toneFlat(NOTE_E4, 1),
					toneNone(1),
					toneFlat(NOTE_E5, 1),
					toneFlat(NOTE_B4, 1),
					toneNone(2),
					toneFlat(NOTE_F5, 1),
					toneNone(1),
					toneFlat(NOTE_C6, 1),
			});
		} },
		{ Games::Marv, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_FS3, 6),
					toneNone(1.5),
					toneFlat(NOTE_E4, 6),
					toneNone(1.5),
					toneFlat(NOTE_FS4, 6),
			});
		} },
		{ Games::Resistron, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_FS4, 2),
					toneNone(1),
					toneFlat(NOTE_GS4, 2),
					toneNone(1),
					toneFlat(NOTE_FS4, 2),
					toneNone(1),
					toneFlat(NOTE_E4, 2),
					toneNone(1),
					toneFlat(NOTE_FS4, 2),
			});
		} },
		{ Games::Buttons, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_D4, 1),
					toneFlat(NOTE_E4, 1),
					toneNone(1),
					toneFlat(NOTE_G4, 1),
					toneNone(1.5),
					toneFlat(NOTE_D4, 0.5),
					toneFlat(NOTE_E4, 1),
					toneNone(1),
					toneFlat(NOTE_G4, 1),
					toneNone(1),
					toneFlat(NOTE_B3, 1),
					toneNone(1),
					toneFlat(NOTE_A3, 1),
					toneNone(1),
					toneFlat(NOTE_D4, 1)
			});
		} },
		{ Games::Artemis, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_E4, 1),
					toneNone(1),
					toneFlat(NOTE_A4, 1),
					toneNone(1),
					toneFlat(NOTE_B4, 1),
					toneFlat(NOTE_A4, 1),
					toneFlat(NOTE_E5, 2),
					toneFlat(NOTE_G5, 2),
					toneNone(2),
					toneFlat(NOTE_B5, 1),
					toneNone(1),
					toneFlat(NOTE_A5, 1),
					toneNone(1),
					toneFlat(NOTE_E5, 2),
			});
		} },
		{ Games::Robby, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_E3, 4),
					toneNone(2),
					toneFlat(NOTE_A4, 1),
					toneFlat(NOTE_F4, 1),
					toneFlat(NOTE_A3, 4),
					toneNone(2),
					toneFlat(NOTE_C4, 1),
					toneFlat(NOTE_E4, 4),
			});
		} },
		// TODO intro sounds for the new games
		{ Games::WackyStacky, [](){
			return new MelodyPlayer(160, {
				toneSlide(NOTE_G3, NOTE_C4, 1), toneFlat(NOTE_C4, 0.5), toneNone(0.5),
				toneFlat(NOTE_E4, 1), toneNone(1),
				toneFlat(NOTE_C5, 1), toneNone(1),
				toneFlat(NOTE_G4, 1), toneNone(1),

				toneSlide(NOTE_E4, NOTE_D5, 1), toneNone(1),
				toneFlat(NOTE_C5, 1), toneNone(1),
				toneFlat(NOTE_A4, 1), toneNone(1),
				toneFlat(NOTE_C5, 1), toneNone(1),

				toneSlide(NOTE_G4, NOTE_E5, 1), toneNone(1),
				toneFlat(NOTE_C5, 1), toneNone(1),
				toneFlat(NOTE_A4, 1), toneNone(1),
				toneFlat(NOTE_C5, 1), toneNone(1),

				/*toneSlide(NOTE_E4, NOTE_D5, 1), toneNone(1),
				toneFlat(NOTE_C5, 1), toneNone(1),
				toneFlat(NOTE_A4, 1), toneNone(1),
				toneFlat(NOTE_C5, 1), toneNone(1),*/

				toneSlide(NOTE_C5, NOTE_C4, 1), toneFlat(NOTE_C4, 3)
			});
		} },
		{ Games::Harald, [](){
			return new MelodyPlayer(130, {
					toneFlat(NOTE_C5, 4), toneFlat(NOTE_F5, 4), toneNone(3),
					toneFlat(NOTE_GS5, 4), toneFlat(NOTE_G5, 4), toneNone(3),
					toneFlat(NOTE_DS5, 4), toneFlat(NOTE_F5, 6)
			});
		} },
		{ Games::Charlie, [](){
			return new MelodyPlayer(160, {
				toneFlat(NOTE_C4, 1), toneNone(2), toneFlat(NOTE_C4, 0.5), toneNone(0.5),
				toneFlat(NOTE_DS4, 0.5), toneNone(1.5), toneFlat(NOTE_F4, 0.5), toneNone(1.5),
				toneFlat(NOTE_FS4, 0.5), toneNone(0.5), toneFlat(NOTE_F4, 0.5), toneNone(1.5), toneFlat(NOTE_DS4, 0.5), toneNone(0.5),
				toneFlat(NOTE_F4, 0.5), toneNone(3.5),
				toneFlat(NOTE_FS4, 0.5), toneNone(0.5), toneFlat(NOTE_F4, 0.5), toneNone(1.5), toneFlat(NOTE_DS4, 0.5), toneNone(0.5),
				toneFlat(NOTE_F4, 0.5), toneNone(1.5), toneFlat(NOTE_DS3, 0.5), toneNone(1.5),
				toneFlat(NOTE_FS4, 0.5), toneNone(0.5), toneFlat(NOTE_F4, 0.5), toneNone(1.5), toneFlat(NOTE_DS4, 0.5), toneNone(0.5),
				toneFlat(NOTE_C4, 1)
			});
		} },
		{ Games::Planck, [](){
			return new MelodyPlayer(160, {
				toneFlat(NOTE_C3, 2), toneSlide(NOTE_C3, NOTE_C4, 1), toneNone(2),
				toneFlat(NOTE_C3, 2), toneSlide(NOTE_C3, NOTE_C4, 1), toneNone(2),
				toneFlat(NOTE_D4, 1), toneFlat(NOTE_C4, 1), toneFlat(NOTE_G3, 1),
				toneNone(2), toneFlat(NOTE_C4, 4)
			});
		} },
		{ Games::Dusty, [](){
			return new MelodyPlayer(160, {
				toneFlat(NOTE_C4, 1), toneNone(1), toneFlat(NOTE_DS4, 1), toneNone(1),
				toneFlat(NOTE_C4, 1), toneNone(1), toneFlat(NOTE_DS4, 1), toneNone(1),
				toneFlat(NOTE_D4, 1), toneNone(1), toneFlat(NOTE_DS4, 1), toneNone(1),
				toneFlat(NOTE_D4, 1), toneNone(1), toneFlat(NOTE_DS4, 1), toneNone(1),
				toneFlat(NOTE_F4, 1), toneNone(1), toneFlat(NOTE_DS4, 1), toneNone(1),
				toneFlat(NOTE_D4, 1), toneNone(1), toneFlat(NOTE_DS4, 1), toneNone(1),
				toneFlat(NOTE_C4, 4)
			});
		} },
		{ Games::Sparkly, [](){
			return new MelodyPlayer(130, {
				toneFlat(NOTE_C5, 0.2),
				toneFlat(NOTE_DS5, 1), toneNone(1), toneFlat(NOTE_F5, 1), toneNone(1),
				toneFlat(NOTE_FS5, 1), toneNone(1), toneFlat(NOTE_F5, 1), toneNone(0.8),
				toneFlat(NOTE_C5, 0.2),
				toneFlat(NOTE_DS5, 1), toneNone(1), toneFlat(NOTE_C5, 1), toneNone(1),
				toneFlat(NOTE_AS4, 1), toneNone(1), toneFlat(NOTE_G4, 1), toneNone(0.8),
				toneFlat(NOTE_G4, 0.2),
				toneFlat(NOTE_AS4, 1), toneNone(1), toneFlat(NOTE_C5, 2)
			});
		} },
};
