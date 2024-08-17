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
	if(millis() - startTime < HoldTime) return;

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
}
