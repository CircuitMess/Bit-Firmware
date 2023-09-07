#include "Crosshair.h"
#include "GameEngine/Rendering/StaticRC.h"

const std::unordered_map<Input::Button, Crosshair::AimDir> Crosshair::BtnAim = {
		{ Input::Button::Up, AimDir::Up },
		{ Input::Button::Down, AimDir::Down },
		{ Input::Button::Left, AimDir::Left },
		{ Input::Button::Right, AimDir::Right },
};

Crosshair::Crosshair(std::function<void(GameObjPtr)> addObject, std::function<File(const char*)> getFile){
	sight = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/aim.raw"), PixelDim { 19, 19 })
	);
	sight->getRenderComponent()->setLayer(60);
	sight->setPos(54, 54);

	sightDir = glm::normalize(glm::vec2 { rand() % 100 - 50, rand() % 100 - 50 });

	addObject(sight);
}

glm::ivec2 Crosshair::getAim(){
	return glm::round(sight->getPos() + 9.5f);
}

void Crosshair::btnAction(Input::Button btn, Input::Data::Action action){
	if(!BtnAim.contains(btn)) return;
	const auto aim = BtnAim.at(btn);

	if(action == Input::Data::Press){
		pressedAim.insert(aim);
	}else{
		pressedAim.erase(aim);
	}
}

void Crosshair::btnReset(){
	pressedAim.clear();
}

void Crosshair::loop(float dt){
	static constexpr glm::vec2 ScreenSize(128.0f, 128.0f);
	static constexpr glm::vec2 ScreenCenter = ScreenSize / 2.0f;

	static constexpr float Randomness = 0.7f;
	static constexpr float CenterFactor = 0.05f;
	static constexpr float RandomSpeed = 30;
	static constexpr float AimFactor = 40;
	static constexpr float SineFactor = 40;
	static constexpr float SineAmp = 4;

	auto pos = sight->getPos() + 10.0f;

	const auto randVel = glm::normalize(glm::vec2(rand() % 1000 - 500, rand() % 1000 - 500)) * Randomness;

	const auto centerDir = ScreenCenter - pos;
	const auto centerDist = glm::length(centerDir);
	const auto centerDistNorm = centerDist / (glm::length(ScreenSize) / 2.0f);

	sightDir += randVel + centerDir * centerDistNorm * CenterFactor;

	if (glm::length(sightDir) > 0.0f) {
		sightDir = glm::normalize(sightDir) * RandomSpeed;
	}

	const auto aimDir = getAimDir();
	if(glm::length(aimDir) != 0){
		sightDir *= 0.3f;
	}

	pos += sightDir * dt;

	pos += getAimDir() * AimFactor * dt;

	glm::vec2 sine = { sin(sineTime * 4) * SineAmp, sin(sineTime * 5) * SineAmp };
	pos -= sine;
	sineTime += dt;
	sine = { sin(sineTime * 4) * SineAmp, sin(sineTime * 5) * SineAmp };
	pos += sine;

	pos = glm::clamp(pos, {1, 1}, ScreenSize);
	sight->setPos(pos - 10.0f);
}

glm::vec2 Crosshair::getAimDir(){
	glm::vec2 aim = {};

	if(pressedAim.contains(AimDir::Up)){
		aim += glm::vec2(0, -1);
	}
	if(pressedAim.contains(AimDir::Down)){
		aim += glm::vec2(0, 1);
	}
	if(pressedAim.contains(AimDir::Left)){
		aim += glm::vec2(-1, 0);
	}
	if(pressedAim.contains(AimDir::Right)){
		aim += glm::vec2(1, 0);
	}

	return aim;
}

void Crosshair::hide(){
	sight->getRenderComponent()->setVisible(false);
}
