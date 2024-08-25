#include "NewRobot.h"
#include "LV_Interface/LVScreen.h"
#include "Util/Services.h"
#include "Services/RobotManager.h"
#include "Services/ChirpSystem.h"
#include "Util/Notes.h"
#include "Filepaths.hpp"

NewRobot::NewRobot(LVScreen* parent, RobotData rob, bool isNew) : LVModal(parent), rob(rob), isNew(isNew){
	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	auto audio = (ChirpSystem*) Services.get(Service::Audio);
	if(audio == nullptr){
		return;
	}

	if(Robots::isGame(rob)){
		buildMain();

		audio->play({
				Chirp{ NOTE_C3, NOTE_C4, 100 },
				Chirp{ NOTE_C4, NOTE_C4, 50 },
				Chirp{ 0, 0, 50 },
				Chirp{ NOTE_C5, NOTE_C5, 100 }
		});
	}else{
		buildNew();
		++stage;

		audio->play({
				Chirp{ NOTE_G4, NOTE_G4, 100 },
				Chirp{ 0, 0, 50 },
				Chirp{ NOTE_DS5, NOTE_DS5, 100 },
				Chirp{ 0, 0, 50 },
				Chirp{ NOTE_G5, NOTE_G5, 100 }
		});
	}

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (NewRobot*) e->user_data;
		modal->click();
	}, LV_EVENT_CLICKED, this);

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (NewRobot*) e->user_data;
		modal->click();
	}, LV_EVENT_CANCEL, this);

	lv_group_add_obj(inputGroup, *this);
	lv_group_focus_obj(*this);
}

void NewRobot::click(){
	if(!isNew || stage > 0){
		delete this;
		return;
	}

	auto audio = (ChirpSystem*) Services.get(Service::Audio);
	if(audio == nullptr){
		return;
	}

	audio->play({
			Chirp{ NOTE_G4, NOTE_G4, 100 },
			Chirp{ 0, 0, 50 },
			Chirp{ NOTE_DS5, NOTE_DS5, 100 },
			Chirp{ 0, 0, 50 },
			Chirp{ NOTE_G5, NOTE_G5, 100 }
	});

	stage++;
	lv_obj_clean(*this);
	buildNew();
}

void NewRobot::buildMain(){
	std::string imgPath("S:/Menu/");
	imgPath += RobotIcons[rob.robot >= Robot::COUNT ? (uint8_t) Robot::COUNT + (uint8_t) rob.token : (uint8_t) rob.robot];
	imgPath += ".bin";
	auto icon = lv_img_create(*this);
	lv_img_set_src(icon, imgPath.c_str());

	std::string text(RobotNames[rob.robot >= Robot::COUNT ? (uint8_t) Robot::COUNT + (uint8_t) rob.token : (uint8_t) rob.robot]);
	text += "\nplugged in!";
	auto label = lv_label_create(*this);
	lv_obj_set_size(label, lv_pct(100), LV_SIZE_CONTENT);
	lv_label_set_text(label, text.c_str());
	lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_line_space(label, 3, 0);
	lv_obj_set_style_text_color(label, lv_color_make(85, 126, 150), 0);
}

void NewRobot::buildNew(){
	if(isNew && Robots::isGame(rob)){
		auto icon = lv_img_create(*this);
		lv_img_set_src(icon, Filepath::Unlocked);
	}else if(Robots::isPet(rob)){
		auto icon = lv_img_create(*this);
		lv_img_set_src(icon, PetIcons[(uint8_t) RobotManager::toPet(rob)]);
	}else if(Robots::isTheme(rob)){
		auto icon = lv_img_create(*this);
		lv_img_set_src(icon, ThemeIcons[(uint8_t) RobotManager::toTheme(rob)]);
	}

	auto label = lv_label_create(*this);
	lv_obj_set_size(label, lv_pct(100), LV_SIZE_CONTENT);

	const std::string text(RobotNames[rob.robot >= Robot::COUNT ? (uint8_t) Robot::COUNT + (uint8_t) rob.token : (uint8_t) rob.robot]);

	if(Robots::isPet(rob)){
		if(isNew){
			lv_label_set_text(label, (text + "\nPET UNLOCKED").c_str());
		}else{
			lv_label_set_text(label, (text + "\nPET INSERTED").c_str());
		}
	}else if(Robots::isTheme(rob)){
		if(isNew){
			lv_label_set_text(label, (text + "\nTHEME UNLOCKED").c_str());
		}else{
			lv_label_set_text(label, (text + "\nTHEME INSERTED").c_str());
		}
	}else if(Robots::isGame(rob)){
		lv_label_set_text(label, "GAME UNLOCKED");
	}

	lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_line_space(label, 3, 0);
	lv_obj_set_style_text_color(label, lv_color_make(85, 126, 150), 0);
}
