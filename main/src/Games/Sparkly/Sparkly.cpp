#include "Sparkly.h"
#include <gtx/rotate_vector.hpp>
#include <esp_random.h>
#include "GameEngine/Rendering/StaticRC.h"

Sparkly::Sparkly::Sparkly(Sprite& canvas) : Game(canvas, Games::Sparkly, "/Games/Sparkly", {
		{ esp_random() % 2 == 0 ? "/Landscape2.raw" : "/Landscape1.raw", {}, true },
		{ "/Tiles-top.raw", {}, true },
		{ "/Tiles-bttm.raw", {}, true },
		{ "/Opponent1.raw", {}, true },
		{ "/Opponent2.raw", {}, true },
		{ "/Opponent3.raw", {}, true },
		{ "/Opponent4.raw", {}, true },
		{ "/Opponent5.raw", {}, true },
		{ "/Opponent6.raw", {}, true },
		{ "/Opponent7.raw", {}, true },
		{ "/Opponent8.raw", {}, true },
		{ "/Sparkly1.raw", {}, true },
		{ "/Sparkly2.raw", {}, true },
		{ "/Sparkly3.raw", {}, true },
		{ "/Speed-circle.raw", {}, false },
		{ "/Speed-line.raw", {}, false },
		{ "/Start1.raw", {}, false },
		{ "/Start2.raw", {}, false },
		{ "/Start3.raw", {}, false },
		{ "/Start4.raw", {}, false },
}), Proj(glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, 20.0f)){
	forward = glm::rotateY(forward, glm::radians(CameraAngle));
	const glm::mat4 view = glm::lookAt(camPos, camPos + forward, Up);
	vpMat = Proj * view;
	vpInv = glm::inverse(vpMat);
}

void Sparkly::Sparkly::onLoad(){
	Game::onLoad();

	skybox = getFile("/Landscape1.raw");
	if(!skybox){
		skybox = getFile("/Landscape2.raw");
	}

	for(const BillboardInfo& board : Billboards){
		GameObjPtr obj = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(BillboardAssets[board.assetId]), BillboardAssetDims[board.assetId]),
				nullptr
		);
		addObject(obj);
		billboardGameObjs.emplace_back(obj);
	}

	enemyCar = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/Opponent1.raw"), PixelDim{79, 29}),
			nullptr
	);
	addObject(enemyCar);
	enemyCar->getRenderComponent()->setLayer(billboardGameObjs.size() - 1);
	billboardGameObjs.emplace_back(enemyCar);

	playerCar = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/Sparkly2.raw"), PixelDim{50, 28}),
			nullptr
	);
	addObject(playerCar);
	playerCar->setPos(64.0f - 25.0f, 70.0f); // 39, 70
	playerCar->getRenderComponent()->setLayer(billboardGameObjs.size());

	startCountdown = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/Start1.raw"), PixelDim{50, 25}),
			nullptr
	);
	addObject(startCountdown);
	startCountdown->setPos(64.0f - 25.0f, 15.0f);
	startCountdown->getRenderComponent()->setLayer(100);

	speedLine = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/Speed-line.raw"), PixelDim{10, 1}),
			nullptr
	);
	addObject(speedLine);
	speedLine->setPos(112.0f, 115.0f);
	speedLine->setRot(-90.0f);
	speedLine->getRenderComponent()->setLayer(100);

	GameObjPtr speedCircle = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/Speed-circle.raw"), PixelDim{14, 14}),
			nullptr
	);
	addObject(speedCircle);
	speedCircle->setPos(110.0f, 110.0f);
	speedCircle->getRenderComponent()->setLayer(100);

	raceTimeElement = std::make_unique<RaceTime>();
	addObject(raceTimeElement->getGO());
	raceTimeElement->getGO()->setPos({ 2, 2 });
	raceTimeElement->getGO()->getRenderComponent()->setLayer(billboardGameObjs.size());
}

void Sparkly::Sparkly::onStop(){
	acceleration = 0;
	spdZ = 0;
	lPressed = rPressed = false;
	lastA = Input::Data::Release;
	lastB = Input::Data::Release;
}

