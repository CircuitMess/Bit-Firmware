#include "WackyStacky.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Collision/RectCC.h"
#include "Util/stdafx.h"
#include <gtx/rotate_vector.hpp>
#include <esp_random.h>

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
}){

}

uint32_t WackyStacky::WackyStacky::getXP() const{
	return score * 3;
}

uint32_t WackyStacky::WackyStacky::getScore() const{
	return score;
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

	spawnRobot();

	for(size_t i = 0; i < ActiveCloudCount; ++i){
		const uint8_t cloudIndex = esp_random() % CloudCount;

		clouds[i] = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(CloudPaths[cloudIndex]), CloudDims[cloudIndex]),
				nullptr
		);

		clouds[i]->getRenderComponent()->setLayer(0);
		clouds[i]->setPos(glm::vec2{ (esp_random() % 110) - 10.0f, (esp_random() % 200) - 100.0f });

		addObject(clouds[i]);
	}
}

void WackyStacky::WackyStacky::handleInput(const Input::Data& data){
	if(data.action != Input::Data::Press || data.btn != Input::A) return;
	if(hookedRobot == nullptr || dropping || lives == 0 || scrolling) return;
	drop();
}

void WackyStacky::WackyStacky::onLoop(float deltaTime){
	swingAnim(deltaTime);

	// Has to be before tower swing anim
	dropAnim(deltaTime);

	scrollAnim(deltaTime);

	// Has to be after scroll anim
	towerSwingAnim(deltaTime);

	if(lives == 0){
		deadTimer += deltaTime;

		if(deadTimer >= 2.0f && !hookedRobot){
			exit();
		}
	}
}

void WackyStacky::WackyStacky::scrollAnim(float dt){
	if(!scrolling) return;

	size_t visibleCount = 0;
	for(size_t i = 0; i < VisibleRobotCount; ++i){
		if(!visibleRobots[i]){
			continue;
		}else{
			++visibleCount;
		}
	}

	float move = 25.0f * dt;
	if(move > scrollDelta){
		move = scrollDelta;
	}

	if(floor){
		floor->setPos(floor->getPos() + glm::vec2{ 0.0f, 1.0f } * move);
	}

	for(size_t i = 0; i < VisibleRobotCount; ++i){
		if(!visibleRobots[i]){
			continue;
		}

		visibleRobots[i].pos = visibleRobots[i].pos + glm::vec2{ 0.0f, 1.0f } * move;
		visibleRobots[i].go->setPos(visibleRobots[i].pos);
	}

	for(size_t i = 0; i < ActiveCloudCount; ++i){
		if(!clouds[i]){
			continue;
		}

		clouds[i]->setPos(clouds[i]->getPos() + glm::vec2{ 0.0f, 0.5f + (esp_random() % 6) * 0.05f } * move * 0.2f);

		if(clouds[i]->getPos().y >= 128.0f){
			clouds[i]->setPos(glm::vec2{ (esp_random() % 110) - 10.0f, -(esp_random() % 100) -30.0f });
		}
	}

	scrollDelta -= move;
	if(scrollDelta <= 0){
		scrollDelta = 0;
		scrolling = false;
		spawnRobot();
	}
}

void WackyStacky::WackyStacky::swingAnim(float dt){
	swingT += dt * SwingSpeed;
	if(swingT >= M_PI*2.0f){
		swingT -= M_PI*2.0f;
	}

	applyHookRot(map(std::cos(swingT), -1, 1, SwingLimits.x, SwingLimits.y));
}

void WackyStacky::WackyStacky::towerSwingAnim(float dt){
	if(score < 10 || ! visibleRobots.back()) return;

	towSwingT += dt * TowerSwingSpeed;
	if(towSwingT >= M_PI*2.0f){
		towSwingT -= M_PI*2.0f;
	}

	const float factor = std::min((float) (score-9) / 60.0f, 1.0f);

	for(auto& rob : visibleRobots){
		glm::vec2 diff = {
				std::sin(towSwingT) * 50.0f,
				std::abs(std::sin(towSwingT)) * 10
		};
		diff *= factor;

		rob.go->setPos(rob.pos + diff * factor);
		rob.posDiff = diff * factor;
	}
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
		updateRobotPos();
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
		newRot = dropStart.rot - map(std::min(std::pow(dropT * 1.5f, 2.0f), 1.0f), 0, 1, 0, std::abs(dropStart.rot)) * (dropStart.rot > 0 ? 1.0f : -1.0f);
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

void WackyStacky::WackyStacky::updateRobotPos(){
	const std::string path = getRobotPath(currentRobot);
	const glm::vec2 dimensions = getRobotDim(currentRobot);

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

	hookedRobot->setPos(robotCenter - dimensions / 2.0f);
	hookedRobot->setRot(hook->getRot());
}

bool WackyStacky::WackyStacky::scrollStart(){
	if(scrolling) return true;

	size_t visibleCount = 0;
	for(size_t i = 0; i < VisibleRobotCount; ++i){
		if(!visibleRobots[i]){
			continue;
		}else{
			++visibleCount;
		}
	}

	if(visibleCount < 2) return false;

	float currentHeight = visibleRobots.back().pos.y;
	if(currentHeight >= TargetHeight) return false;

	scrollDelta = TargetHeight - currentHeight;
	scrolling = true;

	return true;
}

void WackyStacky::WackyStacky::spawnRobot(){
	if(hookedRobot) return;

	currentRobot = esp_random() % 7;

	const std::string path = getRobotPath(currentRobot);
	const glm::vec2 dimensions = getRobotDim(currentRobot);

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
			collision.addPair(*visibleRobots[i].go, *hookedRobot, onCollision);
		}
	}

	updateRobotPos();
}

