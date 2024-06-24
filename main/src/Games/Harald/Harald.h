#ifndef BIT_FIRMWARE_HARALD_H
#define BIT_FIRMWARE_HARALD_H

#include "GameEngine/Game.h"

namespace Harald {
class Harald : public Game {
public:
	Harald(Sprite& canvas);

private:
	struct GameElement {
		uint8_t id = 0; // 0 - 11
	};

	GameObjPtr background;
	std::array<GameObjPtr, 4 * 4> elements;
};
} // Harald

#endif //BIT_FIRMWARE_HARALD_H