#include "Sparkly.h"
#include <gtx/rotate_vector.hpp>
#include <esp_random.h>
#include "Util/Services.h"
#include "GameEngine/Rendering/StaticRC.h"

Sparkly::Sparkly::Sparkly(Sprite& canvas) : Game(canvas, Games::Sparkly, "/Games/Sparkly", {
		{ "/Tilemap.raw", {}, true },
		{ esp_random() % 2 == 0 ? "/Landscape2.raw" : "/Landscape1.raw", {}, true },
		{ "/Arrow-l.raw", {}, true },
		{ "/Arrow-r.raw", {}, true },
		{ "/Bush1.raw", {}, true },
		{ "/Bush2.raw", {}, true },
		{ "/Light-l.raw", {}, true },
		{ "/Light-r.raw", {}, true },
		{ "/Opponent1.raw", {}, true },
		{ "/Opponent2.raw", {}, true },
		{ "/Opponent3.raw", {}, true },
		{ "/Opponent4.raw", {}, true },
		{ "/Opponent5.raw", {}, true },
		{ "/Opponent6.raw", {}, true },
		{ "/Opponent7.raw", {}, true },
		{ "/Opponent8.raw", {}, true },
		{ "/Poster1.raw", {}, true },
		{ "/Poster2.raw", {}, true },
		{ "/Sparkly1.raw", {}, true },
		{ "/Sparkly2.raw", {}, true },
		{ "/Sparkly3.raw", {}, true },
		{ "/Speed-circle.raw", {}, true },
		{ "/Speed-line.raw", {}, true },
		{ "/Start1.raw", {}, true },
		{ "/Start2.raw", {}, true },
		{ "/Start3.raw", {}, true },
		{ "/Start4.raw", {}, true },
		{ "/Tree2.raw", {}, false },
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

	for(glm::vec2 board : Billboards){
		GameObjPtr obj = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile("/Bush1.raw"), PixelDim{ 32, 28 }),
				nullptr
		);
		addObject(obj);
		billboardGameObjs.emplace_back(obj);
	}

	playerCar = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/Sparkly2.raw"), PixelDim{50, 28}),
			nullptr
	);
	addObject(playerCar);
	playerCar->setPos(64.0f - 25.0f, 70.0f); // 39, 70
	playerCar->getRenderComponent()->setLayer(billboardGameObjs.size() - 1);
}

void Sparkly::Sparkly::onLoop(float deltaTime){
	Game::onLoop(deltaTime);

	movement(deltaTime);
}

void Sparkly::Sparkly::preRender(Sprite& canvas){
	Game::preRender(canvas);

	sampleGround(canvas);
	positionBillboards();
}

void Sparkly::Sparkly::handleInput(const Input::Data& data){
	Game::handleInput(data);

	if(data.btn == Input::A){
		spd += (data.action == Input::Data::Press) ? 1.0f : -1.0f;
	}else if(data.btn == Input::B){
		spd += (data.action == Input::Data::Press) ? -1.0f : 1.0f;
	}else if(data.btn == Input::Left){
		spdZ += (data.action == Input::Data::Press) ? 1.0f : -1.0f;
	}else if(data.btn == Input::Right){
		spdZ += (data.action == Input::Data::Press) ? -1.0f : 1.0f;
	}

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

	File tilemap = getFile("/Tilemap.raw");

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

			// Start pixel coords of needed sprite inside sprite-sheet [96x96]
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

			if(!tilemap.seek(index * 2)){
				continue;
			}

			uint16_t color;
			tilemap.read((uint8_t*) &color, 2);

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
	const float collisionFactor = collisionPoints > 0 ? 2.5f : 1.0f;

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

	for(size_t i = 0; i < billboardGameObjs.size(); ++i){
		const glm::vec3 origin(Billboards[i], 0.0f);

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

		screenCoords.x -= (float) 32.0f * 0.5f * scale;
		screenCoords.y += (float) 28.0f * scale;

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

	// Start pixel coords of needed sprite inside sprite-sheet [96x96]
	const int spriteStartX = spriteX * 32;
	const int spriteStartY = spriteY * 32;

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