void Sparkly::Sparkly::onLoop(float deltaTime){
	Game::onLoop(deltaTime);

	if(speedLine){
		speedLine->setRot(map(glm::abs(spd), 0.0f, 4.0f, -90.0f, 180.0f));
	}

	if(inputEnabled){
		timeSinceRaceStart += deltaTime;
	}

	if(raceTimeElement){
		raceTimeElement->setTime(timeSinceRaceStart);
	}

	timeSinceGameStart += deltaTime;

	// Enemy movement
	{
		enemyTargetStep = glm::clamp(enemyTargetStep, (size_t) 0, (size_t) 14);

		float timeInStep = timeSinceRaceStart;
		timeInStep -= EnemyMovementPoints[enemyTargetStep - 1].timestamp;

		const float timePercent = glm::min(glm::pow(timeInStep / (EnemyMovementPoints[enemyTargetStep].timestamp - EnemyMovementPoints[enemyTargetStep - 1].timestamp), 2.0f), 1.0f);

		const glm::vec2 newPos = EnemyMovementPoints[enemyTargetStep - 1].pos + timePercent * (EnemyMovementPoints[enemyTargetStep].pos - EnemyMovementPoints[enemyTargetStep - 1].pos);

		enemyPos = glm::vec3(newPos, 0.0f);
		enemyAngle = EnemyMovementPoints[enemyTargetStep - 1].rotation + timePercent * (EnemyMovementPoints[enemyTargetStep].rotation - EnemyMovementPoints[enemyTargetStep - 1].rotation);
		enemyForward = glm::rotateZ(glm::vec3{1.0f, 0.0f, 0.0f}, glm::radians(enemyAngle));

		if(timePercent >= 0.99f){
			timeInStep = timeSinceRaceStart;

			enemyTargetStep++;
		}
	}

	// Start UI changes
	{
		StaticRC* rc = nullptr;
		if(startCountdown){
			rc = (StaticRC*) startCountdown->getRenderComponent().get();
		}

		if(timeSinceGameStart >= 4.0f && startCountdown){
			removeObject(startCountdown);
			startCountdown.reset();
		}else if(timeSinceGameStart >= 3.0f){
			inputEnabled = true;

			if(rc != nullptr){
				rc->setFile(getFile("/Start4.raw"));
			}
		}else if(timeSinceGameStart >= 2.0f && rc != nullptr){
			rc->setFile(getFile("/Start3.raw"));
		}else if(timeSinceGameStart >= 1.0f && rc != nullptr){
			rc->setFile(getFile("/Start2.raw"));
		}
	}

	const glm::vec3 fw2 = glm::rotateZ(glm::vec3(1.0f, 0, 0), glm::radians(rotZ)); // Forward vector disregarding up/down rotation
	const glm::vec3 startingForward = glm::rotateZ(glm::vec3(1.0f, 0, 0), glm::radians(-90.0f));

	if(TopLeftCollision.spriteIndex == 28 ||
		TopLeftCollision.spriteIndex == 29 ||
		TopLeftCollision.spriteIndex == 17 ||
		TopRightCollision.spriteIndex == 28 ||
		TopRightCollision.spriteIndex == 29 ||
		TopRightCollision.spriteIndex == 17){
		if(!inFinish){
			inFinish = true;

			if(glm::dot(fw2, startingForward) > 0.0f && spd > 0.0f){
				++finishCrossings;
			}else{
				--finishCrossings;
			}
		}
	}else{
		inFinish = false;
	}

	if(finishCrossings >= 2){
		inputEnabled = false;
		acceleration = 0.0f;
		spdZ = 0.0f;
		finishTime = timeSinceRaceStart;
	}

	if(finishTime > 0.0f && timeSinceGameStart - finishTime >= 4.5f){
		if(!braked){
			addAchi(Achievement::Sparkly_nobrake, 1);
		}

		if(!hitBarrier){
			addAchi(Achievement::Sparkly_road, 1);
		}

		exit();
		return;
	}

	if(isColliding() > 0){
		spd = 0.0f;
		hitBarrier = true;
	}

	if(finishTime > 0.0f && timeSinceGameStart > finishTime){
		movement(0.25f * deltaTime);
	}else{
		movement(deltaTime);
	}
}

