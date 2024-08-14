#include "Score.h"

Harald::Score::Score(const std::string& title): title(title){
	go = std::make_shared<GameObject>(
			std::make_unique<SpriteRC>(PixelDim{ 128, 8 }),
			nullptr
	);

	rc = std::static_pointer_cast<SpriteRC>(go->getRenderComponent());
	rc->setLayer(10);
	sprite = rc->getSprite();

	sprite->setTextFont(0);
	sprite->setTextSize(0);

	setScore(0);
}

std::shared_ptr<GameObject> Harald::Score::getGO() const{
	return go;
}

void Harald::Score::setScore(uint16_t score){
	sprite->clear(TFT_TRANSPARENT);
	sprite->setCursor(0, 0);

	if(title.empty()){
		sprite->setTextColor(TFT_WHITE);
		sprite->printf("%d", score);
	}else{
		sprite->setTextColor(TFT_DARKCYAN);
		sprite->printf("%s ", title.c_str());
		sprite->setTextColor(TFT_WHITE);
		sprite->printf("%d", score);
	}
}
