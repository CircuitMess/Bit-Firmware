#ifndef BIT_FIRMWARE_FLY_H
#define BIT_FIRMWARE_FLY_H

#include <GameEngine/GameObject.h>

namespace CharlieGame {

class Fly {
public:
	Fly(/*const std::function<void(const GameObjPtr& obj)> addObject, const std::function<void(const GameObjPtr& obj)> removeObject,*/
		std::shared_ptr<RenderComponent> flyRC, std::shared_ptr<RenderComponent> plotRC, std::shared_ptr<RenderComponent> unrollRC,
		struct Cacoon* rescue = nullptr, std::function<void(struct Cacoon*)> onRescued = {});
//	virtual ~Fly();

	operator GameObjPtr();


	void update(float dt);

	static constexpr glm::vec2 SpriteSize = { 21, 33 };

	void goCac();
	void goAway();
	void done();

	bool isPlotting();
	bool isCacooned();
	bool isRescuing();
	bool isDone();

//	glm::vec2 getCachedPos() const;

private:
//	const std::function<void(const GameObjPtr& obj)> addObject;
//	const std::function<void(const GameObjPtr& obj)> removeObject;

	std::shared_ptr<RenderComponent> flyRC;
	std::shared_ptr<RenderComponent> plotRC;
	std::shared_ptr<RenderComponent> unrollRC;
	GameObjPtr go;

	enum State {
		FlyingIn, Plotting, FlyingOut, Cacoon, Rescuing, Done
	} state = FlyingIn;

	glm::vec2 startPos = { 0, 0 };
	glm::vec2 destPos = { 0, 0 };
	float t = 0;

	static constexpr float PlotTimeout = 5.0f;
	static constexpr float RescueTimeout = 2.0f;

	struct Cacoon* rescue;
	std::function<void(struct Cacoon*)> onRescued; // also used when fly is done plotting

	void setState(State newState);

	void updateAnim();

	static glm::vec2 randPoint(float centerDistance);

	std::shared_ptr<MultiRC> rc;

//	glm::vec2 cachedPos{};

};

struct Cacoon {
	GameObjPtr go;
	float t;
	Fly* fly;

	bool beingRescued;
	Fly* rescuer;
};

};


#endif //BIT_FIRMWARE_FLY_H
