#include "MainMenu.h"
#include "MenuItem.h"
#include "LV_Interface/FSLVGL.h"
#include "LV_Interface/InputLVGL.h"
#include "Services/RobotManager.h"
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
#include "MenuHeader.h"
#include "Screens/Game/GameSplashScreen.h"
#include "Filepaths.hpp"
#include "../Profile/ProfileScreen.h"

uint8_t MainMenu::lastCursor = 0;

struct Entry {
	const char* icon;
	RobotData rob = { Robot::COUNT, Token::COUNT };
	Games game = Games::COUNT;
};

static constexpr Entry MenuEntries[] = {
		{ .icon = "Blocks", .game = Games::Blocks },
		{ .icon = "Pong", .game = Games::Pong },
		{ .icon = "Snake", .game = Games::Snake },
		{ .icon = "Stacky", .game = Games::WackyStacky },
		{ .icon = "Arte", .rob = { .robot = Robot::Artemis }, .game = Games::Artemis },
		{ .icon = "Bee", .rob = { .robot = Robot::MrBee }, .game = Games::MrBee },
		{ .icon = "Bob", .rob = { .robot = Robot::Bob }, .game = Games::Bob },
		{ .icon = "Butt", .rob = { .robot = Robot::Buttons }, .game = Games::Buttons },
		{ .icon = "Capa", .rob = { .robot = Robot::Capacitron }, .game = Games::Capacitron },
		{ .icon = "Hertz", .rob = { .robot = Robot::Hertz }, .game = Games::Hertz },
		{ .icon = "Marv", .rob = { .robot = Robot::Marv }, .game = Games::Marv },
		{ .icon = "Resis", .rob = { .robot = Robot::Resistron }, .game = Games::Resistron },
		{ .icon = "Robby", .rob = { .robot = Robot::Robby }, .game = Games::Robby },
		{ .icon = "Harald", .rob = { .token = Token::Harald }, .game = Games::Harald },
		{ .icon = "Charlie", .rob = { .token = Token::Charlie }, .game = Games::Charlie },
		{ .icon = "Planck", .rob = { .token = Token::Planck }, .game = Games::Planck },
		{ .icon = "Dusty", .rob = { .token = Token::Dusty }, .game = Games::Dusty },
		{ .icon = "Sparkly", .rob = { .token = Token::Sparkly }, .game = Games::Sparkly },
};

std::optional<RobotManager::Event> MainMenu::gmEvt = std::nullopt;
std::atomic<bool> MainMenu::running = false;

MainMenu::MainMenu(bool delayed) : delayed(delayed), events(12), audio((ChirpSystem*) Services.get(Service::Audio)){
	buildUI();
}

MainMenu::~MainMenu(){
	lv_obj_remove_event_cb(*this, onScrollEnd); // just in case
}

void MainMenu::launch(Games game){
	auto games = (RobotManager*) Services.get(Service::RobotManager);
	if(!games->isUnlocked(game)){
		auto audio = (ChirpSystem*) Services.get(Service::Audio);
		audio->play({ { 300, 300, 50 },
					  { 0,   0,   50 },
					  { 200, 200, 250 } });
		const auto rob = RobotManager::GameRobot.at(game);
		new LockedGame(this, rob);
		return;
	}

	auto ui = (UIThread*) Services.get(Service::UI);
	ui->startScreen([game](){ return std::make_unique<GameSplashScreen>(game); });
}

void MainMenu::onStarting(){
	const auto height = lv_obj_get_height(itemCont) + 128 + 2 * 13;
	lv_obj_scroll_to(*this, 0, 0, LV_ANIM_OFF); // set y to <height> to scroll from top. 0 to scroll from bottom

	loopBlocked = true;
}

void MainMenu::onStart(){
	Events::listen(Facility::Games, &events);
	Events::listen(Facility::Pets, &events);
	Events::listen(Facility::Themes, &events);
	Events::listen(Facility::Input, &events);

	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), nullptr);

	if(!delayed){
		lv_obj_scroll_to(*this, 0, 128, LV_ANIM_ON);
	}else{
		startTime = millis();
	}

	lv_obj_add_event_cb(*this, MainMenu::onScrollEnd, LV_EVENT_SCROLL_END, this);
}

