#include "WackyStacky.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Collision/RectCC.h"
#include "Util/stdafx.h"
#include <gtx/rotate_vector.hpp>

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
    Events::listen(Facility::Input, &queue);
	currentRobot = rand() % 7;
	towerSwingLimits = TowerSwingCoords[rand() % TowerSwingCoordsCount];
}

WackyStacky::WackyStacky::~WackyStacky() {
    Events::unlisten(&queue);
}

uint32_t WackyStacky::WackyStacky::getXP() const{
	return score * 3;
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
            std::make_unique<RectCC>(glm::vec2{ 128, 12 })
	);

	floor->setPos(0, 112);
	floor->getRenderComponent()->setLayer(1);
	addObject(floor);

	scoreDisplay = std::make_unique<Score>(getFile(FILE_GOBLET));
	scoreDisplay->getGO()->setPos({ 128 - 2 - 28, 2 });
	scoreDisplay->getGO()->getRenderComponent()->setLayer(1);
	addObject(scoreDisplay->getGO());
    scoreDisplay->setScore(0);

    hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
    hearts->getGO()->setPos({ 2, 2 });
	hearts->getGO()->getRenderComponent()->setLayer(1);
    addObject(hearts->getGO());
    hearts->setLives(3);

	hook = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/hook.raw"), PixelDim{ 17, 40 }),
			nullptr
	);

	hook->setPos(56, 0);
	hook->getRenderComponent()->setLayer(2);
	addObject(hook);

	attachRobot(currentRobot);

	for(size_t i = 0; i < ActiveCloudCount; ++i){
		const uint8_t cloudIndex = rand() % CloudCount;

		clouds[i] = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(CloudPaths[cloudIndex]), CloudDims[cloudIndex]),
				nullptr
		);

		clouds[i]->getRenderComponent()->setLayer(0);
		clouds[i]->setPos(glm::vec2{ (rand() % 110) - 10.0f, (rand() % 200) - 100.0f });

		addObject(clouds[i]);
	}
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

        if(inputData->action == Input::Data::Press && inputData->btn == Input::A && hookedRobot && !dropping){
			drop();
		}

		free(e.data);
    }

	if(score >= 10 && visibleRobots.back()){
		// Swing the stack
		const float speed = 0.1f * TowerSwingSpeed;
		const float horizontalDirection = visibleRobots.back()->getPos().x < towerSwingLimits.x ? 1.0f : -1.0f;
		const float verticalDirection = (visibleRobots.back()->getPos().y < towerSwingLimits.y ? 1.0f : -1.0f);

		for(size_t i = 0; i < VisibleRobotCount; ++i){
			if(!visibleRobots[i]){
				continue;
			}

			visibleRobots[i]->setPos(visibleRobots[i]->getPos() + glm::vec2{ horizontalDirection, verticalDirection } * speed * 0.5f);
		}

		if(glm::abs(visibleRobots.back()->getPos().x - towerSwingLimits.x) <= 0.1f){
			towerSwingLimits.x = TowerSwingCoords[rand() % TowerSwingCoordsCount].x;
		}

		if(glm::abs(visibleRobots.back()->getPos().y - towerSwingLimits.y) <= 0.1f){
			towerSwingLimits.y = TowerSwingCoords[rand() % TowerSwingCoordsCount].y;
		}
	}

	swingAnim(deltaTime);
	dropAnim(deltaTime);

	if(moveDelta > 0.0f){
		size_t visibleCount = 0;
		for(size_t i = 0; i < VisibleRobotCount; ++i){
			if(!visibleRobots[i]){
				continue;
			}else{
				++visibleCount;
			}
		}

		if(visibleCount >= 1){
			float move = 25.0f * deltaTime;
			if(move > moveDelta){
				move = moveDelta;
			}

			if(floor){
				floor->setPos(floor->getPos() + glm::vec2{ 0.0f, 1.0f } * move);
			}

			if(dropping && hookedRobot){
				hookedRobot->setPos(hookedRobot->getPos() + glm::vec2{ 0.0f, 1.0f } * move);
			}

			for(size_t i = 0; i < VisibleRobotCount; ++i){
				if(!visibleRobots[i]){
					continue;
				}

				visibleRobots[i]->setPos(visibleRobots[i]->getPos() + glm::vec2{ 0.0f, 1.0f } * move);
			}

			for(size_t i = 0; i < ActiveCloudCount; ++i){
				if(!clouds[i]){
					continue;
				}

				clouds[i]->setPos(clouds[i]->getPos() + glm::vec2{ 0.0f, 0.5f + (rand() % 6) * 0.05f } * move);

				if(clouds[i]->getPos().y >= 128.0f){
					clouds[i]->setPos(glm::vec2{ (rand() % 110) - 10.0f, -(rand() % 100) -30.0f });
				}
			}

			moveDelta -= move;
		}else{
			moveDelta = 0.0f;
		}
	}else if(moveDelta == 0.0f && !hookedRobot){
		attachRobot(currentRobot);
	}

	if(lives == 0){
		exit();
		return;
	}
}

