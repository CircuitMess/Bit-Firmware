#ifndef BIT_FIRMWARE_FLY_H
#define BIT_FIRMWARE_FLY_H

#include <GameEngine/GameObject.h>

namespace CharlieGame {

class Fly {
public:
	Fly(std::function<File(const char*)> getFile);

	operator GameObjPtr();

	void update(float dt);

	static constexpr glm::vec2 SpriteSize = { 21, 33 };

	void goCac();
	void goAway();
	void done();

	bool isPlotting();
	bool isDone();

private:
	const std::function<File(const char*)> getFile;
	GameObjPtr go;

	enum State {
		FlyingIn, Plotting, FlyingOut, Cacoon, Done
	} state = FlyingIn;

	glm::vec2 startPos = { 0, 0 };
	glm::vec2 destPos = { 0, 0 };
	float t = 0;

	static constexpr float PlotTimeout = 5.0f;

	void goPlot();
	void goOut();
	void setState(State newState);

	void updateAnim();

};

};


#endif //BIT_FIRMWARE_FLY_H
