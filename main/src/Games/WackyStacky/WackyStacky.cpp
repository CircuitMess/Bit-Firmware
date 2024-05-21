#include "WackyStacky.h"
#include "GameEngine/Rendering/StaticRC.h"

WackyStacky::WackyStacky::WackyStacky(Sprite& base): Game(base, Games::WackyStacky, "/Games/Stacky", {
		RES_GOBLET,
		{ "/bg.raw", {}, true },
		{ "/floor.raw", {}, true },
		{ "/hook.raw", {}, true },
		{ CloudPaths[0], {}, true },
		{ CloudPaths[1], {}, true },
		{ CloudPaths[2], {}, true },
		{ CloudPaths[3], {}, true },
		{ RobotPaths[0], {}, true },
		{ RobotPaths[1], {}, true },
		{ RobotPaths[2], {}, true },
		{ RobotPaths[3], {}, true },
		{ RobotPaths[4], {}, true },
		{ RobotPaths[5], {}, true },
		{ RobotPaths[6], {}, true },
}){

}

uint32_t WackyStacky::WackyStacky::getXP() const{
	return 0;
}

void WackyStacky::WackyStacky::onLoad(){
	Game::onLoad();

	auto bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);

	bg->getRenderComponent()->setLayer(-1);
	addObject(bg);

	auto floor = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/floor.raw"), PixelDim{ 128, 16 }),
			nullptr
	);

	floor->setPos(0, 112);
	floor->getRenderComponent()->setLayer(0);
	addObject(floor);

	scoreDisplay = std::make_unique<Score>(getFile(FILE_GOBLET));
	scoreDisplay->getGO()->setPos({ 128 - 2 - 28, 2 });
	addObject(scoreDisplay->getGO());

	hook = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/hook.raw"), PixelDim{ 17, 40 }),
			nullptr
	);
	hook->setPos(56, 56);
	hook->setRot(1);
	addObject(hook);
}

void WackyStacky::WackyStacky::onLoop(float deltaTime){
	Game::onLoop(deltaTime);


}