void MainMenu::onScrollEnd(lv_event_t* evt){
	auto menu = (MainMenu*) evt->user_data;
	lv_obj_remove_event_cb(*menu, onScrollEnd);

	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), menu->inputGroup);

	running = true;
	if(gmEvt.has_value()){
		if(Robots::isGame(gmEvt->rob)){
			menu->handleGameInsert(*gmEvt);
		}else if(Robots::isTheme(gmEvt->rob)){
			menu->handleThemeInsert(*gmEvt);
		}else if(Robots::isPet(gmEvt->rob)){
			menu->handlePetInsert(*gmEvt);
		}

		menu->handleGameInsert(*gmEvt);
		gmEvt.reset();
	}

	menu->loopBlocked = false;
	menu->events.reset();
}

void MainMenu::onStop(){
	Events::unlisten(&events);
	lv_obj_remove_event_cb(*this, onScrollEnd);

	auto obj = lv_group_get_focused(inputGroup);
	if(obj == menuHeader->operator lv_obj_t *()){
		lastCursor = 0;
	}else{
		lastCursor = lv_obj_get_index(obj) + 1;
	}

	gmEvt.reset();
	running = false;
}

void MainMenu::loop(){
	batt->loop();

	if(loopBlocked){
		if(delayed && millis() - startTime > 100){
			delayed = false;
			startTime = 0;
			lv_obj_scroll_to(*this, 0, 128, LV_ANIM_ON);
		}

		return;
	}

	Event evt{};
	if(events.get(evt, 0)){
		auto data = (RobotManager::Event*) evt.data;

		if(evt.facility == Facility::Games){
			handleGameInsert(*data);
		}else if(evt.facility == Facility::Themes){
			handleThemeInsert(*data);
		}else if(evt.facility == Facility::Pets){
			handlePetInsert(*data);
		}else if(evt.facility == Facility::Input){
			auto inputData = (Input::Data*) evt.data;
			handleInput(*inputData);
		}
		free(evt.data);
	}
}

void MainMenu::handleGameInsert(const RobotManager::Event& evt){
	if(evt.action == RobotManager::Event::Unknown){
		new UpdateRobot(this);
		return;
	}else if(evt.action != RobotManager::Event::Inserted) return;

	auto rob = evt.rob;
	auto isNew = evt.isNew;

	// "Coming soon" games
	std::set<RobotData> comingSoon = { { Robot::COUNT, Token::Frank }, { Robot::COUNT, Token::Fred } };
	if(comingSoon.contains(rob)){
		new UpdateRobot(this);
		return;
	}

	if(isNew && robGames.count(rob.robot >= Robot::COUNT ? (uint8_t) Robot::COUNT + (uint8_t) rob.token : (uint8_t) rob.robot)){
		FSLVGL::reloadMenu();
		MenuItem* item = robGames.at(rob.robot >= Robot::COUNT ? (uint8_t) Robot::COUNT + (uint8_t) rob.token : (uint8_t) rob.robot);
		item->setLocked(false);
	}

	new NewRobot(this, rob, isNew);
}

void MainMenu::handleThemeInsert(const RobotManager::Event& evt){
	if(evt.action == RobotManager::Event::Unknown){
		new UpdateRobot(this);
		return;
	}else if(evt.action != RobotManager::Event::Inserted) return;

	auto rob = evt.rob;
	auto isNew = evt.isNew;

	// "Coming soon" themes
	std::unordered_set<Token> comingSoon = { };
	if(comingSoon.contains(rob.token)){
		new UpdateRobot(this);
		return;
	}

	new NewRobot(this, rob, isNew);
}

