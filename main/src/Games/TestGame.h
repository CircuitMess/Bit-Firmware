#ifndef CIRCUITPET_FIRMWARE_TESTGAME_H
#define CIRCUITPET_FIRMWARE_TESTGAME_H

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

private:
	struct Object {
		std::shared_ptr<GameObject> gObj;
		glm::vec2 velocity;
	};

	std::vector<Object> objs;

	std::shared_ptr<GameObject> pat;
	std::shared_ptr<GameObject> bg;

	bool gravity = true;
};


#endif //CIRCUITPET_FIRMWARE_TESTGAME_H
