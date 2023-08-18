#include "Blocks.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "GameEngine/Collision/RectCC.h"

Blocks::Blocks(Sprite& canvas) : Game(canvas, "/Games/Blocks", {
}), nextBlock(Block::Type::ReverseS, TFT_BLACK, NextBlockPos){

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

	scoreLabel = std::make_shared<GameObject>(
			std::make_unique<TextRC>("SCORE", TextStyle{ &Font0, TFT_WHITE, 1, TC_DATUM }),
			nullptr
	);
	scoreLabel->setPos(GridPos.x / 2, 5);
	addObject(scoreLabel);
	scoreNumLabel = std::make_shared<GameObject>(
			std::make_unique<TextRC>("00000", TextStyle{ &Font0, TFT_WHITE, 1, TC_DATUM }),
			nullptr
	);
	scoreNumLabel->setPos(GridPos.x / 2, 15);
	scoreTextRC = std::static_pointer_cast<TextRC>(scoreNumLabel->getRenderComponent());
	addObject(scoreNumLabel);

	nextLabel = std::make_shared<GameObject>(
			std::make_unique<TextRC>("NEXT", TextStyle{ &Font0, TFT_WHITE, 1, TC_DATUM }),
			nullptr
	);
	nextLabel->setPos(GridPos.x / 2, 65);
	addObject(nextLabel);

	levelLabel = std::make_shared<GameObject>(
			std::make_unique<TextRC>("LV. 1", TextStyle{ &Font0, TFT_WHITE, 1, TC_DATUM }),
			nullptr
	);
	levelLabel->setPos(128 - GridPos.x / 2 + 1, 5);
	levelTextRC = std::static_pointer_cast<TextRC>(levelLabel->getRenderComponent());
	addObject(levelLabel);


	linesLabel = std::make_shared<GameObject>(
			std::make_unique<TextRC>("LINES", TextStyle{ &Font0, TFT_WHITE, 1, TC_DATUM }),
			nullptr
	);
	linesLabel->setPos(128 - GridPos.x / 2 + 1, 20);
	addObject(linesLabel);
	linesNumLabel = std::make_shared<GameObject>(
			std::make_unique<TextRC>("00000", TextStyle{ &Font0, TFT_WHITE, 1, TC_DATUM }),
			nullptr
	);
	linesNumLabel->setPos(128 - GridPos.x / 2 + 1, 30);
	linesTextRC = std::static_pointer_cast<TextRC>(linesNumLabel->getRenderComponent());
	addObject(linesNumLabel);

	nextBlock = Block(Block::Type((rand() % BlockTypesNum) + 1), colors[rand() % BlockColorsNum], NextBlockPos);
	for(auto& segment : nextBlock.segments){
		addObject(segment);
	}

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
			exit();

			return;
		}
	}else if(state == State::GameWin){
		popCounter += deltaTime;
		if(popCounter >= PopPause){
			exit();
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
		exit();
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
	nextBlock.setPos({ 5, 0 });
	blocks.push_back(std::move(nextBlock));


	nextBlock = Block(Block::Type((rand() % BlockTypesNum) + 1), colors[rand() % BlockColorsNum], NextBlockPos);

	for(auto& segment : nextBlock.segments){
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
		checkLineClear();
		updateScore();

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
		if(blocksMatrix[gridpos.x - 1][gridpos.y]) return true;
	}

	return false;
}

bool Blocks::checkBoundRight(const Block& block){
	for(const auto& seg : blocks.back().segments){
		auto gridpos = globalToGridPos(seg->getPos());
		if(gridpos.x == (GridDim.x - 1)) return true;
		if(blocksMatrix[gridpos.x + 1][gridpos.y]) return true;
	}

	return false;
}

void Blocks::checkLineClear(){
	uint8_t multipleLinesCleared = 0;
	for(int y = 0; y < GridDim.y; ++y){
		bool clear = true;
		for(int x = 0; x < GridDim.x; ++x){
			if(!blocksMatrix[x][y]){
				clear = false;
				break;
			}
		}

		if(clear){
			for(int x = 0; x < GridDim.x; ++x){
				blocksMatrix[x][y] = false;
			}
			//clear all segment objects
			for(int i = 0; i < blocks.size(); i++){
				for(auto& segment : blocks[i].segments){
					if(segment == nullptr) continue;
					if(globalToGridPos(segment->getPos()).y == y){
						clearSegment(blocks[i], segment);
					}
				}
			}

			//shift all other segment objects
			for(int x = 0; x < GridDim.x; ++x){
				for(int y2 = y; y2 > 0; --y2){
					blocksMatrix[x][y2] = blocksMatrix[x][y2 - 1];
					for(int i = 0; i < blocks.size(); i++){
						for(auto& segment : blocks[i].segments){
							if(segment == nullptr) continue;
							if(globalToGridPos(segment->getPos()) == PixelDim{ x, y2 - 1 }){
								segment->setPos(gridToGlobalPos({ x, y2 }));
							}
						}
					}
				}
			}

			//clean blocks with no segments
			for(auto& block : blocks){
				if(block.segments[0] == nullptr && block.segments[1] == nullptr && block.segments[2] == nullptr && block.segments[3] == nullptr){
					for(auto it = blocks.begin(); it != blocks.end(); ++it){
						if(&(*it) == &block){
							blocks.erase(it);
							break;
						}
					}
				}
			}
			++multipleLinesCleared;
		}
	}
	score += (multipleLinesCleared > 1) ? (multipleLinesCleared * (level + 1)) : multipleLinesCleared;
	linesCleared += multipleLinesCleared;
	if(linesCleared >= (level * 10)){
		if((level + 1) <= MaxLevel){
			level++;
		}
	}
}

void Blocks::clearSegment(Block& block, GameObjPtr segment){
	removeObject(segment);
	block.clearSegment(segment);
}

void Blocks::updateScore(){
	std::string s = std::to_string(score);
	s.insert(s.begin(), 5 - s.length(), '0');
	scoreTextRC->setText(s);

	s = std::to_string(linesCleared);
	s.insert(s.begin(), 5 - s.length(), '0');
	linesTextRC->setText(s);

	s = "LV. " + std::to_string(level);
	levelTextRC->setText(s);
}
