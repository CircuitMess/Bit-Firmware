#ifndef BIT_FIRMWARE_ACHIEVEMENTVIEW_H
#define BIT_FIRMWARE_ACHIEVEMENTVIEW_H

#include "LV_Interface/LVSelectable.h"
#include "LV_Interface/LVStyle.h"

class AchievementView : public LVSelectable{
public:
	explicit AchievementView(lv_obj_t* parent);
	void setOverlay(lv_obj_t* overlay);
protected:
	void onSelect() override;
	void onDeselect() override;
private:
	inline static constexpr const uint8_t AchievementCount = 32; // TODO
	static constexpr const uint8_t RowWidth = 4; //4 elements wide grid

	LVStyle defaultStyle;

	lv_anim_t focusBlinkAnim;

	lv_obj_t* focusOverlay = nullptr;

	void initStyles();
	void buildUI();
	void startAnim();
	void stopAnim();

	static void animFunc(void* var, int32_t val);

};


#endif //BIT_FIRMWARE_ACHIEVEMENTVIEW_H
