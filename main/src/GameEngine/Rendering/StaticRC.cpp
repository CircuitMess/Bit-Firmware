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

	if(rot == 0){
		Display::drawFile(parent, file, std::round(pos.x), std::round(pos.y), dim.x, dim.y);
	}else{
		Sprite rotated = Sprite();
		rotated.createSprite(dim.x, dim.y);
		rotated.clear(TFT_TRANSPARENT);
		Display::drawFile(rotated, file, std::round(pos.x), std::round(pos.y), dim.x, dim.y);

		// TODO there is a fuckup here somewhere
		//rotated.pushRotateZoomWithAA(std::round(pos.x + (float) dim.x / 2.0), std::round(pos.y + (float) dim.y / 2.0), rot, 1, 1, TFT_TRANSPARENT);
		//parent.setPivot(pos.x, pos.y);
		rotated.pushRotatedWithAA(parent.getParent(), 0, TFT_TRANSPARENT);
		//parent.setPivot(0, 0);
	}
}

void StaticRC::setFile(File file){
	StaticRC::file = file;
}

void StaticRC::setFile(File file, PixelDim dim){
	StaticRC::file = file;
	StaticRC::dim = dim;
}
