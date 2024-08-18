#include "Fly.h"
#include "GameEngine/Rendering/AnimRC.h"
#include <esp_random.h>
#include <gtx/vector_angle.hpp>

CharlieGame::Fly::Fly(std::function<File(const char*)> getFile, struct Cacoon* rescue, std::function<void(struct Cacoon*)> onRescued) : getFile(getFile), rescue(rescue), onRescued(onRescued){
	go = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/fly_fly.gif")),
			nullptr
	);

	const glm::vec2 center = (glm::vec2 { 128, 128 } - SpriteSize) / 2.0f;
	const float randDir = M_PI * 2 * ((float) esp_random() / (float) UINT32_MAX);
	const glm::vec2 dir = glm::rotate(glm::vec2 { 0, 1 }, randDir);
	startPos = center + dir * 120.0f;
	go->setPos(startPos);

	if(rescue){
		destPos = rescue->fly->go->getPos() + glm::vec2 { -1, 19 };
	}else{
		destPos = glm::vec2 {
				((float) esp_random() / (float) UINT32_MAX),
				((float) esp_random() / (float) UINT32_MAX)
		} * (glm::vec2 { 128, 128 } - SpriteSize);
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

		auto pos = startPos + (destPos - startPos) * t * speed;
		go->setPos(pos);
		// TODO: update rotation

		if(t >= 1.0f / speed){
			if(state == FlyingIn){
				if(rescue){
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
			goAway();
		}
	}else if(state == Rescuing){
		if(t >= RescueTimeout){
			onRescued(rescue);
			rescue->fly->goAway();
			rescue = nullptr;
			goAway();
		}
	}
}

void CharlieGame::Fly::updateAnim(){
	auto rc = (AnimRC*) go->getRenderComponent().get();

	if(state == FlyingIn || state == FlyingOut){
		rc->setAnim(getFile("/fly_fly.gif"));
		rc->setLayer(3);
	}else if(state == Plotting){
		rc->setAnim(getFile("/fly_plot.gif"));
		rc->setLayer(0);
	}else if(state == Rescuing){
		rc->setAnim(getFile("/fly_unroll.gif"));
		rc->setLayer(0);
	}else if(state == Cacoon || state == Done){
		rc->stop();
		rc->setVisible(false);
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
		startPos = go->getPos();

		rescue->beingRescued = false;
		rescue->rescuer = nullptr;
		rescue = nullptr;
	}else{
		startPos = destPos;
	}

	const glm::vec2 center = (glm::vec2 { 128, 128 } - SpriteSize) / 2.0f;
	const float randDir = M_PI * 2 * ((float) esp_random() / (float) UINT32_MAX);
	const glm::vec2 dir = glm::rotate(glm::vec2 { 0, 1 }, randDir);
	destPos = center + dir * 120.0f;

	setState(FlyingOut);
}

void CharlieGame::Fly::done(){
	setState(Done);
}

void CharlieGame::Fly::setState(CharlieGame::Fly::State newState){
	t = 0;
	state = newState;
	updateAnim();
}
