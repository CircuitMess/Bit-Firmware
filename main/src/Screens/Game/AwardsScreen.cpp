#include "AwardsScreen.h"
#include "Devices/Input.h"
#include "LV_Interface/InputLVGL.h"
#include "UIThread.h"
#include "Util/Services.h"
#include "HighScoreScreen.h"
#include "Periph/NVSFlash.h"
#include <sstream>
#include "Util/stdafx.h"
#include "Services/XPSystem.h"
#include "GameMenuScreen.h"
#include "Screens/XPBar.h"
#include "Util/Notes.h"
#include "Settings/Settings.h"
#include "Filepaths.hpp"
#include "Screens/AchievementElement.h"

AwardsScreen::AwardsScreen(Games current, uint32_t highScore, uint32_t xp, std::vector<AchievementData>& achievements) :
		highScore(highScore), xp(xp), achievements(std::move(achievements)), evts(6), currentGame(current), start(millis()){
	const HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore);
	if(hsm == nullptr){
		return;
	}

	const XPSystem* xpSystem = (XPSystem*) Services.get(Service::XPSystem);
	if(xpSystem == nullptr){
		return;
	}

	if(xp > 0){
		setAwardMode(Award::XP);
	}else if(!this->achievements.empty()){
		setAwardMode(Award::Achievement);
	}else if(hsm->isHighScore(currentGame, highScore)){
		setAwardMode(Award::HighScore);
	}else{
		exit();
	}
}

