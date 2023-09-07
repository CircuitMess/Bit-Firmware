#include "Windows.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "Ray.h"

struct CharInfo {
	const char* path;
	PixelDim size;
};

static const CharInfo ResInfos[] = {
		{ "/win1.raw", { 17, 20 } },
		{ "/win2.raw", { 17, 20 } },
		{ "/win3.raw", { 15, 19 } }
};

static constexpr float CharOverHidden = 3;

static constexpr glm::ivec2 Offsets[] = {
		{ 0, 0 },
		{ 0, 0 },
		{ 1, 1 }
};

static constexpr glm::ivec2 WindowPos[] = {
		{ 22, 34 },
		{ 55, 34 },
		{ 88, 34 },
};

Windows::Windows(std::function<void(GameObjPtr)> addObject, std::function<File(const char* path)> getFile, std::function<void()> hitGood, std::function<void()> hitBad) : hitGood(hitGood), hitBad(hitBad){
	for(int i = 0; i < 3; i++){
		const auto& res = ResInfos[i];

		files[i] = getFile(res.path);
		auto chr = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(files[i], res.size)
		);
		chr->getRenderComponent()->setLayer(-2);
		addObject(chr);
		chars.push_back(chr);
	}

	relocChars();
	repos();
}

bool Windows::hit(glm::ivec2 pos){
	for(int i = 0; i < 3; i++){
		if(!alive[i]) continue;

		const auto size = ResInfos[i].size;
		const auto glmSize = glm::vec2(size.x, size.y);

		const auto inside = Ray::within(pos, chars[i]->getPos(), chars[i]->getPos() + glmSize);
		if(!inside) continue;

		const auto hit = Ray::hitTest(pos - glm::ivec2(chars[i]->getPos()), files[i], size);

		if(hit){
			if(i == 2){
				hitBad(); // Artemis
			}else{
				hitGood();
			}

			alive[i] = false;
			return true;
		}
	}

	return false;
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
			if(dieT[2] < 0 && !done){
				dieT[2] = 0;
				alive[2] = true;
				chars[2]->getRenderComponent()->setVisible(true);
			}
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

	for(int i = 0; i < 3; i++){
		if(alive[i] || dieT[i] < 0) continue;

		dieT[i] += SpeedMove * dt;

		if(dieT[i] >= 1){
			chars[i]->getRenderComponent()->setVisible(false);
			dieT[i] = -1; // -1 means death anim done
		}
	}

	repos();
}

void Windows::relocChars(){
	if(allDead()) return;

	std::vector<uint8_t> indexes = { 0, 1, 2 }; // indices of free windows

	for(int i = 0; i < 3; i++){
		const uint8_t indexIndex = rand() % indexes.size(); // index in the above vector
		const uint8_t window = indexes[indexIndex]; // window index for the current character
		indexes.erase(indexes.cbegin() + indexIndex); // remove the taken index from the free window indices vector
		// i: character index, window: window index
		// i.e.: character (i) is getting relocated to the (window) window

		const int posX = WindowPos[window].x + Offsets[i].x;
		const int posY = WindowPos[window].y + Offsets[i].y + ResInfos[i].size.y - CharOverHidden;

		charLoc[i] = window;

		if(!alive[i]) continue;
		chars[i]->setPos(posX, posY);
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
		if(!alive[i] && dieT[i] < 0) continue;

		const uint8_t window = charLoc[i];
		const float hideMove = (ResInfos[i].size.y-CharOverHidden) * 1.5f;

		const float startPos = WindowPos[window].y + Offsets[i].y;

		if(!alive[i]){
			float t = easeInBack(dieT[i]);
			chars[i]->setPosY(startPos + t * hideMove);
			continue;
		}

		if(state == Up){
			chars[i]->setPosY(startPos);
			continue;
		}

		float t = std::clamp(T - timeOffsets[i], 0.0f, 1.0f);
		t = state == Dropping ? easeInQuad(t) : 1.0f - easeOutExp(t);
		chars[i]->setPosY(startPos + t * hideMove);
	}
}

void Windows::hide(){
	done = true;
	for(int i = 0; i < 3; i++){
		if(!alive[i]) continue;
		alive[i] = false;
	}
}

bool Windows::allDead(){
	for(int i = 0; i < 3; i++){
		if(alive[i]) return false;
	}
	return true;
}
