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
	const auto height = lv_obj_get_height(itemCont) + 128 + 2 * 13;
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
	if(loopBlocked) return;

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
	if(InputLVGL::getInstance()->getIndev()->group != inputGroup) return;
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

	padTop = lv_obj_create(*this);
	lv_obj_set_size(padTop, 128, 128);

	auto contentContainer = lv_obj_create(*this);
	lv_obj_set_size(contentContainer, 85, 128);
	/*lv_obj_set_style_pad_top(contentContainer, 5, 0);
	lv_obj_set_style_pad_bottom(contentContainer, 5, 0);*/

	itemCont = lv_obj_create(contentContainer);
	lv_obj_set_size(itemCont, 128, LV_SIZE_CONTENT);
	lv_obj_set_flex_flow(itemCont, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_flex_align(itemCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_style_pad_gap(itemCont, 2, 0);
	lv_obj_set_style_pad_hor(itemCont, 5, 0);

	auto onClick = [](lv_event_t* e){
		auto menu = (ProfileScreen*) e->user_data;
		auto index = lv_obj_get_index(e->current_target);
		printf("Clicked: %lu\n", index);
		/*auto game = MenuEntries[index].game; // TODO
		menu->launch(game);*/
	};

	auto onKey = [](lv_event_t* e){
		auto group = (lv_group_t*) e->user_data;
		auto key = *((uint32_t*) e->param);

		const auto index = lv_obj_get_index(e->target); // only applies to odd number of menu items
		const auto itemCount = sizeof(AchievementEntries) / sizeof(AchievementEntries[0]);

		//Maybe simplify logic in these calculations, seems overkill but math should work for any grid width
		if(key == LV_KEY_UP){
			uint8_t moves;
			if(itemCount % RowWidth != 0 && index < RowWidth){
				moves = index + 1 + std::max(((int) itemCount % (int) RowWidth - (int) index - 1), (int) 0);
			}else{
				moves = RowWidth;
			}

			for(uint8_t i = 0; i < moves; ++i){
				lv_group_focus_prev(group);
			}
		}else if(key == LV_KEY_DOWN){
			uint8_t moves;

			if(itemCount % RowWidth != 0){
				if(index >= (itemCount - RowWidth) && index < (itemCount - itemCount % RowWidth)){ //predzadnji redak, elementi koji "vise" iznad ničega
					moves = RowWidth - (index % RowWidth) - 1 + (itemCount % RowWidth);
				}else if(index >= (itemCount - itemCount % RowWidth)){ //zadnji redak
					moves = itemCount % RowWidth;
				}else{
					moves = RowWidth;
				}
			}else{
				moves = RowWidth;
			}

			for(uint8_t i = 0; i < moves; ++i){
				lv_group_focus_next(group);
			}
		}
	};

	items.reserve(sizeof(AchievementEntries) / sizeof(AchievementEntries[0]));
	for(const auto& entry : AchievementEntries){
		const std::string path = imgUnl(entry.icon);

		auto item = new MenuItem(itemCont, path.c_str());
		lv_obj_add_flag(*item, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
		lv_group_add_obj(inputGroup, *item);

		items.push_back(item);

		lv_obj_add_event_cb(*item, onClick, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(*item, onKey, LV_EVENT_KEY, inputGroup);
	}

	lv_obj_refr_size(itemCont);
	lv_obj_refresh_self_size(itemCont);
	lv_group_focus_obj(*items.front());

	// Padding for intro scroll
	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_style_pad_gap(*this, 13, 0);

	auto padBot = lv_obj_create(*this);
	lv_obj_set_size(padBot, 128, lv_obj_get_height(itemCont));
}

std::string ProfileScreen::imgUnl(const char* game){
	std::string path("S:/GameIcons/");
	path.append(game);
	path.append(".bin");
	return path;
}