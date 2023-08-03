#include "StaticRC.h"
#include <utility>
#include <esp_log.h>
#include "CMFileWrapper.h"

StaticRC::StaticRC(File file, PixelDim dim) : file(std::move(file)), dim(dim){}

void StaticRC::push(Sprite& parent, PixelDim pos, float rot) const{
	if(!file){
		ESP_LOGE("Render", "StaticRC: Pushing closed file");
		return;
	}

	if(rot == 0){
		drawIcon(parent, file, pos.x, pos.y, dim.x, dim.y);
	}else{
		Sprite rotated = Sprite(parent);
		rotated.createSprite(dim.x, dim.y);
		rotated.clear(TFT_TRANSPARENT);
		drawIcon(rotated, file, 0, 0, dim.x, dim.y);
		rotated.pushRotateZoomWithAA(pos.x + dim.x / 2, pos.y + dim.y / 2, rot, 1, 1, TFT_TRANSPARENT);
	}
}

void StaticRC::setFile(File file){
	StaticRC::file = file;
}

void StaticRC::setFile(File file, PixelDim dim){
	StaticRC::file = file;
	StaticRC::dim = dim;
}

void StaticRC::drawIcon(Sprite& sprite, File icon, int16_t x, int16_t y, uint16_t width, uint16_t height, uint8_t scale, int32_t maskingColor){
	icon.seek(0);
	Color buffer[512];
	size_t bufferPos = 0;
	size_t available = icon.read(reinterpret_cast<uint8_t*>(buffer), 512) / 2;

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			if(bufferPos == available){
				available = icon.read(reinterpret_cast<uint8_t*>(buffer), 512) / 2;
				if(available == 0){
					return;
				}
				bufferPos = 0;
			}

			uint16_t color = buffer[bufferPos++];

			if(color != maskingColor || maskingColor == -1){
				sprite.fillRect(x + j * scale, y + i * scale, scale, scale, color);
			}
		}
	}
}
