#include "Blocks.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "GameEngine/Collision/RectCC.h"

Blocks::Blocks(Sprite& canvas) : Game(canvas, "/Games/Blocks", {
}){

}

void Blocks::onLoad(){
	bg = std::make_shared<GameObject>(
			std::make_unique<SpriteRC>(PixelDim{ 128, 128 }),
			nullptr
	);
	addObject(bg);
	bg->getRenderComponent()->setLayer(-2);
	auto bgSprite = std::static_pointer_cast<SpriteRC>(bg->getRenderComponent())->getSprite();
	bgSprite->clear(TFT_BLACK);
	bgSprite->drawFastVLine(GridPos.x - 1, 0, 128, TFT_WHITE);
	bgSprite->drawFastVLine(GridPos.x + GridDim.x * TileDim.x + 1, 0, 128, TFT_WHITE);
	state = State::Running;

	newBlock();
	memset((void*) blocksMatrix, 0, GridDim.x * GridDim.y * sizeof(bool));
}

void Blocks::onLoop(float deltaTime){
	if(state == State::Running){
		handleInputRepeat(deltaTime);
		auto speed = BaseSpeed * LevelSpeedFactors[level];
		if(fastDrop){
			speed = std::max(speed, SoftDropSpeed);
		}
		moveBuffer += deltaTime * speed;
		if(moveBuffer >= TileDim.y){
			moveBuffer -= TileDim.y;
			moveBlock();
		}
	}else if(state == State::GameOver){
		popCounter += deltaTime;
		if(popCounter >= PopPause){
			//pop
			return;
		}
	}else if(state == State::GameWin){
		popCounter += deltaTime;
		if(popCounter >= PopPause){
			//pop
			return;
		}
	}
}

void Blocks::handleInput(const Input::Data& data){
	if(data.action == Input::Data::Release){
		if(data.btn == lastButton){
			lastButtonState = Released;
		}
		if(data.btn == Input::Down){
			fastDrop = false;
		}
	}

	if(data.action != Input::Data::Press) return;

	if(state == State::GameOver || state == State::GameWin){
		//pop
		return;
	}

	if(data.btn == Input::B || data.btn == Input::Menu) return;

	if(data.btn == Input::A){
		blocks.back().rotate((const bool*) blocksMatrix);
	}else if(data.btn == Input::Up){
		while(!moveBlock());
	}else if(data.btn == Input::Down){
		fastDrop = true;
	}

	if(data.btn != Input::Left && data.btn != Input::Right) return;

	lastButton = data.btn;
	lastButtonState = Pressed;
	btnHoldTimer = 0;

	if(data.btn == Input::Left){
		if(checkBoundLeft(blocks.back())) return;
		blocks.back().setPos(blocks.back().getPos() + PixelDim{ -1, 0 });
	}else if(data.btn == Input::Right){
		if(checkBoundRight(blocks.back())) return;
		blocks.back().setPos(blocks.back().getPos() + PixelDim{ 1, 0 });
	}
}

void Blocks::gameOver(){
	state = State::GameOver;
	audio.play({ { 400, 300, 200 },
				 { 0,   0,   50 },
				 { 300, 200, 200 },
				 { 0,   0,   50 },
				 { 200, 50,  400 } });
}

void Blocks::newBlock(){
	blocks.emplace_back(Block::Type((rand() % BlockTypesNum) + 1), colors[rand() % BlockColorsNum], PixelDim{ 5, 0 });

	for(auto& segment : blocks.back().segments){
		addObject(segment);
	}
}

bool Blocks::moveBlock(){
	bool collide = false;

	for(const auto& seg : blocks.back().segments){
		auto gridpos = globalToGridPos(seg->getPos());
		if(gridpos.y >= (GridDim.y - 1)){
			collide = true;
			break;
		}
		if(blocksMatrix[gridpos.x][gridpos.y + 1]){
			collide = true;
			break;
		}
	}

	if(collide){
		bool kill = false;
		blocks.back().placed();
		for(auto& segment : blocks.back().segments){
			auto gridpos = globalToGridPos(segment->getPos());
			blocksMatrix[gridpos.x][gridpos.y] = true;
			if(gridpos.y <= (GridDim.y - KillHeight)){
				kill = true;
			}
		}

		if(kill){
			gameOver();
			return true;
		}
		//check for lineclear
		newBlock();
		return true;
	}

	auto pos = blocks.back().getPos();
	blocks.back().setPos(pos + PixelDim{ 0, 1 });

	return false;
}

void Blocks::handleInputRepeat(float deltaT){
	if(lastButtonState == Released) return;

	btnHoldTimer += deltaT;
	if(lastButtonState == Pressed && btnHoldTimer >= LongPressTime){
		lastButtonState = Held;
		btnHoldTimer = 0;
		sendLastInput();
	}else if(lastButtonState == Held && btnHoldTimer >= LongPressRepeatTime){
		btnHoldTimer = 0;
		sendLastInput();
	}
}

void Blocks::sendLastInput(){
	if(lastButton == Input::Left){
		if(checkBoundLeft(blocks.back())) return;
		blocks.back().setPos(blocks.back().getPos() + PixelDim{ -1, 0 });
	}else if(lastButton == Input::Right){
		if(checkBoundRight(blocks.back())) return;
		blocks.back().setPos(blocks.back().getPos() + PixelDim{ 1, 0 });
	}
}

bool Blocks::checkBoundLeft(const Block& block){
	for(const auto& seg : blocks.back().segments){
		auto gridpos = globalToGridPos(seg->getPos());
		if(gridpos.x == 0) return true;
	}

	return false;
}

bool Blocks::checkBoundRight(const Block& block){
	for(const auto& seg : blocks.back().segments){
		auto gridpos = globalToGridPos(seg->getPos());
		if(gridpos.x == (GridDim.x - 1)) return true;
	}

	return false;
}
