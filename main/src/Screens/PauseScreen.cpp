#include "PauseScreen.h"
#include "Screens/MainMenu.h"
#include "Screens/Settings/BoolElement.h"
#include "Screens/Settings/SliderElement.h"
#include "Devices/Input.h"
#include "UIThread.h"
#include "Util/Services.h"
#include "Services/BacklightBrightness.h"
#include "Settings/Settings.h"

PauseScreen::PauseScreen() : evts(6){
	buildUI();
}

void PauseScreen::onStart(){
	bg->start();
	Events::listen(Facility::Input, &evts);
}

void PauseScreen::onStop(){
	bg->stop();
	Events::unlisten(&evts);

	auto settings = (Settings*) Services.get(Service::Settings);

	auto set = settings->get();
	set.screenBrightness = blSlider->getValue();
	set.sound = audioSwitch->getValue();

	settings->set(set);
	settings->store();
}

void PauseScreen::loop(){
	Event e;
	if(evts.get(e, 0)){
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

void PauseScreen::buildUI(){
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);

	bg = new LVGIF(*this, "S:/bg");
	lv_obj_add_flag(*bg, LV_OBJ_FLAG_FLOATING);

	auto top = lv_obj_create(*this);
	lv_obj_set_size(top, 128, 32);
	lv_obj_set_style_pad_ver(top, 4, 0);

	auto img = lv_img_create(top);
	lv_img_set_src(img, "S:/Paused.bin");
	lv_obj_center(img);

	auto rest = lv_obj_create(*this);
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

	auto settings = (Settings*) Services.get(Service::Settings);
	auto initSet = settings->get();

	audioSwitch = new BoolElement(rest, "Sound", [](bool value){
		auto chirp = (ChirpSystem*) Services.get(Service::Audio);
		chirp->setMute(!value);
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

	mkBtn("Controls...");
	auto exit = mkBtn("Exit game");

	lv_obj_add_event_cb(exit, [](lv_event_t* e){
		auto ui = (UIThread*) Services.get(Service::UI);
		ui->startScreen([](){ return std::make_unique<MainMenu>(); });
	}, LV_EVENT_CLICKED, nullptr);

	lv_group_focus_obj(lv_obj_get_child(rest, 0)); // TODO: move to onStarting if this is a persistent screen
}