void AwardsScreen::setAwardMode(Award award){
	lastChange = millis();
	chirped = false;

	const Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	const XPSystem* xpSystem = (XPSystem*) Services.get(Service::XPSystem);
	if(xpSystem == nullptr){
		return;
	}

	if(levelSet == 0){
		levelSet = xpSystem->getLevel();
	}else if(award == Award::LevelUp){
		levelSet = std::min(levelSet+1, (uint32_t) XPSystem::MaxLevel);
	}

	ChirpSystem* chirp = (ChirpSystem*) Services.get(Service::Audio);
	if(chirp == nullptr){
		return;
	}

	if(award == Award::LevelUp){
		chirp->play({{ NOTE_C6, NOTE_C6, 90 },
					 { 0, 0, 20 },
					 { NOTE_C6, NOTE_C6, 90 },
					 { NOTE_G6, NOTE_G6, 200 }});
	}else if(award == Award::HighScore){
		chirp->play({{ NOTE_G6, NOTE_G6, 100 },
					 { NOTE_C6, NOTE_C6, 300 }});
	}else if(award == Award::XP && levelSet != XPSystem::MaxLevel){
		chirp->play({{ NOTE_C3, NOTE_C6, AnimLength }});
	}

	lv_obj_clean(*this);
	lv_obj_invalidate(*this);
	itemStyle = {};

	auto bg = lv_img_create(*this);
	lv_img_set_src(bg, THEMED_FILE(Background, settings->get().theme));

	auto bgSmall = lv_img_create(*this);
	lv_img_set_src(bgSmall, Filepath::Modal);
	lv_obj_align(bgSmall, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_flag(bgSmall, LV_OBJ_FLAG_FLOATING);
	lv_obj_center(bgSmall);


	rest = lv_obj_create(*this);
	lv_obj_set_size(rest, 102, 92);
	lv_obj_set_style_pad_all(rest, 8, 0);
	lv_obj_set_style_pad_left(rest, 5, 0);
	lv_obj_set_style_pad_bottom(rest, 5, 0);
	lv_obj_set_flex_flow(rest, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(rest, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_align(rest, LV_ALIGN_CENTER);
	lv_obj_add_flag(rest, LV_OBJ_FLAG_FLOATING);
	lv_obj_center(rest);

	if(award == Award::HighScore){
		lv_style_set_width(itemStyle, lv_pct(86));
		lv_style_set_height(itemStyle, 16);
		lv_style_set_border_width(itemStyle, 1);
		lv_style_set_border_color(itemStyle, lv_color_make(85, 126, 150));
		lv_style_set_border_opa(itemStyle, LV_OPA_COVER);
		lv_style_set_radius(itemStyle, 2);
		lv_style_set_align(itemStyle, LV_ALIGN_CENTER);

		lv_obj_t* enterImg = lv_img_create(rest);
		lv_img_set_src(enterImg, "S:/Award/highscore.bin");

		auto label = lv_label_create(rest);
		lv_label_set_text_fmt(label, "SCORE: %d", (int)highScore);
		lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_center(label);
		lv_obj_set_style_text_color(label, lv_color_make(85, 126, 150), 0);
		lv_obj_set_align(label, LV_ALIGN_CENTER);
		lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

		item = lv_obj_create(rest);
		lv_obj_add_style(item, itemStyle, 0);

		lv_obj_set_align(item, LV_ALIGN_CENTER);
		lv_obj_set_flex_flow(item, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(item, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		name[0] = lv_label_create(item);
		lv_label_set_text(name[0], (std::stringstream() << characters[0]).str().c_str());
		lv_obj_set_size(name[0], LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_border_color(name[0], lv_color_make(85, 126, 150), 0);
		lv_obj_set_style_border_width(name[0], 1, 0);
		lv_obj_set_style_text_color(name[0], lv_color_make(85, 126, 150), 0);

		name[1] = lv_label_create(item);
		lv_label_set_text(name[1], (std::stringstream() << characters[1]).str().c_str());
		lv_obj_set_size(name[1], LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_border_color(name[1], lv_color_make(85, 126, 150), 0);
		lv_obj_set_style_border_width(name[1], 1, 0);
		lv_obj_set_style_text_color(name[1], lv_color_make(85, 126, 150), 0);

		name[2] = lv_label_create(item);
		lv_label_set_text(name[2], (std::stringstream() << characters[2]).str().c_str());
		lv_obj_set_size(name[2], LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_border_color(name[2], lv_color_make(85, 126, 150), 0);
		lv_obj_set_style_border_width(name[2], 1, 0);
		lv_obj_set_style_text_color(name[2], lv_color_make(85, 126, 150), 0);

		InputLVGL::getInstance()->setVertNav(true);

	}else if(award == Award::XP){
		lv_style_set_width(itemStyle, lv_pct(86));
		lv_style_set_height(itemStyle, 16);
		lv_style_set_radius(itemStyle, 2);
		lv_style_set_align(itemStyle, LV_ALIGN_CENTER);

		auto mkLabel = [this](const char* text){
			auto item = lv_obj_create(rest);
			lv_obj_add_style(item, itemStyle, 0);
			lv_obj_set_style_height(item, 13, 0);
			lv_obj_set_align(item, LV_ALIGN_CENTER);

			auto label = lv_label_create(item);
			lv_label_set_text(label, text);
			lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
			lv_obj_center(label);
			lv_obj_set_style_text_color(label, lv_color_make(85, 126, 150), 0);
			lv_obj_set_align(label, LV_ALIGN_CENTER);
			lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

			return item;
		};

		auto text = lv_img_create(rest);
		lv_img_set_src(text, Filepath::Award::XpGained);
		lv_obj_set_align(text, LV_ALIGN_CENTER);

		mkLabel(("+" + std::to_string(xp) + "XP").c_str());

		auto lvl = mkLabel(("Level " + std::to_string(xpSystem->getLevel())).c_str());

		auto bar = lv_img_create(rest);
		lv_img_set_src(bar, Filepath::Award::XpFrame);
		lv_obj_set_align(bar, LV_ALIGN_CENTER);
		lv_obj_set_style_bg_img_src(bar, Filepath::Award::XpBackground, 0);
		lv_obj_set_style_bg_opa(bar, 100, 0);

		xpBar = new XPBar(XPBarLength::Long, bar, xpSystem->MapXPToLevel(xpSystem->getXP()).progress);

		const LevelProgress progress = xpSystem->MapXPToLevel(xpSystem->getXP() + xp);
		xpBar->setFill(progress.nextLvl > levelSet + 1 ? 1.0f : progress.progress, true);

		lv_obj_set_align(*xpBar, LV_ALIGN_CENTER);
	}else if(award == Award::LevelUp){
		lv_style_set_width(itemStyle, lv_pct(86));
		lv_style_set_height(itemStyle, 16);
		lv_style_set_radius(itemStyle, 2);
		lv_style_set_align(itemStyle, LV_ALIGN_CENTER);

		auto mkLabel = [this](const char* text){
			auto item = lv_obj_create(rest);
			lv_obj_add_style(item, itemStyle, 0);
			lv_obj_set_style_height(item, 13, 0);
			lv_obj_set_align(item, LV_ALIGN_CENTER);

			auto label = lv_label_create(item);
			lv_label_set_text(label, text);
			lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
			lv_obj_center(label);
			lv_obj_set_style_text_color(label, lv_color_make(85, 126, 150), 0);
			lv_obj_set_align(label, LV_ALIGN_CENTER);
			lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

			return item;
		};

		auto text = lv_img_create(rest);
		lv_img_set_src(text, Filepath::Award::LeveledUp);
		lv_obj_set_align(text, LV_ALIGN_CENTER);

		auto lvl = mkLabel(("Level " + std::to_string(levelSet)).c_str());

		auto bar = lv_img_create(rest);
		lv_img_set_src(bar, Filepath::Award::XpFrame);
		lv_obj_set_align(bar, LV_ALIGN_CENTER);
		lv_obj_set_style_bg_img_src(bar, Filepath::Award::XpBackground, 0);
		lv_obj_set_style_bg_opa(bar, 100, 0);

		xpBar = new XPBar(XPBarLength::Long, bar, levelSet == XPSystem::MaxLevel ? 1.0f : 0.0f);
		lv_obj_set_align(*xpBar, LV_ALIGN_CENTER);
	}else if(award == Award::Achievement){
		auto img = lv_img_create(rest);
		lv_img_set_src(img, Filepath::Award::AchievementsUnlocked);
		lv_obj_set_align(img, LV_ALIGN_CENTER);
	}else{
		exit();
	}

	lv_group_focus_obj(lv_obj_get_child(rest, 0));

	awardMode = award;
}

void AwardsScreen::onStart(){
	Events::listen(Facility::Input, &evts);

	if(awardMode == Award::HighScore){
		InputLVGL::getInstance()->setVertNav(true);
	}
}

void AwardsScreen::onStop(){
	Events::unlisten(&evts);
	InputLVGL::getInstance()->setVertNav(false);

	if(XPSystem* xpSystem = (XPSystem*) Services.get(Service::XPSystem)){
		xpSystem->increment(xp);
	}

	HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore);
	if(hsm == nullptr){
		return;
	}

	if(!hsm->isHighScore(currentGame, highScore)){
		return;
	}

	HighScore score = { .score = highScore };
	score.id[0] = characters[0];
	score.id[1] = characters[1];
	score.id[2] = characters[2];

	hsm->saveScore(currentGame, score);
}

void AwardsScreen::loop(){
	const XPSystem* xpSystem = (XPSystem*) Services.get(Service::XPSystem);
	if(xpSystem == nullptr){
		return;
	}

	if(awardMode == Award::LevelUp && millis() - lastChange >= 400 && !chirped){
		chirped = true;

		if(xpBar != nullptr){
			if(XPSystem::MapXPToLevel(xpSystem->getXP() + xp).nextLvl == levelSet && levelSet == XPSystem::MaxLevel){
				xpBar->setFill(1.0f, false);
			}else{
				xpBar->setFill(XPSystem::MapXPToLevel(xpSystem->getXP() + xp).nextLvl > levelSet + 1 ? 1.0f : xpSystem->MapXPToLevel(xpSystem->getXP() + xp).progress, true);
			}
		}

		if(levelSet < XPSystem::MaxLevel){
			if(ChirpSystem* chirp = (ChirpSystem*) Services.get(Service::Audio)){
				chirp->play({{ NOTE_C3, NOTE_C6, AnimLength }});
			}
		}
	}

	if(awardMode <= Award::LevelUp && millis() - lastChange >= AnimLength * 3 &&
	   (
			   (XPSystem::MapXPToLevel(xpSystem->getXP() + xp).currLvl < XPSystem::MapXPToLevel(xpSystem->getXP() + xp).nextLvl && XPSystem::MapXPToLevel(xpSystem->getXP() + xp).nextLvl > levelSet + 1)
			   ||
			   (XPSystem::MapXPToLevel(xpSystem->getXP() + xp).currLvl == XPSystem::MapXPToLevel(xpSystem->getXP() + xp).nextLvl && levelSet < XPSystem::MaxLevel)
	   )
			){
		setAwardMode(Award::LevelUp);
	}

	HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore);
	if(hsm == nullptr){
		return;
	}

	for(Event e{}; evts.get(e, 0);){
		if(e.facility != Facility::Input){
			free(e.data);
			continue;
		}

		auto data = (Input::Data*) e.data;
		if(data->action == Input::Data::Release){
			if(data->btn == Input::A || data->btn == Input::B){
				if(awardMode == Award::Achievement && !achievements.empty()){
					const auto achi = achievements.back().achievementID;
					achievements.pop_back();
					lv_obj_clean(rest);
					new AchievementElement(rest, achi);
				}else if(awardMode < Award::Achievement && !achievements.empty()){
					setAwardMode(Award::Achievement);
				}else if(awardMode < Award::HighScore && hsm->isHighScore(currentGame, highScore)){
					setAwardMode(Award::HighScore);
				}else{
					free(e.data);
					exit();
					return;
				}
			}else if(data->btn == Input::Up){
				labelChanged = 0;
			}else if(data->btn == Input::Down){
				labelChanged = 0;
			}else if(data->btn == Input::Left){
				activeIndex = activeIndex == 0 ? 2 : activeIndex - 1;
			}else if(data->btn == Input::Right){
				activeIndex = (activeIndex + 1) % 3;
			}
		}else{
			if(data->btn == Input::Up){
				labelChanged = 1;
			}else if(data->btn == Input::Down){
				labelChanged = -1;
			}
		}

		free(e.data);
	}

	if(awardMode == Award::HighScore){
		const uint64_t interval = (millis() - start) / blinkTime;

		if(interval % 2 == 0){
			for(size_t i = 0; i < 3; ++i){
				lv_obj_set_style_border_opa(name[i], 0, 0);
			}
		}else if(interval % 2 == 1){
			for(size_t i = 0; i < 3; ++i){
				if(i == activeIndex){
					lv_obj_set_style_border_opa(name[i], 100, 0);
				}else{
					lv_obj_set_style_border_opa(name[i], 0, 0);
				}
			}
		}

		if(labelChanged != 0){
			if(millis() - lastChanged >= 200){
				if(labelChanged > 0){
					characters[activeIndex] = getChar(characters[activeIndex] + 1, 1);
				}else{
					characters[activeIndex] = getChar(characters[activeIndex] - 1, -1);
				}

				lastChanged = millis();
			}

			for(size_t i = 0; i < 3; ++i){
				lv_label_set_text(name[i], (std::stringstream() << characters[i]).str().c_str());
			}
		}
	}
}

void AwardsScreen::exit(){
	auto ui = (UIThread*) Services.get(Service::UI);
	if(ui == nullptr){
		return;
	}

	const HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore);
	if(hsm == nullptr){
		return;
	}

	if(hsm->isHighScore(currentGame, highScore)){
		ui->startScreen([this](){ return std::make_unique<HighScoreScreen>(currentGame); });
	}else{
		ui->startScreen([this](){ return std::make_unique<GameMenuScreen>(currentGame); });
	}
}
