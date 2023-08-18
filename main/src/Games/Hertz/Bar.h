#ifndef BIT_FIRMWARE_BAR_H
#define BIT_FIRMWARE_BAR_H

#include "Devices/Display.h"
#include "Color.h"

class Bar {
public:
	Bar(std::shared_ptr<Sprite> sprite);
	void resetGoal();
	int getY();
	Pixel getColor(int diff);

private:
	void draw();
	uint8_t xGoal;
	std::shared_ptr<Sprite> sprite;
};


#endif //BIT_FIRMWARE_BAR_H
