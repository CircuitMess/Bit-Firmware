#ifndef BIT_FIRMWARE_CHARACTERPICKER_H
#define BIT_FIRMWARE_CHARACTERPICKER_H

#include "LV_Interface/LVObject.h"
#include "Settings/Settings.h"
#include "Services/RobotManager.h"
#include "LV_Interface/LVStyle.h"

class CharacterPicker : public LVObject {
public:
	CharacterPicker(lv_obj_t* parent);
	void toggleState();
	void scrollUp();
	void scrollDown();
	void scrollLeft();
	void scrollRight();
	uint8_t getCharacterIndex() const;
	Pet getPet() const;

private:
	uint8_t avatarIndex = 0;
	int8_t petIndex = 0;
	uint8_t PetCount = 0;
	std::vector<Pet> unlockedPets;
	uint8_t level = 0;


	lv_obj_t* character;
	lv_obj_t* petContainer;
	lv_obj_t* pet;

	lv_obj_t* arrowUp;
	lv_obj_t* arrowDown;
	lv_obj_t* arrowLeft;
	lv_obj_t* arrowRight;
	bool arrowsState = false;
	lv_anim_t animChar;
	lv_anim_t animPet;
	static constexpr uint8_t CharactersNum = 3;

	LVStyle debugStyle;

	Settings& settings;

	void buildUI();

	void initAnim();

	void setCharacterSrc();
	void setPetSrc();

	void exit();
};


#endif //BIT_FIRMWARE_CHARACTERPICKER_H
