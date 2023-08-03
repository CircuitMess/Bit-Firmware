#ifndef BIT_FIRMWARE_DISPLAY_H
#define BIT_FIRMWARE_DISPLAY_H

#include <LovyanGFX.h>

typedef lgfx::LGFX_Sprite Sprite;

class Display {
public:
	Display();
	virtual ~Display();

	LGFX_Device& getLGFX();

	Sprite& getCanvas();

	void drawTest();

private:
	lgfx::Bus_SPI bus;
	lgfx::Panel_ST7735S panel;
	LGFX_Device lgfx;
	Sprite canvas;

	void setupBus();
	void setupPanel();

};


#endif //BIT_FIRMWARE_DISPLAY_H
