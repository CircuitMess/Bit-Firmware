#include "MainMenu.h"
#include "MenuItem.h"
#include "LV_Interface/FSLVGL.h"
#include "Services/GameManager.h"
#include "Util/Services.h"
#include "Util/stdafx.h"
#include "UIThread.h"
#include <Games/TestGame.h>
#include <Modals/NewRobot.h>
#include <Modals/LockedGame.h>
#include <Modals/UnknownRobot.h>
#include <Screens/Settings/SettingsScreen.h>

struct Entry {
	const char* icon;
	Robot rob = Robot::COUNT;
	Games game = Games::COUNT;
};

static constexpr Entry MenuEntries[] = {
		{ .icon = "Blocks", .game = Games::Blocks },
		{ .icon = "Pong", .game = Games::Pong },
		{ .icon = "Snake", .game = Games::Snake },
		{ .icon = "Arte", .rob = Artemis, .game = Games::Artemis },
		{ .icon = "Bee", .rob = MrBee, .game = Games::MrBee },
		{ .icon = "Bob", .rob = Bob, .game = Games::Bob },
		{ .icon = "Butt", .rob = Buttons, .game = Games::Buttons },
		{ .icon = "Capa", .rob = Capacitron, .game = Games::Capacitron },
		{ .icon = "Hertz", .rob = Hertz, .game = Games::Hertz },
		{ .icon = "Marv", .rob = Marv, .game = Games::Marv },
		{ .icon = "Resis", .rob = Resistron, .game = Games::Resistron },
		{ .icon = "Robby", .rob = Robby, .game = Games::Robby }
};

MainMenu::MainMenu() : events(12){
	buildUI();
}

MainMenu::~MainMenu(){

}

void MainMenu::launch(Games game){
	auto games = (GameManager*) Services.get(Service::Games);
	if(!games->isUnlocked(game)){
		const auto rob = GameManager::GameRobot.at(game);
		new LockedGame(this, rob);
		return;
	}

	auto ui = (UIThread*) Services.get(Service::UI);
	ui->startGame(game);
}

void MainMenu::onStarting(){
	const auto height = lv_obj_get_height(itemCont) + 128 + 2*13;
	lv_obj_scroll_to(*this, 0, 0, LV_ANIM_OFF); // set y to <height> to scroll from top. 0 to scroll from bottom
}

void MainMenu::onStart(){
	Events::listen(Facility::Games, &events);
	Events::listen(Facility::Input, &events);
	bg->start();
	lv_obj_scroll_to(*this, 0, 128, LV_ANIM_ON);
}

void MainMenu::onStop(){
	bg->stop();
	Events::unlisten(&events);
}

void MainMenu::loop(){
	batt->loop();

	Event evt{};
	if(events.get(evt, 0)){
		if(evt.facility == Facility::Games){
			auto data = (GameManager::Event*) evt.data;
			handleInsert(*data);
		}else if(evt.facility == Facility::Input){
			auto data = (Input::Data*) evt.data;
			handleInput(*data);
		}
		free(evt.data);
	}
}

void MainMenu::handleInsert(const GameManager::Event& evt){
	if(evt.action == GameManager::Event::Unknown){
		new UnknownRobot(this);
		return;
	}else if(evt.action != GameManager::Event::Inserted) return;

	auto rob = evt.rob;
	auto isNew = evt.isNew;

	if(isNew && robGames.count(rob)){
		MenuItem* item = robGames.at(rob);
		const auto icon = RobotIcons[rob];
		const auto path = imgUnl(icon);
		item->setIcon(path.c_str());
	}

	new NewRobot(this, rob, isNew);
}

void MainMenu::handleInput(const Input::Data& evt){
	if(evt.btn == Input::Menu && evt.action == Input::Data::Release){
		auto ui = (UIThread*) Services.get(Service::UI);
		ui->startScreen([](){ return std::make_unique<SettingsScreen>(); });
	}
}

void MainMenu::buildUI(){
	lv_obj_set_size(*this, 128, 128);
	lv_obj_add_flag(*this, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_scroll_dir(*this, LV_DIR_VER);
	lv_obj_set_scrollbar_mode(*this, LV_SCROLLBAR_MODE_OFF);

	bg = new LVGIF(*this, "S:/bg");
	lv_obj_add_flag(*bg, LV_OBJ_FLAG_FLOATING);
	lv_obj_set_pos(*bg, 0, 0);
	bg->start();

	padTop = lv_obj_create(*this);
	lv_obj_set_size(padTop, 128, 128);

	auto contentContainer = lv_obj_create(*this);
	lv_obj_set_size(contentContainer, 128, 128);
	lv_obj_set_pos(contentContainer, 0, 0);
	lv_obj_add_flag(contentContainer, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
	lv_obj_set_style_pad_ver(contentContainer, 13, 0);

	itemCont = lv_obj_create(contentContainer);
	lv_obj_add_flag(itemCont, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
	lv_obj_set_size(itemCont, 128, LV_SIZE_CONTENT);
	lv_obj_set_flex_flow(itemCont, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_flex_align(itemCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_style_pad_gap(itemCont, 6, 0);
	lv_obj_set_style_pad_hor(itemCont, 19, 0);

	auto onClick = [](lv_event_t* e){
		auto menu = (MainMenu*) e->user_data;
		auto index = lv_obj_get_index(e->current_target);
		auto game = MenuEntries[index].game;
		menu->launch(game);
	};

	auto onKey = [](lv_event_t* e){
		auto group = (lv_group_t*) e->user_data;
		auto key = *((uint32_t*) e->param);
		if(key == LV_KEY_UP){
			lv_group_focus_prev(group);
			lv_group_focus_prev(group);
		}else if(key == LV_KEY_DOWN){
			lv_group_focus_next(group);
			lv_group_focus_next(group);
		}
	};

	auto games = (GameManager*) Services.get(Service::Games);
	items.reserve(sizeof(MenuEntries) / sizeof(MenuEntries[0]));
	for(const auto& entry : MenuEntries){
		std::string path;
		if(entry.rob == COUNT || entry.game == Games::COUNT || games->isUnlocked(entry.game)){
			path = imgUnl(entry.icon);
		}else{
			path = imgLoc(entry.icon);
		}

		auto item = new MenuItem(itemCont, path.c_str());
		lv_obj_add_flag(*item, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
		lv_group_add_obj(inputGroup, *item);

		items.push_back(item);
		if(entry.rob != COUNT){
			robGames.insert(std::make_pair(entry.rob, item));
		}

		lv_obj_add_event_cb(*item, onClick, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(*item, onKey, LV_EVENT_KEY, inputGroup);
	}

	lv_obj_refr_size(itemCont);
	lv_obj_refresh_self_size(itemCont);
	lv_group_focus_obj(*items.front());

	// Battery
	batt = new BatteryElement(*this);
	lv_obj_add_flag(*batt, LV_OBJ_FLAG_FLOATING);
	lv_obj_align(*batt, LV_ALIGN_TOP_RIGHT, -2, 2);

	// Padding for intro scroll
	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);

	auto padBot = lv_obj_create(*this);
	lv_obj_set_size(padBot, 128, lv_obj_get_height(itemCont));
}

std::string MainMenu::imgUnl(const char* game){
	std::string path("S:/GameIcons/");
	path.append(game);
	path.append(".bin");
	return path;
}

std::string MainMenu::imgLoc(const char* game){
	std::string path("S:/GameIcons/");
	path.append(game);
	path.append("L.bin");
	return path;
}
