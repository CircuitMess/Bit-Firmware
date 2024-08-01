#include "StaticRC.h"
#include <utility>
#include <esp_log.h>
#include "Color.h"

StaticRC::StaticRC(File file, PixelDim dim) : file(std::move(file)), dim(dim){}

void StaticRC::push(Sprite& parent, PixelDim pos, float rot, bool flipX, bool flipY) const{
	if(!file){
		ESP_LOGE("Render", "StaticRC: Pushing closed file");
		return;
	}

	Sprite rotated = Sprite(&parent);
	rotated.createSprite(dim.x, dim.y);
	rotated.clear(TFT_TRANSPARENT);
	Display::drawFile(rotated, file, 0, 0, dim.x, dim.y);
	rotated.pushRotateZoomWithAA(std::round(pos.x + (float) dim.x / 2.0), std::round(pos.y + (float) dim.y / 2.0), rot, scale.x, scale.y, TFT_TRANSPARENT);
}

void StaticRC::setFile(File file){
	StaticRC::file = file;
}

void StaticRC::setFile(File file, PixelDim dim){
	StaticRC::file = file;
	StaticRC::dim = dim;
}

void StaticRC::setScale(glm::vec2 value){
	scale = value;
}
