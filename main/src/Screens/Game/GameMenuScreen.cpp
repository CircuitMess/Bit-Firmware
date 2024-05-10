#include "GameMenuScreen.h"
#include "Screens/MainMenu/MainMenu.h"
#include "Devices/Input.h"
#include "LV_Interface/InputLVGL.h"
#include "UIThread.h"
#include "Util/Services.h"
#include "Settings/Settings.h"
#include "HighScoreScreen.h"
#include "InstructionsScreen.h"
#include "Services/HighScoreManager.h"

GameMenuScreen::GameMenuScreen(Games current) : evts(6), currentGame(current){
	buildUI();
}

void GameMenuScreen::buildUI(){
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
	lv_style_set_height(itemStyle, 17);
	lv_style_set_border_width(itemStyle, 1);
	lv_style_set_border_color(itemStyle, lv_color_make(217, 153, 186));
	lv_style_set_border_opa(itemStyle, LV_OPA_COVER);
	lv_style_set_radius(itemStyle, 2);

	lv_style_set_bg_color(focusStyle, lv_color_make(217, 153, 186));
	lv_style_set_bg_opa(focusStyle, LV_OPA_30);

	auto mkBtn = [this](const char* text){
		auto item = lv_obj_create(rest);
		lv_group_add_obj(inputGroup, item);
		lv_obj_add_style(item, itemStyle, 0);
		lv_obj_add_style(item, focusStyle, LV_STATE_FOCUSED);
		lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);
		lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);

		auto label = lv_label_create(item);
		lv_label_set_text(label, text);
		lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_center(label);

		return item;
	};

	auto play = mkBtn("Play");

	lv_obj_add_event_cb(play, [](lv_event_t* e){
		lv_async_call([](void* arg){
			auto screen = (GameMenuScreen*) arg;

			if(auto ui = (UIThread*) Services.get(Service::UI)){
				ui->startGame(screen->currentGame);
			}
		}, e->user_data);
	}, LV_EVENT_PRESSED, this);

	if(const HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore)){
		if(hsm->hasScore(currentGame)){
			auto score = mkBtn("High score");

			lv_obj_add_event_cb(score, [](lv_event_t* e){
				lv_async_call([](void* arg){
					auto screen = (GameMenuScreen*) arg;

					if(auto ui = (UIThread*) Services.get(Service::UI)){
						ui->startScreen([screen](){ return std::make_unique<HighScoreScreen>(screen->currentGame); });
					}
				}, e->user_data);
			}, LV_EVENT_PRESSED, this);
		}
	}

	auto instructions = mkBtn("Instructions");

	lv_obj_add_event_cb(instructions, [](lv_event_t* e){
		lv_async_call([](void* arg){
			auto screen = (GameMenuScreen*) arg;

			if(auto ui = (UIThread*) Services.get(Service::UI)){
				ui->startScreen([screen](){ return std::make_unique<InstructionsScreen>(screen->currentGame); });
			}
		}, e->user_data);
	}, LV_EVENT_PRESSED, this);

	auto exit = mkBtn("Exit game");

	lv_obj_add_event_cb(exit, [](lv_event_t* e){
		lv_async_call([](void* arg){
			auto screen = (GameMenuScreen*) arg;
			screen->exit();
		}, e->user_data);
	}, LV_EVENT_PRESSED, this);

	lv_group_focus_obj(lv_obj_get_child(rest, 0));
}

void GameMenuScreen::onStart(){
	Events::listen(Facility::Input, &evts);
	InputLVGL::getInstance()->setVertNav(true);
}

void GameMenuScreen::onStop(){
	Events::unlisten(&evts);
	InputLVGL::getInstance()->setVertNav(false);
}

void GameMenuScreen::loop(){
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

void GameMenuScreen::exit(){
	auto disp = (Display*) Services.get(Service::Display);
	auto lgfx = disp->getLGFX();
	lgfx.drawBmpFile("/spiffs/bgSplash.bmp");

	auto ui = (UIThread*) Services.get(Service::UI);
	ui->startScreen([](){ return std::make_unique<MainMenu>(); });
}
