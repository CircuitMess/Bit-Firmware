#include <string>
#include "CharacterPicker.h"
#include "Util/Services.h"
#include "Services/XPSystem.h"

static void HorizontalAnimCB(void* obj, int32_t v){
	lv_obj_set_style_translate_x((lv_obj_t*) obj, v, 0);
	lv_obj_invalidate(((lv_obj_t*) obj)->parent);
}

static void VerticalAnimCB(void* obj, int32_t v){
	lv_obj_set_style_translate_y((lv_obj_t*) obj, v, 0);
	lv_obj_invalidate(((lv_obj_t*) obj)->parent);
}

CharacterPicker::CharacterPicker(lv_obj_t* parent) : LVObject(parent), settings(*(Settings*) Services.get(Service::Settings)){
	petIndex = settings.get().pet;
	avatarIndex = settings.get().avatar;
	level = ((XPSystem*) Services.get(Service::XPSystem))->getLevel();

	RobotManager& robotManager = *(RobotManager*) Services.get(Service::RobotManager);
	for(uint8_t i = 0; i < (uint8_t) Pet::COUNT; ++i){
		if(robotManager.isUnlocked((Pet) i)){
			unlockedPets.push_back((Pet) i);
		}
	}

	buildUI();
	initArrowAnim();

	//toggle on click
	lv_obj_add_event_cb(obj, [](lv_event_t* event){
		((CharacterPicker*) event->user_data)->toggleState();
	}, LV_EVENT_CLICKED, this);

	lv_obj_add_event_cb(obj, [](lv_event_t* event){
		uint32_t c = lv_event_get_key(event);
		auto charPicker = static_cast<CharacterPicker*>(event->user_data);

		if(!charPicker->isEditing()) return;

		switch(c){
			case LV_KEY_LEFT:
				charPicker->scrollLeft();
				break;
			case LV_KEY_RIGHT:
				charPicker->scrollRight();
				break;
			case LV_KEY_UP:
				charPicker->scrollUp();
				break;
			case LV_KEY_DOWN:
				charPicker->scrollDown();
				break;
			case LV_KEY_ESC:
				charPicker->exit();
				break;
			default:
				break;
		}
	}, LV_EVENT_KEY, this);

	startPetBounce();
}

