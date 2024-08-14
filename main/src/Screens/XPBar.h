#ifndef BIT_FIRMWARE_XPBAR_H
#define BIT_FIRMWARE_XPBAR_H

#include "LV_Interface/LVObject.h"

enum class XPBarLength : uint8_t {
	Short,
	Medium,
	Long
};

class XPBar : public LVObject {
public:
	/**
	 * @param length - Length
	 * @param parent - Parent object
	 * @param fillPercent - Initial fill percentage
	 */
	XPBar(XPBarLength length, lv_obj_t* parent, float fillPercent = 0);

	void setFill(float percent, bool anim = false);

private:
	void buildUI();

	lv_obj_t* bar;

	const XPBarLength length;

	static constexpr uint16_t Widths[] = { 45, 74, 84 };
	static constexpr uint16_t Height = 5;

	static constexpr uint32_t AnimSpeed = 500; //[ms]
};


#endif //BIT_FIRMWARE_XPBAR_H