void Sparkly::Sparkly::preRender(Sprite& canvas){
	Game::preRender(canvas);

	sampleGround(canvas);
	positionBillboards();

	const glm::vec3 enemyRight = glm::cross(enemyForward, Up);
	const glm::vec3 camToEnemy = glm::normalize(enemyPos - glm::vec3{ camPos.x, camPos.y, 0.0f });

	const float forwardDot = glm::dot(forward, enemyForward);
	const float rightDot = glm::dot(enemyRight, camToEnemy);

	float angle = glm::degrees(glm::acos(forwardDot));

	if(angle >= 22.5f && angle <= 157.5){
		angle *= SIGN(rightDot);
	}

	File carFile;
	if(glm::abs(angle) < 22.5f){
		carFile = getFile("/Opponent5.raw");
	}else if(glm::abs(angle) > 157.5f){
		carFile = getFile("/Opponent1.raw");
	}else if(angle >= 22.5f && angle < 67.5f){ // These are on the left side of the car
		carFile = getFile("/Opponent6.raw");
	}else if(angle >= 67.5f && angle < 112.5f){
		carFile = getFile("/Opponent7.raw");
	}else if(angle >= 112.5f && angle <= 157.5f){
		carFile = getFile("/Opponent8.raw");
	}else if(angle <= -22.5f && angle > -67.5f){ // These are on the right side of the car
		carFile = getFile("/Opponent4.raw");
	}else if(angle <= -67.5f && angle > -112.5f){
		carFile = getFile("/Opponent3.raw");
	}else if(angle <= -112.5f && angle >= -157.5f){
		carFile = getFile("/Opponent2.raw");
	}

	if(!carFile){
		return;
	}

	StaticRC* rc = (StaticRC*) enemyCar->getRenderComponent().get();
	if(rc == nullptr){
		return;
	}

	if(rc->getFile() == carFile){
		return;
	}

	rc->setFile(carFile);
}

void Sparkly::Sparkly::handleInput(const Input::Data& data){
	Game::handleInput(data);

	if(data.btn == Input::A && data.action != lastA){
		acceleration += (data.action == Input::Data::Press) ? 1.0f : -1.0f;
	}else if(data.btn == Input::B && data.action != lastB){
		if(inputEnabled && data.action == Input::Data::Press && spd > 0){
			braked = true;
		}

		acceleration += (data.action == Input::Data::Press) ? -1.0f : 1.0f;
	}else if(data.btn == Input::Left){
		if(data.action == Input::Data::Press || (data.action == Input::Data::Release && lPressed)){
			spdZ += (data.action == Input::Data::Press) ? 1.0f : -1.0f;
			lPressed = true;
		}
	}else if(data.btn == Input::Right){
		if(data.action == Input::Data::Press || (data.action == Input::Data::Release && rPressed)){
			rPressed = true;
			spdZ += (data.action == Input::Data::Press) ? -1.0f : 1.0f;
		}
	}

	if(data.btn == Input::A){
		lastA = data.action;
	}else if(data.btn == Input::B){
		lastB = data.action;
	}

	if(!inputEnabled) return;

	if(!playerCar){
		return;
	}

	StaticRC* rc = (StaticRC*) playerCar->getRenderComponent().get();
	if(rc == nullptr){
		return;
	}

	File sparkly;

	if(spdZ < -0.1f){
		sparkly = getFile("/Sparkly3.raw");
	}else if(spdZ > 0.1f){
		sparkly = getFile("/Sparkly1.raw");
	}else{
		sparkly = getFile("/Sparkly2.raw");
	}

	if(sparkly && rc->getFile() != sparkly){
		rc->setFile(sparkly);
	}
}