void CharacterPicker::buildUI(){
	lv_obj_set_layout(obj, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
	lv_obj_set_style_pad_gap(obj, -3, 0);
	lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

	lv_style_set_border_color(debugStyle, lv_palette_main(LV_PALETTE_RED));
	lv_style_set_border_opa(debugStyle, LV_OPA_COVER);
	lv_style_set_border_width(debugStyle, 1);


	arrowLeft = lv_img_create(obj);

	character = lv_img_create(obj);
	setCharacterSrc();

	petContainer = lv_obj_create(character);
	lv_obj_set_layout(petContainer, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(petContainer, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_style_pad_gap(petContainer, -1, 0);
	lv_obj_set_flex_align(petContainer, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_size(petContainer, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_clear_flag(petContainer, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_align(petContainer, LV_ALIGN_CENTER);
	lv_obj_set_pos(petContainer, 10, 15);

	arrowUp = lv_img_create(petContainer);

	pet = lv_img_create(petContainer);
	lv_obj_set_size(pet, 17, 22);
	setPetSrc();

	arrowDown = lv_img_create(petContainer);
	lv_obj_update_layout(petContainer);
	lv_obj_set_height(petContainer, lv_obj_get_height(petContainer) + 12);

	arrowRight = lv_img_create(obj);
	lv_img_set_src(arrowLeft, "S:/Profile/arrowLeft.bin");
	lv_img_set_src(arrowRight, "S:/Profile/arrowRight.bin");
	lv_img_set_src(arrowUp, "S:/Profile/arrowUp.bin");
	lv_img_set_src(arrowDown, "S:/Profile/arrowDown.bin");

	lv_obj_set_style_opa(arrowLeft, LV_OPA_0, 0);
	lv_obj_set_style_opa(arrowRight, LV_OPA_0, 0);
	lv_obj_set_style_opa(arrowUp, LV_OPA_0, 0);
	lv_obj_set_style_opa(arrowDown, LV_OPA_0, 0);

	lv_obj_update_layout(obj);
	lv_obj_set_width(obj, lv_obj_get_width(obj) + 4);

//	lv_obj_add_style(obj, debugStyle, 0);
//	lv_obj_add_style(character, debugStyle, 0);
//	lv_obj_add_style(petContainer, debugStyle, 0);

}

void CharacterPicker::initArrowAnim(){
	lv_anim_init(&animChar);
	lv_anim_set_var(&animChar, arrowLeft);
	lv_anim_set_playback_time(&animChar, 75);
	lv_anim_set_repeat_count(&animChar, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_path_cb(&animChar, lv_anim_path_step);
	lv_anim_set_exec_cb(&animChar, HorizontalAnimCB);
	lv_anim_set_values(&animChar, -2, 0);
	lv_anim_set_values(&animChar, 2, 0);
	lv_anim_set_var(&animChar, arrowRight);

	lv_anim_init(&animPet);
	lv_anim_set_var(&animPet, arrowDown);
	lv_anim_set_playback_time(&animPet, 75);
	lv_anim_set_repeat_count(&animPet, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_path_cb(&animPet, lv_anim_path_step);
	lv_anim_set_exec_cb(&animPet, VerticalAnimCB);
	lv_anim_set_values(&animPet, -2, 0);
	lv_anim_set_values(&animPet, 2, 0);
	lv_anim_set_var(&animPet, arrowUp);
}

void CharacterPicker::toggleState(){
	if(!arrowsState){
		lv_obj_set_style_opa(arrowLeft, LV_OPA_100, 0);
		lv_obj_set_style_opa(arrowRight, LV_OPA_100, 0);

		if(!unlockedPets.empty()){
			lv_obj_set_style_opa(arrowUp, LV_OPA_100, 0);
			lv_obj_set_style_opa(arrowDown, LV_OPA_100, 0);
		}

		lv_anim_set_values(&animChar, -2, 0);
		lv_anim_set_var(&animChar, arrowLeft);
		lv_anim_start(&animChar);

		lv_anim_set_values(&animChar, 2, 0);
		lv_anim_set_var(&animChar, arrowRight);
		lv_anim_start(&animChar);

		lv_anim_set_values(&animPet, 2, 0);
		lv_anim_set_var(&animPet, arrowDown);
		lv_anim_start(&animPet);

		lv_anim_set_values(&animPet, -2, 0);
		lv_anim_set_var(&animPet, arrowUp);
		lv_anim_start(&animPet);


		lv_group_set_editing((lv_group_t*) lv_obj_get_group(obj), true);
		arrowsState = true;
		stopPetBounce();
	}else{
		exit();
	}
}

void CharacterPicker::scrollUp(){
	if(unlockedPets.empty()) return;

	if(petIndex == unlockedPets.size() - 1){
		petIndex = -1;
	}else{
		petIndex = (petIndex + 1) % unlockedPets.size();
	}

	setPetSrc();

	lv_anim_del(arrowLeft, HorizontalAnimCB);
	lv_anim_del(arrowRight, HorizontalAnimCB);
	lv_anim_del(arrowDown, VerticalAnimCB);
	lv_anim_del(arrowUp, VerticalAnimCB);

	lv_anim_set_values(&animPet, -2, 0);
	lv_anim_set_var(&animPet, arrowUp);
	lv_anim_start(&animPet);

	lv_anim_set_values(&animChar, 2, 0);
	lv_anim_set_var(&animChar, arrowRight);
	lv_anim_start(&animChar);

	lv_anim_set_values(&animChar, -2, 0);
	lv_anim_set_var(&animChar, arrowLeft);
	lv_anim_start(&animChar);

	lv_anim_set_values(&animPet, 2, 0);
	lv_anim_set_var(&animPet, arrowDown);
	lv_anim_start(&animPet);
}

void CharacterPicker::scrollDown(){
	if(unlockedPets.empty()) return;

	if(petIndex == -1){
		petIndex = unlockedPets.size() - 1;
	}else{
		petIndex--;
	}

	setPetSrc();

	lv_anim_del(arrowLeft, HorizontalAnimCB);
	lv_anim_del(arrowRight, HorizontalAnimCB);
	lv_anim_del(arrowDown, VerticalAnimCB);
	lv_anim_del(arrowUp, VerticalAnimCB);

	lv_anim_set_values(&animPet, 2, 0);
	lv_anim_set_var(&animPet, arrowDown);
	lv_anim_start(&animPet);

	lv_anim_set_values(&animChar, 2, 0);
	lv_anim_set_var(&animChar, arrowRight);
	lv_anim_start(&animChar);

	lv_anim_set_values(&animChar, -2, 0);
	lv_anim_set_var(&animChar, arrowLeft);
	lv_anim_start(&animChar);
	lv_anim_set_values(&animPet, -2, 0);
	lv_anim_set_var(&animPet, arrowUp);
	lv_anim_start(&animPet);
}

void CharacterPicker::scrollLeft(){
	if(avatarIndex == 0){
		avatarIndex = CharactersNum - 1;
	}else{
		avatarIndex--;
	}
	setCharacterSrc();

	lv_anim_del(arrowLeft, HorizontalAnimCB);
	lv_anim_del(arrowRight, HorizontalAnimCB);
	lv_anim_del(arrowDown, VerticalAnimCB);
	lv_anim_del(arrowUp, VerticalAnimCB);

	lv_anim_set_values(&animChar, -2, 0);
	lv_anim_set_var(&animChar, arrowLeft);
	lv_anim_start(&animChar);

	lv_anim_set_values(&animChar, 2, 0);
	lv_anim_set_var(&animChar, arrowRight);
	lv_anim_start(&animChar);

	lv_anim_set_values(&animPet, 2, 0);
	lv_anim_set_var(&animPet, arrowDown);
	lv_anim_start(&animPet);

	lv_anim_set_values(&animPet, -2, 0);
	lv_anim_set_var(&animPet, arrowUp);
	lv_anim_start(&animPet);
}

void CharacterPicker::scrollRight(){
	avatarIndex = (avatarIndex + 1) % CharactersNum;
	setCharacterSrc();

	lv_anim_del(arrowLeft, HorizontalAnimCB);
	lv_anim_del(arrowRight, HorizontalAnimCB);
	lv_anim_del(arrowDown, VerticalAnimCB);
	lv_anim_del(arrowUp, VerticalAnimCB);

	lv_anim_set_values(&animChar, -2, 0);
	lv_anim_set_var(&animChar, arrowLeft);
	lv_anim_start(&animChar);

	lv_anim_set_values(&animPet, 2, 0);
	lv_anim_set_var(&animPet, arrowDown);
	lv_anim_start(&animPet);

	lv_anim_set_values(&animPet, -2, 0);
	lv_anim_set_var(&animPet, arrowUp);
	lv_anim_start(&animPet);

	lv_anim_set_values(&animChar, 2, 0);
	lv_anim_set_var(&animChar, arrowRight);
	lv_anim_start(&animChar);
}

uint8_t CharacterPicker::getCharacterIndex() const{
	return avatarIndex;
}

Pet CharacterPicker::getPet() const{
	if(petIndex < 0) return Pet::COUNT;

	return unlockedPets[petIndex];
}

void CharacterPicker::setCharacterSrc(){
	char path[50];
	sprintf(path, "S:/Profile/character%d-%02d.bin", avatarIndex + 1, level);
	lv_img_set_src(character, path);
}

void CharacterPicker::setPetSrc(){
	if(petIndex == -1){
		lv_obj_set_style_opa(pet, LV_OPA_0, 0);
	}else{
		lv_obj_set_style_opa(pet, LV_OPA_COVER, 0);
		if(!unlockedPets.empty() && petIndex >= 0){
			lv_img_set_src(pet, PetIcons[(uint8_t) unlockedPets[petIndex]]);
		}
	}
}

void CharacterPicker::exit(){
	lv_obj_set_style_opa(arrowLeft, LV_OPA_0, 0);
	lv_obj_set_style_opa(arrowRight, LV_OPA_0, 0);
	lv_obj_set_style_opa(arrowDown, LV_OPA_0, 0);
	lv_obj_set_style_opa(arrowUp, LV_OPA_0, 0);
	lv_anim_del(arrowLeft, HorizontalAnimCB);
	lv_anim_del(arrowRight, HorizontalAnimCB);
	lv_anim_del(arrowDown, VerticalAnimCB);
	lv_anim_del(arrowUp, VerticalAnimCB);

	lv_group_set_editing((lv_group_t*) lv_obj_get_group(obj), false);
	arrowsState = false;

	startPetBounce();
}

bool CharacterPicker::isEditing() const{
	return arrowsState;
}

void CharacterPicker::startPetBounce(){
	lv_anim_init(&animPetSprite);
	lv_anim_set_playback_time(&animPetSprite, 500);
	lv_anim_set_repeat_count(&animPetSprite, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_path_cb(&animPetSprite, lv_anim_path_ease_in_out);
	lv_anim_set_exec_cb(&animPetSprite, VerticalAnimCB);
	lv_anim_set_values(&animPetSprite, -5, 5);
	lv_anim_set_var(&animPetSprite, petContainer);

	lv_anim_start(&animPetSprite);
}

void CharacterPicker::stopPetBounce(){
	lv_anim_del(petContainer, VerticalAnimCB);
	lv_obj_set_style_translate_y(petContainer, 0, 0);
	lv_obj_invalidate(petContainer);
}
