#include "ProfileScreen.h"
#include "LV_Interface/FSLVGL.h"
#include "LV_Interface/InputLVGL.h"
#include "Util/Services.h"
#include "UIThread.h"
#include "Screens/Settings/SettingsScreen.h"
#include <unordered_set>
#include "Screens/XPBar.h"
#include "Services/XPSystem.h"
#include "LV_Interface/Theme/theme.h"
#include "../MainMenu/MainMenu.h"
#include "Modals/NewRobot.h"
#include "Modals/LockedGame.h"
#include "Modals/UpdateRobot.h"

ProfileScreen::ProfileScreen() : events(12), audio((ChirpSystem*) Services.get(Service::Audio)){
	auto settings = (Settings*) Services.get(Service::Settings);
	oldTheme = settings ? settings->get().theme : Theme::Theme1;

	setupThemes();
	buildUI();
}

ProfileScreen::~ProfileScreen(){}

void ProfileScreen::onStart(){
	Events::listen(Facility::Input, &events);
	Events::listen(Facility::Games, &events);
	Events::listen(Facility::Themes, &events);
	Events::listen(Facility::Pets, &events);
}

void ProfileScreen::onStop(){
	Events::unlisten(&events);

	auto settings = (Settings*) Services.get(Service::Settings);
	auto saved = settings->get();
	saved.avatar = characterSection.getCharacterIndex();
	saved.theme = themeSection->getSelected();
	saved.pet = characterSection.getPet() == Pet::COUNT ? -1 : (int8_t) characterSection.getPet();
	settings->set(saved);
	settings->store();

	if(saved.theme != oldTheme){
		FSLVGL::themeChange();
	}
}

