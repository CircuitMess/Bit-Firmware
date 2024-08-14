#include "Test3D.h"
#include "Devices/Input.h"
#include "gtx/rotate_vector.hpp"
#include <esp_random.h>
#include "GameEngine/Rendering/StaticRC.h"

Test3D::Test3D(Sprite& base) : Game(base, Games::Sparkly, "/Games/Sparkly", {
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
}), canvas(base), evts(12), Proj(glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 20.0f)){
	canvas.setTextColor(TFT_GREEN);
	canvas.setTextFont(0);
	canvas.setTextSize(1);

	Events::listen(Facility::Input, &evts);
}

void Test3D::onLoad(){
	Game::onLoad();

	for(auto board : Boards){
		GameObjPtr obj = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(Assets[board.index].path), Assets[board.index].dim),
				nullptr
		);
		addObject(obj);
		billboardGameObjs.emplace_back(obj);
	}
}

void Test3D::preRender(Sprite& canvas){
	calcView();
	renderGround();
	renderBillboards();
}

void Test3D::onLoop(float dt){
	if(dt == 0) return;

	movement(dt);

	// Framerate display
	const float fps = 1.0f / dt;
	char fpstext[32];
	sprintf(fpstext, "%.1f", fps);

	canvas.setCursor(2, 2);
	canvas.write(fpstext);

	// Push to display
	canvas.pushSprite(0, 0);
}

void Test3D::movement(float dt){
	Event evt {};
	if(evts.get(evt, 0)){
		const auto data = (Input::Data*) evt.data;

		if(data->btn == Input::A){
			spd += (data->action == Input::Data::Press) ? 1 : -1;
		}else if(data->btn == Input::B){
			spd += (data->action == Input::Data::Press) ? -1 : 1;
		}else if(data->btn == Input::Left){
			spdZ += (data->action == Input::Data::Press) ? 1 : -1;
		}else if(data->btn == Input::Right){
			spdZ += (data->action == Input::Data::Press) ? -1 : 1;
		}else if(data->btn == Input::Up){
			spdUD += (data->action == Input::Data::Press) ? 1 : -1;
		}else if(data->btn == Input::Down){
			spdUD += (data->action == Input::Data::Press) ? -1 : 1;
		}
	}

	rotUD += spdUD * dt * 140.0f;
	rotZ += spdZ * dt * 140.0f;

	while(rotZ >= 360){
		rotZ -= 360.0f;
	}
	while(rotZ <= 360){
		rotZ += 360.0f;
	}
	if(rotUD < -89){
		rotUD = -89;
	}else if(rotUD > 89){
		rotUD = 89;
	}

	forward = { 1, 0, 0 };
	forward = glm::rotateY(forward, glm::radians(-rotUD)); // Rotation is limited to [-89, 89] deg (above) - disregard gimbal lock
	forward = glm::rotateZ(forward, glm::radians(rotZ));
	// After rotations, the forward vector should remain normalized (disregarding fp rounding errors)

	camPos += forward * spd * dt;
}

void Test3D::calcView(){
	const auto view = glm::lookAt(camPos, camPos + forward, Up);
	vpMat = Proj * view;
	vpInv = glm::inverse(vpMat);
}

void IRAM_ATTR Test3D::renderGround(){
	canvas.clear(TFT_BLACK);

	if(camPos.z < 0.01f) return; // Camera is below the ground plane

	for(int x = 0; x < 128; x++){
		for(int y = 0; y < 128; y++){
			// The magic number is 1/127 -- multiplication is faster than division. This optimisation alone yields ~10% faster rendering
			const float posX = ((float) x * 0.007874016f) * 2.0f - 1.0f;
			const float posY = ((float) y * 0.007874016f) * 2.0f - 1.0f;

			const glm::vec4 screenPos = glm::vec4(posX, -posY, 1.0f, 1.0f);
			const glm::vec4 worldPos = vpInv * screenPos;

			// Should be normalized or at the very least divided by worldPos.w -- omitted for optimization
			const glm::vec3 ray = glm::vec3(worldPos);

			const float t = - camPos.z / ray.z;
			if(t < 0) continue; // Behind camera
			if(t > 10) continue; // Front limit, adjust empirically

			// x/y coords on the z=0 plane (world space)
			const float planeX = t * ray.x + camPos.x;
			const float planeY = t * ray.y + camPos.y;

			const auto c = sampleGround(planeX, planeY);
			canvas.drawPixel(x, y, c);
		}
	}
}