void IRAM_ATTR Sparkly::Sparkly::sampleGround(Sprite& canvas){
	canvas.clear(TFT_BLACK);

	if(canvas.bufferLength() == 0){
		return;
	}

	File tilemapTop = getFile("/Tiles-top.raw");
	File tilemapBttm = getFile("/Tiles-bttm.raw");

	if(camPos.z < 0.01f) return; // Camera is below the ground plane

	for(int x = 0; x < 128; ++x){
		for(int y = 0; y < 128; ++y){
			// The magic number is 1/127 -- multiplication is faster than division. This optimisation alone yields ~10% faster rendering
			const float posX = ((float) x * 0.007874016f) * 2.0f - 1.0f;
			const float posY = ((float) y * 0.007874016f) * 2.0f - 1.0f;

			const glm::vec4 screenPos = glm::vec4(posX, -posY, 1.0f, 1.0f);
			const glm::vec4 worldPos = vpInv * screenPos;

			// Should be normalized or at the very least divided by worldPos.w -- omitted for optimization
			const glm::vec3 ray = glm::vec3(worldPos);

			const float t = - camPos.z / ray.z;
			if(t < 0){
				canvas.drawPixel(x, y, sampleSkybox(x, y));
				continue; // Behind camera
			}

			if(t > 20){
				canvas.drawPixel(x, y, sampleSkybox(x, y));
				continue; // Front limit, adjust empirically
			}

			// x/y coords on the z=0 plane (world space)
			float planeX = t * ray.x + camPos.x;
			float planeY = t * ray.y + camPos.y;

			if(planeX >= 10 || planeX < -10 || planeY >= 10 || planeY < -10){
				canvas.drawPixel(x, y, sampleSkybox(x, y));
				continue;
			}

			planeX = planeX + 10.0f;
			planeY = planeY + 10.0f;
			const int planeXfloor = planeX; // opt: var should be float, value should be floored
			const int planeYfloor = planeY; // opt: var should be float, value should be floored

			const int spriteIndex = Field[(int) planeYfloor][(int) planeXfloor];
			// Location of sprite inside sprite-sheet [7x6]
			const int spriteY = spriteIndex * 0.142858f;
			const int spriteX = spriteIndex - spriteY * 7;

			// Start pixel coords of needed sprite inside sprite-sheet [224x192]
			const int spriteStartX = spriteX * 32;
			const int spriteStartY = spriteY * 32;

			// Pixel pos inside needed sprite [0x1]
			const float pixelX = planeX - (float) planeXfloor;
			const float pixelY = planeY - (float) planeYfloor;
			// Pixel pos inside needed sprite [32x32]
			const int spritePixelX = pixelX * 32.0f; // opt: value should be floored
			const int spritePixelY = pixelY * 32.0f; // opt: value should be floored

			const glm::vec<2, int> coords = { x, y };
			if(coords == CarTopRight){
				TopRightCollision.skybox = false;
				TopRightCollision.spriteIndex = spriteIndex;
				TopRightCollision.spriteCoords = { spritePixelX, spritePixelY };
			}else if(coords == CarTopLeft){
				TopLeftCollision.skybox = false;
				TopLeftCollision.spriteIndex = spriteIndex;
				TopLeftCollision.spriteCoords = { spritePixelX, spritePixelY };
			}else if(coords == CarBttmLeft){
				BttmLeftCollision.skybox = false;
				BttmLeftCollision.spriteIndex = spriteIndex;
				BttmLeftCollision.spriteCoords = { spritePixelX, spritePixelY };
			}else if(coords == CarBttmRight){
				BttmRightCollision.skybox = false;
				BttmRightCollision.spriteIndex = spriteIndex;
				BttmRightCollision.spriteCoords = { spritePixelX, spritePixelY };
			}

			const int index = (spriteStartX + spritePixelX) + (spriteStartY + spritePixelY) * 224;

			uint16_t color;
			if(spriteIndex <= 20){
				tilemapTop.seek(index * 2);
				tilemapTop.read((uint8_t*) &color, 2);
			}else{
				tilemapBttm.seek((index - 21504) * 2);
				tilemapBttm.read((uint8_t*) &color, 2);
			}

			canvas.drawPixel(x, y, color);
		}
	}
}

