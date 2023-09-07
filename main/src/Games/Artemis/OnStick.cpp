#include "OnStick.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "Ray.h"

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

glm::ivec2 ArteAnimOffset = { -11, -19 };

OnStick::OnStick(Char chr, int8_t layer, std::function<void(GameObjPtr)> addObject, std::function<File(const char*)> getFile, std::function<void()> hitGood, std::function<void()> hitBad) : chr(chr), layer(layer),
addObject(addObject), charOffset(Offsets.at(chr)), MoveSpeed((float) (30 + rand() % 30) / 100.0f), hitGood(hitGood), hitBad(hitBad){
	static constexpr uint8_t StickHeight = 20;

	const uint8_t stickHeight = StickHeight/4 + rand() % (3*StickHeight/4);
	stickStartY = 84.0f + StickHeight - (float) stickHeight;

	objStick = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/stick.raw"), PixelDim { 2, stickHeight })
	);
	objStick->getRenderComponent()->setLayer(layer);
	objStick->setPos(0, stickStartY); // startX: endBack in moveWaves()
	addObject(objStick);

	const auto& res = ResInfos.at(chr);
	fileChar = getFile(res.path);
	objChar = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(fileChar, res.size)
	);
	objChar->getRenderComponent()->setLayer(layer+1);
	objChar->setPos(0, stickStartY + charOffset.y);
	addObject(objChar);

	std::string path(res.path);
	path.erase(0, 1);
	path.erase(path.size()-3, 3);
	std::string animPath("/hit_");
	animPath += path;
	animPath += "gif";
	fileAnimChar = getFile(animPath.c_str());

	if(chr == Artemis){
		objCharArte = objChar;

		auto anim = std::make_unique<AnimRC>(fileAnimChar);
		anim->setVisible(false);
		anim->setLoopMode(GIF::Infinite);
		anim->setLayer(layer+1);
		anim->stop();

		objCharArteAnim = std::make_shared<GameObject>(std::move(anim));
		addObject(objCharArteAnim);
	}

	// stick on layer 8, char on layer 9 -> waveFront is 10, curtains 11

	T = (float) (rand() % 100) / 100.0f;
	dir = (rand() % 100) < 50 ? 1 : -1;
	updatePos();
}

bool OnStick::hit(glm::ivec2 pos){
	if(state != Alive && chr != Artemis) return false;

	const auto size = ResInfos.at(chr).size;
	const auto glmSize = glm::vec2(size.x, size.y);

	const auto inside = Ray::within(pos, objChar->getPos(), objChar->getPos() + glmSize);
	if(!inside) return false;

	const auto hit = Ray::hitTest(pos - glm::ivec2(objChar->getPos()), fileChar, size);

	// Artemis is hit and wubulu anim is playing => dont allow hit passthrough
	if(hit && state != Alive && chr == Artemis) return true;

	if(hit){
		objChar->getRenderComponent()->setVisible(false);
		objStick->getRenderComponent()->setVisible(false);

		state = Drop;
		arteHitT = T;
		T = 0;

		if(chr == Artemis){
			hitBad();
			arteAnimStart();
		}else{
			hitGood();

			const auto pos = objChar->getPos();
			objChar.reset();

			auto anim = std::make_unique<AnimRC>(fileAnimChar);
			anim->setLoopMode(GIF::Single);
			anim->setLayer(layer+1);
			anim->start();

			objChar = std::make_shared<GameObject>(std::move(anim));
			objChar->setPos(pos);
			addObject(objChar);
		}
	}

	return hit;
}

void OnStick::loop(float dt){
	if(state == Dead) return;

	if(state == Alive){
		T += MoveSpeed * dt * dir;
		if(T >= 1.0f || T <= 0.0f){
			dir *= -1;

			if(T < 0){
				T = -T;
			}else if(T > 1){
				T = 2.0f - T;
			}
		}

		updatePos();
	}else if(state == Drop){
		T += DropSpeed * dt;
		if(T >= 1){
			state = Dead;
			T = 1;

			if(chr == Artemis){
				arteAnimStop();

				state = Alive;
				T = arteHitT;

				updatePos();
			}else{
				objChar->getRenderComponent()->setVisible(false);
				objStick->getRenderComponent()->setVisible(false);
			}
		}

		updateDropPos();
	}
}

void OnStick::updatePos(){
	static constexpr float posStart = 14;
	static constexpr float posEnd = 114;

	const auto pos = posStart + (posEnd - posStart) * T;

	objStick->setPosX(pos - 1.0f);
	objChar->setPosX(pos + (float) charOffset.x - 1.0f);
}

void OnStick::updateDropPos(){
	if(chr == Artemis) return;

	const auto pos = stickStartY + DropSize * T;

	objStick->setPosY(pos);
	objChar->setPosY(pos + (float) charOffset.y);
}

OnStick::Char OnStick::getChar() const{
	return chr;
}

void OnStick::hide(){
	if(chr == Artemis){
		arteAnimStart();
		state = Dead;
		return;
	}

	if(state != Alive) return;

	state = Drop;
	T = 0;
}

void OnStick::arteAnimStart(){
	if(chr != Artemis) return;

	objStick->getRenderComponent()->setVisible(true);
	objCharArte->getRenderComponent()->setVisible(false);
	objCharArteAnim->getRenderComponent()->setVisible(true);

	const auto pos = objStick->getPos() + glm::vec2(ArteAnimOffset);
	objCharArteAnim->setPos(pos);

	auto anim = std::reinterpret_pointer_cast<AnimRC>(objCharArteAnim->getRenderComponent());
	anim->start();
}

void OnStick::arteAnimStop(){
	if(chr != Artemis) return;

	objCharArte->getRenderComponent()->setVisible(true);
	objCharArteAnim->getRenderComponent()->setVisible(false);

	auto anim = std::reinterpret_pointer_cast<AnimRC>(objCharArteAnim->getRenderComponent());
	anim->stop();
}
