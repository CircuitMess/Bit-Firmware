#include "WackyStacky.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Collision/RectCC.h"
#include "Util/stdafx.h"

WackyStacky::WackyStacky::WackyStacky(Sprite& base): Game(base, Games::WackyStacky, "/Games/Stacky", {
		RES_GOBLET,
        RES_HEART,
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
}), queue(16) {
	//swingLimits = { -((rand() * 40.0f + 20.0f) / INT_MAX), (rand() * 40.0f + 20.0f) / INT_MAX };
    Events::listen(Facility::Input, &queue);
}

WackyStacky::WackyStacky::~WackyStacky() {
    Events::unlisten(&queue);
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

	floor = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/floor.raw"), PixelDim{ 128, 16 }),
            std::make_unique<RectCC>(glm::vec2{ 128, 12 }, glm::vec2{ 64, 6 }) // TODO figure out exact dimensions and offset later if changing needed
	);

	floor->setPos(0, 112);
	floor->getRenderComponent()->setLayer(0);
	addObject(floor);

	scoreDisplay = std::make_unique<Score>(getFile(FILE_GOBLET));
	scoreDisplay->getGO()->setPos({ 128 - 2 - 28, 2 });
	addObject(scoreDisplay->getGO());
    scoreDisplay->setScore(robots.size());

    hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
    hearts->getGO()->setPos({ 2, 2 });
    addObject(hearts->getGO());
    hearts->setLives(3);

	hook = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/hook.raw"), PixelDim{ 17, 40 }),
			nullptr
	);

	hook->setPos(56, 0);
	addObject(hook);
}

void WackyStacky::WackyStacky::onLoop(float deltaTime){
	Game::onLoop(deltaTime);

    for(Event e; queue.get(e, 0); ){
        if(e.facility != Facility::Input){
            continue;
        }

        Input::Data* inputData = (Input::Data*) e.data;
        if(inputData == nullptr){
            continue;
        }

        if(inputData->action == Input::Data::Press && inputData->btn == Input::A){
            lastDrop = millis();
            // Drop the thing
            // Start timer to create a new one in the hook
            // Start to scroll up (move all robots and the floor down)
        }
    }

	if(hook->getRot() <= SwingLimits.x){
		swingDir = 1.0f;

		//swingLimits.y = (rand() * 40.0f + 20.0f) / INT_MAX;
	}else if(hook->getRot() >= SwingLimits.y){
		swingDir = -1.0f;

		//swingLimits.x = -((rand() * 40.0f + 20.0f) / INT_MAX);
	}

	const float speed = glm::max(-0.00001f * glm::pow(hook->getRot(), 4.0f) + SwingSpeed, 1.0f);

	rotateHook(hook->getRot() + swingDir * speed * deltaTime);
}

void WackyStacky::WackyStacky::rotateHook(float deg) const{
	if(!hook){
		return;
	}

	static constexpr const glm::vec2 HookBaseRelativeLocation = {
			0,
			-20
	};

	const glm::vec2 hookRotationTransformBefore = {
			HookBaseRelativeLocation.x * glm::cos(glm::radians(hook->getRot())) - HookBaseRelativeLocation.y * glm::sin(glm::radians(hook->getRot())),
			HookBaseRelativeLocation.x *  glm::sin(glm::radians(hook->getRot())) + HookBaseRelativeLocation.y * glm::cos(glm::radians(hook->getRot()))
	};

	hook->setPos(hook->getPos() + (hookRotationTransformBefore - HookBaseRelativeLocation));

	const glm::vec2 hookRotationTransform = {
			HookBaseRelativeLocation.x * glm::cos(glm::radians(deg)) - HookBaseRelativeLocation.y * glm::sin(glm::radians(deg)),
			HookBaseRelativeLocation.x *  glm::sin(glm::radians(deg)) + HookBaseRelativeLocation.y * glm::cos(glm::radians(deg))
	};

	hook->setRot(deg);
	hook->setPos(hook->getPos() - (hookRotationTransform - HookBaseRelativeLocation));

    if(hookedRobot){
        // Rotate the robot with the hook
    }
}

void WackyStacky::WackyStacky::attachRobot(uint8_t robot) const {

}
