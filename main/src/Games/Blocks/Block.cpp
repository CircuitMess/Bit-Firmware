#include "Block.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "Blocks.h"

Block::Block(Block::Type type, Color c, const PixelDim& gridPos) : type(type), color(c), gridPos(gridPos){
	for(auto& segment : segments){
		segment = std::make_shared<GameObject>(
				std::make_unique<SpriteRC>(Blocks::TileDim),
				nullptr
		);
		auto sprite = std::static_pointer_cast<SpriteRC>(segment->getRenderComponent())->getSprite();
		sprite->clear(TFT_TRANSPARENT);
		sprite->drawRect(0, 0, Blocks::TileDim.x, Blocks::TileDim.y, color);
	}

	switch(type){
		case Type::Line:
			segments[0]->setPos(gridPos.x, gridPos.y + 2);
			segments[1]->setPos(gridPos.x, gridPos.y + 3);
			segments[2]->setPos(gridPos.x, gridPos.y + 1);
			segments[3]->setPos(gridPos.x, gridPos.y);
			break;
		case Type::Cube:
			segments[0]->setPos((gridPos.x) - 1, gridPos.y);
			segments[1]->setPos((gridPos.x), gridPos.y);
			segments[2]->setPos((gridPos.x) - 1, gridPos.y + 1);
			segments[3]->setPos((gridPos.x), gridPos.y + 1);
			break;
		case Type::T:
			segments[0]->setPos(gridPos.x, gridPos.y);
			segments[1]->setPos((gridPos.x) - 1, gridPos.y);
			segments[2]->setPos(gridPos.x, gridPos.y + 1);
			segments[3]->setPos(gridPos.x, gridPos.y - 1);
			break;
		case Type::L:
			segments[0]->setPos(gridPos.x, gridPos.y + 1);
			segments[1]->setPos(gridPos.x, gridPos.y);
			segments[2]->setPos(gridPos.x, gridPos.y + 2);
			segments[3]->setPos((gridPos.x) + 1, gridPos.y + 2);
			break;
		case Type::ReverseL:
			segments[0]->setPos((gridPos.x), gridPos.y + 1);
			segments[1]->setPos((gridPos.x), gridPos.y);
			segments[2]->setPos((gridPos.x), gridPos.y + 2);
			segments[3]->setPos((gridPos.x) - 1, gridPos.y + 2);
			break;
		case Type::S:
			segments[0]->setPos(gridPos.x, gridPos.y);
			segments[1]->setPos(gridPos.x, gridPos.y + 1);
			segments[2]->setPos((gridPos.x) - 1, gridPos.y);
			segments[3]->setPos((gridPos.x) - 1, gridPos.y - 1);
			break;
		case Type::ReverseS:
			segments[0]->setPos((gridPos.x) - 1, gridPos.y);
			segments[1]->setPos((gridPos.x) - 1, gridPos.y + 1);
			segments[2]->setPos((gridPos.x), gridPos.y);
			segments[3]->setPos((gridPos.x), gridPos.y - 1);
			break;
	}

	for(auto& segment : segments){
		auto pos = segment->getPos();
		pos *= Blocks::TileDim.x;
		pos += Blocks::GridPos;
		segment->setPos(pos);
	}
}

void Block::setPos(PixelDim gridPos){
	auto diff = gridPos - this->gridPos;
	this->gridPos = gridPos;

	for(auto& segment : segments){
		if(segment == nullptr) continue;
		auto pos = segment->getPos();
		pos += diff * Blocks::TileDim.x;
		segment->setPos(pos);
	}
}

PixelDim Block::getPos() const{
	return gridPos;
}

void Block::rotate(const bool* blocksMatrix){
	rotate();

	//check if clipping out of bounds, then kickoff
	//if clipping into blocks after kickoff, derotate, de-kickoff
	PixelDim kickoff{ 0, 0 };
	for(auto& segment : segments){
		const auto gridpos = Blocks::globalToGridPos(segment->getPos());
		kickoff.x = std::max((int) kickoff.x, -gridpos.x); //left kickoff
		kickoff.x = std::min((int) kickoff.x, (Blocks::GridDim.x - 1) - gridpos.x); //right kickoff

	}
	setPos(gridPos + kickoff);
	for(auto& segment : segments){
		const auto gridpos = Blocks::globalToGridPos(segment->getPos());
		if(blocksMatrix[gridpos.x * Blocks::GridDim.y + gridpos.y]){
			for(int i = 0; i < 3; ++i){
				rotate();
			}
			setPos(gridPos - kickoff);
			break;
		}
	}
}