void WackyStacky::WackyStacky::swingAnim(float dt){
	swingT += dt * SwingSpeed;
	if(swingT >= M_PI*2.0f){
		swingT -= M_PI*2.0f;
	}

	applyHookRot(map(std::cos(swingT), -1, 1, SwingLimits.x, SwingLimits.y));
}

void WackyStacky::WackyStacky::applyHookRot(float deg){
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

    if(hookedRobot && !dropping){
		attachRobot(currentRobot);
    }
}

void WackyStacky::WackyStacky::drop(){
	if(dropping || !hookedRobot) return;

	dropping = true;
	dropT = 0;
	dropPos = dropSpeed = {};

	dropStart = {
			hookedRobot->getPos(),
			hookedRobot->getRot()
	};
}

void WackyStacky::WackyStacky::dropAnim(float dt){
	if(!dropping || !hookedRobot) return;

	dropT += dt;

	dropSpeed += glm::vec2 { 0, 1.0f } * 140.0f * dt;
	dropPos += dropSpeed * dt;

	auto newPos = dropStart.pos + dropPos;
	auto newRot = 0.0f;
	if(falling){
		newRot = map(std::min(std::pow(dropT, 2.0f), 1.0f), 0, 1, 0, 20) * fallDir;
	}else{
		newRot = dropStart.rot - map(std::min(std::pow(dropT, 2.0f), 1.0f), 0, 1, 0, std::abs(dropStart.rot)) * (dropStart.rot > 0 ? 1.0f : -1.0f);
	}

	hookedRobot->setPos(newPos);
	hookedRobot->setRot(newRot);

	if(hookedRobot->getPos().y > 128.0f){
		if(!falling){
			miss();
		}

		robotFallen();
	}
}

void WackyStacky::WackyStacky::attachRobot(uint8_t robot){
	const std::string path = getRobotPath(robot);
	const glm::vec2 dimensions = getRobotDim(robot);

	if(!hook){
		return;
	}

	const glm::vec2 hookCenter = hook->getPos() + glm::vec2{ 8.0f, 19.5f };

	const glm::vec2 robotCenterRelativeLocation = glm::vec2{ 0, 19.5f } + glm::vec2{ 0, dimensions.y / 2.0f };

	const glm::vec2 robotRotationTransform = {
			robotCenterRelativeLocation.x * glm::cos(glm::radians(hook->getRot())) - robotCenterRelativeLocation.y * glm::sin(glm::radians(hook->getRot())),
			robotCenterRelativeLocation.x *  glm::sin(glm::radians(hook->getRot())) + robotCenterRelativeLocation.y * glm::cos(glm::radians(hook->getRot()))
	};

	const glm::vec2 robotCenter = hookCenter + robotRotationTransform;

	if(!hookedRobot){
		hookedRobot = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(path), dimensions),
				std::make_unique<RectCC>(dimensions)
		);

		hookedRobot->getRenderComponent()->setLayer(3);

		addObject(hookedRobot);

		auto onCollision = [this](){
			this->dropped();
		};

		collision.addPair(*floor, *hookedRobot, onCollision);

		for(size_t i = 0; i < VisibleRobotCount; ++i){
			if(visibleRobots[i]){
				collision.addPair(*visibleRobots[i], *hookedRobot, onCollision);
			}
		}
	}

	hookedRobot->setPos(robotCenter - dimensions / 2.0f);
	hookedRobot->setRot(hook->getRot());
}

