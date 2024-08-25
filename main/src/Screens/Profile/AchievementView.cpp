#include "AchievementView.h"
#include "Services/AchievementSystem.h"
#include "Filepaths.hpp"
#include "../GrayscaleImageElement.h"
#include "Util/Services.h"
#include "Screens/Profile/AchievementModal.h"

AchievementView::AchievementView(LVScreen* screen, lv_obj_t* parent, uint8_t rows, uint16_t width, uint16_t height) :
		LVSelectable(parent), screen(screen), RowWidth(rows), width(width), height(height){
	auto achievementSystem = (AchievementSystem*) Services.get(Service::Achievements);
	achievementSystem->getAll(achievementsVector);

	initStyles();
	buildUI();
}

void AchievementView::initStyles(){
	lv_style_set_pad_all(defaultStyle, 1);
	lv_style_set_border_width(defaultStyle, 1);
	lv_style_set_border_color(defaultStyle, lv_color_black());
	lv_style_set_border_opa(defaultStyle, LV_OPA_TRANSP);
}

void AchievementView::buildUI(){
	lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_size(obj, width, height);

	lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW_WRAP);

	//TODO - just a quickfix, should be universal or not set at all and left to the client
	if(RowWidth <= 3){
		lv_obj_set_style_pad_gap(obj, 2, 0);
	}

	auto onKey = [](lv_event_t* e){
		auto ach = (AchievementView*) e->user_data;
		auto group = ach->inputGroup;
		auto key = lv_event_get_key(e);
		const auto RowWidth = ach->RowWidth;

		const auto index = lv_obj_get_index(e->target); // only applies to odd number of menu items
		const auto itemCount = (uint32_t) ach->achievementsVector.size();

		//Maybe simplify logic in these calculations, seems overkill but math should work for any grid width
		if(key == LV_KEY_UP){
			uint8_t moves;
			if(itemCount % RowWidth != 0 && index < RowWidth){
				moves = index + 1 + std::max((itemCount % RowWidth - index - 1), 0UL);
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
			ach->deselect();
		}
	};

	auto onPress = [](lv_event_t* e){
		const uint32_t index = lv_obj_get_index(e->target);

		AchievementView* view = (AchievementView*) e->user_data;
		if(view == nullptr || view->screen == nullptr){
			return;
		}

		auto achi = view->achievementsVector[index].achievementID;

		view->modal = new AchievementModal(view->screen, achi);
	};

	for(size_t i = 0; i < achievementsVector.size(); ++i){
		auto base = lv_obj_create(obj);
		lv_obj_set_size(base, 22, 22);
		lv_obj_add_flag(base, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
		lv_obj_clear_flag(base, LV_OBJ_FLAG_SCROLLABLE);
		lv_group_add_obj(inputGroup, base);
		lv_obj_add_style(base, defaultStyle, 0);
		lv_obj_set_style_bg_img_src(base, "S:/Ach/bg.bin", LV_STATE_DEFAULT);

		auto img = new GrayscaleImageElement(base, AchivementFile(achievementsVector[i].achievementID),
											 AchivementFileBW(achievementsVector[i].achievementID),
											 !achievementsVector[i].unlocked());
		lv_obj_center(*img);

		lv_obj_add_event_cb(base, onKey, LV_EVENT_KEY, this);
		lv_obj_add_event_cb(base, onPress, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(base, [](lv_event_t* e){
			lv_obj_set_style_bg_img_src(lv_event_get_target(e), "S:/Ach/bgSelected.bin", LV_STATE_FOCUSED);
		}, LV_EVENT_FOCUSED, nullptr);
		lv_obj_add_event_cb(base, [](lv_event_t* e){
			lv_obj_set_style_bg_img_src(lv_event_get_target(e), "S:/Ach/bg.bin", LV_STATE_FOCUSED);
		}, LV_EVENT_DEFOCUSED, nullptr);
	}
}

void AchievementView::onSelect(){
	lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_bg_img_src(lv_group_get_focused(inputGroup), "S:/Ach/bgSelected.bin", LV_STATE_FOCUSED);
}

void AchievementView::onDeselect(){
	lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_bg_img_src(lv_group_get_focused(inputGroup), "S:/Ach/bg.bin", LV_STATE_FOCUSED);
	if(!returnFunc) return;
	returnFunc();
}

void AchievementView::setReturnFunc(std::function<void()> returnFunc){
	this->returnFunc = returnFunc;
}
