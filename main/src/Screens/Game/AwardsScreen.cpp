#include "AwardsScreen.h"
#include "Devices/Input.h"
#include "LV_Interface/InputLVGL.h"
#include "UIThread.h"
#include "Util/Services.h"
#include "HighScoreScreen.h"
#include "Periph/NVSFlash.h"
#include <sstream>
#include "Util/stdafx.h"

AwardsScreen::AwardsScreen(Games current, uint32_t highScore) : highScore(highScore), evts(6), currentGame(current), start(millis()){
	buildUI();
}

void AwardsScreen::buildUI(){
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

	rest = lv_obj_create(*this);
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

	auto mkLabel = [this](const char* text){
		auto item = lv_obj_create(rest);
		lv_obj_add_style(item, itemStyle, 0);

		auto label = lv_label_create(item);
		lv_label_set_text(label, text);
		lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_center(label);

		return item;
	};

	mkLabel("ENTER HIGH SCORE:");

	item = lv_obj_create(rest);
	lv_obj_add_style(item, itemStyle, 0);

	lv_obj_set_align(item, LV_ALIGN_CENTER);
	lv_obj_set_flex_flow(item, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(item, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	name[0] = lv_label_create(item);
	lv_label_set_text(name[0], (std::stringstream() << characters[0]).str().c_str());
	lv_obj_set_size(name[0], LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_style_border_color(name[0], lv_color_make(217, 153, 186), 0);
	lv_obj_set_style_border_width(name[0], 1, 0);

	name[1] = lv_label_create(item);
	lv_label_set_text(name[1], (std::stringstream() << characters[1]).str().c_str());
	lv_obj_set_size(name[1], LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_style_border_color(name[1], lv_color_make(217, 153, 186), 0);
	lv_obj_set_style_border_width(name[1], 1, 0);

	name[2] = lv_label_create(item);
	lv_label_set_text(name[2], (std::stringstream() << characters[2]).str().c_str());
	lv_obj_set_size(name[2], LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_style_border_color(name[2], lv_color_make(217, 153, 186), 0);
	lv_obj_set_style_border_width(name[2], 1, 0);

	value = lv_label_create(item);
	lv_label_set_text(value, (std::stringstream() << ": " << highScore).str().c_str());
	lv_obj_set_size(value, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

	lv_group_focus_obj(lv_obj_get_child(rest, 0));
}

void AwardsScreen::onStart(){
	Events::listen(Facility::Input, &evts);
	InputLVGL::getInstance()->setVertNav(true);
}

void AwardsScreen::onStop(){
	Events::unlisten(&evts);
	InputLVGL::getInstance()->setVertNav(false);

	if(HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore)){
		HighScore score = {.score = highScore, .valid = true};
		score.id[0] = characters[0];
		score.id[1] = characters[1];
		score.id[2] = characters[2];

		hsm->saveScore(currentGame, score);
	}
}

void AwardsScreen::loop(){
	batt->loop();

	for(Event e{}; evts.get(e, 0); ){
		if(e.facility != Facility::Input){
			free(e.data);
			continue;
		}

		auto data = (Input::Data*) e.data;
		if(data->action == Input::Data::Release){
			if(data->btn == Input::Menu || data->btn == Input::B || data->btn == Input::A){
				exit();

				free(e.data);
				return;
			}else if(data->btn == Input::Up){
				labelChanged = 0;
			}else if(data->btn == Input::Down){
				labelChanged = 0;
			}else if(data->btn == Input::Left){
				activeIndex = activeIndex == 0 ? 2 : activeIndex - 1;
			}else if(data->btn == Input::Right){
				activeIndex = (activeIndex + 1) % 3;
			}
		}else{
			if(data->btn == Input::Up){
				labelChanged = 1;
			}else if(data->btn == Input::Down){
				labelChanged = -1;
			}
		}

		free(e.data);
	}

	const uint64_t interval = (millis() - start) / blinkTime;

	if(interval % 2 == 0){
		for(size_t i = 0; i < 3; ++i){
			lv_obj_set_style_border_opa(name[i], 0, 0);
		}
	}else if(interval % 2 == 1){
		for(size_t i = 0; i < 3; ++i){
			if(i == activeIndex){
				lv_obj_set_style_border_opa(name[i], 100, 0);
			}else{
				lv_obj_set_style_border_opa(name[i], 0, 0);
			}
		}
	}

	if(labelChanged != 0){
		if(millis() - lastChanged >= 200){
			if(labelChanged > 0){
				characters[activeIndex] = getChar(characters[activeIndex] + 1, 1);
			}else{
				characters[activeIndex] = getChar(characters[activeIndex] - 1, -1);
			}

			lastChanged = millis();
		}

		for(size_t i = 0; i < 3; ++i){
			lv_label_set_text(name[i], (std::stringstream() << characters[i]).str().c_str());
		}
	}
}

void AwardsScreen::exit(){
	auto ui = (UIThread*) Services.get(Service::UI);
	ui->startScreen([this](){ return std::make_unique<HighScoreScreen>(currentGame); });
}
