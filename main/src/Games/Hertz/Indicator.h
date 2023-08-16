#ifndef BIT_FIRMWARE_INDICATOR_H
#define BIT_FIRMWARE_INDICATOR_H

#include <memory>
#include "GameEngine/GameObject.h"

class Indicator {
public:
	Indicator(std::shared_ptr<GameObject> indicator);
	void move(float deltaTime);
	int getDifference();
	void setGoal(int goal);

private:
	std::shared_ptr<GameObject> indicator;
	int xPos;
	int xGoal;
	float indicatorVal = 0.0f;
	float speed = 0.0f;
	float speedMin = 1.0f;
	float speedMax = 2.0f;
	bool goingLeft = false;
	static constexpr float barMax = 1.0f;
	static constexpr float x1 = -barMax;
	static constexpr float x2 = barMax;
	static constexpr int MinX = 6;
	static constexpr int MaxX = 117;
};


#endif //BIT_FIRMWARE_INDICATOR_H