uint16_t Sparkly::Sparkly::sampleSkybox(int x, int y){
	static constexpr const float OverPI = 1.0f / glm::pi<float>();
	static constexpr const uint16_t Width = 256;
	static constexpr const uint16_t Height = 64;

	const glm::vec<2, int> coord{x, y};

	if(coord == CarTopLeft){
		TopLeftCollision = {};
		TopLeftCollision.skybox = true;
	}else if(coord == CarTopRight){
		TopRightCollision = {};
		TopRightCollision.skybox = true;
	}else if(coord == CarBttmLeft){
		BttmLeftCollision = {};
		BttmLeftCollision.skybox = true;
	}else if(coord == CarBttmRight){
		BttmRightCollision = {};
		BttmRightCollision.skybox = true;
	}

	if(!skybox){
		return 0;
	}

	const glm::vec3 fw2 = glm::rotateZ(glm::vec3(1.0f, 0, 0), glm::radians(rotZ)); // Forward vector disregarding up/down rotation
	const float dot = glm::dot(fw2, glm::vec3{ 0.0f, 1.0f, 0.0f });
	const float angle = glm::acos(dot);
	const float anglePercent = angle * OverPI;

	int xSkybox = (fw2.x >= 0.0f ? 1.0f : -1.0f) * anglePercent * Width + x * 0.5f;
	if(xSkybox < 0){
		xSkybox += Width;
	}

	int ySkybox = y * 0.5f;

	uint16_t color = 0;
	skybox.seek(2 * (ySkybox * Width + xSkybox));
	skybox.read((uint8_t*) &color, 2);

	return color;
}

void Sparkly::Sparkly::movement(float dt){
	const glm::vec3 fw2 = glm::rotateZ(glm::vec3(1.0f, 0, 0), glm::radians(rotZ)); // Forward vector disregarding up/down rotation

	const uint16_t collisionPoints = isColliding();
	const float collisionFactor = collisionPoints > 0 ? 5.0f : 1.0f;

	if(inputEnabled){
		spd += acceleration * dt * collisionFactor - SIGN(spd) * Friction * dt;
		spd = glm::clamp(spd, -4.0f, 4.0f);
	}

	if(spd != 0.0f){
		rotZ += (spd > 0 ? 1.0f : -1.0f) * spdZ * dt * 50.0f * collisionFactor;
		rotZ -= (int) (rotZ / 360.0f) * 360.0f;
		camPos += fw2 * spd * dt * collisionFactor;
	}

	{
		forward = glm::vec3{ 1.0f, 0.0f, 0.0f };
		forward = glm::rotateY(forward, glm::radians(CameraAngle));
		forward = glm::rotateZ(forward, glm::radians(rotZ));

		const glm::mat4 view = glm::lookAt(camPos, camPos + forward, Up);
		vpMat = Proj * view;
		vpInv = glm::inverse(vpMat);
	}

	const uint16_t tempCollisionPoints = isCollidingTemporary();

	if(tempCollisionPoints > 0 && collisionPoints > 0 && tempCollisionPoints >= collisionPoints){
		if(spd != 0.0f){
			rotZ -= (spd > 0 ? 1.0f : -1.0f) * spdZ * dt * 50.0f * collisionFactor;
			if(rotZ < 0.0f){
				rotZ += 360.0f;
			}
			camPos -= fw2 * spd * dt * collisionFactor;
		}

		{
			forward = glm::vec3{ 1.0f, 0.0f, 0.0f };
			forward = glm::rotateY(forward, glm::radians(CameraAngle));
			forward = glm::rotateZ(forward, glm::radians(rotZ));

			const glm::mat4 view = glm::lookAt(camPos, camPos + forward, Up);
			vpMat = Proj * view;
			vpInv = glm::inverse(vpMat);
		}
	}
}

