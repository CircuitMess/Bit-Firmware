#include "Char.h"
#include "GameEngine/Rendering/AnimRC.h"

CharlieGame::Char::Char(std::function<File(const char*)> getFile) : getFile(getFile){
	go = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/ch_idle.gif")),
			nullptr
	);
	go->setPos((glm::vec2 { 128, 128 } - SpriteSize) / 2.0f);

	auto rc = (AnimRC*) go->getRenderComponent().get();
	rc->setLayer(1);
	updateAnim();
}

CharlieGame::Char::operator GameObjPtr(){
	return go;
}

void CharlieGame::Char::btnPress(Input::Button btn){
	started = true;

	if(btn == Input::Right){
		moveDir += glm::vec2 { 1, 0 };
	}else if(btn == Input::Left){
		moveDir += glm::vec2 { -1, 0 };
	}else if(btn == Input::Up){
		moveDir += glm::vec2 { 0, -1 };
	}else if(btn == Input::Down){
		moveDir += glm::vec2 { 0, 1 };
	}

	updateAnim();
}

void CharlieGame::Char::btnRelease(Input::Button btn){
	if(!started) return;

	if(btn == Input::Right){
		moveDir -= glm::vec2 { 1, 0 };
	}else if(btn == Input::Left){
		moveDir -= glm::vec2 { -1, 0 };
	}else if(btn == Input::Up){
		moveDir -= glm::vec2 { 0, -1 };
	}else if(btn == Input::Down){
		moveDir -= glm::vec2 { 0, 1 };
	}

	updateAnim();
}

void CharlieGame::Char::update(float dt){
	if(stopped || rolling || glm::length(moveDir) == 0) return;

	glm::vec2 dir = glm::normalize(moveDir);

	auto pos = go->getPos() + dir * MoveSpeed * dt;
	pos = glm::clamp(pos, glm::vec2 { 0, 0 }, glm::vec2 { 128, 128 } - SpriteSize);
	go->setPos(pos);

	const auto rot = std::atan2(dir.y, dir.x) * 180.0f / M_PI + 90.0f;
	go->setRot(rot);
}

void CharlieGame::Char::updateAnim(){
	auto rc = (AnimRC*) go->getRenderComponent().get();

	if(rolling){
		rc->setAnim(getFile("/ch_roll.gif"));
	}else if(glm::length(moveDir) == 0 || stopped){
		rc->setAnim(getFile("/ch_idle.gif"));
	}else{
		rc->setAnim(getFile("/ch_walk.gif"));
	}

	rc->start();
}

void CharlieGame::Char::setRoll(bool rolling){
	this->rolling = rolling;
	updateAnim();
}

void CharlieGame::Char::setStop(bool stop){
	this->stopped = stop;
	updateAnim();
}
