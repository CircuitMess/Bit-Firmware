#include "SpriteRC.h"

SpriteRC::SpriteRC(PixelDim dim) : sprite(std::make_shared<Sprite>(nullptr)){
	sprite->createSprite(dim.x, dim.y);
	sprite->setColorDepth(lgfx::rgb565_2Byte);
}

std::shared_ptr<Sprite> SpriteRC::getSprite() const{
	return sprite;
}

void SpriteRC::push(Sprite& parent, PixelDim pos, float rot, bool flipX, bool flipY) const{
	sprite->pushRotateZoomWithAA(&parent, (pos.x + (float) sprite->width() / 2.0), (pos.y + (float) sprite->height() / 2.0), rot, scale.x, scale.y, TFT_TRANSPARENT);
}

void SpriteRC::setScale(glm::vec2 value){
	this->scale = value;
}