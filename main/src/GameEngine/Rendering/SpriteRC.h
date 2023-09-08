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
	void setScale(float scale);

protected:
	void push(Sprite& parent, PixelDim pos, float rot, bool flipX, bool flipY) const override;

private:
	std::shared_ptr<Sprite> sprite;
	float scale = 1.0;
};


#endif //BIT_FIRMWARE_SPRITERC_H
