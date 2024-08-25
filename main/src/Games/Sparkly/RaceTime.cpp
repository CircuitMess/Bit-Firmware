#include "RaceTime.h"

Sparkly::RaceTime::RaceTime(){
	go = std::make_shared<GameObject>(
			std::make_unique<SpriteRC>(PixelDim{ 64, 8 }),
			nullptr
	);

	rc = std::static_pointer_cast<SpriteRC>(go->getRenderComponent());
	rc->setLayer(10);
	sprite = rc->getSprite();

	sprite->setTextFont(0);
	sprite->setTextSize(0);
	sprite->setTextColor(TFT_BLACK);

	setTime(0.0f);
}

std::shared_ptr<GameObject> Sparkly::RaceTime::getGO() const{
	return go;
}

void Sparkly::RaceTime::setTime(float value){
	if(value < 0.0f){
		value = 0.0f;
	}

	sprite->clear(TFT_TRANSPARENT);

	const int minutes = ((int) value) / 60;
	const int seconds = ((int) value) % 60;
	const int rest = (int)((value - (int) value) * 10.0f);

	sprite->setCursor(0, 0);
	sprite->printf("%02d:%02d:%1d", minutes, seconds, rest);
}