void IRAM_ATTR Test3D::renderBillboards(){
	const glm::vec3 fw2 = glm::rotateZ(glm::vec3(1.0f, 0, 0), glm::radians(rotZ)); // Forward vector disregarding up/down rotation
	const auto Right = glm::cross(Up, fw2); // Right vector in world-space according to camera view

	struct DrawInfo {
		glm::vec2 pos;
		float scale;
		int index;
		float distance;
	};
	std::vector<DrawInfo> draws;
	draws.reserve(Count(Boards));

	for(int i = 0; i < Count(Boards); ++i){
		auto board = Boards[i];
		const auto& asset = Assets[board.index];

		const glm::vec3 origin(board.pos, 0);

		// Projecting the origin onto the forward vector. T indicates where the projected origin lies on the vector
		const glm::vec3 B = origin - camPos;
		const float t = glm::dot(B, forward);
		if(t < 0) continue; // Behind camera
		if(t > 10) continue; // Front limit, adjust empirically

		const glm::vec4 originH = glm::vec4(origin, 1.0f);
		const auto screenPos = vpMat * originH;
		auto screenCoords = (glm::vec2(screenPos) / screenPos.w) * glm::vec2(64, 64) + glm::vec2(64, -64);

		const glm::vec4 posB = originH + glm::vec4(Right, 0);
		const auto screenPosB = vpMat * posB;
		const float scale = (screenPos.x / screenPos.w - screenPosB.x / screenPosB.w) * board.scale;

		screenCoords.x -= (float) asset.dim.x * 0.5f * scale;
		screenCoords.y += (float) asset.dim.y * scale;

		screenCoords.y = -screenCoords.y;

		draws.emplace_back(DrawInfo {
				.pos = screenCoords,
				.scale = scale,
				.index = i,
				.distance = t
		});
	}

	std::sort(draws.begin(), draws.end(), [](const DrawInfo& a, const DrawInfo& b){ return a.distance > b.distance; });

	for(size_t i = 0; i < draws.size(); ++i){
		const auto& draw = draws[i];
		GameObjPtr gameObj = billboardGameObjs[draw.index];
		gameObj->setPos(draw.pos);

		StaticRC* rc = (StaticRC*) gameObj->getRenderComponent().get();
		rc->setScale({ draw.scale, draw.scale });
		rc->setLayer(i + 1);

		if(draw.distance <= 0.0f || draw.distance >= 10.0f){
			rc->setVisible(false);
		}else{
			rc->setVisible(true);
		}
	}
}

uint16_t IRAM_ATTR Test3D::sampleGround(float planeX, float planeY){
	/**
	 * The ground plane is from x/y [-2, -2] to [2, 2] and is divided into a 4x4 grid. It
	 * uses sprites from the spritemap according to the Field structure. This function gets
	 * called for every screen pixel that intersects the z=0 plane.
	 * Pixels that are outside [-2, 2] on either x or y are black. Other pixels sample the spritemap.
	 */

	// im pretty sure this code is wrong, but the end result looks acceptable for testing

	if(planeX >= 2 || planeX < -2 || planeY >= 2 || planeY < -2) return TFT_BLACK;

	planeX = planeX + 2.0f;
	planeY = planeY + 2.0f;
	const int planeXfloor = planeX; // opt: var should be float, value should be floored
	const int planeYfloor = planeY; // opt: var should be float, value should be floored

	const int spriteIndex = Field[(int) planeYfloor][(int) planeXfloor];
	// Location of sprite inside sprite-sheet [3x3]
	const int spriteY = spriteIndex * 0.142857f;
	const int spriteX = spriteIndex - spriteY * 7;

	// Start pixel coords of needed sprite inside sprite-sheet
	const int spriteStartX = spriteX * 32;
	const int spriteStartY = spriteY * 32;

	// Pixel pos inside needed sprite [0x1]
	const float pixelX = planeX - (float) planeXfloor;
	const float pixelY = planeY - (float) planeYfloor;
	// Pixel pos inside needed sprite [32x32]
	const int spritePixelX = pixelX * 32.0f; // opt: value should be floored
	const int spritePixelY = pixelY * 32.0f; // opt: value should be floored

	const int index = (spriteStartX + spritePixelX) + (spriteStartY + spritePixelY) * 224;

	uint16_t pixel = 0;
	File file = getFile(Assets[0].path);
	file.seek(index * 2);
	file.read((uint8_t*) &pixel, 2);

	return pixel;
}