void WackyStacky::WackyStacky::miss(){
	--lives;
	hearts->setLives(lives);
	audio.play({ { 200, 50, 100 } });

	collision.removePair(*floor, *hookedRobot);
	for(size_t i = 0; i < VisibleRobotCount; ++i){
		if(visibleRobots[i]){
			collision.removePair(*visibleRobots[i], *hookedRobot);
		}
	}
}

void WackyStacky::WackyStacky::robotFallen(){
	dropping = false;
	falling = false;
	moveDelta = 0.0f;
	removeObject(hookedRobot);
	hookedRobot.reset();
	currentRobot = rand() % 7;
}

void WackyStacky::WackyStacky::dropped(){
	if(hookedRobot && hookedRobot->getPos().y < 40.0f){
		return;
	}

	bool perfectHit = false;

	if(visibleRobots.back() && hookedRobot){
		auto hookedRect = CollisionSystem::getRotatedTranslatedRect(*hookedRobot.get());
		auto topRect = CollisionSystem::getRotatedTranslatedRect(*visibleRobots.back().get());

		float hookedCenter = 0;
		float topCenter = 0;
		glm::vec2 limits = { 128.0f, 0.0f };

		for(auto point : topRect){
			if(point.x < limits.x){
				limits.x = point.x;
			}

			if(point.x > limits.y){
				limits.y = point.x;
			}
		}

		for(auto point : hookedRect){
			hookedCenter += point.x;
		}

		hookedCenter /= hookedRect.size();

		for(auto point : topRect){
			topCenter += point.x;
		}

		topCenter /= topRect.size();

		if(hookedCenter < limits.x || hookedCenter > limits.y){
			miss();
			falling = true;
			fallDir = (hookedCenter < limits.x ? -1.0f : 1.0f);
			dropSpeed += glm::vec2 { fallDir * 40.0f, -60.0f };
			return;
		}

		if(glm::abs(topCenter - hookedCenter) <= 2){
			perfectHit = true;
		}
	}

	dropping = false;

	if(hookedRobot->getRot() != 0.0f){
		auto rect = CollisionSystem::getRotatedTranslatedRect(*hookedRobot.get());

		glm::vec2 lowestPoint = { 0.0f, 0.0f};
		for(auto point : rect){
			if(point.y > lowestPoint.y){
				lowestPoint = point;
			}
		}

		hookedRobot->setRot(0.0f);

		const float refPoint = visibleRobots.back() ? visibleRobots.back()->getPos().y : floor->getPos().y;
		const float delta = getRobotDim(currentRobot).y;
		const float center = hookedRobot->getPos().x + getRobotDim(currentRobot).x / 2;

		hookedRobot->setPos(glm::vec2{ lowestPoint.x <= center ? lowestPoint.x : lowestPoint.x - getRobotDim(currentRobot).x, refPoint } - glm::vec2{ 0.0f, delta });
	}

	size_t visibleCount = 0;
	for(size_t i = 0; i < VisibleRobotCount; ++i){
		if(!visibleRobots[i]){
			continue;
		}else{
			++visibleCount;
		}
	}

	++score;
	if(perfectHit){
		++score;
	}

	scoreDisplay->setScore(score);

	moveDelta = getRobotDim(currentRobot).y / (visibleCount < 1 ? 2.0f : 1.0f);

	currentRobot = rand() % 7;

	if(visibleRobots.front()){
		removeObject(visibleRobots.front());
	}

	for(size_t i = 0; i < VisibleRobotCount - 1; ++i){
		visibleRobots[i] = visibleRobots[i + 1];
	}

	visibleRobots.back() = hookedRobot;
	hookedRobot->getRenderComponent()->setLayer(1);
	hookedRobot.reset();

	if(perfectHit){
		audio.play({ { 400, 600, 70 }, { 0, 0, 5 }, { 600, 900, 75 } });
	}else{
		audio.play({ { 400, 600, 75 } });
	}
}
