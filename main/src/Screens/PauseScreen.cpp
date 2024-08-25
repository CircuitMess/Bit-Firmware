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
#include "InstructionsElement.h"
#include "Services/LEDService.h"

PauseScreen::PauseScreen(Games current) : evts(6), currentGame(current){
	buildUI();
}

void PauseScreen::onStart(){
	Events::listen(Facility::Input, &evts);
	InputLVGL::getInstance()->setVertNav(true);

	auto led = (LEDService*) Services.get(Service::LED);
	led->twinkle();
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
	Event e{};
	if(evts.get(e, 0)){
		if(e.facility != Facility::Input){
			free(e.data);
			return;
		}

		auto data = (Input::Data*) e.data;
		if(state == State::Pause && (data->btn == Input::Menu || data->btn == Input::B) && data->action == Input::Data::Press){
			auto ui = (UIThread*) Services.get(Service::UI);
			ui->resumeGame();

			free(e.data);
			return;
		}else if((data->btn == Input::Menu || data->btn == Input::B || data->btn == Input::A) && data->action == Input::Data::Release){
			if(state == State::IgnoreInput){
				state = State::Controls;
			}else if(state == State::Controls){
				exitControls();
			}
		}
		free(e.data);
	}
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
	lv_img_set_src(bg, THEMED_FILE(PausedBg, settings->get().theme));
	lv_obj_add_flag(bg, LV_OBJ_FLAG_FLOATING);

	auto top = lv_obj_create(*this);
	lv_obj_set_size(top, 128, 32);
	lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(top, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_ver(top, 4, 0);
	lv_obj_set_style_pad_right(top, 2, 0);
	lv_obj_set_style_pad_left(top, 6, 0);

	auto img = lv_img_create(top);
	lv_img_set_src(img, THEMED_FILE(Paused, theme));

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
	lv_style_set_bg_opa(focusStyle, LV_OPA_70);

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
	lv_obj_add_style(*audioSwitch, focusStyle, LV_STATE_FOCUSED);

	blSlider = new SliderElement(rest, "Brightness", [](uint8_t value){
		auto bl = (BacklightBrightness*) Services.get(Service::Backlight);
		bl->setBrightness(value);
	}, initSet.screenBrightness);
	lv_group_add_obj(inputGroup, *blSlider);
	lv_obj_add_style(*blSlider, focusStyle, LV_STATE_FOCUSED);

	auto mkBtn = [this, &rest, &theme](const char* text){
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
		lv_obj_set_style_text_color(label, THEMED_COLOR(PausedForeground, theme), 0);

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

void PauseScreen::showControls(){
	state = State::IgnoreInput;

	lv_obj_clean(*this);
	lv_obj_invalidate(*this);

	new InstructionsElement(*this, currentGame);

	auto led = (LEDService*) Services.get(Service::LED);
	led->ctrls(currentGame);
}

void PauseScreen::exitControls(){
	state = State::Pause;

	lv_timer_handler();

	lv_obj_clean(*this);
	lv_obj_invalidate(*this);

	buildUI();

	auto led = (LEDService*) Services.get(Service::LED);
	led->twinkle();
}
