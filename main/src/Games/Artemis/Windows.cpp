#include "Windows.h"
#include "GameEngine/Rendering/StaticRC.h"
#include <list>

struct CharInfo {
	const char* path;
	PixelDim size;
};

static const CharInfo ResInfos[] = {
		{ "/win1.raw", { 17, 17 } },
		{ "/win2.raw", { 17, 17 } },
		{ "/win3.raw", { 15, 16 } }
};

static const glm::ivec2 Offsets[] = {
		{ 0, 0 },
		{ 0, 0 },
		{ 1, 1 }
};

static const glm::ivec2 WindowPos[] = {
		{ 22, 34 },
		{ 55, 34 },
		{ 88, 34 },
};

Windows::Windows(std::function<void(GameObjPtr)> addObject, std::function<File(const char* path)> getFile){
	for(int i = 0; i < 3; i++){
		const auto& res = ResInfos[i];

		auto chr = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(res.path), res.size)
		);
		chr->getRenderComponent()->setLayer(-2);
		addObject(chr);
		chars.push_back(chr);
	}

	relocChars();
	repos();
}

void Windows::loop(float dt){
	if(state == Up){
		T += SpeedWait * dt;
	}else{
		T += SpeedMove * dt;
	}

	if((state == Up && T >= 1.0) || (state != Up && T >= 1.6)){
		T = 0;

		if(state == Dropping){
			relocChars();
			randOffsets();
			state = Rising;
		}else if(state == Rising){
			state = Up;
		}else if(state == Up){
			state = Dropping;
			randOffsets();
		}
	}

	repos();
}

void Windows::relocChars(){
	std::vector<uint8_t> indexes = { 0, 1, 2 }; // indices of free windows

	for(int i = 0; i < 3; i++){
		const uint8_t indexIndex = rand() % indexes.size(); // index in the above vector
		const uint8_t window = indexes[indexIndex]; // window index for the current character
		indexes.erase(indexes.cbegin() + indexIndex); // remove the taken index from the free window indices vector
		// i: character index, window: window index
		// i.e.: character (i) is getting relocated to the (window) window

		const int posX = WindowPos[window].x + Offsets[i].x;
		const int posY = WindowPos[window].y + Offsets[i].y + ResInfos[i].size.y;

		chars[i]->setPos(posX, posY);
		charLoc[i] = window;
	}
}

void Windows::randOffsets(){
	std::vector<float> offsets = { 0.2, 0.4, 0.6 };

	for(int i = 0; i < 3; i++){
		const uint8_t index = rand() % offsets.size(); // index in the above vector
		timeOffsets[i] = offsets[index]; // window index for the current character
		offsets.erase(offsets.cbegin() + index); // remove the taken index from the free window indices vector
	}
}

void Windows::repos(){
	for(int i = 0; i < 3; i++){
		reposChar(i);
	}
}

void Windows::reposChar(uint8_t i){
	const uint8_t window = charLoc[i];
	const float hideMove = ResInfos[i].size.y * 1.5f;

	const float startPos = WindowPos[window].y + Offsets[i].y;
	if(state == Up){
		chars[i]->setPosY(startPos);
		return;
	}

	float t = std::clamp(T - timeOffsets[i], 0.0f, 1.0f);
	t = state == Dropping ? easeInQuad(t) : 1.0f - easeOutExp(t);
	chars[i]->setPosY(startPos + t * hideMove);
}
