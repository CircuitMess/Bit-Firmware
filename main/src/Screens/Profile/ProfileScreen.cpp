#include "ProfileScreen.h"
#include "LV_Interface/FSLVGL.h"
#include "LV_Interface/InputLVGL.h"
#include "Services/GameManager.h"
#include "Util/Services.h"
#include "UIThread.h"
#include "Screens/Settings/SettingsScreen.h"
#include <unordered_set>
#include "Screens/XPBar.h"
#include "Services/XPSystem.h"
#include "LV_Interface/Theme/theme.h"

ProfileScreen::ProfileScreen() : events(12), audio((ChirpSystem*) Services.get(Service::Audio)){
	buildUI();
}

ProfileScreen::~ProfileScreen(){}

void ProfileScreen::onStart(){
	Events::listen(Facility::Input, &events);
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

	auto onFocus = [](lv_event_t * e){
		lv_obj_t* obj = (lv_obj_t*) e->user_data;
		lv_obj_set_style_bg_color(obj, lv_color_make(0xFF, 0, 0), 0);
		lv_obj_set_style_bg_opa(obj, 100, 0);
	};

	auto onUnfocus = [](lv_event_t * e){
		lv_obj_t* obj = (lv_obj_t*) e->user_data;
		lv_obj_set_style_bg_color(obj, lv_color_make(0xFF, 0, 0), 0);
		lv_obj_set_style_bg_opa(obj, 0, 0);
	};

	characterSection = lv_obj_create(*this);
	lv_obj_set_size(characterSection, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_align(characterSection, LV_ALIGN_TOP_LEFT);
	lv_obj_set_pos(characterSection, 4, 4);
	lv_group_add_obj(inputGroup, characterSection);
	lv_obj_add_event_cb(characterSection, onFocus, LV_EVENT_FOCUSED, characterSection);
	lv_obj_add_event_cb(characterSection, onUnfocus, LV_EVENT_DEFOCUSED, characterSection);
	lv_obj_add_event_cb(characterSection, ProfileScreen::onClick, LV_EVENT_KEY, this);

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
	lv_obj_set_align(themeSection, LV_ALIGN_TOP_LEFT);
	lv_obj_set_pos(themeSection, 4, 86);
	lv_obj_set_size(themeSection, 46, 38);
	lv_group_add_obj(inputGroup, themeSection);
	lv_obj_add_event_cb(themeSection, onFocus, LV_EVENT_FOCUSED, themeSection);
	lv_obj_add_event_cb(themeSection, onUnfocus, LV_EVENT_DEFOCUSED, themeSection);
	lv_obj_add_event_cb(themeSection, ProfileScreen::onClick, LV_EVENT_KEY, this);

	lv_obj_set_flex_align(themeSection, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_SPACE_EVENLY);
	lv_obj_set_flex_flow(themeSection, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_style_pad_gap(themeSection, 1, 0);
	lv_obj_set_style_pad_left(themeSection, 1, 0);
	lv_obj_set_style_pad_top(themeSection, 9, 0);

	// TODO: check if themes are locked, if so, show the lock on them as well

	auto img = lv_img_create(themeSection);
	lv_img_set_src(img, "S:/Profile/theme-01-icon.bin");
	lv_group_add_obj(inputGroup, img);
	lv_obj_add_event_cb(img, onFocus, LV_EVENT_FOCUSED, img);
	lv_obj_add_event_cb(img, onUnfocus, LV_EVENT_DEFOCUSED, img);
	lv_obj_add_event_cb(img, ProfileScreen::onClick, LV_EVENT_KEY, this);

	img = lv_img_create(themeSection);
	lv_img_set_src(img, "S:/Profile/theme-02-icon.bin");
	auto lock = lv_img_create(img);
	lv_img_set_src(lock, "S:/Profile/lock.bin");
	lv_obj_set_align(lock, LV_ALIGN_CENTER);
	lv_group_add_obj(inputGroup, img);
	lv_obj_add_event_cb(img, onFocus, LV_EVENT_FOCUSED, img);
	lv_obj_add_event_cb(img, onUnfocus, LV_EVENT_DEFOCUSED, img);
	lv_obj_add_event_cb(img, ProfileScreen::onClick, LV_EVENT_KEY, this);

	img = lv_img_create(themeSection);
	lv_img_set_src(img, "S:/Profile/theme-03-icon.bin");
	lock = lv_img_create(img);
	lv_img_set_src(lock, "S:/Profile/lock.bin");
	lv_obj_set_align(lock, LV_ALIGN_CENTER);
	lv_group_add_obj(inputGroup, img);
	lv_obj_add_event_cb(img, onFocus, LV_EVENT_FOCUSED, img);
	lv_obj_add_event_cb(img, onUnfocus, LV_EVENT_DEFOCUSED, img);
	lv_obj_add_event_cb(img, ProfileScreen::onClick, LV_EVENT_KEY, this);

	img = lv_img_create(themeSection);
	lv_img_set_src(img, "S:/Profile/theme-04-icon.bin");
	lock = lv_img_create(img);
	lv_img_set_src(lock, "S:/Profile/lock.bin");
	lv_obj_set_align(lock, LV_ALIGN_CENTER);
	lv_group_add_obj(inputGroup, img);
	lv_obj_add_event_cb(img, onFocus, LV_EVENT_FOCUSED, img);
	lv_obj_add_event_cb(img, onUnfocus, LV_EVENT_DEFOCUSED, img);
	lv_obj_add_event_cb(img, ProfileScreen::onClick, LV_EVENT_KEY, this);

	achievementSection = lv_obj_create(*this);
	lv_obj_add_flag(achievementSection, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_align(achievementSection, LV_ALIGN_TOP_LEFT);
	lv_obj_set_pos(achievementSection, 50, 4);
	lv_obj_set_size(achievementSection, 78, 123);

	lv_obj_set_flex_align(achievementSection, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_flex_flow(achievementSection, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_add_flag(achievementSection, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_gap(achievementSection, 0, 0);

	// TODO once all achievements are added, do this for every achievement instead of just random placeholders
	for(size_t i = 0; i < AchievementCount; ++i){
		auto base = lv_obj_create(achievementSection);
		lv_obj_set_size(base, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_add_flag(base, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
		lv_obj_add_flag(base, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
		lv_group_add_obj(inputGroup, base);

		lv_obj_add_event_cb(base, ProfileScreen::onClick, LV_EVENT_KEY, this);

		auto ach = lv_img_create(base);
		lv_img_set_src(ach, "S:/Profile/ach-placeholder.bin");
		lv_obj_set_align(ach, LV_ALIGN_CENTER);

		auto border = lv_img_create(base);
		lv_img_set_src(border, "S:/Profile/ach-border.bin"); // TODO this file should get removed when everything when visuals are done
		lv_obj_set_align(border, LV_ALIGN_CENTER);

		lv_obj_add_event_cb(base, onFocus, LV_EVENT_FOCUSED, border);
		lv_obj_add_event_cb(base, onUnfocus, LV_EVENT_DEFOCUSED, border);
	}

	lv_obj_refr_size(achievementSection);
	lv_obj_refresh_self_size(achievementSection);
	lv_group_focus_obj(lv_obj_get_child(achievementSection, 0));
}

void ProfileScreen::onClick(lv_event_t * e){
	return;

	ProfileScreen* screen = (ProfileScreen*) e->user_data;
	if(screen == nullptr){
		return;
	}

	const uint32_t key = *((uint32_t*) e->param);
	printf("Key: %lu\n", key);
	printf("Focused: %d\n", screen->focusedIndex);

	if(screen->focusedIndex == ThemeInputIndex && key == LV_KEY_ENTER){
		++screen->focusedIndex;
		lv_group_focus_next(screen->inputGroup);
		screen->pickingTheme = true;
		return;
	}

	if(screen->focusedIndex == CharacterInputIndex && key == LV_KEY_ENTER){
		screen->pickingCharacter = true;
		return;
	}

	if(screen->pickingTheme){
		if(key == LV_KEY_ESC){
			screen->pickingTheme = false;

			while(screen->focusedIndex > ThemeInputIndex){
				--screen->focusedIndex;
				lv_group_focus_prev(screen->inputGroup);
			}

			return;
		}

		// TODO go through themes
	}else if(screen->pickingCharacter){
		if(key == LV_KEY_ESC){
			screen->pickingCharacter = false;
			return;
		}

		// TODO switch character and pet icons
	}else{
		if(key == LV_KEY_UP){
			if(screen->focusedIndex == ThemeInputIndex){
				--screen->focusedIndex;
				lv_group_focus_prev(screen->inputGroup);
			}else if(screen->focusedIndex == CharacterInputIndex){
				++screen->focusedIndex;
				lv_group_focus_next(screen->inputGroup);
			}else if(screen->focusedIndex >= AchievementInputIndex){
				// TODO
			}

			return;
		}

		if(key == LV_KEY_DOWN){
			if(screen->focusedIndex == ThemeInputIndex){
				--screen->focusedIndex;
				lv_group_focus_prev(screen->inputGroup);
			}else if(screen->focusedIndex == CharacterInputIndex){
				++screen->focusedIndex;
				lv_group_focus_next(screen->inputGroup);
			}else if(screen->focusedIndex >= AchievementInputIndex){
				// TODO
			}

			return;
		}

		if(key == LV_KEY_LEFT){
			if(screen->focusedIndex == ThemeInputIndex){
				// TODO
			}else if(screen->focusedIndex == CharacterInputIndex){
				// TODO
			}else if(screen->focusedIndex >= AchievementInputIndex){
				if(screen->focusedIndex % AchievementColumns == 0){
					while(screen->focusedIndex > CharacterInputIndex){
						--screen->focusedIndex;
						lv_group_focus_prev(screen->inputGroup);
					}
				}else{
					--screen->focusedIndex;
					lv_group_focus_prev(screen->inputGroup);
				}
			}

			return;
		}

		if(key == LV_KEY_RIGHT){
			if(screen->focusedIndex == ThemeInputIndex){
				while(screen->focusedIndex < AchievementInputIndex){
					++screen->focusedIndex;
					lv_group_focus_next(screen->inputGroup);
				}
			}else if(screen->focusedIndex == CharacterInputIndex){
				while(screen->focusedIndex < AchievementInputIndex){
					++screen->focusedIndex;
					lv_group_focus_next(screen->inputGroup);
				}
			}else if(screen->focusedIndex >= AchievementInputIndex){
				if(screen->focusedIndex % AchievementColumns == 3){
					while(screen->focusedIndex > CharacterInputIndex){
						--screen->focusedIndex;
						lv_group_focus_prev(screen->inputGroup);
					}
				}else{
					++screen->focusedIndex;
					lv_group_focus_next(screen->inputGroup);
				}
			}

			return;
		}
	}
}