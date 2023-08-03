#include "SpriteRC.h"

SpriteRC::SpriteRC(PixelDim dim) : sprite(std::make_shared<Sprite>(nullptr)){
	sprite->createSprite(dim.x, dim.y);
	sprite->setColorDepth(lgfx::rgb565_2Byte);
}

std::shared_ptr<Sprite> SpriteRC::getSprite() const{
	return sprite;
}

void SpriteRC::push(Sprite& parent, PixelDim pos, float rot) const{
	if(rot == 0){
		sprite->pushSprite(&parent, pos.x, pos.y);
	}else{
		sprite->pushRotateZoomWithAA(&parent, pos.x + sprite->width() / 2, pos.y + sprite->height() / 2, rot, 1, 1, TFT_TRANSPARENT);
	}
}