void ProfileScreen::loop(){
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

void ProfileScreen::handleGameInsert(const RobotManager::Event& evt){
	MainMenu::gameEvent({ .action = RobotManager::Event::Remove });

	if(evt.action == RobotManager::Event::Unknown){
		new UpdateRobot(this);
		return;
	}else if(evt.action != RobotManager::Event::Inserted) return;

	auto rob = evt.rob;
	auto isNew = evt.isNew;

	// "Coming soon" games
	std::set<RobotData> comingSoon = {{ Robot::COUNT, Token::Frank },
									  { Robot::COUNT, Token::Fred }};
	if(comingSoon.contains(rob)){
		new UpdateRobot(this);
		return;
	}

	FSLVGL::reloadMenu();

	new NewRobot(this, rob, isNew);
}

void ProfileScreen::handleThemeInsert(const RobotManager::Event& evt){
	MainMenu::gameEvent({ .action = RobotManager::Event::Remove });

	if(evt.action == RobotManager::Event::Unknown){
		new UpdateRobot(this);
		return;
	}else if(evt.action != RobotManager::Event::Inserted) return;

	auto rob = evt.rob;
	auto isNew = evt.isNew;

	// "Coming soon" themes
	std::unordered_set<Token> comingSoon = {};
	if(comingSoon.contains(rob.token)){
		new UpdateRobot(this);
		return;
	}

	delete themeSection;
	themeSection = new ThemePicker(*this);

	buildUI();

	new NewRobot(this, rob, isNew);
}

void ProfileScreen::handlePetInsert(const RobotManager::Event& evt){
	MainMenu::gameEvent({ .action = RobotManager::Event::Remove });

	if(evt.action == RobotManager::Event::Unknown){
		new UpdateRobot(this);
		return;
	}else if(evt.action != RobotManager::Event::Inserted) return;

	auto rob = evt.rob;
	auto isNew = evt.isNew;

	// "Coming soon" pets
	std::unordered_set<Token> comingSoon = {};
	if(comingSoon.contains(rob.token)){
		new UpdateRobot(this);
		return;
	}

	new NewRobot(this, rob, isNew);
}

void ProfileScreen::handleInput(const Input::Data& evt){
	if(InputLVGL::getInstance()->getIndev()->group != inputGroup){
		return;
	}

	if(evt.btn == Input::Menu && evt.action == Input::Data::Release){
		audio->play({{ 350, 350, 80 },
					 { 0,   0,   80 },
					 { 150, 150, 80 },
					 { 0,   0,   80 },
					 { 600, 600, 80 }});

		auto ui = (UIThread*) Services.get(Service::UI);
		ui->startScreen([](){ return std::make_unique<SettingsScreen>(); });
	}

	if(evt.btn == Input::B && evt.action == Input::Data::Press){

		if(achievementSection.isActive() || themeSection->isActive() || characterSection.isEditing()) return;

		auto ui = (UIThread*) Services.get(Service::UI);
		ui->startScreen([](){ return std::make_unique<MainMenu>(); });
	}

	if((evt.btn == Input::Left || evt.btn == Input::Right || evt.btn == Input::Up || evt.btn == Input::Down) && evt.action == Input::Data::Press){
		audio->play({{ 400, 400, 50 }});
	}
}

void ProfileScreen::setupThemes(){
	lv_style_set_bg_color(unfocusedSection, lv_color_white());
}

void ProfileScreen::buildUI(){
	lv_obj_set_size(*this, 128, 128);
	lv_obj_set_style_bg_img_src(*this, "S:/Profile/bg.bin", 0);

	//Character section
	lv_obj_set_align(characterSection, LV_ALIGN_TOP_LEFT);
	lv_obj_set_pos(characterSection, 0, 4);
	lv_group_add_obj(inputGroup, characterSection);
	lv_obj_add_style(characterSection, unfocusedSection, LV_STATE_DEFAULT);
	lv_obj_add_event_cb(characterSection, [](lv_event_t* e){
		auto screen = (ProfileScreen*) e->user_data;
		screen->startAnim(Section::Character);
	}, LV_EVENT_FOCUSED, this);
	lv_obj_add_event_cb(characterSection, [](lv_event_t* e){
		auto screen = (ProfileScreen*) e->user_data;
		screen->stopAnim(Section::Character);
	}, LV_EVENT_DEFOCUSED, this);


	//XP bar
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
	lv_label_set_text(lvlText, ("LVL " + std::to_string(xpSystem->getLevel())).c_str());

	//Achievement section
	achievementOverlay = lv_obj_create(*this);
	lv_obj_add_flag(achievementOverlay, LV_OBJ_FLAG_FLOATING);
	lv_obj_set_align(achievementOverlay, LV_ALIGN_TOP_LEFT);
	lv_obj_set_pos(achievementOverlay, 55, 4);
	lv_obj_set_style_radius(achievementOverlay, 3, 0);
	lv_obj_set_size(achievementOverlay, lv_obj_get_width(achievementSection), lv_obj_get_height(achievementSection));
	lv_obj_add_style(achievementOverlay, unfocusedSection, LV_STATE_DEFAULT);

	lv_obj_set_align(achievementSection, LV_ALIGN_TOP_LEFT);
	lv_obj_set_pos(achievementSection, 55, 4);
	lv_group_add_obj(inputGroup, achievementSection);

	lv_obj_add_event_cb(achievementSection, [](lv_event_t* e){
		auto screen = (ProfileScreen*) e->user_data;
		screen->startAnim(Section::Achievement);
	}, LV_EVENT_FOCUSED, this);
	lv_obj_add_event_cb(achievementSection, [](lv_event_t* e){
		auto screen = (ProfileScreen*) e->user_data;
		screen->stopAnim(Section::Achievement);
	}, LV_EVENT_DEFOCUSED, this);

	lv_obj_move_foreground(achievementOverlay);
	achievementSection.setReturnFunc([this](){ startAnim(Section::Achievement); });

	lv_obj_refr_size(achievementSection);
	lv_obj_refresh_self_size(achievementSection);


	//Theme section
	lv_obj_set_align(*themeSection, LV_ALIGN_TOP_LEFT);
	lv_obj_set_pos(*themeSection, 4, 86);
	lv_obj_add_style(*themeSection, unfocusedSection, LV_STATE_DEFAULT);
	lv_group_add_obj(inputGroup, *themeSection);

	lv_obj_add_event_cb(*themeSection, [](lv_event_t* e){
		auto screen = (ProfileScreen*) e->user_data;
		screen->startAnim(Section::Theme);
	}, LV_EVENT_FOCUSED, this);
	lv_obj_add_event_cb(*themeSection, [](lv_event_t* e){
		auto screen = (ProfileScreen*) e->user_data;
		screen->stopAnim(Section::Theme);
	}, LV_EVENT_DEFOCUSED, this);


	//manual focus definitions
	lv_group_set_editing(inputGroup, true);
	lv_obj_add_event_cb(characterSection, [](lv_event_t* e){
		ProfileScreen* screen = (ProfileScreen*) e->user_data;
		if(screen->characterSection.isEditing()) return;

		switch(lv_event_get_key(e)){
			case LV_KEY_RIGHT:
				lv_group_focus_obj(screen->achievementSection);
				break;
			case LV_KEY_DOWN:
				lv_group_focus_obj(*screen->themeSection);
				break;
			default:
				lv_group_focus_obj(screen->characterSection);
		}
		lv_group_set_editing(screen->inputGroup, true);
	}, LV_EVENT_KEY, this);

	lv_obj_add_event_cb(achievementSection, [](lv_event_t* e){
		ProfileScreen* screen = (ProfileScreen*) e->user_data;
		if(screen->achievementSection.isActive()) return;

		switch(lv_event_get_key(e)){
			case LV_KEY_LEFT:
				lv_group_focus_obj(screen->characterSection);
				break;
			default:
				lv_group_focus_obj(screen->achievementSection);
		}
		lv_group_set_editing(screen->inputGroup, true);
	}, LV_EVENT_KEY, this);

	lv_obj_add_event_cb(*themeSection, [](lv_event_t* e){
		ProfileScreen* screen = (ProfileScreen*) e->user_data;
		if(screen->themeSection->isActive()) return;

		switch(lv_event_get_key(e)){
			case LV_KEY_RIGHT:
				lv_group_focus_obj(screen->achievementSection);
				break;
			case LV_KEY_UP:
				lv_group_focus_obj(screen->characterSection);
				break;
			default:
				lv_group_focus_obj(*screen->themeSection);
		}
		lv_group_set_editing(screen->inputGroup, true);
	}, LV_EVENT_KEY, this);

	lv_obj_add_event_cb(achievementSection, [](lv_event_t* event){
		ProfileScreen* screen = (ProfileScreen*) event->user_data;
		auto ach = &screen->achievementSection;

//		printf("ach click, active: %d\n", ach->isActive());

		if(ach->isActive()){
			screen->stopAnim(Section::Achievement);
		}else{
			screen->startAnim(Section::Achievement);
		}
	}, LV_EVENT_CLICKED, this);

}

static void FocusedSectionAnimCB(void* obj, int32_t v){
	lv_obj_set_style_bg_opa((lv_obj_t*) obj, v, 0);
}

void ProfileScreen::startAnim(ProfileScreen::Section section){
	lv_anim_init(&focusedSectionAnim);
	lv_anim_set_playback_time(&focusedSectionAnim, 250);
	lv_anim_set_repeat_count(&focusedSectionAnim, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_path_cb(&focusedSectionAnim, lv_anim_path_linear);
	lv_anim_set_exec_cb(&focusedSectionAnim, FocusedSectionAnimCB);
	lv_anim_set_values(&focusedSectionAnim, 0, 140);
	lv_obj_t* var;
	switch(section){
		case Section::Achievement:
			var = achievementOverlay;
			break;
		case Section::Theme:
			var = *themeSection;
			break;
		case Section::Character:
			var = characterSection;
			break;
		default:
			var = nullptr;
			break;
	}
	lv_anim_set_var(&focusedSectionAnim, var);

	lv_anim_start(&focusedSectionAnim);
}

void ProfileScreen::stopAnim(Section section){
	lv_obj_t* var;
	switch(section){
		case Section::Achievement:
			var = achievementOverlay;
			break;
		case Section::Theme:
			var = *themeSection;
			break;
		case Section::Character:
			var = characterSection;
			break;
		default:
			var = nullptr;
			break;
	}
	lv_anim_del(var, FocusedSectionAnimCB);

	lv_obj_set_style_bg_opa(var, LV_OPA_TRANSP, 0);
}
