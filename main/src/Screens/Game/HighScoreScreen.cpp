#include "HighScoreScreen.h"
#include "Devices/Input.h"
#include "LV_Interface/InputLVGL.h"
#include "UIThread.h"
#include "Util/Services.h"
#include "GameMenuScreen.h"
#include "Periph/NVSFlash.h"
#include <sstream>
#include "Services/HighScoreManager.h"

HighScoreScreen::HighScoreScreen(Games current) : evts(6), currentGame(current){
	buildUI();
}

void HighScoreScreen::buildUI(){
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);

	auto bg = lv_img_create(*this);
	lv_img_set_src(bg, "S:/bg.bin");
	lv_obj_add_flag(bg, LV_OBJ_FLAG_FLOATING);

	auto top = lv_obj_create(*this);
	lv_obj_set_size(top, 128, 32);
	lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(top, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_ver(top, 4, 0);
	lv_obj_set_style_pad_right(top, 2, 0);
	lv_obj_set_style_pad_left(top, 6, 0);

	auto img = lv_img_create(top);
	lv_img_set_src(img, "S:/Paused.bin"); // TODO this needs to be changed depending on the game launched

	batt = new BatteryElement(top);

	auto rest = lv_obj_create(*this);
	lv_obj_set_size(rest, 128, 96);
	lv_obj_set_flex_flow(rest, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(rest, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_all(rest, 4, 0);

	lv_style_set_width(itemStyle, lv_pct(100));
	lv_style_set_height(itemStyle, 16);
	lv_style_set_border_width(itemStyle, 1);
	lv_style_set_border_color(itemStyle, lv_color_make(217, 153, 186));
	lv_style_set_border_opa(itemStyle, LV_OPA_COVER);
	lv_style_set_radius(itemStyle, 2);

	auto mkLabel = [this, &rest](const char* text){
		auto item = lv_obj_create(rest);
		lv_obj_add_style(item, itemStyle, 0);

		auto label = lv_label_create(item);
		lv_label_set_text(label, text);
		lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_center(label);

		return item;
	};

	mkLabel("HIGH SCORE:");

	if(const HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore)){
		if(hsm->hasHighScore(currentGame)){
			const std::array<HighScore, 5>& highScores = hsm->getAll(currentGame);

			for(const HighScore& highScore : highScores){
				if(highScore.score > 0){
					std::stringstream label;
					label << highScore.id[0];
					label << highScore.id[1];
					label << highScore.id[2];
					label << ": ";
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
	batt->loop();

	for(Event e{}; evts.get(e, 0); ){
		if(e.facility != Facility::Input){
			free(e.data);
			continue;
		}

		auto data = (Input::Data*) e.data;
		if((data->btn == Input::Menu || data->btn == Input::B) && data->action == Input::Data::Release){
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