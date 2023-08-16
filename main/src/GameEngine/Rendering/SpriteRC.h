#ifndef BIT_FIRMWARE_SPRITERC_H
#define BIT_FIRMWARE_SPRITERC_H

#include <memory>
#include "Devices/Display.h"
#include "../PixelDim.hpp"
#include "RenderComponent.h"

class SpriteRC : public RenderComponent {
public:
	SpriteRC(PixelDim dim);
	virtual ~SpriteRC() = default;
	std::shared_ptr<Sprite> getSprite() const;

protected:
	void push(Sprite& parent, PixelDim pos, float rot) const override;

private:
	std::shared_ptr<Sprite> sprite;
};


#endif //BIT_FIRMWARE_SPRITERC_H