void WackyStacky::WackyStacky::miss(){
	--lives;
	hearts->setLives(lives);

	if(lives == 0){
		audio.play({
						   { 400, 200, 200 },
						   { 0, 0, 150 },
						   { 300, 150, 200 },
						   { 0, 0, 150 },
						   { 200, 100, 200 },
						   { 100, 100, 100 }
				   });
	}else{
		audio.play({ { 200, 50, 100 } });
	}

	collision.removePair(*floor, *hookedRobot);
	for(size_t i = 0; i < VisibleRobotCount; ++i){
		if(visibleRobots[i]){
			collision.removePair(*visibleRobots[i].go, *hookedRobot);
		}
	}
}

void WackyStacky::WackyStacky::robotFallen(){
	dropping = false;
	falling = false;
	removeObject(hookedRobot);
	hookedRobot.reset();

	if(lives != 0){
		if(!scrollStart()){
			spawnRobot();
		}
	}
}

void WackyStacky::WackyStacky::dropped(){
	if(!hookedRobot || (hookedRobot && hookedRobot->getPos().y < 40.0f)){
		return;
	}

	bool perfectHit = false;

	if(visibleRobots.back() && hookedRobot){
		auto hookedRect = CollisionSystem::getRotatedTranslatedRect(*hookedRobot.get());
		auto topRect = CollisionSystem::getRotatedTranslatedRect(*visibleRobots.back().go.get());

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

	{
		auto rect = CollisionSystem::getRotatedTranslatedRect(*hookedRobot.get());

		glm::vec2 lowestPoint = { 0.0f, 0.0f};
		for(auto point : rect){
			if(point.y > lowestPoint.y){
				lowestPoint = point;
			}
		}

		hookedRobot->setRot(0.0f);

		const float refPoint = visibleRobots.back() ? visibleRobots.back().go->getPos().y - visibleRobots.back().posDiff.y : floor->getPos().y;
		const float delta = getRobotDim(currentRobot).y;
		float center = hookedRobot->getPos().x + getRobotDim(currentRobot).x / 2;

		if(visibleRobots.back()){
			lowestPoint -= visibleRobots.back().posDiff;
			center -= visibleRobots.back().posDiff.x;
		}

		glm::vec2 pos = glm::vec2 { lowestPoint.x <= center ? lowestPoint.x : lowestPoint.x - getRobotDim(currentRobot).x, refPoint } - glm::vec2{ 0.0f, delta };
		hookedRobot->setPos(pos);
	}

	++score;
	if(perfectHit){
		++score;
	}

	scoreDisplay->setScore(score);

	if(visibleRobots.front()){
		removeObject(visibleRobots.front().go);
	}

	for(size_t i = 0; i < VisibleRobotCount - 1; ++i){
		visibleRobots[i] = visibleRobots[i + 1];
	}

	visibleRobots.back().go = hookedRobot;
	visibleRobots.back().pos = hookedRobot->getPos();
	hookedRobot->getRenderComponent()->setLayer(1);

	hookedRobot.reset();

	if(!scrollStart()){
		spawnRobot();
	}

	if(perfectHit){
		audio.play({ { 400, 600, 70 }, { 0, 0, 5 }, { 600, 900, 75 } });
	}else{
		audio.play({ { 400, 600, 75 } });
	}

	if(perfectHit){
		perfectCount++;
		setAchiIfBigger(Achievement::Stacky_5, perfectCount);
	}else{
		perfectCount = 0;
	}

	setAchiIfBigger(Achievement::Stacky_highrise, score);
	setAchiIfBigger(Achievement::Stacky_skyscraper, score);
	setAchiIfBigger(Achievement::Stacky_super, score);
}
