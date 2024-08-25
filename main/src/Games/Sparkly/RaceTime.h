#ifndef BIT_FIRMWARE_RACETIME_H
#define BIT_FIRMWARE_RACETIME_H

#include "GameEngine/GameObject.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "FS/File.h"

namespace Sparkly{
class RaceTime {
public:
	RaceTime();

	std::shared_ptr<GameObject> getGO() const;
	void setTime(float value);

private:
	std::shared_ptr<GameObject> go;
	std::shared_ptr<SpriteRC> rc;
	std::shared_ptr<Sprite> sprite;
};
}

#endif //BIT_FIRMWARE_RACETIME_H
