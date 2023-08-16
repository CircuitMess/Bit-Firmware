#ifndef BIT_FIRMWARE_PROGRESSBAR_H
#define BIT_FIRMWARE_PROGRESSBAR_H

#include "Devices/Display.h"
#include "GameEngine/GameObject.h"
#include "Color.h"

class ProgressBar {
public:
	ProgressBar(std::shared_ptr<Sprite> sprite);
	void fill(float fillPercent);

private:
	std::shared_ptr<Sprite> sprite;
	std::shared_ptr<GameObject> gameObject;
	static constexpr Color FillColor = C_RGB(169, 59, 59);
	static constexpr Color EmptyColor = TFT_WHITE;
};


#endif //BIT_FIRMWARE_PROGRESSBAR_H
