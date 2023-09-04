#include "OnStick.h"
#include "GameEngine/Rendering/StaticRC.h"

struct CharInfo {
	const char* path;
	PixelDim size;
};

static const std::unordered_map<OnStick::Char, CharInfo> ResInfos = {
		{ OnStick::DuckYellow, { "/stick1.raw", { 19, 17 } } },
		{ OnStick::DuckPurple, { "/stick2.raw", { 19, 15 } } },
		{ OnStick::DuckSmallYellow, { "/stick3.raw", { 13, 9 } } },
		{ OnStick::DuckSmallPurple, { "/stick4.raw", { 13, 11 } } },
		{ OnStick::Artemis, { "/stick5.raw", { 19, 20 } } }
};

static const std::unordered_map<OnStick::Char, glm::ivec2> Offsets = {
		{ OnStick::DuckYellow, { -10, -17 } },
		{ OnStick::DuckPurple, { -7, -15 } },
		{ OnStick::DuckSmallYellow, { -5, -9 } },
		{ OnStick::DuckSmallPurple, { -6, -11 } },
		{ OnStick::Artemis, { -9, -11 } }
};

OnStick::OnStick(Char chr, std::function<void(GameObjPtr)> addObject, std::function<File(const char*)> getFile) : Speed((float) (30 + rand() % 30) / 100.0f), charOffset(Offsets.at(chr)){
	static constexpr uint8_t StickHeight = 20;

	const uint8_t stickHeight = StickHeight/2 + rand() % (StickHeight/2);
	objStick = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/stick.raw"), PixelDim { 2, stickHeight })
	);
	objStick->getRenderComponent()->setLayer(8);
	objStick->setPos(0, 84 + StickHeight - stickHeight); // startX: endBack in moveWaves()
	addObject(objStick);

	const auto& res = ResInfos.at(chr);
	objChar = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile(res.path), res.size)
	);
	objChar->getRenderComponent()->setLayer(9);
	objChar->setPos(0, 84 + StickHeight - stickHeight + charOffset.y);
	addObject(objChar);

	// stick on layer 8, char on layer 9 -> waveFront is 10, curtains 11

	T = (float) (rand() % 100) / 100.0f;
	dir = (rand() % 100) < 50 ? 1 : -1;
	updatePos();
}

void OnStick::loop(float dt){

	T += Speed * dt * dir;
	if(T >= 1.0f || T <= 0.0f){
		dir *= -1;

		if(T < 0){
			T = -T;
		}else if(T > 1){
			T = 2.0f - T;
		}
	}

	updatePos();
}

void OnStick::updatePos(){
	static constexpr float posStart = 14;
	static constexpr float posEnd = 114;

	const auto pos = posStart + (posEnd - posStart) * T;
	objStick->setPosX(pos - 1.0f);

	objChar->setPosX(pos + (float) charOffset.x - 1.0f);
}
