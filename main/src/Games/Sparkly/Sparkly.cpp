#include "Sparkly.h"
#include "GameEngine/Rendering/SpriteRC.h"
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
		{ "/Tree1.raw", {}, true },
		{ "/Tree2.raw", {}, false },
}), Proj(glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 20.0f)){
	const glm::mat4 view = glm::inverse(cameraTransform.getModelMatrix());
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
		spd += (data.action == Input::Data::Press) ? 1 : -1;
	}else if(data.btn == Input::B){
		spd += (data.action == Input::Data::Press) ? -1 : 1;
	}else if(data.btn == Input::Left){
		spdZ += (data.action == Input::Data::Press) ? 1 : -1;
	}else if(data.btn == Input::Right){
		spdZ += (data.action == Input::Data::Press) ? -1 : 1;
	}else if(data.btn == Input::Up){
		spdUD += (data.action == Input::Data::Press) ? 1 : -1;
	}else if(data.btn == Input::Down){
		spdUD += (data.action == Input::Data::Press) ? -1 : 1;
	}
}

void IRAM_ATTR Sparkly::Sparkly::sampleGround(Sprite& canvas){
	canvas.clear(TFT_BLACK);

	if(canvas.bufferLength() == 0){
		return;
	}

	File tilemap = getFile("/Tilemap.raw");

	glm::vec3 camPos = cameraTransform.getPosition();

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
			// Location of sprite inside sprite-sheet [3x3]
			const int spriteY = spriteIndex * 0.142857f;
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

	if(!skybox){
		return 0;
	}

	const glm::vec3 right = cameraTransform.getRight(); // Right vector in world-space according to camera view
	const glm::vec3 fw2 = glm::normalize(glm::cross(Up, right)); // Forward vector disregarding up/down rotation
	const float dot = glm::dot(fw2, glm::vec3{ 0.0f, 1.0f, 0.0f });
	const float angle = glm::acos(dot);
	const float anglePercent = angle * OverPI;

	int xSkybox = (fw2.x >= 0.0f ? 1.0f : -1.0f) * anglePercent * Width + x * 0.5f;
	if(xSkybox < 0){
		xSkybox += Width;
	}

	int ySkybox = y * 0.5f;
	if(glm::dot(cameraTransform.getUp(), Up) < 0.0f){
		ySkybox = Height - ySkybox;
	}

	uint16_t color = 0;
	skybox.seek(2 * (ySkybox * Width + xSkybox));
	skybox.read((uint8_t*) &color, 2);

	return color;
}

void Sparkly::Sparkly::movement(float dt){
	cameraTransform.rotate(cameraTransform.getRight(), spdUD * dt);
	cameraTransform.rotate(Up, spdZ * dt);
	cameraTransform.translate(cameraTransform.getPosition() + cameraTransform.getForward() * spd * dt);

	const glm::mat4 view = glm::inverse(cameraTransform.getModelMatrix());
	vpMat = Proj * view;
	vpInv = glm::inverse(vpMat);
}

void Sparkly::Sparkly::positionBillboards(){
	const glm::vec3 right = cameraTransform.getRight(); // Right vector in world-space according to camera view

	std::vector<std::pair<float, size_t>> billboardSortedIndexes;
	billboardSortedIndexes.reserve(billboardGameObjs.size());

	for(size_t i = 0; i < billboardGameObjs.size(); ++i){
		const glm::vec3 origin(Billboards[i], 0.0f);

		// Projecting the origin onto the forward vector. T indicates where the projected origin lies on the vector
		const glm::vec3 B = origin - cameraTransform.getPosition();
		const float t = glm::dot(B, cameraTransform.getForward());
		billboardSortedIndexes.emplace_back(t, i);
		if(t < 0) continue; // Behind camera
		if(t > 20) continue; // Front limit, adjust empirically

		const glm::vec4 originH = glm::vec4(origin, 1.0f);
		const auto screenPos = vpMat * originH;
		auto screenCoords = (glm::vec2(screenPos) / screenPos.w) * glm::vec2(64.0f, 64.0f) + glm::vec2(64.0f, -64.0f);

		const glm::vec4 posB = originH + glm::vec4(right, 0.0f);
		const auto screenPosB = vpMat * posB;
		const float scale = glm::abs(screenPos.x / screenPos.w - screenPosB.x / screenPosB.w) * 1.0f;

		// TODO figure out what to do with this BS because it makes the billboards float at certain angles
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

	for(size_t i = 0; i < billboardSortedIndexes.size(); ++i){
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
}
