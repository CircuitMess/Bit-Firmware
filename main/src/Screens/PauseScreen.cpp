#include "PauseScreen.h"
#include "Screens/MainMenu/MainMenu.h"
#include "Screens/Game/GameMenuScreen.h"
#include "Screens/Settings/BoolElement.h"
#include "Screens/Settings/SliderElement.h"
#include "Devices/Input.h"
#include "LV_Interface/InputLVGL.h"
#include "UIThread.h"
#include "Util/Services.h"
#include "Services/BacklightBrightness.h"
#include "Settings/Settings.h"
#include "Services/ChirpSystem.h"
#include "Filepaths.hpp"

PauseScreen::PauseScreen(Games current) : evts(6), currentGame(current){
	buildUI();
}

void PauseScreen::onStart(){
	Events::listen(Facility::Input, &evts);
	InputLVGL::getInstance()->setVertNav(true);
}

void PauseScreen::onStop(){
	Events::unlisten(&evts);
	InputLVGL::getInstance()->setVertNav(false);

	auto settings = (Settings*) Services.get(Service::Settings);

	auto set = settings->get();
	set.screenBrightness = blSlider->getValue();
	set.sound = audioSwitch->getValue();

	settings->set(set);
	settings->store();
}

void PauseScreen::loop(){
	batt->loop();

	Event e{};
	if(evts.get(e, 0)){
		if(e.facility != Facility::Input){
			free(e.data);
			return;
		}

		auto data = (Input::Data*) e.data;
		if((data->btn == Input::Menu || data->btn == Input::B) && data->action == Input::Data::Release){
			auto ui = (UIThread*) Services.get(Service::UI);
			ui->resumeGame();

			free(e.data);
			return;
		}
		free(e.data);
	}
}

void PauseScreen::showControls(){
	auto icon = GameIcons[(int) currentGame];
	std::string instr("/spiffs/Splash/"); instr += icon; instr += "_instr.bmp";

	auto disp = (Display*) Services.get(Service::Display);
	auto lgfx = disp->getLGFX();
	lgfx.drawBmpFile(instr.c_str());

	evts.reset();
	Events::listen(Facility::Battery, &evts);
	for(;;){
		Event evt{};
		if(!evts.get(evt, portMAX_DELAY)) continue;

		if(evt.facility == Facility::Input){
			auto data = (Input::Data*) evt.data;
			if(data->action == Input::Data::Release){
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

	evts.reset();
	Events::listen(Facility::Input, &evts);

	lv_obj_invalidate(*this);
}

void PauseScreen::exit(){
	auto disp = (Display*) Services.get(Service::Display);
	if(disp == nullptr){
		return;
	}

	auto lgfx = disp->getLGFX();
	lgfx.drawBmpFile("/spiffs/bgSplash.bmp");

	auto ui = (UIThread*) Services.get(Service::UI);
	if(ui == nullptr){
		return;
	}

	ui->exitGame();
}

void PauseScreen::buildUI(){
	const Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	auto theme = settings->get().theme;

	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);

	auto bg = lv_img_create(*this);
	lv_img_set_src(bg, THEMED_FILE(Background, settings->get().theme));
	lv_obj_add_flag(bg, LV_OBJ_FLAG_FLOATING);

	auto top = lv_obj_create(*this);
	lv_obj_set_size(top, 128, 32);
	lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(top, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_ver(top, 4, 0);
	lv_obj_set_style_pad_right(top, 2, 0);
	lv_obj_set_style_pad_left(top, 6, 0);

	auto img = lv_img_create(top);
	lv_img_set_src(img, Filepath::Paused);

	batt = new BatteryElement(top);

	auto rest = lv_obj_create(*this);
	lv_obj_set_size(rest, 128, 96);
	lv_obj_set_flex_flow(rest, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(rest, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_all(rest, 4, 0);

	lv_style_set_width(itemStyle, lv_pct(100));
	lv_style_set_height(itemStyle, 17);
	lv_style_set_border_width(itemStyle, 1);
	lv_style_set_border_color(itemStyle, THEMED_COLOR(HighlightPrimary, theme));
	lv_style_set_border_opa(itemStyle, LV_OPA_COVER);
	lv_style_set_radius(itemStyle, 2);

	lv_style_set_bg_color(focusStyle, THEMED_COLOR(HighlightPrimary, theme));
	lv_style_set_bg_opa(focusStyle, LV_OPA_30);

	auto initSet = settings->get();

	audioSwitch = new BoolElement(rest, "Sound", [this](bool value){
		auto settings = (Settings*) Services.get(Service::Settings);
		auto set = settings->get();
		set.sound = audioSwitch->getValue();
		settings->set(set);

		auto chirp = (ChirpSystem*) Services.get(Service::Audio);
		chirp->play({
							Chirp{ .startFreq = 400, .endFreq = 600, .duration = 50 },
							Chirp{ .startFreq = 0, .endFreq = 0, .duration = 100 },
							Chirp{ .startFreq = 600, .endFreq = 400, .duration = 50 }
					});
	}, initSet.sound);
	lv_group_add_obj(inputGroup, *audioSwitch);

	blSlider = new SliderElement(rest, "Brightness", [](uint8_t value){
		auto bl = (BacklightBrightness*) Services.get(Service::Backlight);
		bl->setBrightness(value);
	}, initSet.screenBrightness);
	lv_group_add_obj(inputGroup, *blSlider);

	auto mkBtn = [this, &rest](const char* text){
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

	auto ctrl = mkBtn("Controls...");
	auto exit = mkBtn("Exit game");

	lv_obj_add_event_cb(ctrl, [](lv_event_t* e){
		auto pause = (PauseScreen*) e->user_data;
		pause->showControls();
	}, LV_EVENT_PRESSED, this);

	lv_obj_add_event_cb(exit, [](lv_event_t* e){
		lv_async_call([](void* arg){
			auto pause = (PauseScreen*) arg;
			pause->exit();
		}, e->user_data);
	}, LV_EVENT_PRESSED, this);

	lv_group_focus_obj(lv_obj_get_child(rest, 0)); // TODO: move to onStarting if this is a persistent screen
}
