#include "Debree.h"
#include "GameEngine/Rendering/StaticRC.h"
#include <gtx/rotate_vector.hpp>
#include <esp_random.h>

Debree::Debree(std::function<void(GameObjPtr)> addObject, std::function<void(GameObjPtr)> removeObject, std::function<File(const char*)> getFile) : addObj(addObject), remObj(removeObject), getFile(getFile){

}

void Debree::loop(float dt){
	for(auto& deb : objs){
		deb.velocity += glm::vec2(0, 140) * dt;

		auto pos = deb.obj->getPos();
		pos += deb.velocity * dt;

		deb.obj->setPos(pos);
	}
}

void Debree::place(glm::ivec2 pos){
	auto obj = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/debree.raw"), PixelDim { 2, 2 })
	);
	obj->getRenderComponent()->setLayer(45);
	obj->setPos(glm::vec2(pos));
	addObj(obj);

	const float angle = ((float) esp_random() / (float) RAND_MAX) * M_PI_2 - M_PI_4;
	const auto dir = glm::rotate(glm::vec2(0, -1.0), angle);

	if(objs.size() >= Limit){
		auto first = objs.front();
		remObj(first.obj);
		objs.pop_front();
	}

	objs.push_back({ obj, dir * 60.0f });
}
