#include "GameSplashScreen.h"
#include "LV_Interface/InputLVGL.h"
#include "UIThread.h"
#include "Util/Services.h"
#include "Periph/NVSFlash.h"
#include "Services/RobotManager.h"
#include "Settings/Settings.h"
#include "Services/TwinkleService.h"
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
	lv_img_set_src(bg, (gameUIPath + "bg.bin").c_str());
	lv_obj_add_flag(bg, LV_OBJ_FLAG_FLOATING);

	auto top = lv_obj_create(*this);
	lv_obj_set_size(top, 128, 42);
	lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(top, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);\

	auto img = lv_img_create(top);
	lv_img_set_src(img, (gameUIPath + "title.bin").c_str());
	lv_obj_set_align(img, LV_ALIGN_CENTER);
	lv_obj_set_style_pad_top(img, 8, 0);

	auto rest = lv_obj_create(*this);
	lv_obj_set_size(rest, 128, 86);
	lv_obj_set_flex_flow(rest, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(rest, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_all(rest, 4, 0);

	auto instructionsImg = lv_img_create(rest);
	lv_img_set_src(instructionsImg, (gameUIPath + "char.bin").c_str());
	lv_obj_set_align(instructionsImg, LV_ALIGN_CENTER);
}

void GameSplashScreen::loop(){
	if(millis() - startTime < HoldTime || melody->isPlaying()) return;

	if(auto ui = (UIThread*) Services.get(Service::UI)){
		ui->startScreen([this](){ return std::make_unique<GameMenuScreen>(currentGame); });
	}
}

void GameSplashScreen::onStart(){
	startTime = millis();

	if(auto led = (LEDService*) Services.get(Service::LED)){
		auto buttons = GameButtonsUsed[(uint8_t) currentGame];
		if(buttons.up){
			led->on(LED::Up);
		}
		if(buttons.down){
			led->on(LED::Down);
		}
		if(buttons.left){
			led->on(LED::Left);
		}
		if(buttons.right){
			led->on(LED::Right);
		}
		if(buttons.a){
			led->on(LED::A);
		}
		if(buttons.b){
			led->on(LED::B);
		}
	}

	if(IntroSounds.contains(currentGame)){
		melody = IntroSounds.at(currentGame)();
		if(melody){
			melody->play();
		}
	}
}

void GameSplashScreen::onStop(){
	delete melody;
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
