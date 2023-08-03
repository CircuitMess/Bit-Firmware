#include "RectCC.h"

RectCC::RectCC(glm::vec2 dim) : CollisionComponent(CollisionType::Rect), dim(dim){

}

glm::vec2 RectCC::getDim() const{
	return dim;
}
