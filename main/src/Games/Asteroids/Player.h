#ifndef BIT_FIRMWARE_ASTEROIDS_PLAYER_H
#define BIT_FIRMWARE_ASTEROIDS_PLAYER_H

#include "GameEngine/GameObject.h"

namespace Asteroids {

class Player {
	typedef std::shared_ptr<GameObject> ObjPtr;

public:
	void setObj(ObjPtr obj);
	const ObjPtr& getObj() const;

	void leftTurn(float delta);
	void rightTurn(float delta);

	float getAngle() const;
private:
	float angle = 0.f; //0 - 360

	constexpr static float rotSpeed = 180.f;

	ObjPtr obj;
};

}

#endif //BIT_FIRMWARE_ASTEROIDS_PLAYER_H
