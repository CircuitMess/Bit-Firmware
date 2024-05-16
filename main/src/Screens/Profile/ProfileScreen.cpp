#include "ProfileScreen.h"
#include "LV_Interface/FSLVGL.h"
#include "LV_Interface/InputLVGL.h"
#include "Services/GameManager.h"
#include "Util/Services.h"
#include "UIThread.h"
#include <Modals/LockedGame.h>
#include <Modals/UpdateRobot.h>
#include <Screens/Settings/SettingsScreen.h>
#include <unordered_set>
#include "Screens/XPBar.h"
#include "Services/XPSystem.h"
#include "LV_Interface/Theme/theme.h"

ProfileScreen::ProfileScreen() : events(12), audio((ChirpSystem*) Services.get(Service::Audio)){
	buildUI();
}

ProfileScreen::~ProfileScreen(){}

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
	//lv_obj_scroll_to(*this, 0, 0, LV_ANIM_OFF); // set y to <height> to scroll from top. 0 to scroll from bottom
}

void ProfileScreen::onStart(){
	Events::listen(Facility::Input, &events);
	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), nullptr);

	//lv_obj_scroll_to(*this, 0, 128, LV_ANIM_ON);
}

void ProfileScreen::onStop(){
	Events::unlisten(&events);
}

void ProfileScreen::loop(){
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

	characterSection = lv_obj_create(*this);
	lv_obj_set_size(characterSection, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_align(characterSection, LV_ALIGN_TOP_LEFT);
	lv_obj_set_pos(characterSection, 4, 4);

	auto character = lv_img_create(characterSection);
	lv_img_set_src(character, "S:/Profile/character2-01.bin");
	lv_obj_set_align(character, LV_ALIGN_CENTER);

	auto pet = lv_img_create(characterSection);
	lv_img_set_src(pet, "S:/Profile/pet2.bin");
	lv_obj_set_align(pet, LV_ALIGN_CENTER);
	lv_obj_set_pos(pet, 12, 18);

	auto xpBackground = lv_img_create(*this);
	lv_img_set_src(xpBackground, "S:/Profile/barOutline.bin");
	lv_obj_set_pos(xpBackground, 3, 76);

	const XPSystem* xpSystem = (XPSystem*) Services.get(Service::XPSystem);
	if(xpSystem == nullptr){
		return;
	}

	xpBar = new XPBar(XPBarLength::Short, xpBackground, xpSystem->MapXPToLevel(xpSystem->getXP()).progress);
	lv_obj_set_align(*xpBar, LV_ALIGN_TOP_LEFT);
	lv_obj_set_pos(*xpBar, 1, 1);

	auto lvlText = lv_label_create(*this);
	lv_obj_set_pos(lvlText, 16, 69);
	lv_obj_set_style_text_font(lvlText, &landerfont, 0);
	lv_obj_set_style_text_color(lvlText, lv_color_make(85, 126, 150), 0);
	lv_label_set_text(lvlText, ("LVL " + std::to_string(xpSystem->MapXPToLevel(xpSystem->getXP()).nextLvl - 1)).c_str());

	themeSection = lv_obj_create(*this);
	lv_obj_set_size(themeSection, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_align(themeSection, LV_ALIGN_TOP_LEFT);
	lv_obj_set_pos(themeSection, 4, 86);
	lv_obj_set_size(themeSection, 46, 38);

	lv_obj_set_flex_align(themeSection, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_SPACE_EVENLY);
	lv_obj_set_flex_flow(themeSection, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_style_pad_gap(themeSection, 1, 0);
	lv_obj_set_style_pad_left(themeSection, 1, 0);
	lv_obj_set_style_pad_top(themeSection, 9, 0);

	// TODO: check if themes are locked, if so, show the lock on them as well

	auto img = lv_img_create(themeSection);
	lv_img_set_src(img, "S:/Profile/theme-01-icon.bin");

	img = lv_img_create(themeSection);
	lv_img_set_src(img, "S:/Profile/theme-02-icon.bin");
	auto lock = lv_img_create(img);
	lv_img_set_src(lock, "S:/Profile/lock.bin");
	lv_obj_set_align(lock, LV_ALIGN_CENTER);

	img = lv_img_create(themeSection);
	lv_img_set_src(img, "S:/Profile/theme-03-icon.bin");
	lock = lv_img_create(img);
	lv_img_set_src(lock, "S:/Profile/lock.bin");
	lv_obj_set_align(lock, LV_ALIGN_CENTER);

	img = lv_img_create(themeSection);
	lv_img_set_src(img, "S:/Profile/theme-04-icon.bin");
	lock = lv_img_create(img);
	lv_img_set_src(lock, "S:/Profile/lock.bin");
	lv_obj_set_align(lock, LV_ALIGN_CENTER);
}