void Sparkly::Sparkly::positionBillboards(){
	const glm::vec3 fw2 = glm::rotateZ(glm::vec3(1.0f, 0, 0), glm::radians(rotZ)); // Forward vector disregarding up/down rotation
	const auto Right = glm::cross(Up, fw2); // Right vector in world-space according to camera view

	std::vector<std::pair<float, size_t>> billboardSortedIndexes;
	billboardSortedIndexes.reserve(billboardGameObjs.size());

	{
		const glm::vec3 origin(enemyPos.x, enemyPos.y, 0.0f);

		// Projecting the origin onto the forward vector. T indicates where the projected origin lies on the vector
		const glm::vec3 B = origin - camPos;
		const float t = glm::dot(B, forward);
		billboardSortedIndexes.emplace_back(t, billboardGameObjs.size() - 1);

		if(t >= 0.0f && t <= 20.0f){
			const glm::vec4 originH = glm::vec4(origin, 1.0f);
			const auto screenPos = vpMat * originH;
			auto screenCoords = (glm::vec2(screenPos) / screenPos.w) * glm::vec2(64.0f, 64.0f) + glm::vec2(64.0f, -64.0f);

			const glm::vec4 posB = originH + glm::vec4(Right, 0.0f);
			const auto screenPosB = vpMat * posB;
			const float scale = glm::abs(screenPos.x / screenPos.w - screenPosB.x / screenPosB.w) * 0.65f;

			screenCoords.x -= (float) 79.0f * 0.5f * scale;
			screenCoords.y += (float) 29.0f * scale;

			screenCoords.y = -screenCoords.y;

			enemyCar->setPos(screenCoords);

			if(StaticRC* rc = (StaticRC*) enemyCar->getRenderComponent().get()){
				rc->setScale(glm::vec2{scale});
			}
		}
	}

	for(size_t i = 0; i < billboardGameObjs.size() - 1; ++i){
		const glm::vec3 origin(Billboards[i].position, 0.0f);

		// Projecting the origin onto the forward vector. T indicates where the projected origin lies on the vector
		const glm::vec3 B = origin - camPos;
		const float t = glm::dot(B, forward);
		billboardSortedIndexes.emplace_back(t, i);
		if(t < 0) continue; // Behind camera
		if(t > 20) continue; // Front limit, adjust empirically

		const glm::vec4 originH = glm::vec4(origin, 1.0f);
		const auto screenPos = vpMat * originH;
		auto screenCoords = (glm::vec2(screenPos) / screenPos.w) * glm::vec2(64.0f, 64.0f) + glm::vec2(64.0f, -64.0f);

		const glm::vec4 posB = originH + glm::vec4(Right, 0.0f);
		const auto screenPosB = vpMat * posB;
		const float scale = glm::abs(screenPos.x / screenPos.w - screenPosB.x / screenPosB.w) * 1.0f;

		screenCoords.x -= (float) BillboardAssetDims[Billboards[i].assetId].x * 0.5f * scale;
		screenCoords.y += (float) BillboardAssetDims[Billboards[i].assetId].y * scale;

		screenCoords.y = -screenCoords.y;

		billboardGameObjs[i]->setPos(screenCoords);

		StaticRC* rc = (StaticRC*) billboardGameObjs[i]->getRenderComponent().get();
		if(rc == nullptr){
			continue;
		}

		rc->setScale(glm::vec2{scale});
	}

	std::sort(billboardSortedIndexes.begin(), billboardSortedIndexes.end(), [](const std::pair<float, size_t>& a, const std::pair<float, size_t>& b) {
		return a.first > b.first;
	});

	size_t i;
	for(i = 0; i < billboardSortedIndexes.size(); ++i){
		StaticRC* rc = (StaticRC*) billboardGameObjs[billboardSortedIndexes[i].second]->getRenderComponent().get();
		if(rc == nullptr){
			continue;
		}

		if(billboardSortedIndexes[i].first > 10 || billboardSortedIndexes[i].first < 0){
			rc->setVisible(false);
		}else{
			rc->setVisible(true);
		}

		rc->setLayer(i);
	}

	if(playerCar){
		playerCar->getRenderComponent()->setVisible(true);
		playerCar->getRenderComponent()->setLayer(i - 1);
	}
}

