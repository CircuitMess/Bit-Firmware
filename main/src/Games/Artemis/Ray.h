#ifndef BIT_FIRMWARE_RAY_H
#define BIT_FIRMWARE_RAY_H

#include "vec2.hpp"
#include "FS/File.h"
#include "GameEngine/PixelDim.hpp"

class Ray {
public:
	static bool within(glm::vec2 point, glm::vec2 topLeft, glm::vec2 botRight);
	static bool hitTest(glm::ivec2 point, File file, PixelDim size);

private:


};


#endif //BIT_FIRMWARE_RAY_H
