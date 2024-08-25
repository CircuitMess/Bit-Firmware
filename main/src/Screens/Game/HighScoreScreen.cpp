#include "HighScoreScreen.h"
#include "Devices/Input.h"
#include "LV_Interface/InputLVGL.h"
#include "UIThread.h"
#include "Util/Services.h"
#include "GameMenuScreen.h"
#include "Periph/NVSFlash.h"
#include <sstream>
#include "Services/HighScoreManager.h"
#include "Settings/Settings.h"
#include "Filepaths.hpp"

HighScoreScreen::HighScoreScreen(Games current) : evts(6), currentGame(current){
	Input* input = (Input*) Services.get(Service::Input);
	if(input == nullptr){
		return;
	}

	if(input->isPressed(Input::Button::A) || input->isPressed(Input::Button::B) || input->isPressed(Input::Button::Menu)){
		ignoreFirstInput = true;
	}

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

	selectPath = gameUIPath + "select.bin";

	buildUI();
}

void HighScoreScreen::buildUI(){
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

	auto rest = lv_obj_create(*this);
	lv_obj_set_size(rest, 128, 88);
	lv_obj_set_flex_flow(rest, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(rest, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_ver(rest, 4, 0);

	lv_style_set_width(itemStyle, lv_pct(100));
	lv_style_set_height(itemStyle, 18);
	lv_style_set_bg_img_opa(itemStyle, LV_OPA_100);
	lv_style_set_bg_img_src(itemStyle, selectPath.c_str());

	auto mkLabel = [this, &rest](const char* text){
		auto item = lv_obj_create(rest);
		lv_obj_add_style(item, itemStyle, 0);

		auto label = lv_label_create(item);
		lv_label_set_text(label, text);
		lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_center(label);
		lv_obj_set_style_text_color(label, GameTextColors[(uint8_t) currentGame], 0);

		return item;
	};

	if(const HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore)){
		if(hsm->hasHighScore(currentGame)){
			const auto& highScores = hsm->getAll(currentGame);

			for(const HighScore& highScore : highScores){
				if(highScore.score > 0){
					std::stringstream label;

					for(char i : highScore.id){
						if(i != ' '){
							label << i;
						}
					}

					label << " : ";
					label << highScore.score;

					mkLabel(label.str().c_str());
				}
			}
		}else{
			mkLabel("NO HIGH SCORE");
		}
	}

	lv_group_focus_obj(lv_obj_get_child(rest, 0));
}

void HighScoreScreen::onStart(){
	Events::listen(Facility::Input, &evts);
	InputLVGL::getInstance()->setVertNav(true);
}

void HighScoreScreen::onStop(){
	Events::unlisten(&evts);
	InputLVGL::getInstance()->setVertNav(false);
}

void HighScoreScreen::loop(){
	for(Event e{}; evts.get(e, 0); ){
		if(e.facility != Facility::Input){
			free(e.data);
			continue;
		}

		if(ignoreFirstInput){
			ignoreFirstInput = false;
			free(e.data);
			continue;
		}

		auto data = (Input::Data*) e.data;
		if((data->btn == Input::Menu || data->btn == Input::B || data->btn == Input::A) && data->action == Input::Data::Release){
			exit();

			free(e.data);
			return;
		}

		free(e.data);
	}
}

void HighScoreScreen::exit(){
	auto ui = (UIThread*) Services.get(Service::UI);
	ui->startScreen([this](){ return std::make_unique<GameMenuScreen>(currentGame); });
}