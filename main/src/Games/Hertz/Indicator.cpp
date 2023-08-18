#include "Indicator.h"

Indicator::Indicator(std::shared_ptr<GameObject> indicator) : indicator(indicator){}

void Indicator::move(float deltaTime){
	speed = std::max(speedMin, speedMax - abs(xPos - xGoal) / 100.0f);
	if(goingLeft){
		indicatorVal -= deltaTime * speed;
	}else{
		indicatorVal += deltaTime * speed;
	}
	if(indicatorVal >= barMax){
		indicatorVal = 2 * barMax - indicatorVal;
		goingLeft = true;
	}else if(indicatorVal <= -barMax){
		indicatorVal = -2 * barMax - indicatorVal;
		goingLeft = false;
	}

	xPos = (x2 - indicatorVal) * MinX / (x2 - x1) + (x1 - indicatorVal) * MaxX / (x1 - x2);
	indicator->setPos({ xPos, 35 });
}

int Indicator::getDifference(){
	return abs(xPos - xGoal);
}

void Indicator::setGoal(int goal){
	this->xGoal = goal;
}
