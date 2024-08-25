#include "AchievementModal.h"
#include <sstream>
#include "Filepaths.hpp"
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "Screens/XPBar.h"
#include "Screens/AchievementElement.h"

AchievementModal::AchievementModal(LVScreen* parent, Achievement achi): LVModal(parent){
	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (AchievementModal*) e->user_data;
		delete modal;
	}, LV_EVENT_CLICKED, this);

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (AchievementModal*) e->user_data;
		delete modal;
	}, LV_EVENT_CANCEL, this);

	setBg(Filepath::Modal);

	auto info = new AchievementElement(*this, achi);

	lv_group_add_obj(inputGroup, *this);
	lv_group_focus_obj(*this);
}
