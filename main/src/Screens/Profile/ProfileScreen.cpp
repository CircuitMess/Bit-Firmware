#include "ProfileScreen.h"
#include "LV_Interface/FSLVGL.h"
#include "LV_Interface/InputLVGL.h"
#include "Services/GameManager.h"
#include "Util/Services.h"
#include "Util/stdafx.h"
#include "UIThread.h"
#include <Modals/NewRobot.h>
#include <Modals/LockedGame.h>
#include <Modals/UpdateRobot.h>
#include <Modals/UnknownRobot.h>
#include <Screens/Settings/SettingsScreen.h>
#include <unordered_set>
#include "Util/Notes.h"

struct Entry {
	const char* icon;
	// TODO achievement info
};

static constexpr Entry AchievementEntries[] = {
		{ .icon = "Blocks" },
		{ .icon = "Pong" },
		{ .icon = "Snake" },
		{ .icon = "Arte" },
		{ .icon = "Bee" },
		{ .icon = "Bob" },
		{ .icon = "Butt" },
		{ .icon = "Capa" },
		{ .icon = "Hertz" },
		{ .icon = "Marv" },
		{ .icon = "Resis" },
		{ .icon = "Robby" }
};

std::atomic<bool> ProfileScreen::running = false;

ProfileScreen::ProfileScreen() : events(12), audio((ChirpSystem*) Services.get(Service::Audio)){
	buildUI();
}

ProfileScreen::~ProfileScreen(){
	lv_obj_remove_event_cb(*this, onScrollEnd); // just in case
}

void ProfileScreen::launch(Games game){
	auto games = (GameManager*) Services.get(Service::Games);
	if(!games->isUnlocked(game)){
		auto audio = (ChirpSystem*) Services.get(Service::Audio);
		audio->play({ { 300, 300, 50 },
					  { 0,   0,   50 },
					  { 200, 200, 250 } });
		const auto rob = GameManager::GameRobot.at(game);
		new LockedGame(this, rob);
		return;
	}

	auto ui = (UIThread*) Services.get(Service::UI);
	ui->startGame(game);
}

void ProfileScreen::onStarting(){
	lv_obj_scroll_to(*this, 0, 0, LV_ANIM_OFF); // set y to <height> to scroll from top. 0 to scroll from bottom

	loopBlocked = true;
}

void ProfileScreen::onStart(){
	Events::listen(Facility::Input, &events);
	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), nullptr);

	lv_obj_scroll_to(*this, 0, 128, LV_ANIM_ON);
	lv_obj_add_event_cb(*this, ProfileScreen::onScrollEnd, LV_EVENT_SCROLL_END, this);
}

void ProfileScreen::onScrollEnd(lv_event_t* evt){
	auto menu = (ProfileScreen*) evt->user_data;
	lv_obj_remove_event_cb(*menu, onScrollEnd);

	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), menu->inputGroup);

	running = true;

	menu->loopBlocked = false;
	menu->events.reset();
}

void ProfileScreen::onStop(){
	Events::unlisten(&events);
	lv_obj_remove_event_cb(*this, onScrollEnd);

	running = false;
}

void ProfileScreen::loop(){
	if(loopBlocked){
		return;
	}

	Event evt{};
	if(events.get(evt, 0)){
		if(evt.facility == Facility::Input){
			auto data = (Input::Data*) evt.data;
			handleInput(*data);
		}
		free(evt.data);
	}
}

void ProfileScreen::handleInput(const Input::Data& evt){
	if(InputLVGL::getInstance()->getIndev()->group != inputGroup){
		return;
	}

	if(evt.btn == Input::Menu && evt.action == Input::Data::Release){
		audio->play({ { 350, 350, 80 },
					  { 0,   0,   80 },
					  { 150, 150, 80 },
					  { 0,   0,   80 },
					  { 600, 600, 80 } });

		auto ui = (UIThread*) Services.get(Service::UI);
		ui->startScreen([](){ return std::make_unique<SettingsScreen>(); });
	}

	if((evt.btn == Input::Left || evt.btn == Input::Right || evt.btn == Input::Up || evt.btn == Input::Down) && evt.action == Input::Data::Press){
		audio->play({ { 400, 400, 50 } });
	}
}

void ProfileScreen::buildUI(){
	lv_obj_set_size(*this, 128, 128);

	auto bg = lv_img_create(*this);
	lv_img_set_src(bg, "S:/Profile/bg.bin");
	lv_obj_add_flag(bg, LV_OBJ_FLAG_FLOATING);
	lv_obj_set_pos(bg, 0, 0);
}