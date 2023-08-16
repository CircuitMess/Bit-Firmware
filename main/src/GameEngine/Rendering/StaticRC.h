#ifndef BIT_FIRMWARE_STATICRC_H
#define BIT_FIRMWARE_STATICRC_H

#include "RenderComponent.h"
#include "FS/File.h"

class StaticRC : public RenderComponent {
public:
	/**
	 * @param file Raw RGB565 image file.
	 * @param dim Width, height.
	 */
	StaticRC(File file, PixelDim dim);
	~StaticRC() override = default;

	void setFile(File file); //no change in dim
	void setFile(File file, PixelDim dim); //change in dim

protected:
	void push(Sprite& parent, PixelDim pos, float rot) const override;

private:
	File file;
	PixelDim dim;
};


#endif //BIT_FIRMWARE_STATICRC_H
