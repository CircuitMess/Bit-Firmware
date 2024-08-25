#include "Fly.h"
#include "GameEngine/Rendering/AnimRC.h"
#include <esp_random.h>
#include <gtx/vector_angle.hpp>

CharlieGame::Fly::Fly(std::function<File(const char*)> getFile, struct Cacoon* rescue, std::function<void(struct Cacoon*)> onRescued) : getFile(getFile), rescue(rescue), onRescued(onRescued){
	go = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/fly_fly.gif")),
			nullptr
	);

	startPos = randPoint(120);
	go->setPos(startPos);

	if(rescue){
		destPos = rescue->go->getPos() + glm::vec2 { -4.5f, 1 };
	}else{
		destPos = glm::vec2 { 0, 10 } + glm::vec2 {
				((float) esp_random() / (float) UINT32_MAX),
				((float) esp_random() / (float) UINT32_MAX)
		} * (glm::vec2 { 128, 128 } - SpriteSize - glm::vec2 { 0, 10 });
	}

	auto rc = (AnimRC*) go->getRenderComponent().get();
	rc->setLayer(2);
	updateAnim();
}

CharlieGame::Fly::operator GameObjPtr(){
	return go;
}

bool CharlieGame::Fly::isPlotting(){
	return state == Plotting;
}

bool CharlieGame::Fly::isCacooned(){
	return state == Cacoon;
}

bool CharlieGame::Fly::isRescuing(){
	return rescue && state == Rescuing;
}

bool CharlieGame::Fly::isDone(){
	return state == Done;
}

void CharlieGame::Fly::update(float dt){
	t += dt;

	if(state == FlyingIn || state == FlyingOut){
		const float speed = (state == FlyingIn ? 0.2f : 0.5f) * (rescue ? 2.0f : 1.0f);

		const glm::vec2 moveDir = destPos - startPos;
		const float moveAngle = std::atan2(moveDir.y, moveDir.x);

		glm::vec2 altMove = { 0, 0 };
		if(state != FlyingIn || rescue == nullptr){
			altMove = glm::rotate(glm::vec2 { std::sin(t * 3.0f) * 40.0f, 0 }, moveAngle + (float) M_PI_2);
			altMove *= glm::length(destPos - go->getPos()) / glm::length(moveDir);
		}

		const auto pos = startPos + moveDir * t * speed + altMove * t * speed * 0.5f;

		const auto relDir = glm::normalize(pos - go->getPos());
		const float rot = std::atan2(relDir.y, relDir.x) * 180.0f / M_PI; // use moveDir instead of relDir

		go->setPos(pos);
		go->setRot(rot + 90.0f);

		if(t >= 1.0f / speed){
			if(state == FlyingIn){
				if(rescue){
					go->setPos(destPos);
					setState(Rescuing);
				}else{
					setState(Plotting);
				}
			}else{
				done();
			}
		}
	}else if(state == Plotting){
		if(t >= PlotTimeout){
			if(onRescued){
				onRescued(rescue);
			}
			goAway();
		}
	}else if(state == Rescuing){
		if(t >= RescueTimeout){
			if(onRescued && rescue){
				rescue->fly->goAway();
				onRescued(rescue);
			}

			goAway();
		}
	}
}

void CharlieGame::Fly::updateAnim(){
	auto rc = (AnimRC*) go->getRenderComponent().get();

	if(state == FlyingIn || state == FlyingOut){
		rc->setAnim(getFile("/fly_fly.gif"), true);
		rc->setLayer(3);
	}else if(state == Plotting){
		rc->setAnim(getFile("/fly_plot.gif"), true);
		rc->setLayer(0);
	}else if(state == Rescuing){
		rc->setAnim(getFile("/fly_unroll.gif"), true);
		rc->setLayer(0);
		go->setRot(0);
	}else if(state == Cacoon || state == Done){
		rc->setVisible(false);
		rc->stop();
		go->setRot(0);
		return;
	}

	rc->start();
	rc->setVisible(true);
}

void CharlieGame::Fly::goCac(){
	setState(Cacoon);
}

void CharlieGame::Fly::goAway(){
	if(rescue){
		rescue->beingRescued = false;
		rescue->rescuer = nullptr;
		rescue = nullptr;
	}

	if(state == FlyingOut) return;

	startPos = go->getPos();
	destPos = randPoint(120);

	setState(FlyingOut);
}

void CharlieGame::Fly::done(){
	setState(Done);
}

void CharlieGame::Fly::setState(CharlieGame::Fly::State newState){
	if(newState == state) return;

	t = 0;
	state = newState;
	updateAnim();
}

glm::vec2 CharlieGame::Fly::randPoint(float centerDistance){
	const glm::vec2 center = (glm::vec2 { 128, 128 } - SpriteSize) / 2.0f;
	const float randDir = (float) M_PI * 2.0f * ((float) esp_random() / (float) UINT32_MAX);
	const glm::vec2 dir = glm::rotate(glm::vec2 { 0, 1 }, randDir);
	return center + dir * centerDistance;
}
