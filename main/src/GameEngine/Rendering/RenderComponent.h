#ifndef CIRCUITPET_FIRMWARE_RENDERCOMPONENT_H
#define CIRCUITPET_FIRMWARE_RENDERCOMPONENT_H

#include "Devices/Display.h"
#include "../PixelDim.hpp"

class RenderSystem;

class RenderComponent {
	friend RenderSystem;

public:
	virtual ~RenderComponent() = default;
	int8_t getLayer() const;
	void setLayer(int8_t layer);

	bool isVisible() const;
	void setVisible(bool visible);

protected:
	/**
	 * Update is used for animated RenderComponents
	 * @param deltaMicros Time delta since last update.
	 */
	virtual void update(uint32_t deltaMicros);
	virtual void push(Sprite& parent, PixelDim pos, float rot) const = 0;

private:
	bool visible = true;
	int8_t layer = 0;
};


#endif //CIRCUITPET_FIRMWARE_RENDERCOMPONENT_H
