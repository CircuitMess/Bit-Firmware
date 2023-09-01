#include "SpriteRC.h"

SpriteRC::SpriteRC(PixelDim dim) : sprite(std::make_shared<Sprite>(nullptr)){
	sprite->createSprite(dim.x, dim.y);
	sprite->setColorDepth(lgfx::rgb565_2Byte);
}

std::shared_ptr<Sprite> SpriteRC::getSprite() const{
	return sprite;
}

void SpriteRC::push(Sprite& parent, PixelDim pos, float rot, bool flipX, bool flipY) const{
	if(rot == 0){
		sprite->pushSprite(&parent, std::round(pos.x), std::round(pos.y), TFT_TRANSPARENT);
	}else{
		sprite->pushRotateZoomWithAA(&parent, (pos.x + (float) sprite->width() / 2.0), (pos.y + (float) sprite->height() / 2.0), rot, 1, 1, TFT_TRANSPARENT);
	}
}
