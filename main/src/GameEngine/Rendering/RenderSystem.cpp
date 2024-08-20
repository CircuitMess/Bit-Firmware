#include <set>
#include <map>
#include "RenderSystem.h"

RenderSystem::RenderSystem(const Game* game, Sprite& canvas) : GameSystem(game), canvas(canvas){
}

void RenderSystem::update(uint32_t deltaMicros){
	std::vector<int8_t> layers;
	std::map<int16_t, std::vector<std::shared_ptr<const GameObject>>> layerMap;


	//map objects to layers
	for(const auto& object : getObjects()){
		auto rc = object->getRenderComponent();
		if(!rc) continue;
		if(!rc->isVisible()) continue;

		int8_t layer = rc->getLayer();
		layers.emplace_back(layer);
		layerMap[layer].push_back(object);

	}

	std::sort(layers.begin(), layers.end());

	//iterate objects layer by layer
	for(auto layer : layers){
//		Serial.println(layer);
		for(const auto& object : layerMap[layer]){
			object->getRenderComponent()->update(deltaMicros);
			auto pos = object->getPos();
			object->getRenderComponent()->push(canvas, { (int16_t) std::round(pos.x), (int16_t) std::round(pos.y) }, object->getRot(),
											   object->getRenderComponent()->flipX, object->getRenderComponent()->flipY);
		}
	}
}
