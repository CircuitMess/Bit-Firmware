#ifndef BIT_FIRMWARE_TESTGAME_H
#define BIT_FIRMWARE_TESTGAME_H

#include "../GameEngine/Game.h"

class TestGame : public Game {
protected:
public:
	TestGame(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void onStart() override;
	void onStop() override;
	void onRender(Sprite& canvas) override;

	void handleInput(const Input::Data& data) override;
	uint32_t getXP() const override;

private:
	struct Object {
		GameObjPtr gObj;
		glm::vec2 velocity;
	};

	std::vector<Object> objs;

	GameObjPtr pat;
	GameObjPtr bg;

	GameObjPtr label;

	bool gravity = true;

	uint64_t startTime;
};


#endif //BIT_FIRMWARE_TESTGAME_H