bool Sparkly::Sparkly::isCollisionBlocking(const CollisionInfo& collision){
	if(collision.skybox){
		return true;
	}

	if(collision.spriteIndex < 27 || collision.spriteIndex > 38){
		return false;
	}

	if(collision.spriteIndex == 30 || collision.spriteIndex == 33){
		return true;
	}

	if(collision.spriteIndex == 29 || collision.spriteIndex == 28){
		return false;
	}

	bool linear = false;
	glm::vec<2, int> corner = {};

	switch(collision.spriteIndex){
		case 27:{
			corner = { 0, 0 };
			break;
		}
		case 31:{
			corner = { 32, 32 };
			break;
		}
		case 32:{
			corner = { 0, 32 };
			break;
		}
		case 34:{
			linear = true;
			corner = { 32, 32 };
			break;
		}
		case 35:{
			linear = true;
			corner = { 0, 0 };
			break;
		}
		case 36:{
			corner = { 32, 0 };
			break;
		}
		case 37:{
			linear = true;
			corner = { 0, 32 };
			break;
		}
		case 38:{
			linear = true;
			corner = { 32, 0 };
			break;
		}
		default:{
			return false;
		}
	}

	if(linear){
		glm::vec2 dirVec = { 32.0f, 32.0f };

		if(collision.spriteCoords.x < corner.x){
			dirVec.x *= -1.0f;
		}

		if(collision.spriteCoords.y < corner.y){
			dirVec.y *= -1.0f;
		}

		const float x = (collision.spriteCoords.x - corner.x) / dirVec.x;
		const float y = (collision.spriteCoords.y - corner.y) / dirVec.y;

		return x + y > 1.0f;
	}else{
		return glm::distance(glm::vec2(corner), glm::vec2(collision.spriteCoords)) > 32.0f;
	}
}

Sparkly::Sparkly::CollisionInfo Sparkly::Sparkly::getCollision(int x, int y) const{
	if(camPos.z < 0.01f) return {.skybox = true}; // Camera is below the ground plane

	// The magic number is 1/127 -- multiplication is faster than division. This optimisation alone yields ~10% faster rendering
	const float posX = ((float) x * 0.007874016f) * 2.0f - 1.0f;
	const float posY = ((float) y * 0.007874016f) * 2.0f - 1.0f;

	const glm::vec4 screenPos = glm::vec4(posX, -posY, 1.0f, 1.0f);
	const glm::vec4 worldPos = vpInv * screenPos;

	// Should be normalized or at the very least divided by worldPos.w -- omitted for optimization
	const glm::vec3 ray = glm::vec3(worldPos);

	const float t = - camPos.z / ray.z;
	if(t < 0){
		return {.skybox = true};
	}

	if(t > 20){
		return {.skybox = true};
	}

	// x/y coords on the z=0 plane (world space)
	float planeX = t * ray.x + camPos.x;
	float planeY = t * ray.y + camPos.y;

	if(planeX >= 10 || planeX < -10 || planeY >= 10 || planeY < -10){
		return {.skybox = true};
	}

	planeX = planeX + 10.0f;
	planeY = planeY + 10.0f;
	const int planeXfloor = planeX; // opt: var should be float, value should be floored
	const int planeYfloor = planeY; // opt: var should be float, value should be floored

	const int spriteIndex = Field[(int) planeYfloor][(int) planeXfloor];
	// Location of sprite inside sprite-sheet [7x6]
	const int spriteY = spriteIndex * 0.142858f;
	const int spriteX = spriteIndex - spriteY * 7;

	// Pixel pos inside needed sprite [0x1]
	const float pixelX = planeX - (float) planeXfloor;
	const float pixelY = planeY - (float) planeYfloor;
	// Pixel pos inside needed sprite [32x32]
	const int spritePixelX = pixelX * 32.0f; // opt: value should be floored
	const int spritePixelY = pixelY * 32.0f; // opt: value should be floored

	return {.spriteIndex = spriteIndex, .spriteCoords = { spritePixelX, spritePixelY }, .skybox = false};
}

uint16_t Sparkly::Sparkly::isColliding() const{
	return (uint16_t) isCollisionBlocking(TopRightCollision) + (uint16_t) isCollisionBlocking(TopLeftCollision) + (uint16_t) isCollisionBlocking(BttmRightCollision) + (uint16_t) isCollisionBlocking(BttmLeftCollision);
}

uint16_t Sparkly::Sparkly::isCollidingTemporary() const{
	return (uint16_t) isCollisionBlocking(getCollision(CarTopRight.x, CarTopRight.y)) + (uint16_t) isCollisionBlocking(getCollision(CarTopLeft.x, CarTopLeft.y)) + (uint16_t) isCollisionBlocking(getCollision(CarBttmRight.x, CarBttmRight.y)) + (uint16_t) isCollisionBlocking(getCollision(CarBttmLeft.x, CarBttmLeft.y));
}
