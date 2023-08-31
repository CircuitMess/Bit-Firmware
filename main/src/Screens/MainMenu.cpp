#include "MainMenu.h"
#include "MenuItem.h"
#include "LV_Interface/FSLVGL.h"
#include "LV_Interface/InputLVGL.h"
#include "Services/GameManager.h"
#include "Util/Services.h"
#include "Util/stdafx.h"
#include "UIThread.h"
#include <Games/TestGame.h>
#include <Modals/NewRobot.h>
#include <Modals/LockedGame.h>
#include <Modals/UpdateRobot.h>
#include <Modals/UnknownRobot.h>
#include <Screens/Settings/SettingsScreen.h>
#include <unordered_set>
#include "Util/Notes.h"

struct Entry {
	const char* icon;
	Robot rob = Robot::COUNT;
	Games game = Games::COUNT;
};

static constexpr Entry MenuEntries[] = {
		{ .icon = "Blocks", .game = Games::Blocks },
		{ .icon = "Pong", .game = Games::Pong },
		{ .icon = "Snake", .game = Games::Snake },
		// { .icon = "Arte", .rob = Artemis, .game = Games::Artemis },
		{ .icon = "Bee", .rob = MrBee, .game = Games::MrBee },
		{ .icon = "Bob", .rob = Bob, .game = Games::Bob },
		// { .icon = "Butt", .rob = Buttons, .game = Games::Buttons },
		{ .icon = "Capa", .rob = Capacitron, .game = Games::Capacitron },
		{ .icon = "Hertz", .rob = Hertz, .game = Games::Hertz },
		{ .icon = "Marv", .rob = Marv, .game = Games::Marv },
		{ .icon = "Resis", .rob = Resistron, .game = Games::Resistron },
		 { .icon = "Robby", .rob = Robby, .game = Games::Robby }
};

std::optional<GameManager::Event> MainMenu::gmEvt = std::nullopt;
std::atomic<bool> MainMenu::running = false;

MainMenu::MainMenu() : events(12), audio((ChirpSystem*) Services.get(Service::Audio)){
	buildUI();
}

MainMenu::~MainMenu(){
	lv_obj_remove_event_cb(*this, onScrollEnd); // just in case
}

void MainMenu::launch(Games game){
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

	audio->play({ { 100, 100, 80 },
				  { 0,   0,   80 },
				  { 300, 300, 80 },
				  { 0,   0,   80 },
				  { 500, 500, 80 } });

	auto ui = (UIThread*) Services.get(Service::UI);
	ui->startGame(game);
}

void MainMenu::onStarting(){
	const auto height = lv_obj_get_height(itemCont) + 128 + 2 * 13;
	lv_obj_scroll_to(*this, 0, 0, LV_ANIM_OFF); // set y to <height> to scroll from top. 0 to scroll from bottom

	loopBlocked = true;
}

void MainMenu::onStart(){
	Events::listen(Facility::Games, &events);
	Events::listen(Facility::Input, &events);
	bg->start();
	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), nullptr);

	lv_obj_scroll_to(*this, 0, 128, LV_ANIM_ON);
	lv_obj_add_event_cb(*this, MainMenu::onScrollEnd, LV_EVENT_SCROLL_END, this);
}

void MainMenu::onScrollEnd(lv_event_t* evt){
	auto menu = (MainMenu*) evt->user_data;
	lv_obj_remove_event_cb(*menu, onScrollEnd);

	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), menu->inputGroup);

	running = true;
	if(gmEvt.has_value()){
		menu->handleInsert(*gmEvt);
		gmEvt.reset();
	}

	menu->loopBlocked = false;
	menu->events.reset();
}

void MainMenu::onStop(){
	bg->stop();
	Events::unlisten(&events);
	lv_obj_remove_event_cb(*this, onScrollEnd);

	gmEvt.reset();
	running = false;
}

void MainMenu::loop(){
	batt->loop();

	if(loopBlocked) return;

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
		new UpdateRobot(this);
		return;
	}else if(evt.action != GameManager::Event::Inserted) return;

	auto rob = evt.rob;
	auto isNew = evt.isNew;

	std::unordered_set<Robot> comingSoon = { Robot::Artemis, Robot::Buttons };
	if(comingSoon.contains(rob)){
		new UpdateRobot(this);
		return;
	}

	if(isNew && robGames.count(rob)){
		MenuItem* item = robGames.at(rob);
		const auto icon = RobotIcons[rob];
		const auto path = imgUnl(icon);
		item->setIcon(path.c_str());
	}

	new NewRobot(this, rob, isNew);
}

void MainMenu::handleInput(const Input::Data& evt){
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

void MainMenu::gameEvent(GameManager::Event evt){
	if(running) return;
	if(evt.action == GameManager::Event::Remove){
		gmEvt.reset();
	}else{
		gmEvt = { evt };
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
		auto index = lv_obj_get_index(e->target); // TODO: only applies to odd number of menu items; remove once all games are added
		if(key == LV_KEY_UP){
			if(index == 0){
				lv_group_focus_prev(group);
			}else{
				lv_group_focus_prev(group);
				lv_group_focus_prev(group);
			}
		}else if(key == LV_KEY_DOWN){
			if(index == 7 || index == 8){
				lv_group_focus_next(group);
			}else{
				lv_group_focus_next(group);
				lv_group_focus_next(group);
			}
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
	lv_obj_align(*batt, LV_ALIGN_TOP_RIGHT, -2, 8);

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