void Block::placed(){
	for(auto& segment : segments){
		auto sprite = std::static_pointer_cast<SpriteRC>(segment->getRenderComponent())->getSprite();
		sprite->clear(TFT_TRANSPARENT);
		sprite->fillRect(0, 0, Blocks::TileDim.x, Blocks::TileDim.y, TFT_BLUE);
	}
}

void Block::rotate(){
	rotation = (rotation + 1) % 4;
	const auto current_rotation = rotation + 1;
	switch(type){
		case Type::Line:
			if(current_rotation == 1 || current_rotation == 3){
				segments[1]->setPos(segments[0]->getPos().x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);
				segments[2]->setPos(segments[0]->getPos().x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);
				segments[3]->setPos(segments[0]->getPos().x, segments[0]->getPos().y - 2 * Blocks::TileDim.y);

			}else{
				segments[1]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[2]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[3]->setPos(segments[0]->getPos().x + 2 * Blocks::TileDim.x, segments[0]->getPos().y);
			}
			break;

		case Type::T:
			if(current_rotation == 1){
				segments[1]->setPos(segments[0]->getPos().x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);
				segments[2]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[3]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y);

			}else if(current_rotation == 2){
				segments[1]->setPos(segments[0]->getPos().x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);
				segments[2]->setPos(segments[0]->getPos().x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);
				segments[3]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y);

			}else if(current_rotation == 3){
				segments[1]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[2]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[3]->setPos(segments[0]->getPos().x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);

			}else if(current_rotation == 4){
				segments[1]->setPos(segments[0]->getPos().x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);
				segments[2]->setPos(segments[0]->getPos().x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);
				segments[3]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y);

			}
			break;

		case Type::L:
			if(current_rotation == 4){
				segments[1]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[2]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[3]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);

			}else if(current_rotation == 1){
				segments[1]->setPos(segments[0]->getPos().x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);
				segments[2]->setPos(segments[0]->getPos().x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);
				segments[3]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);

			}else if(current_rotation == 2){
				segments[1]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[2]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[3]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);

			}else if(current_rotation == 3){
				segments[1]->setPos(segments[0]->getPos().x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);
				segments[2]->setPos(segments[0]->getPos().x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);
				segments[3]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);
			}
			break;

		case Type::ReverseL:
			if(current_rotation == 4){
				segments[1]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[2]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[3]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);

			}else if(current_rotation == 1){
				segments[1]->setPos(segments[0]->getPos().x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);
				segments[2]->setPos(segments[0]->getPos().x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);
				segments[3]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);
			}else if(current_rotation == 2){
				segments[1]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[2]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[3]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);

			}else if(current_rotation == 3){
				segments[1]->setPos(segments[0]->getPos().x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);
				segments[2]->setPos(segments[0]->getPos().x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);
				segments[3]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);

			}
			break;

		case Type::S:
			if(current_rotation == 2 || current_rotation == 4){
				segments[1]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[2]->setPos(segments[0]->getPos().x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);
				segments[3]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);

			}else if(current_rotation == 1 || current_rotation == 3){
				segments[1]->setPos(segments[0]->getPos().x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);
				segments[2]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[3]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);

			}
			break;

		case Type::ReverseS:
			if(current_rotation == 2 || current_rotation == 4){
				segments[1]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[2]->setPos(segments[0]->getPos().x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);
				segments[3]->setPos(segments[0]->getPos().x - 1 * Blocks::TileDim.x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);

			}else if(current_rotation == 1 || current_rotation == 3){
				segments[1]->setPos(segments[0]->getPos().x, segments[0]->getPos().y + 1 * Blocks::TileDim.y);
				segments[2]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y);
				segments[3]->setPos(segments[0]->getPos().x + 1 * Blocks::TileDim.x, segments[0]->getPos().y - 1 * Blocks::TileDim.y);

			}
			break;
		default:
			break;
	}
}

void Block::clearSegment(GameObjPtr segment){
	for(auto& seg : segments){
		if(seg.get() == segment.get()){
			seg.reset();
		}
	}
}
