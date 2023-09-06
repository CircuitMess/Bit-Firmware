#include "Ray.h"
#include "LovyanGFX.hpp"

bool Ray::within(glm::vec2 point, glm::vec2 topLeft, glm::vec2 botRight){
	return point.x >= topLeft.x && point.x < botRight.x && point.y >= topLeft.y && point.y < botRight.y;
}

bool Ray::hitTest(glm::ivec2 point, File file, PixelDim size){
	int i = point.y * size.x + point.x;
	if(i < 0 || i >= size.x * size.y) return false;

	uint16_t pixel;
	file.seek(i*2);
	file.read((uint8_t*) &pixel, 2);

	return pixel != TFT_TRANSPARENT;
}
