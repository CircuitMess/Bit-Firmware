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

void GameMenuScreen::buildUI(){
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

	rest = lv_obj_create(*this);
	lv_obj_set_size(rest, 128, 88);
	lv_obj_set_flex_flow(rest, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(rest, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_ver(rest, 4, 0);

	lv_style_set_width(itemStyle, lv_pct(94));
	lv_style_set_height(itemStyle, 18);
	lv_style_set_radius(itemStyle, 2);

	lv_style_set_bg_img_opa(focusStyle, LV_OPA_100);
	lv_style_set_bg_img_src(focusStyle, selectPath.c_str());

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
		lv_obj_set_style_text_color(label, GameTextColors[(uint8_t) currentGame], 0);

		return item;
	};

	auto play = mkBtn("Play");

	lv_obj_add_event_cb(play, [](lv_event_t* e){
		lv_async_call([](void* arg){
			auto screen = (GameMenuScreen*) arg;

			if(auto ui = (UIThread*) Services.get(Service::UI)){
				ui->startScreen([screen](){ return std::make_unique<InstructionsScreen>(screen->currentGame, true); });
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
				ui->startScreen([screen](){ return std::make_unique<InstructionsScreen>(screen->currentGame, false); });
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
	auto ui = (UIThread*) Services.get(Service::UI);
	ui->startScreen([](){ return std::make_unique<MainMenu>(); });
}