void MainMenu::handlePetInsert(const RobotManager::Event& evt){
	if(evt.action == RobotManager::Event::Unknown){
		new UpdateRobot(this);
		return;
	}else if(evt.action != RobotManager::Event::Inserted) return;

	auto rob = evt.rob;
	auto isNew = evt.isNew;

	// "Coming soon" pets
	std::unordered_set<Token> comingSoon = { };
	if(comingSoon.contains(rob.token)){
		new UpdateRobot(this);
		return;
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

void MainMenu::gameEvent(RobotManager::Event evt){
	if(running) return;
	if(evt.action == RobotManager::Event::Remove){
		gmEvt.reset();
	}else{
		gmEvt = { evt };
	}
}

void MainMenu::buildUI(){
	const Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	lv_obj_set_size(*this, 128, 128);

	auto img = lv_img_create(*this);
	lv_img_set_src(img, THEMED_FILE(Background, settings->get().theme));
	lv_obj_add_flag(img, LV_OBJ_FLAG_FLOATING);

	padTop = lv_obj_create(*this);
	lv_obj_set_size(padTop, 128, 128);

	auto contentContainer = lv_obj_create(*this);
	lv_obj_set_size(contentContainer, 128, 115);
	lv_obj_set_style_pad_top(contentContainer, 3, 0);
	lv_obj_set_style_pad_bottom(contentContainer, 3, 0);

	itemCont = lv_obj_create(contentContainer);
	lv_obj_set_size(itemCont, 128, LV_SIZE_CONTENT);
	lv_obj_set_flex_flow(itemCont, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_flex_align(itemCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_style_pad_gap(itemCont, 2, 0);
	lv_obj_set_style_pad_hor(itemCont, 5, 0);
	lv_obj_set_style_pad_ver(itemCont, 2, 0);

	auto onClick = [](lv_event_t* e){
		auto menu = (MainMenu*) e->user_data;
		auto index = lv_obj_get_index(e->current_target);
		auto game = MenuEntries[index].game;
		menu->launch(game);
	};

	auto onKey = [](lv_event_t* e){
		auto group = (lv_group_t*) e->user_data;
		auto key = *((uint32_t*) e->param);

		const auto index = lv_obj_get_index(e->target); // only applies to odd number of menu items
		const auto itemCount = sizeof(MenuEntries) / sizeof(MenuEntries[0]);

		//Maybe simplify logic in these calculations, seems overkill but math should work for any grid width
		if(key == LV_KEY_UP){
			uint8_t moves;
			if(index < RowWidth){
				moves = index + 1; // towards MenuHeader
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
					moves = 0; //no wrap
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

	menuHeader = new MenuHeader(*this);
	lv_obj_add_flag(*menuHeader, LV_OBJ_FLAG_FLOATING);
	lv_obj_set_pos(*menuHeader, 0, 0);
	lv_group_add_obj(inputGroup, *menuHeader);

	lv_obj_add_event_cb(*menuHeader, [](lv_event_t* e){
		auto group = (lv_group_t*) e->user_data;
		auto key = *((uint32_t*) e->param);

		if(key == LV_KEY_UP) return;

		if(key == LV_KEY_DOWN){
			lv_group_focus_next(group);
		}

	}, LV_EVENT_KEY, inputGroup);
	lv_obj_add_event_cb(*menuHeader, [](lv_event_t* e){
		auto ui = (UIThread*) Services.get(Service::UI);
		ui->startScreen([](){ return std::make_unique<ProfileScreen>(); });
	}, LV_EVENT_CLICKED, this);

	auto games = (RobotManager*) Services.get(Service::RobotManager);
	items.reserve(sizeof(MenuEntries) / sizeof(MenuEntries[0]));
	for(const auto& entry : MenuEntries){
		const std::string path = imgFullPath(entry.icon);
		const std::string pathGrayscale = imgGrayscalePath(entry.icon);
		bool locked = true;
		if((entry.rob.robot == Robot::COUNT && entry.rob.token == Token::COUNT) || entry.game == Games::COUNT || games->isUnlocked(entry.game)){
			locked = false;
		}

		auto item = new MenuItem(itemCont, path, pathGrayscale, locked);
		lv_obj_add_flag(*item, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
		lv_group_add_obj(inputGroup, *item);

		items.push_back(item);
		if(entry.rob.robot != Robot::COUNT || entry.rob.token != Token::COUNT){
			robGames.insert(std::make_pair(entry.rob.robot >= Robot::COUNT ? (uint8_t) Robot::COUNT + (uint8_t) entry.rob.token : (uint8_t) entry.rob.robot, item));
		}

		lv_obj_add_event_cb(*item, onClick, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(*item, onKey, LV_EVENT_KEY, inputGroup);
	}

	lv_obj_refr_size(itemCont);
	lv_obj_refresh_self_size(itemCont);
	lv_group_set_wrap(inputGroup, false);

	// Battery
	batt = new BatteryElement(*this);
	lv_obj_add_flag(*batt, LV_OBJ_FLAG_FLOATING);
	lv_obj_set_pos(*batt, 108, 0);

	// Padding for intro scroll
	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_style_pad_gap(*this, 13, 0);

	auto padBot = lv_obj_create(*this);
	lv_obj_set_size(padBot, 128, lv_obj_get_height(itemCont));

	lv_obj_t* obj;
	if(lastCursor == 0){
		obj = menuHeader->operator lv_obj_t *();
	}else{
		obj = lv_obj_get_child(itemCont, lastCursor-1);
	}
	lv_group_focus_obj(obj);
}

std::string MainMenu::imgFullPath(const char* game){
	std::string path("S:/Menu/");
	path.append(game);
	path.append(".bin");
	return path;
}

std::string MainMenu::imgGrayscalePath(const char* game){
	std::string path("S:/Menu/BW/");
	path.append(game);
	path.append(".bin");
	return path;
}
