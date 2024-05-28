#include "ThemePicker.h"
#include "Settings/Settings.h"
#include "Util/Services.h"

ThemePicker::ThemePicker(lv_obj_t* parent) : LVSelectable(parent), settings(*(Settings*) Services.get(Service::Settings)),
											 robotManager(*(RobotManager*) Services.get(Service::RobotManager)){
	lv_style_set_border_width(defaultStyle, 1);
	lv_style_set_border_color(defaultStyle, lv_color_white());
	lv_style_set_border_opa(defaultStyle, LV_OPA_TRANSP);

	lv_style_set_border_opa(focusedStyle, LV_OPA_COVER);

	lv_obj_set_size(obj, 46, 38);

	lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_SPACE_EVENLY);
	lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_style_pad_gap(obj, 1, 0);
	lv_obj_set_style_pad_left(obj, 1, 0);
	lv_obj_set_style_pad_top(obj, 9, 0);

	auto onKey = [](lv_event_t* e){
		auto themeview = (ThemePicker*) e->user_data;
		auto group = themeview->inputGroup;
		auto key = lv_event_get_key(e);

		const auto index = lv_obj_get_index(e->target); // only applies to odd number of menu items
		const auto itemCount = (uint8_t) Theme::COUNT;

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
		}else if(key == LV_KEY_ESC){
			themeview->deselect();
		}
	};

	for(uint8_t i = 0; i < (uint8_t) Theme::COUNT; i++){
		lv_obj_t* img = lv_img_create(obj);
		char path[50];
		sprintf(path, "S:/Profile/theme-%02d-icon.bin", i + 1);
		lv_img_set_src(img, path);

		Theme theme = (Theme) i;
		if(!robotManager.isUnlocked(theme)){
			auto lock = lv_img_create(img);
			lv_img_set_src(lock, "S:/Profile/lock.bin");
			lv_obj_set_align(lock, LV_ALIGN_CENTER);
		}
		lv_group_add_obj(inputGroup, img);

		lv_obj_add_style(img, defaultStyle, 0);
		lv_obj_add_style(img, focusedStyle, LV_STATE_FOCUSED);
		lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);

		lv_obj_add_event_cb(img, [](lv_event_t* e){
			ThemePicker* view = (ThemePicker*) e->user_data;
			Theme theme = (Theme) lv_obj_get_index(e->target);
			if(view->robotManager.isUnlocked(theme)){
				view->changeTheme(theme);
			}else{
				//TODO - error beep or popup?
			}
		}, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(img, onKey, LV_EVENT_KEY, this);
	}

	Theme current = settings.get().theme;
	lv_group_focus_obj(lv_obj_get_child(obj, (uint32_t) current));
}

void ThemePicker::changeTheme(Theme theme){
	SettingsStruct settingsStruct = settings.get();
	settingsStruct.theme = theme;
	settings.set(settingsStruct);
}

void ThemePicker::onDeselect(){
	Theme current = settings.get().theme;
	lv_group_focus_obj(lv_obj_get_child(obj, (uint32_t) current));

	lv_obj_set_style_bg_opa(obj, LV_OPA_40, LV_STATE_FOCUSED);
}

void ThemePicker::onSelect(){
	lv_obj_set_style_bg_opa(obj, LV_OPA_0, LV_STATE_FOCUSED);
}
