#ifndef BIT_LIBRARY_PINS_HPP
#define BIT_LIBRARY_PINS_HPP

#include <unordered_map>
#include <cstdint>
#include <cstdio>
#include <esp_debug_helpers.h>
#include <vector>

enum class Pin : uint8_t {
	BtnDown,
	BtnUp,
	BtnLeft,
	BtnRight,
	BtnA,
	BtnB,
	BtnMenu,
	LedBl,
	Buzz,
	BattRead,
	BattVref,
	I2cSda,
	I2cScl,
	TftSck,
	TftMosi,
	TftDc,
	TftRst,
	Det1,
	Det2,
	Addr1,
	Addr2,
	Addr3,
	Addr4,
	Addr5,
	Addr6,
	Ctrl1,
	Ctrl2,
	Ctrl3,
	Ctrl4,
	Ctrl5,
	Ctrl6,
	LedDown,
	LedUp,
	LedLeft,
	LedRight,
	LedA,
	LedB,
	LedMenu
};

class Pins {
	typedef std::unordered_map<Pin, int> PinMap;
public:

	static int get(Pin pin);

	static void setLatest();

private:
	Pins();

	PinMap* currentMap = nullptr;

	inline static Pins* instance = nullptr;

	void initPinMaps();

	//For original Bit, Bit 2
	PinMap Revision1;

	//For Bit v3
	PinMap Revision3;

	std::vector<PinMap*> pinMaps = { &Revision1, &Revision3 };
};

#endif //BIT_LIBRARY_PINS_HPP
