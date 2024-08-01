#ifndef BIT_FIRMWARE_SCORE_H
#define BIT_FIRMWARE_SCORE_H

#include "GameEngine/GameObject.h"
#include "GameEngine/Rendering/SpriteRC.h"

namespace Harald {
class Score {
public:
	explicit Score(const std::string& title);

	std::shared_ptr<GameObject> getGO() const;
	void setScore(uint16_t score);

private:
	std::string title;
	std::shared_ptr<GameObject> go;
	std::shared_ptr<SpriteRC> rc;
	std::shared_ptr<Sprite> sprite;
};
} // Harald

#endif //BIT_FIRMWARE_SCORE_H